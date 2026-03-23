#include "AspMapProjection.h"

#include "Camera.h"
#include "CameraFactory.h"
#include "CSMCamera.h"
#include "Cube.h"
#include "Distance.h"
#include "Latitude.h"
#include "LineManager.h"
#include "Longitude.h"
#include "Portal.h"
#include "Preference.h"
#include "Progress.h"
#include "Pvl.h"
#include "SpecialPixel.h"
#include "SurfacePoint.h"
#include "Target.h"
#include "TProjection.h"
#include "UserInterface.h"

#include <gdal.h>
#include <gdal_priv.h>
#include <ogr_spatialref.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Isis { namespace asp {

const double nan = std::numeric_limits<double>::quiet_NaN();

// Lightweight georef class using GDAL/PROJ for coordinate transforms, matching
// ASP/VW behavior exactly. All pixels are 0-based, pixel-is-area convention.
class GeoRef {
public:
  // Construct from a GDAL-readable file (reads WKT and GeoTransform)
  GeoRef(const std::string &filename);
  // Construct from a projection string (WKT or PROJ4). Identity GeoTransform.
  GeoRef(const std::string &projStr, bool fromString);
  // Copy constructor (deep copy, each object owns its resources)
  GeoRef(const GeoRef &other);
  ~GeoRef();

  void pixel_to_lonlat(double col, double row,
                       double &lon, double &lat) const;
  void lonlat_to_pixel(double lon, double lat,
                       double &col, double &row) const;
  void point_to_lonlat(double x, double y,
                       double &lon, double &lat) const;
  void lonlat_to_point(double lon, double lat,
                       double &x, double &y) const;
  void geodetic_to_cartesian(double lon, double lat, double height,
                             double &x, double &y, double &z) const;
  void setGeoTransform(const double gt[6]);

  GeoRef &operator=(const GeoRef &other);

  double semi_major_axis() const { return m_semi_major; }
  double semi_minor_axis() const { return m_semi_minor; }
  const std::string &wkt() const { return m_wkt; }
  bool isGeographic() const;
  std::string isisProjectionName() const;
  double centerLongitude() const;
  double centerLatitude() const;

private:
  void initTransforms(OGRSpatialReference &projCRS);

  std::string m_wkt;
  double m_gt[6];
  double m_inv_gt[6];
  double m_semi_major;
  double m_semi_minor;
  OGRCoordinateTransformation *m_proj_to_lonlat;
  OGRCoordinateTransformation *m_lonlat_to_proj;
};

// Shared initialization: extract radii and create coordinate transforms
void GeoRef::initTransforms(OGRSpatialReference &projCRS) {
  OGRErr err = OGRERR_NONE;
  m_semi_major = projCRS.GetSemiMajor(&err);
  if (err != OGRERR_NONE)
    m_semi_major = 0.0;
  m_semi_minor = projCRS.GetSemiMinor(&err);
  if (err != OGRERR_NONE)
    m_semi_minor = 0.0;

  OGRSpatialReference lonlatCRS;
  lonlatCRS.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
  lonlatCRS.CopyGeogCSFrom(&projCRS);

  m_proj_to_lonlat = OGRCreateCoordinateTransformation(&projCRS, &lonlatCRS);
  m_lonlat_to_proj = OGRCreateCoordinateTransformation(&lonlatCRS, &projCRS);

  if (!m_proj_to_lonlat || !m_lonlat_to_proj)
    throw std::runtime_error("GeoRef: cannot create transforms");
}

// Default values for members that initTransforms() will populate
GeoRef::GeoRef(const std::string &filename):
  m_semi_major(0.0), m_semi_minor(0.0),
  m_proj_to_lonlat(nullptr), m_lonlat_to_proj(nullptr) {

  for (int i = 0; i < 6; i++) {
    m_gt[i] = 0.0;
    m_inv_gt[i] = 0.0;
  }

  GDALAllRegister();
  GDALDataset *ds = (GDALDataset *)GDALOpen(filename.c_str(), GA_ReadOnly);
  if (!ds)
    throw std::runtime_error("GeoRef: cannot open " + filename);

  if (ds->GetGeoTransform(m_gt) != CE_None) {
    GDALClose(ds);
    throw std::runtime_error("GeoRef: no GeoTransform in " + filename);
  }

  if (!GDALInvGeoTransform(m_gt, m_inv_gt)) {
    GDALClose(ds);
    throw std::runtime_error("GeoRef: cannot invert GeoTransform");
  }

  const char *wkt = ds->GetProjectionRef();
  if (!wkt || wkt[0] == '\0') {
    GDALClose(ds);
    throw std::runtime_error("GeoRef: no projection in " + filename);
  }

  OGRSpatialReference projCRS;
  projCRS.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
  if (projCRS.importFromWkt(wkt) != OGRERR_NONE) {
    GDALClose(ds);
    throw std::runtime_error("GeoRef: cannot parse WKT");
  }

  GDALClose(ds);

  // Store WKT for copy constructor
  char *exportedWkt = nullptr;
  projCRS.exportToWkt(&exportedWkt);
  if (exportedWkt) {
    m_wkt = exportedWkt;
    CPLFree(exportedWkt);
  }

  initTransforms(projCRS);
}

// Construct from a WKT or PROJ4 string. Identity GeoTransform.
// Default values for members that initTransforms() will populate.
GeoRef::GeoRef(const std::string &projStr, bool /*fromString*/):
  m_semi_major(0.0), m_semi_minor(0.0),
  m_proj_to_lonlat(nullptr), m_lonlat_to_proj(nullptr) {

  // Identity GeoTransform: pixel coords = projected coords
  m_gt[0] = 0.0; m_gt[1] = 1.0; m_gt[2] = 0.0;
  m_gt[3] = 0.0; m_gt[4] = 0.0; m_gt[5] = -1.0;
  (void)GDALInvGeoTransform(m_gt, m_inv_gt);

  OGRSpatialReference projCRS;
  projCRS.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
  // Try WKT first, then PROJ4
  if (projCRS.importFromWkt(projStr.c_str()) != OGRERR_NONE)
    if (projCRS.importFromProj4(projStr.c_str()) != OGRERR_NONE)
      throw std::runtime_error(
        "GeoRef: cannot parse projection string: " + projStr);

  // Store as WKT for copy constructor
  char *exportedWkt = nullptr;
  projCRS.exportToWkt(&exportedWkt);
  if (exportedWkt) {
    m_wkt = exportedWkt;
    CPLFree(exportedWkt);
  }

  initTransforms(projCRS);
}

// Deep copy: recreate OGR transforms from stored WKT.
// Default values for members that initTransforms() will populate.
GeoRef::GeoRef(const GeoRef &other):
  m_wkt(other.m_wkt),
  m_semi_major(0.0), m_semi_minor(0.0),
  m_proj_to_lonlat(nullptr), m_lonlat_to_proj(nullptr) {

  for (int i = 0; i < 6; i++) {
    m_gt[i] = other.m_gt[i];
    m_inv_gt[i] = other.m_inv_gt[i];
  }

  OGRSpatialReference projCRS;
  projCRS.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
  if (projCRS.importFromWkt(m_wkt.c_str()) != OGRERR_NONE)
    throw std::runtime_error("GeoRef copy: cannot parse stored WKT");

  initTransforms(projCRS);
}

GeoRef &GeoRef::operator=(const GeoRef &other) {
  if (this == &other)
    return *this;

  // Clean up existing transforms
  if (m_proj_to_lonlat)
    OGRCoordinateTransformation::DestroyCT(m_proj_to_lonlat);
  if (m_lonlat_to_proj)
    OGRCoordinateTransformation::DestroyCT(m_lonlat_to_proj);
  m_proj_to_lonlat = nullptr;
  m_lonlat_to_proj = nullptr;

  m_wkt = other.m_wkt;
  for (int i = 0; i < 6; i++) {
    m_gt[i] = other.m_gt[i];
    m_inv_gt[i] = other.m_inv_gt[i];
  }

  OGRSpatialReference projCRS;
  projCRS.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
  if (projCRS.importFromWkt(m_wkt.c_str()) != OGRERR_NONE)
    throw std::runtime_error("GeoRef assign: cannot parse stored WKT");

  initTransforms(projCRS);
  return *this;
}

GeoRef::~GeoRef() {
  if (m_proj_to_lonlat)
    OGRCoordinateTransformation::DestroyCT(m_proj_to_lonlat);
  if (m_lonlat_to_proj)
    OGRCoordinateTransformation::DestroyCT(m_lonlat_to_proj);
}

void GeoRef::point_to_lonlat(double x, double y,
                             double &lon, double &lat) const {
  lon = x;
  lat = y;
  if (!m_proj_to_lonlat->Transform(1, &lon, &lat))
    throw std::runtime_error("GeoRef::point_to_lonlat failed");
}

void GeoRef::lonlat_to_point(double lon, double lat,
                             double &x, double &y) const {
  x = lon;
  y = lat;
  if (!m_lonlat_to_proj->Transform(1, &x, &y))
    throw std::runtime_error("GeoRef::lonlat_to_point failed");
}

void GeoRef::setGeoTransform(const double gt[6]) {
  for (int i = 0; i < 6; i++)
    m_gt[i] = gt[i];
  (void)GDALInvGeoTransform(m_gt, m_inv_gt);
}

bool GeoRef::isGeographic() const {
  OGRSpatialReference srs;
  srs.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
  if (srs.importFromWkt(m_wkt.c_str()) != OGRERR_NONE)
    return false;
  return srs.IsGeographic();
}

// Map OGR projection name to ISIS PVL ProjectionName.
// OGR names like "Polar_Stereographic" must become "PolarStereographic",
// "Lambert_Conformal_Conic_2SP" must become "LambertConformal", etc.
// Matches the GDAL ISIS3 driver (frmts/pds/isis3dataset.cpp).
std::string GeoRef::isisProjectionName() const {
  OGRSpatialReference srs;
  srs.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
  if (srs.importFromWkt(m_wkt.c_str()) != OGRERR_NONE)
    return "Unknown";
  const char *p = srs.GetAttrValue("PROJECTION");
  if (!p)
    return "Unknown";
  std::string name(p);
  std::string isisName;
  if (name == SRS_PT_EQUIRECTANGULAR)            isisName = "Equirectangular";
  else if (name == SRS_PT_SINUSOIDAL)            isisName = "Sinusoidal";
  else if (name == SRS_PT_ORTHOGRAPHIC)          isisName = "Orthographic";
  else if (name == SRS_PT_POLAR_STEREOGRAPHIC)   isisName = "PolarStereographic";
  else if (name == SRS_PT_MERCATOR_1SP)          isisName = "Mercator";
  else if (name == SRS_PT_TRANSVERSE_MERCATOR)   isisName = "TransverseMercator";
  else if (name == SRS_PT_LAMBERT_CONFORMAL_CONIC_2SP) isisName = "LambertConformal";
  else                                           isisName = name;
  return isisName;
}

// Sinusoidal uses LONGITUDE_OF_CENTER, all others use CENTRAL_MERIDIAN.
// Matches the GDAL ISIS3 driver.
double GeoRef::centerLongitude() const {
  OGRSpatialReference srs;
  srs.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
  if (srs.importFromWkt(m_wkt.c_str()) != OGRERR_NONE)
    return 0.0;
  const char *p = srs.GetAttrValue("PROJECTION");
  if (p && std::string(p) == SRS_PT_SINUSOIDAL)
    return srs.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER, 0.0);
  return srs.GetProjParm(SRS_PP_CENTRAL_MERIDIAN, 0.0);
}

// For equirectangular, ISIS CenterLatitude is the true scale latitude
// (STANDARD_PARALLEL_1 / lat_ts), not latitude_of_origin.
// For all other projections, it is LATITUDE_OF_ORIGIN.
// Sinusoidal has no CenterLatitude (returns 0).
// Matches the GDAL ISIS3 driver.
double GeoRef::centerLatitude() const {
  OGRSpatialReference srs;
  srs.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
  if (srs.importFromWkt(m_wkt.c_str()) != OGRERR_NONE)
    return 0.0;
  const char *p = srs.GetAttrValue("PROJECTION");
  // GetProjParm returns the default (0.0) if the parameter is absent,
  // which is indistinguishable from a genuine lat=0. Acceptable since
  // 0.0 is the correct value in both cases.
  if (p && std::string(p) == SRS_PT_EQUIRECTANGULAR)
    return srs.GetProjParm(SRS_PP_STANDARD_PARALLEL_1, 0.0);
  return srs.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN, 0.0);
}

void GeoRef::pixel_to_lonlat(double col, double row,
                             double &lon, double &lat) const {
  // GDAL GeoTransform uses pixel-edge convention (0,0 = UL corner of UL pixel).
  // Add 0.5 to get pixel center, matching ASP/VW convention.
  double ac = col + 0.5;
  double ar = row + 0.5;
  double x = m_gt[0] + ac * m_gt[1] + ar * m_gt[2];
  double y = m_gt[3] + ac * m_gt[4] + ar * m_gt[5];
  lon = x;
  lat = y;
  if (!m_proj_to_lonlat->Transform(1, &lon, &lat))
    throw std::runtime_error("GeoRef::pixel_to_lonlat failed");
}

void GeoRef::lonlat_to_pixel(double lon, double lat,
                             double &col, double &row) const {
  double x = lon;
  double y = lat;
  if (!m_lonlat_to_proj->Transform(1, &x, &y))
    throw std::runtime_error("GeoRef::lonlat_to_pixel failed");
  double ac = m_inv_gt[0] + x * m_inv_gt[1] + y * m_inv_gt[2];
  double ar = m_inv_gt[3] + x * m_inv_gt[4] + y * m_inv_gt[5];
  // Convert from GDAL pixel-edge back to pixel-center convention
  col = ac - 0.5;
  row = ar - 0.5;
}

// Implementation of VW geodetic_to_cartesian.
void GeoRef::geodetic_to_cartesian(double lon, double lat, double height,
                                   double &x, double &y, double &z) const {
  double a = m_semi_major;
  double b = m_semi_minor;
  double a2 = a * a;
  double b2 = b * b;
  double e2 = (a2 - b2) / a2;
  double lonRad = lon * M_PI / 180.0;
  double latRad = lat * M_PI / 180.0;
  double slat = sin(latRad);
  double clat = cos(latRad);
  double slon = sin(lonRad);
  double clon = cos(lonRad);
  double N = a / sqrt(1.0 - e2 * slat * slat);
  x = (N + height) * clat * clon;
  y = (N + height) * clat * slon;
  z = (N * (1.0 - e2) + height) * slat;
}

// Cartographic utility. Convert an ISIS PVL Mapping group to a WKT string
// using OGR setters. Mirrors GDAL's ISIS3 driver (frmts/pds/isis3dataset.cpp)
// for consistency. Radii default to the DEM's if not present in the map file.
std::string pvlToWkt(const PvlGroup &mapGrp,
                     double defaultSemiMajor,
                     double defaultSemiMinor) {

  std::string projName = mapGrp["ProjectionName"][0].toLower().toStdString();

  double a = defaultSemiMajor;
  double b = defaultSemiMinor;
  if (mapGrp.hasKeyword("EquatorialRadius"))
    a = toDouble(mapGrp["EquatorialRadius"][0]);
  if (mapGrp.hasKeyword("PolarRadius"))
    b = toDouble(mapGrp["PolarRadius"][0]);

  double lon0 = 0.0;
  if (mapGrp.hasKeyword("CenterLongitude"))
    lon0 = toDouble(mapGrp["CenterLongitude"][0]);

  double lat0 = 0.0;
  if (mapGrp.hasKeyword("CenterLatitude"))
    lat0 = toDouble(mapGrp["CenterLatitude"][0]);

  double scaleFactor = 1.0;
  if (mapGrp.hasKeyword("ScaleFactor"))
    scaleFactor = toDouble(mapGrp["ScaleFactor"][0]);

  // Set projection (matching GDAL ISIS3 driver)
  OGRSpatialReference oSRS;
  oSRS.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);

  if (projName == "pointperspective" || projName == "obliquecylindrical")
    throw IException(IException::User,
      "ASP_MAP: projection not yet supported in PVL map file: " +
      QString::fromStdString(projName) +
      ". Provide the projection via a georeferenced image "
      "(e.g., a .cub or .tif) as the map= argument instead.",
      _FILEINFO_);
  else if (projName == "equirectangular" || projName == "simplecylindrical")
    oSRS.SetEquirectangular2(0.0, lon0, lat0, 0, 0);
  else if (projName == "orthographic")
    oSRS.SetOrthographic(lat0, lon0, 0, 0);
  else if (projName == "sinusoidal")
    oSRS.SetSinusoidal(lon0, 0, 0);
  else if (projName == "mercator")
    oSRS.SetMercator(lat0, lon0, scaleFactor, 0, 0);
  else if (projName == "polarstereographic")
    oSRS.SetPS(lat0, lon0, scaleFactor, 0, 0);
  else if (projName == "stereographic")
    oSRS.SetStereographic(lat0, lon0, scaleFactor, 0, 0);
  else if (projName == "transversemercator")
    oSRS.SetTM(lat0, lon0, scaleFactor, 0, 0);
  else if (projName == "lambertconformal") {
    double par1 = 0.0, par2 = 0.0;
    if (mapGrp.hasKeyword("FirstStandardParallel"))
      par1 = toDouble(mapGrp["FirstStandardParallel"][0]);
    if (mapGrp.hasKeyword("SecondStandardParallel"))
      par2 = toDouble(mapGrp["SecondStandardParallel"][0]);
    oSRS.SetLCC(par1, par2, lat0, lon0, 0, 0);
  } else if (projName == "lambertazimuthalequalarea")
    oSRS.SetLAEA(lat0, lon0, 0, 0);
  else if (projName == "mollweide")
    oSRS.SetMollweide(lon0, 0, 0);
  else if (projName == "robinson")
    oSRS.SetRobinson(lon0, 0, 0);
  else
    throw IException(IException::User,
      "ASP_MAP: unsupported projection in PVL map file: " +
      QString::fromStdString(projName) +
      ". Provide the projection via a georeferenced image "
      "(e.g., a .cub or .tif) as the map= argument instead.",
      _FILEINFO_);

  // Set ProjCS name, e.g. "Equirectangular Moon"
  std::string targetName = "Body";
  if (mapGrp.hasKeyword("TargetName"))
    targetName = mapGrp["TargetName"][0].toStdString();
  oSRS.SetProjCS((mapGrp["ProjectionName"][0].toStdString() +
                  " " + targetName).c_str());

  // Set ellipsoid (matching GDAL ISIS3 driver's per-projection logic)
  bool bIsGeographic = true;
  if (mapGrp.hasKeyword("LatitudeType") &&
      mapGrp["LatitudeType"][0].toLower() == "planetocentric")
    bIsGeographic = false;

  double iflattening = 0.0;
  if ((a - b) >= 0.0000001)
    iflattening = a / (a - b);

  std::string geogName = "GCS_" + targetName;
  std::string datumName = "D_" + targetName;
  std::string sphereName = targetName;

  if (projName == "polarstereographic" ||
      (projName == "stereographic" && fabs(lat0) == 90.0)) {
    if (bIsGeographic)
      oSRS.SetGeogCS(geogName.c_str(), datumName.c_str(), sphereName.c_str(),
                     a, iflattening, "Reference_Meridian", 0.0);
    else
      oSRS.SetGeogCS(geogName.c_str(), datumName.c_str(),
                     (sphereName + "_polarRadius").c_str(),
                     b, 0.0, "Reference_Meridian", 0.0);
  } else if (projName == "simplecylindrical" ||
             projName == "orthographic" ||
             projName == "sinusoidal") {
    // ISIS uses spherical equations for these
    oSRS.SetGeogCS(geogName.c_str(), datumName.c_str(), sphereName.c_str(),
                   a, 0.0, "Reference_Meridian", 0.0);
  } else if (projName == "equirectangular") {
    // Local radius at CenterLatitude
    double radLat = lat0 * M_PI / 180.0;
    double meanRadius = sqrt(pow(b * cos(radLat), 2) +
                             pow(a * sin(radLat), 2));
    double localRadius = (meanRadius == 0.0) ? 0.0 : a * b / meanRadius;
    oSRS.SetGeogCS(geogName.c_str(), datumName.c_str(),
                   (sphereName + "_localRadius").c_str(),
                   localRadius, 0.0, "Reference_Meridian", 0.0);
  } else {
    // Mercator, TM, LCC, LAEA, Mollweide, Robinson
    if (bIsGeographic)
      oSRS.SetGeogCS(geogName.c_str(), datumName.c_str(), sphereName.c_str(),
                     a, iflattening, "Reference_Meridian", 0.0);
    else
      oSRS.SetGeogCS(geogName.c_str(), datumName.c_str(), sphereName.c_str(),
                     a, 0.0, "Reference_Meridian", 0.0);
  }

  // Export to WKT
  char *wkt = nullptr;
  oSRS.exportToWkt(&wkt);
  if (!wkt)
    throw IException(IException::Programmer,
      "Failed to export projection to WKT", _FILEINFO_);
  std::string result(wkt);
  CPLFree(wkt);
  return result;
}

// Tiled raster reader using ISIS Portal for chunk-cached random access.
// Portals are small buffers (1x1, 2x2, 4x4) that read from disk on demand
// with ISIS's chunk caching - never loads the full image into memory.
// All coordinates are 0-based externally; converted to 1-based internally.
class RasterReader {
public:
  RasterReader(const std::string &filename) {
    m_cube.open(QString::fromStdString(filename));
    m_cols = m_cube.sampleCount();
    m_rows = m_cube.lineCount();
    m_bands = m_cube.bandCount();
    if (m_cols <= 0 || m_rows <= 0)
      throw IException(IException::User,
        "Raster file [" + filename + "] has zero-size dimensions (" +
        std::to_string(m_cols) + "x" + std::to_string(m_rows) + ")",
        _FILEINFO_);
    m_portal1 = std::make_shared<Portal>(1, 1, m_cube.pixelType());
    // 2x2 portal for bilinear; hotspot (0,0) per ISIS convention (0-based)
    m_portal2 = std::make_shared<Portal>(2, 2, m_cube.pixelType(), 0, 0);
    // 4x4 portal for bicubic; hotspot (1,1) per ISIS convention (0-based)
    m_portal4 = std::make_shared<Portal>(4, 4, m_cube.pixelType(), 1, 1);
  }

  ~RasterReader() {
    m_cube.close();
  }

  // Read a single pixel value (0-based col, row).
  // Caller should check IsSpecial() for nodata.
  double pixelVal(int col, int row, int band = 1) {
    m_portal1->SetPosition(col + 1.0, row + 1.0, band);
    m_cube.read(*m_portal1);
    return m_portal1->DoubleBuffer()[0];
  }

  // Bilinear interpolation at fractional 0-based (col, row).
  // Returns NaN for out-of-bounds or special pixels.
  double bilinearVal(double col, double row, int band = 1) {
    int ix = (int)floor(col);
    int iy = (int)floor(row);
    if (ix < 0 || ix + 1 >= m_cols || iy < 0 || iy + 1 >= m_rows)
      return nan;

    m_portal2->SetPosition(ix + 1.0, iy + 1.0, band);
    m_cube.read(*m_portal2);
    const double *buf = m_portal2->DoubleBuffer();

    for (int i = 0; i < 4; i++)
      if (IsSpecial(buf[i]))
        return nan;

    double fx = col - ix;
    double fy = row - iy;
    return (1 - fx) * (1 - fy) * buf[0] + fx * (1 - fy) * buf[1] +
           (1 - fx) * fy * buf[2] + fx * fy * buf[3];
  }

  // Bicubic interpolation at fractional 0-based (col, row).
  // VW-matching kernel. Returns NaN for out-of-bounds or special pixels.
  double bicubicVal(double col, double row, int band = 1) {
    int ix = (int)floor(col);
    int iy = (int)floor(row);
    if (ix < 1 || ix + 2 >= m_cols || iy < 1 || iy + 2 >= m_rows)
      return nan;

    m_portal4->SetPosition(ix + 1.0, iy + 1.0, band);
    m_cube.read(*m_portal4);
    const double *buf = m_portal4->DoubleBuffer();

    for (int i = 0; i < 16; i++)
      if (IsSpecial(buf[i]))
        return nan;

    double fx = col - ix;
    double fy = row - iy;

    // VW bicubic weights (from vw/Image/Interpolation.h)
    double sx[4], sy[4];
    sx[0] = ((2.0 - fx) * fx - 1.0) * fx;
    sx[1] = (3.0 * fx - 5.0) * fx * fx + 2.0;
    sx[2] = ((4.0 - 3.0 * fx) * fx + 1.0) * fx;
    sx[3] = (fx - 1.0) * fx * fx;
    sy[0] = ((2.0 - fy) * fy - 1.0) * fy;
    sy[1] = (3.0 * fy - 5.0) * fy * fy + 2.0;
    sy[2] = ((4.0 - 3.0 * fy) * fy + 1.0) * fy;
    sy[3] = (fy - 1.0) * fy * fy;

    double result = 0.0;
    for (int jr = 0; jr < 4; jr++)
      for (int jc = 0; jc < 4; jc++)
        result += sy[jr] * sx[jc] * buf[jr * 4 + jc];
    return result * 0.25;
  }

  int cols() const { return m_cols; }
  int rows() const { return m_rows; }
  int bands() const { return m_bands; }

private:
  Cube m_cube;
  std::shared_ptr<Portal> m_portal1;
  std::shared_ptr<Portal> m_portal2;
  std::shared_ptr<Portal> m_portal4;
  int m_cols, m_rows, m_bands;
};

// Bilinear interpolation with zero edge extension for DEM.
// Estimate average DEM height by sampling up to ~20 evenly spaced points.
// Matches VW's demHeightGuess: step = rows/10 x cols/10, stop after 20.
double demHeightGuess(RasterReader &demReader) {
  int demCols = demReader.cols();
  int demRows = demReader.rows();
  double sum = 0.0;
  int count = 0;
  bool done = false;
  double rowStep = demRows / 10.0;
  double colStep = demCols / 10.0;
  for (double r = 0; r < demRows && !done; r += rowStep) {
    for (double c = 0; c < demCols; c += colStep) {
      double v = demReader.pixelVal((int)c, (int)r);
      if (IsSpecial(v))
        continue;
      sum += v;
      count++;
      if (count > 20) {
        done = true;
        break;
      }
    }
  }
  if (count == 0)
    return 0.0;
  return sum / count;
}

// Ray-ellipsoid intersection. Returns intersection XYZ, or zero vector
// if no intersection. Same math as VW's datum_intersection.
void datumIntersection(double semiMajor, double semiMinor,
                       const double camCtr[3], const double camVec[3],
                       // Outputs
                       bool &hit, double xyz[3]) {

  // Initialize outputs
  hit = false;
  xyz[0] = 0.0; xyz[1] = 0.0; xyz[2] = 0.0;

  // Scale Z to transform ellipsoid into sphere
  double zScale = semiMajor / semiMinor;
  double ctr[3] = {camCtr[0], camCtr[1], camCtr[2] * zScale};
  double vec[3] = {camVec[0], camVec[1], camVec[2] * zScale};
  double vecLen = sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
  if (vecLen == 0.0)
    return;
  vec[0] /= vecLen; vec[1] /= vecLen; vec[2] /= vecLen;

  double r2 = semiMajor * semiMajor;
  double alpha = -(ctr[0]*vec[0] + ctr[1]*vec[1] + ctr[2]*vec[2]);
  double px = ctr[0] + alpha * vec[0];
  double py = ctr[1] + alpha * vec[1];
  double pz = ctr[2] + alpha * vec[2];
  double proj2 = px*px + py*py + pz*pz;
  if (proj2 > r2)
    return;

  alpha -= sqrt(r2 - proj2);
  xyz[0] = ctr[0] + alpha * vec[0];
  xyz[1] = ctr[1] + alpha * vec[1];
  xyz[2] = (ctr[2] + alpha * vec[2]) / zScale;
  hit = true;
}

// ECEF XYZ to geodetic lon/lat/height (degrees, meters). Models the ellipsoid. 
void xyzToLonLat(const double xyz[3], double semiMajor, double semiMinor,
                 double &lon, double &lat, double &height) {
  double x = xyz[0], y = xyz[1], z = xyz[2];
  lon = atan2(y, x) * 180.0 / M_PI;
  double p = sqrt(x*x + y*y);
  // Iterative method for geodetic latitude
  double a2 = semiMajor * semiMajor;
  double b2 = semiMinor * semiMinor;
  double e2 = (a2 - b2) / a2;
  lat = atan2(z, p * (1.0 - e2)); // initial estimate
  for (int i = 0; i < 5; i++) {
    double slat = sin(lat);
    double N = semiMajor / sqrt(1.0 - e2 * slat * slat);
    lat = atan2(z + e2 * N * slat, p);
  }
  double slat = sin(lat);
  double clat = cos(lat);
  double N = semiMajor / sqrt(1.0 - e2 * slat * slat);
  if (std::abs(clat) > 1e-10)
    height = p / clat - N;
  else
    height = std::abs(z) / std::abs(slat) - N * (1.0 - e2);
  lat *= 180.0 / M_PI;
}

// DEM height lookup for secant method. Uses bilinear interpolation,
// matching VW's RayDEMIntersectionLMA.
double demHeightAtLonLatSecant(double lon, double lat,
                              RasterReader &demReader,
                              const GeoRef &demGeoRef) {
  double col = 0.0, row = 0.0;
  demGeoRef.lonlat_to_pixel(lon, lat, col, row);
  // Out of bounds: matching VW's Helper (0 <= x <= cols-1, 0 <= y <= rows-1)
  if (col < 0.0 || col > demReader.cols() - 1.0 ||
      row < 0.0 || row > demReader.rows() - 1.0)
    return nan;
  return demReader.bilinearVal(col, row);
}

// Error function for secant ray-DEM intersection. Returns DEM height minus
// ray height at parameter t along the ray. Returns bigVal (not NaN) for
// points outside the DEM, matching VW's RayDEMIntersectionLMA behavior.
double secantErrFunc(double t,
                     const double camCtr[3], const double camVec[3],
                     double semiMajor, double semiMinor,
                     RasterReader &demReader,
                     const GeoRef &demGeoRef, double bigVal) {
  double pt[3] = {camCtr[0] + t * camVec[0],
                  camCtr[1] + t * camVec[1],
                  camCtr[2] + t * camVec[2]};
  double lon = 0.0, lat = 0.0, h = 0.0;
  xyzToLonLat(pt, semiMajor, semiMinor, lon, lat, h);
  double demH = demHeightAtLonLatSecant(lon, lat, demReader, demGeoRef);
  if (std::isnan(demH))
    return bigVal;
  return demH - h;
}

// Secant method: intersect camera ray with DEM surface.
// camCtr, camVec are ECEF camera center and unit direction.
// xyzGuess: if non-null, use as initial guess instead of datum intersection.
// Returns true if intersection found, with xyz set.
bool secantRayDem(const double camCtr[3], const double camVec[3],
                  RasterReader &demReader,
                  const GeoRef &demGeoRef, double heightGuess,
                  double xyz[3],
                  const double *xyzGuess = nullptr) {
  double semiMajor = demGeoRef.semi_major_axis();
  double semiMinor = demGeoRef.semi_minor_axis();

  if (xyzGuess) {
    // Use provided ECEF guess directly
    xyz[0] = xyzGuess[0];
    xyz[1] = xyzGuess[1];
    xyz[2] = xyzGuess[2];
  } else {
    // Initial guess: intersect ray with datum at heightGuess
    bool hit = false;
    datumIntersection(semiMajor + heightGuess, semiMinor + heightGuess,
                      camCtr, camVec, hit, xyz);
    if (!hit)
      return false;
  }

  // t = distance along ray from camCtr (use norm_2, matching VW)
  double dx = xyz[0] - camCtr[0];
  double dy = xyz[1] - camCtr[1];
  double dz = xyz[2] - camCtr[2];
  double t0 = sqrt(dx * dx + dy * dy + dz * dz);
  const double bigVal = 1.0e+50;
  
  // Define errFunc to avoid carrying all the params around. Only t varies
  // across calls as we move along the ray.
  auto errFunc = [&](double t) {
    return secantErrFunc(t, camCtr, camVec, semiMajor, semiMinor,
                         demReader, demGeoRef, bigVal);
  };

  // Step 1: Wiggle t0 along the ray until we land over valid DEM terrain.
  // Matches VW's findInitPositionAboveDEM: expand delta up to 2% of radius.
  double radius = sqrt(xyz[0] * xyz[0] + xyz[1] * xyz[1] + xyz[2] * xyz[2]);
  int wiggleIters = 10;
  double smallDelta = radius * 0.02 / (1 << wiggleIters);
  double f0 = errFunc(t0);
  if (std::abs(f0) >= bigVal / 10.0) {
    bool found = false;
    double t0_orig = t0;
    for (int wi = 0; wi <= wiggleIters && !found; wi++) {
      double delta = (wi == 0) ? 0.0 : smallDelta * (1 << wi);
      for (int k = -1; k <= 1; k += 2) {
        t0 = t0_orig - k * delta;
        f0 = errFunc(t0);
        if (std::abs(f0) < bigVal / 10.0) {
          found = true;
          break;
        }
      }
    }
    if (!found)
      return false;
  }

  // Step 2: Secant method. Match VW: try 100 outer attempts with increasing
  // step sizes, each with 100 inner secant iterations.
  double tol = 1e-3; // meters
  int numJ = 100;
  for (int j = 0; j <= numJ; j++) {
    double x0 = t0;
    f0 = errFunc(t0);

    if (std::abs(f0) < tol) {
      // Already close enough
      t0 = x0;
      break;
    }

    // Try positive perturbation first, then negative, matching VW but
    // also handling cases where the zero crossing is in the other direction
    double step = 10.0 * (j + 1);
    double x1 = t0 + step;
    double f1 = errFunc(x1);
    // If positive gives bigVal, try negative
    if (std::abs(f1) >= bigVal / 10.0) {
      double x1n = t0 - step;
      double f1n = errFunc(x1n);
      if (std::abs(f1n) < bigVal / 10.0) {
        x1 = x1n;
        f1 = f1n;
      }
    }

    if (std::abs(f0 - f1) < 1e-6 && j < numJ)
      continue; // f values too close, try larger step

    // Inner secant iterations
    for (int iter = 0; iter < 100; iter++) {
      double denom = f1 - f0;
      if (denom == 0.0)
        break;
      double x2 = x1 - f1 * (x1 - x0) / denom;
      if (std::isnan(x2) || std::isinf(x2))
        break;
      double f2 = errFunc(x2);
      if (std::abs(f2) < tol) {
        x0 = x2;
        f0 = f2;
        break;
      }
      x0 = x1; f0 = f1;
      x1 = x2; f1 = f2;
    }

    if (std::abs(f0) < tol) {
      t0 = x0;
      break;
    }
  }

  if (std::abs(errFunc(t0)) > tol)
    return false;

  double t1 = t0;

  xyz[0] = camCtr[0] + t1 * camVec[0];
  xyz[1] = camCtr[1] + t1 * camVec[1];
  xyz[2] = camCtr[2] + t1 * camVec[2];
  return true;
}

// Get ISIS camera ray for a given pixel (1-based), then intersect with DEM.
// xyzGuess: if non-null, use as initial ECEF guess for secant method.
// Returns true if intersection found. Sets lon, lat, xyz.
bool pixToDemXyz(Camera *cam, double sample, double line,
                 RasterReader &demReader,
                 const GeoRef &demGeoRef, double heightGuess,
                 double &lon, double &lat, double xyz[3],
                 const double *xyzGuess = nullptr) {
  try {
    // SetImage returns false when the ray doesn't intersect the ISIS shape
    // model, but we only need the camera ray (position + look direction),
    // which is set regardless. We do our own DEM intersection via secant.
    cam->SetImage(sample, line);

    // Camera position in body-fixed frame (km -> m)
    double camCtr[3];
    cam->instrumentBodyFixedPosition(camCtr);
    camCtr[0] *= 1000.0;
    camCtr[1] *= 1000.0;
    camCtr[2] *= 1000.0;

    // Look direction (unit vector in body-fixed frame, set by SetImage
    // even when it returns false)
    std::vector<double> look = cam->lookDirectionBodyFixed();
    double camVec[3] = {look[0], look[1], look[2]};

    if (!secantRayDem(camCtr, camVec, demReader,
                      demGeoRef, heightGuess, xyz, xyzGuess))
      return false;

    double h = 0.0;
    xyzToLonLat(xyz, demGeoRef.semi_major_axis(),
                demGeoRef.semi_minor_axis(), lon, lat, h);
    return true;
  } catch (...) {
    return false;
  }
}

// Bresenham line matching VW's BresenhamLine class exactly.
// Walks from (x0,y0) toward (x1,y1). The endpoint (x1,y1) is exclusive
// (the last valid point is before it). Outputs every 'step' points.
void bresenhamSample(int x0, int y0, int x1, int y1, int step,
                     std::vector<double> &samplesX,
                     std::vector<double> &samplesY) {
  // VW's BresenhamLine::setup(): steep-aware, swap if needed
  bool steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) { std::swap(x0, y0); std::swap(x1, y1); }
  if (x0 > x1) { std::swap(x0, x1); std::swap(y0, y1); }
  int deltaX = x1 - x0;
  int deltaY = abs(y1 - y0);
  int error = deltaX / 2;
  int yStep = (y0 < y1) ? 1 : -1;

  // Walk the line, sampling every 'step' pixels
  int x = x0, y = y0;
  int count = 0;
  while (x < x1) { // is_good(): x < x1
    if (count % step == 0) {
      if (steep)
        samplesX.push_back((double)y), samplesY.push_back((double)x);
      else
        samplesX.push_back((double)x), samplesY.push_back((double)y);
    }
    // Move along the line
    x++;
    error -= deltaY;
    if (error < 0) { y += yStep; error += deltaX; }
    count++;
  }
}

// Sample image boundary using Bresenham lines, matching VW's
// sampleImageBoundary. Generates 1-based pixel coordinates for ISIS.
void sampleImageBoundary(int cols, int rows, int numSamples,
                         std::vector<double> &samplesX,
                         std::vector<double> &samplesY) {
  int step = (2 * cols + 2 * rows) / numSamples;
  step = std::min(step, cols / 4);
  step = std::min(step, rows / 4);
  step = std::max(step, 1);

  // Perimeter: 4 edges (matching VW's BresenhamLine endpoints)
  std::vector<double> sx0, sy0;
  bresenhamSample(0, 0, cols, 0, step, sx0, sy0); // top
  bresenhamSample(cols - 1, 0, cols - 1, rows, step, sx0, sy0); // right
  bresenhamSample(cols - 1, rows - 1, 0, rows - 1, step, sx0, sy0); // bottom
  bresenhamSample(0, rows - 1, 0, 0, step, sx0, sy0); // left

  // Diagonals (matching VW's X pattern)
  bresenhamSample(0, 0, cols - 1, rows - 1, step, sx0, sy0);
  bresenhamSample(0, rows - 1, cols - 1, 0, step, sx0, sy0);

  // Convert 0-based to 1-based for ISIS
  for (size_t i = 0; i < sx0.size(); i++) {
    samplesX.push_back(sx0[i] + 1.0);
    samplesY.push_back(sy0[i] + 1.0);
  }
}

// Collect valid DEM pixel coordinates on the boundary and diagonals.
// Check if a DEM pixel is valid (not NaN and not nodata).
bool isDemValid(RasterReader &demReader, int col, int row) {
  double v = demReader.pixelVal(col, row);
  return !IsSpecial(v);
}

// Matches VW's sample_points_on_dem: for each sampled column, walk
// inward from top and bottom to find the first valid pixel; same for
// rows (left/right); plus both diagonals.
void sampleDemPoints(RasterReader &demReader,
                     int demStep,
                     std::vector<int> &demPixCols,
                     std::vector<int> &demPixRows) {
  int demCols = demReader.cols();
  int demRows = demReader.rows();
  demPixCols.clear();
  demPixRows.clear();

  // Shorthand: only col and row vary, the rest are fixed for this DEM.
  auto isValid = [&](int col, int row) {
    return isDemValid(demReader, col, row);
  };

  // Top and bottom edge: walk each sampled column
  for (int col0 = 0; col0 < demCols + demStep; col0 += demStep) {
    int col = std::min(col0, demCols - 1);
    // Walk down from top
    for (int row = 0; row < demRows; row++)
      if (isValid(col, row)) {
        demPixCols.push_back(col);
        demPixRows.push_back(row);
        break;
      }
    // Walk up from bottom
    for (int row = demRows - 1; row >= 0; row--) {
      if (isValid(col, row)) {
        demPixCols.push_back(col);
        demPixRows.push_back(row);
        break;
      }
    }
  }

  // Left and right edge: walk each sampled row
  for (int row0 = 0; row0 < demRows + demStep; row0 += demStep) {
    int row = std::min(row0, demRows - 1);
    // Walk right from left
    for (int col = 0; col < demCols; col++)
      if (isValid(col, row)) {
        demPixCols.push_back(col);
        demPixRows.push_back(row);
        break;
      }
    // Walk left from right
    for (int col = demCols - 1; col >= 0; col--) {
      if (isValid(col, row)) {
        demPixCols.push_back(col);
        demPixRows.push_back(row);
        break;
      }
    }
  }

  // Both diagonals
  double diag = sqrt((double)demCols * demCols + (double)demRows * demRows);
  for (double val = 0; val < diag + demStep; val += demStep) {
    int col = std::min((int)round((val / diag) * demCols), demCols - 1);
    int row = std::min((int)round((val / diag) * demRows), demRows - 1);
    // Main diagonal
    if (isValid(col, row)) {
      demPixCols.push_back(col);
      demPixRows.push_back(row);
    }
    // Other diagonal
    int col2 = demCols - 1 - col;
    if (isValid(col2, row)) {
      demPixCols.push_back(col2);
      demPixRows.push_back(row);
    }
  }
}

// Compute camera bounding box in target projection coordinates.
// Also collects sampled pixels that hit the DEM (for GSD computation).
// bbox is {minX, minY, maxX, maxY} in projected coords.
// Map from pixel (sample, line) to ECEF xyz. Matches VW's pix2xyz.
typedef std::map<std::pair<double, double>, std::array<double, 3>> Pix2Xyz;

// Process image pixel samples via ray-DEM intersection. Grows bbox,
// collects valid pixels and their ECEF xyz for GSD computation.
void processSamples(const std::vector<double> &sx,
                    const std::vector<double> &sy,
                    Camera *cam,
                    RasterReader &demReader,
                    const GeoRef &demGeoRef,
                    const GeoRef &targetGeoRef,
                    double heightGuess,
                    // Outputs
                    double &minX, double &minY,
                    double &maxX, double &maxY,
                    std::vector<double> &validSamplesX,
                    std::vector<double> &validSamplesY,
                    Pix2Xyz &pix2xyz) {
  for (size_t i = 0; i < sx.size(); i++) {
    double lon = 0.0, lat = 0.0;
    double xyz[3] = {0, 0, 0};
    if (!pixToDemXyz(cam, sx[i], sy[i], demReader,
                     demGeoRef, heightGuess, lon, lat, xyz))
      continue;
    double px = 0.0, py = 0.0;
    try {
      targetGeoRef.lonlat_to_point(lon, lat, px, py);
    } catch (...) {
      continue;
    }
    if (px < minX) minX = px;
    if (py < minY) minY = py;
    if (px > maxX) maxX = px;
    if (py > maxY) maxY = py;
    validSamplesX.push_back(sx[i]);
    validSamplesY.push_back(sy[i]);
    pix2xyz[std::make_pair(sx[i], sy[i])] = {xyz[0], xyz[1], xyz[2]};
  }
}

void computeCameraBbox(Camera *cam,
                       RasterReader &demReader,
                       const GeoRef &demGeoRef,
                       const GeoRef &targetGeoRef,
                       int imgCols, int imgRows,
                       double heightGuess,
                       // Outputs
                       double &minX, double &minY,
                       double &maxX, double &maxY,
                       std::vector<double> &validSamplesX,
                       std::vector<double> &validSamplesY,
                       Pix2Xyz &pix2xyz) {
  minX = std::numeric_limits<double>::max();
  minY = minX;
  maxX = -minX;
  maxY = -minY;

  // Use same number of samples as in VW
  int numSamples = 1000;

  // Passes 1+2: Image boundary (perimeter + diagonals) using Bresenham
  // lines, matching VW's sampleImageBoundary.
  std::vector<double> boundaryX, boundaryY;
  sampleImageBoundary(imgCols, imgRows, numSamples, boundaryX, boundaryY);
  processSamples(boundaryX, boundaryY, cam, demReader,
                 demGeoRef, targetGeoRef, heightGuess,
                 minX, minY, maxX, maxY, validSamplesX, validSamplesY,
                 pix2xyz);

  // Pass 3: DEM boundary -> project into camera -> grow bbox.
  // Matches VW's sampleDemBoundary: nearest-neighbor DEM lookup,
  // walk inward to find valid-data boundary.
  int demCols = demReader.cols();
  int demRows = demReader.rows();
  int demStep = (2 * demCols + 2 * demRows) / numSamples;
  demStep = std::min(demStep, demCols / 4);
  demStep = std::min(demStep, demRows / 4);
  demStep = std::max(demStep, 1);

  std::vector<int> demPixCols, demPixRows;
  sampleDemPoints(demReader, demStep, demPixCols, demPixRows);

  for (size_t i = 0; i < demPixCols.size(); i++) {
    int dc = demPixCols[i];
    int dr = demPixRows[i];

    // Read DEM height directly (nearest neighbor, matching VW)
    double h = demReader.pixelVal(dc, dr);
    if (IsSpecial(h))
      continue;

    // DEM pixel -> lon/lat (0-based pixel coordinates)
    double lon = 0.0, lat = 0.0;
    try {
      demGeoRef.pixel_to_lonlat((double)dc, (double)dr, lon, lat);
    } catch (...) {
      continue;
    }

    // lon/lat -> projected point in target CRS
    double px = 0.0, py = 0.0;
    try {
      targetGeoRef.lonlat_to_point(lon, lat, px, py);
    } catch (...) {
      continue;
    }

    // Geodetic to ECEF
    double ex = 0.0, ey = 0.0, ez = 0.0;
    demGeoRef.geodetic_to_cartesian(lon, lat, (double)h, ex, ey, ez);
    double rad = sqrt(ex * ex + ey * ey + ez * ez);

    // Project into camera via ISIS SetGround
    try {
      SurfacePoint surfPt(Latitude(lat, Angle::Degrees),
                          Longitude(lon, Angle::Degrees),
                          Distance(rad, Distance::Meters));
      if (!cam->SetGround(surfPt))
        continue;
    } catch (...) {
      continue;
    }

    // ISIS Sample/Line are 1-based; convert to 0-based for bounds check
    double samp0 = cam->Sample() - 1.0;
    double line0 = cam->Line() - 1.0;
    if (samp0 < 0 || line0 < 0 ||
        samp0 > imgCols - 1 || line0 > imgRows - 1)
      continue;

    // Grow bbox
    if (px < minX) minX = px;
    if (py < minY) minY = py;
    if (px > maxX) maxX = px;
    if (py > maxY) maxY = py;

    // Record 1-based pixel and ECEF xyz for GSD computation
    double s1 = cam->Sample();
    double l1 = cam->Line();
    validSamplesX.push_back(s1);
    validSamplesY.push_back(l1);
    pix2xyz[std::make_pair(s1, l1)] = {ex, ey, ez};
  }
}

// Compute mean GSD by measuring pixel spacing on the ground. Uses sampled
// pixels from computeCameraBbox. Trims 10% outliers. demGeoRef is for input
// DEM, while targetGeoRef is for output projection.
double computeMeanGsd(Camera *cam,
                      const std::vector<double> &validSamplesX,
                      const std::vector<double> &validSamplesY,
                      const Pix2Xyz &pix2xyz,
                      RasterReader &demReader,
                      const GeoRef &demGeoRef,
                      const GeoRef &targetGeoRef,
                      int imgCols, int imgRows,
                      double heightGuess) {
  std::vector<double> gsdVals;

  for (size_t i = 0; i < validSamplesX.size(); i++) {
    // Cast to int to match VW's Vector2i truncation in calcMeanGsd.
    // validSamplesX/Y are 1-based ISIS pixels; convert to 0-based first.
    int ctrSamp0 = (int)(validSamplesX[i] - 1.0);
    int ctrLine0 = (int)(validSamplesY[i] - 1.0);
    if (ctrSamp0 < 0 || ctrLine0 < 0 ||
        ctrSamp0 >= imgCols || ctrLine0 >= imgRows)
      continue;

    // Convert back to 1-based for ISIS SetImage
    double ctrSamp = ctrSamp0 + 1.0;
    double ctrLine = ctrLine0 + 1.0;

    // Look up ECEF guess by original (non-truncated) pixel, matching VW
    const double *guess = nullptr;
    double guessXyz[3] = {0, 0, 0};
    auto coord_pair = std::make_pair(validSamplesX[i], validSamplesY[i]);
    auto xyz_it = pix2xyz.find(coord_pair);
    if (xyz_it != pix2xyz.end()) {
      guessXyz[0] = xyz_it->second[0];
      guessXyz[1] = xyz_it->second[1];
      guessXyz[2] = xyz_it->second[2];
      guess = guessXyz;
    }

    // Intersect center pixel
    double ctrLon = 0.0, ctrLat = 0.0;
    double ctrXyz[3] = {0, 0, 0};
    if (!pixToDemXyz(cam, ctrSamp, ctrLine, demReader,
                     demGeoRef, heightGuess, ctrLon, ctrLat,
                     ctrXyz, guess))
      continue;

    double ctrPx = 0.0, ctrPy = 0.0;
    try {
      targetGeoRef.lonlat_to_point(ctrLon, ctrLat, ctrPx, ctrPy);
    } catch (...) {
      continue;
    }

    // 4 neighbors: +1 sample, +1 line, -1 sample, -1 line (0-based)
    int offsets[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    for (int j = 0; j < 4; j++) {
      int nSamp0 = ctrSamp0 + offsets[j][0];
      int nLine0 = ctrLine0 + offsets[j][1];
      if (nSamp0 < 0 || nLine0 < 0 ||
          nSamp0 >= imgCols || nLine0 >= imgRows)
        continue;

      // 1-based for ISIS SetImage
      double nSamp = nSamp0 + 1.0;
      double nLine = nLine0 + 1.0;

      double nLon = 0.0, nLat = 0.0;
      double nXyz[3] = {0, 0, 0};
      if (!pixToDemXyz(cam, nSamp, nLine, demReader,
                       demGeoRef, heightGuess, nLon, nLat,
                       nXyz, guess))
        continue;

      double nPx = 0.0, nPy = 0.0;
      try {
        targetGeoRef.lonlat_to_point(nLon, nLat, nPx, nPy);
      } catch (...) {
        continue;
      }

      double dist = sqrt((nPx - ctrPx) * (nPx - ctrPx) +
                         (nPy - ctrPy) * (nPy - ctrPy));
      if (dist > 0.0)
        gsdVals.push_back(dist);
    }
  }

  if (gsdVals.empty())
    return 0.0;

  // Sort and trim 10% from each end
  std::sort(gsdVals.begin(), gsdVals.end());
  int len = (int)gsdVals.size();
  int beg = (int)(0.1 * len);
  int end = (int)(0.9 * len);
  if (beg >= end) {
    beg = 0;
    end = len;
  }

  double sum = 0.0;
  int count = 0;
  for (int i = beg; i < end; i++) {
    sum += gsdVals[i];
    count++;
  }
  if (count == 0)
    return 0.0;
  double meanGsd = sum / count;
  return meanGsd;
}

// Compute camera footprint bbox (in projected coords) and GSD from DEM.
// demGeoRef is for reading DEM heights, targetGeoRef is for output projection.
void computeAutoGrid(Camera *cam,
                     const GeoRef &demGeoRef,
                     const GeoRef &targetGeoRef,
                     RasterReader &demReader,
                     double &bboxMinX, double &bboxMinY,
                     double &bboxMaxX, double &bboxMaxY,
                     double &gsd) {
  int imgCols = cam->Samples();
  int imgRows = cam->Lines();
  double heightGuess = demHeightGuess(demReader);

  // Compute camera bbox in projected coords
  std::vector<double> validSX, validSY;
  Pix2Xyz pix2xyz;
  computeCameraBbox(cam, demReader, demGeoRef, targetGeoRef,
                    imgCols, imgRows,
                    heightGuess,
                    bboxMinX, bboxMinY, bboxMaxX, bboxMaxY,
                    validSX, validSY, pix2xyz);

  // Compute GSD in projected coords
  gsd = computeMeanGsd(cam, validSX, validSY, pix2xyz,
                       demReader, demGeoRef, targetGeoRef,
                       imgCols, imgRows, heightGuess);
}

// Convert lat/lon bounds to a projected bounding box. Projects all four
// corners and accumulates min/max, since projections can be nonlinear.
void lonlatBboxToProj(double minLat, double maxLat,
                      double minLon, double maxLon,
                      const GeoRef &georef,
                      // Outputs
                      double &minX, double &minY,
                      double &maxX, double &maxY) {
  double lons[4] = {minLon, maxLon, minLon, maxLon};
  double lats[4] = {minLat, minLat, maxLat, maxLat};
  minX = std::numeric_limits<double>::max();
  minY = minX;
  maxX = -minX;
  maxY = -minX;
  for (int i = 0; i < 4; i++) {
    double px = 0.0, py = 0.0;
    georef.lonlat_to_point(lons[i], lats[i], px, py);
    if (px < minX) minX = px;
    if (py < minY) minY = py;
    if (px > maxX) maxX = px;
    if (py > maxY) maxY = py;
  }
}

// Convert pixels-per-degree to meters-per-pixel using the local radius
// at a given latitude. Matches ISIS ProjectionFactory formula:
//   mpp = 2*PI*localRadius / (360*ppd)
// For a sphere (a == c), localRadius = a regardless of latitude.
// For an ellipsoid, localRadius = a*c / sqrt((c*cos)^2 + (a*sin)^2).
// In practice, semiMajor == semiMinor here because pvlToWkt() and the
// GDAL ISIS3 driver bake the local radius at CenterLatitude into the
// ellipsoid as a sphere. The ellipsoid case below is kept for correctness
// if this function is ever called with true ellipsoid axes.
double ppdToMpp(double ppd, double semiMajor, double semiMinor,
                double trueScaleLatDeg) {
  double localRadius = semiMajor;
  if (fabs(semiMajor - semiMinor) > 1e-6) {
    double latRad = trueScaleLatDeg * M_PI / 180.0;
    localRadius = semiMajor * semiMinor /
      sqrt(pow(semiMinor * cos(latRad), 2) +
           pow(semiMajor * sin(latRad), 2));
  }
  return 2.0 * M_PI * localRadius / (360.0 * ppd);
}

// Override GSD if user specified pixres=MPP or pixres=PPD with a resolution.
// For projected CRS, GSD is in meters/pixel:
//   MPP -> use as-is, PPD -> convert to meters/pixel.
// For geographic CRS, GSD is in degrees/pixel:
//   PPD -> 1/ppd, MPP -> convert meters to degrees.
void handleMppPpd(const UserInterface &ui,
                  const GeoRef &targetGeoRef,
                  double &gsd) {
  if (!ui.WasEntered("RESOLUTION"))
    return;

  double a = targetGeoRef.semi_major_axis();
  double b = targetGeoRef.semi_minor_axis();
  QString pixres = ui.GetString("PIXRES");
  double res = ui.GetDouble("RESOLUTION");

  if (targetGeoRef.isGeographic()) {
    // Geographic CRS is currently rejected (not supported for .cub output),
    // so this branch is not reached. The conversion is approximate.
    double metersPerDeg = M_PI * a / 180.0;
    if (pixres == "PPD")
      gsd = 1.0 / res;
    else if (pixres == "MPP")
      gsd = res / metersPerDeg;
  } else {
    if (pixres == "MPP")
      gsd = res;
    else if (pixres == "PPD")
      gsd = ppdToMpp(res, a, b, 0.0);
  }
}

// Read pixel-edge bounds from a GDAL-readable image. Computes pixel-edge
// extent from GeoTransform + raster dimensions.
// Returns true on success, false if the file can't be opened by GDAL.
bool readPixelEdgeBoundsFromImage(const std::string &mapFile,
                                  double &minX, double &minY,
                                  double &maxX, double &maxY) {
  GDALAllRegister();
  GDALDataset *ds = (GDALDataset *)GDALOpen(mapFile.c_str(), GA_ReadOnly);
  if (!ds)
    return false;
  double mgt[6] = {};
  if (ds->GetGeoTransform(mgt) != CE_None) {
    GDALClose(ds);
    return false;
  }
  int mCols = ds->GetRasterXSize();
  int mRows = ds->GetRasterYSize();
  minX = mgt[0];
  maxY = mgt[3];
  maxX = minX + mCols * mgt[1];
  minY = maxY + mRows * mgt[5];
  GDALClose(ds);
  return true;
}

// Read pixel-edge bounds from a PVL map file. Converts lon/lat pixel-edge
// bounds to projected coords.
// Returns true if bounds keywords were found, false otherwise.
bool readPixelEdgeBoundsFromPvl(const std::string &mapFile,
                                const GeoRef &targetGeoRef,
                                double &minX, double &minY,
                                double &maxX, double &maxY) {
  Pvl mapPvl;
  mapPvl.read(QString::fromStdString(mapFile));
  PvlGroup &mg = mapPvl.findGroup("Mapping", Pvl::Traverse);
  if (!mg.hasKeyword("MinimumLatitude") ||
      !mg.hasKeyword("MaximumLatitude") ||
      !mg.hasKeyword("MinimumLongitude") ||
      !mg.hasKeyword("MaximumLongitude"))
    return false;
  double mapMinLat = toDouble(mg["MinimumLatitude"][0]);
  double mapMaxLat = toDouble(mg["MaximumLatitude"][0]);
  double mapMinLon = toDouble(mg["MinimumLongitude"][0]);
  double mapMaxLon = toDouble(mg["MaximumLongitude"][0]);
  lonlatBboxToProj(mapMinLat, mapMaxLat, mapMinLon, mapMaxLon,
                   targetGeoRef, minX, minY, maxX, maxY);
  return true;
}

// Override bounding box with user-specified lat/lon bounds. Converts any
// specified MINLAT/MAXLAT/MINLON/MAXLON to projected coords and overrides
// the corresponding bbox edges. Unspecified bounds keep the auto-computed values.
void handleLonLatBounds(const UserInterface &ui,
                        const GeoRef &targetGeoRef,
                        double &bboxMinX, double &bboxMinY,
                        double &bboxMaxX, double &bboxMaxY) {
  if (!ui.WasEntered("MINLAT") && !ui.WasEntered("MAXLAT") &&
      !ui.WasEntered("MINLON") && !ui.WasEntered("MAXLON"))
    return;
  // Start from auto-computed bbox corners in lon/lat
  double oMinLon = 0.0, oMinLat = 0.0, oMaxLon = 0.0, oMaxLat = 0.0;
  targetGeoRef.point_to_lonlat(bboxMinX, bboxMinY, oMinLon, oMinLat);
  targetGeoRef.point_to_lonlat(bboxMaxX, bboxMaxY, oMaxLon, oMaxLat);
  // Override individual bounds if specified
  if (ui.WasEntered("MINLAT")) oMinLat = ui.GetDouble("MINLAT");
  if (ui.WasEntered("MAXLAT")) oMaxLat = ui.GetDouble("MAXLAT");
  if (ui.WasEntered("MINLON")) oMinLon = ui.GetDouble("MINLON");
  if (ui.WasEntered("MAXLON")) oMaxLon = ui.GetDouble("MAXLON");
  lonlatBboxToProj(oMinLat, oMaxLat, oMinLon, oMaxLon, targetGeoRef,
                   bboxMinX, bboxMinY, bboxMaxX, bboxMaxY);
}

// Read GSD and/or pixel-edge bounds from the map= file.
// pixres=MAP reads GSD from PixelResolution or Scale in the PVL.
// defaultrange=MAP reads pixel-edge bounds from GDAL or PVL.
// Returns true if defaultrange=MAP was used and bounds were read.
// When true, edgeMinX/etc hold the map file's pixel-edge extent and
// the caller should compute image size directly (no grid snapping).
bool handleMapFile(const UserInterface &ui,
                   const GeoRef &targetGeoRef,
                   double &gsd,
                   double &edgeMinX, double &edgeMinY,
                   double &edgeMaxX, double &edgeMaxY) {
  if (!ui.WasEntered("MAP"))
    return false;

  // pixres=MAP: read GSD from map file. Try GDAL first (works for .tif,
  // .cub), fall back to PVL (for .map text files with PixelResolution/Scale).
  if (ui.GetString("PIXRES") == "MAP") {
    std::string mapFile = ui.GetFileName("MAP").toStdString();
    bool gdalGsd = false;
    GDALAllRegister();
    GDALDataset *ds = (GDALDataset *)GDALOpen(mapFile.c_str(), GA_ReadOnly);
    if (ds) {
      double gt[6] = {};
      if (ds->GetGeoTransform(gt) == CE_None) {
        gsd = gt[1];
        gdalGsd = true;
      }
      GDALClose(ds);
    }
    if (!gdalGsd) {
      // GDAL failed, try PVL
      Pvl mapPvl;
      mapPvl.read(ui.GetFileName("MAP"));
      PvlGroup &mg = mapPvl.findGroup("Mapping", Pvl::Traverse);
      if (mg.hasKeyword("PixelResolution"))
        gsd = toDouble(mg["PixelResolution"][0]);
      else if (mg.hasKeyword("Scale"))
        gsd = ppdToMpp(toDouble(mg["Scale"][0]),
                       targetGeoRef.semi_major_axis(),
                       targetGeoRef.semi_minor_axis(), 0.0);
    }
  }

  // defaultrange=MAP: read bounds from map file.
  // Try GDAL first (works for .cub, .tif): returns exact pixel-edge bounds,
  // so the caller can compute image size directly (no grid snapping needed).
  // PVL fallback: lon/lat to projected conversion is imprecise, so return
  // grid-center bounds for the caller to snap.
  if (ui.WasEntered("DEFAULTRANGE") &&
      ui.GetString("DEFAULTRANGE") == "MAP") {
    std::string mapFile = ui.GetFileName("MAP").toStdString();
    if (readPixelEdgeBoundsFromImage(mapFile,
                                     edgeMinX, edgeMinY,
                                     edgeMaxX, edgeMaxY))
      return true; // exact pixel-edge bounds, skip snap

    // PVL fallback: convert lon/lat to projected coords, then inset
    // by half pixel to get grid-center bounds for snapping
    double eMinX = 0.0, eMinY = 0.0, eMaxX = 0.0, eMaxY = 0.0;
    if (readPixelEdgeBoundsFromPvl(mapFile, targetGeoRef,
                                   eMinX, eMinY, eMaxX, eMaxY)) {
      double h = 0.5 * gsd;
      edgeMinX = eMinX + h;
      edgeMinY = eMinY + h;
      edgeMaxX = eMaxX - h;
      edgeMaxY = eMaxY - h;
    }
  }

  return false;
}

// Snap bounding box to integer multiples of GSD and compute the output
// image origin (first pixel center) and dimensions.
// Input bbox values are projected coordinates to cover. After snapping
// outward to GSD multiples, the origin is the first pixel center
// (minX + 0.5*gsd), matching ASP mapproject's PixelAsArea convention.
void snapGridCalcImageSize(double gsd,
                           double bboxMinX, double bboxMinY,
                           double bboxMaxX, double bboxMaxY,
                           double &snapUlx, double &snapUly,
                           int &outSamples, int &outLines) {
  double minX = gsd * floor(bboxMinX / gsd);
  double maxX = gsd * ceil(bboxMaxX / gsd);
  double minY = gsd * floor(bboxMinY / gsd);
  double maxY = gsd * ceil(bboxMaxY / gsd);
  double delta = 0.5 * gsd;
  snapUlx = minX - delta;
  snapUly = maxY + delta;
  // Both min and max are pixel centers at grid multiples, so +1 to include both endpoints.
  outSamples = (int)round((maxX - minX) / gsd) + 1;
  outLines   = (int)round((maxY - minY) / gsd) + 1;
}

// Build the Mapping PVL group for the output cube from camera target info
// and the computed output grid parameters.
PvlGroup buildMappingGroup(Camera *cam,
                           const GeoRef &targetGeoRef,
                           double gsd,
                           double snapUlx, double snapUly) {
  PvlGroup mapGrp("Mapping");
  mapGrp += PvlKeyword("TargetName", cam->target()->name());
  std::vector<Distance> radii = cam->target()->radii();
  mapGrp += PvlKeyword("EquatorialRadius",
                        toString(radii[0].meters()), "meters");
  mapGrp += PvlKeyword("PolarRadius",
                        toString(radii[2].meters()), "meters");
  mapGrp += PvlKeyword("LatitudeType", "Planetocentric");
  mapGrp += PvlKeyword("LongitudeDirection", "PositiveEast");
  mapGrp += PvlKeyword("LongitudeDomain", "360");
  mapGrp += PvlKeyword("ProjectionName",
                        QString::fromStdString(targetGeoRef.isisProjectionName()));
  mapGrp += PvlKeyword("CenterLongitude",
                        toString(targetGeoRef.centerLongitude()));
  mapGrp += PvlKeyword("CenterLatitude",
                        toString(targetGeoRef.centerLatitude()));
  mapGrp += PvlKeyword("CenterLatitudeRadius",
                        toString(radii[0].meters()));
  // Units depend on CRS: meters for projected, degrees for geographic
  QString resUnit = targetGeoRef.isGeographic() ? "degrees/pixel"
                                                : "meters/pixel";
  QString posUnit = targetGeoRef.isGeographic() ? "degrees" : "meters";
  mapGrp += PvlKeyword("PixelResolution", toString(gsd), resUnit);
  mapGrp += PvlKeyword("UpperLeftCornerX", toString(snapUlx), posUnit);
  mapGrp += PvlKeyword("UpperLeftCornerY", toString(snapUly), posUnit);
  return mapGrp;
}

// Build the AspMapproject PVL group with ASP-compatible metadata for the
// output cube, so ASP stereo can consume the mapprojected .cub file.
PvlGroup buildAspGroup(const std::string &inputFile,
                       const std::string &demFile,
                       const std::string &isdFile) {
  PvlGroup aspGrp("AspMapproject");
  aspGrp += PvlKeyword("BUNDLE_ADJUST_PREFIX", "NONE");
  bool useCsm = !isdFile.empty();
  aspGrp += PvlKeyword("CAMERA_MODEL_TYPE", useCsm ? "csm" : "isis");
  aspGrp += PvlKeyword("INPUT_IMAGE_FILE",
                       QString::fromStdString(inputFile));
  aspGrp += PvlKeyword("CAMERA_FILE",
                       QString::fromStdString(useCsm ? isdFile : inputFile));
  aspGrp += PvlKeyword("DEM_FILE",
                       QString::fromStdString(demFile));
  return aspGrp;
}

// Read exact output grid from a GDAL-readable georeferenced image file (.cub,
// .tif). Returns true on success, false if the file can't be opened by GDAL.
bool readGridFromImage(const std::string &mapFile,
                       double &ulx, double &uly, double &gsd,
                       int &samples, int &lines) {
  GDALAllRegister();
  GDALDataset *ds = (GDALDataset *)GDALOpen(mapFile.c_str(), GA_ReadOnly);
  if (!ds)
    return false; // GDAL cannot read it
    
  double gt[6] = {};
  if (ds->GetGeoTransform(gt) != CE_None) {
    GDALClose(ds);
    throw IException(IException::User,
      "Map file has no GeoTransform: " +
      QString::fromStdString(mapFile), _FILEINFO_);
  }
  gsd = gt[1];
  ulx = gt[0];
  uly = gt[3];
  samples = ds->GetRasterXSize();
  lines = ds->GetRasterYSize();
  GDALClose(ds);
  return true;
}

// Read exact output grid from a PVL .map file. Requires UpperLeftCornerX/Y,
// PixelResolution or Scale, and Samples/Lines. Throws if missing.
void readGridFromPvl(const std::string &mapFile,
                     const GeoRef &targetGeoRef,
                     double &ulx, double &uly, double &gsd,
                     int &samples, int &lines) {
  Pvl mapPvl;
  mapPvl.read(QString::fromStdString(mapFile));
  PvlGroup &mg = mapPvl.findGroup("Mapping", Pvl::Traverse);
  if (!mg.hasKeyword("UpperLeftCornerX") ||
      !mg.hasKeyword("UpperLeftCornerY"))
    throw IException(IException::User,
      "MATCHMAP=true requires UpperLeftCornerX/Y in the map file. "
      "Use a .cub or .tif file, or add these keywords to the .map file.",
      _FILEINFO_);
  ulx = toDouble(mg["UpperLeftCornerX"][0]);
  uly = toDouble(mg["UpperLeftCornerY"][0]);
  if (mg.hasKeyword("PixelResolution"))
    gsd = toDouble(mg["PixelResolution"][0]);
  else if (mg.hasKeyword("Scale"))
    gsd = ppdToMpp(toDouble(mg["Scale"][0]),
                   targetGeoRef.semi_major_axis(),
                   targetGeoRef.semi_minor_axis(), 0.0);
  else
    throw IException(IException::User,
      "MATCHMAP=true requires PixelResolution or Scale in the map file.",
      _FILEINFO_);
  if (!mg.hasKeyword("Samples") || !mg.hasKeyword("Lines"))
    throw IException(IException::User,
      "MATCHMAP=true requires Samples and Lines keywords in the map "
      "file. Use a .cub or .tif file instead.", _FILEINFO_);
  samples = toInt(mg["Samples"][0]);
  lines = toInt(mg["Lines"][0]);
}

// Build a GeoRef from a map= file. Tries GDAL first (works for .cub and
// .tif), then falls back to PVL parsing for .map text files.
// defaultSemiMajor/Minor are used only for .map files missing radii.
GeoRef geoRefFromMapFile(const std::string &mapFile,
                         double defaultSemiMajor,
                         double defaultSemiMinor) {

  // Try GDAL first (handles .cub, .tif, and other raster formats)
  GDALAllRegister();
  GDALDataset *ds = (GDALDataset *)GDALOpen(mapFile.c_str(), GA_ReadOnly);
  if (ds) {
    GDALClose(ds);
    return GeoRef(mapFile);
  }

  // Fall back to PVL parsing (for .map text files)
  Pvl mapPvl;
  mapPvl.read(QString::fromStdString(mapFile));
  PvlGroup &mapGrp = mapPvl.findGroup("Mapping", Pvl::Traverse);

  std::string wkt = pvlToWkt(mapGrp, defaultSemiMajor, defaultSemiMinor);
  bool fromString = true;
  return GeoRef(wkt, fromString);
}

// Validate user options for ASP_MAP mode. Check mutual exclusions and
// required parameters before any heavy computation.
void validateUi(const UserInterface &ui) {

  if (!ui.WasEntered("DEM")) {
    QString msg = "A DEM must be provided when ASP_MAP is true.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // ASP_MAP auto-computes bounds from camera footprint in projected coords.
  // DEFAULTRANGE=CAMERA unlocks MINLAT/MAXLAT/MINLON/MAXLON overrides.
  // DEFAULTRANGE=MAP takes bounds from the map= file.
  // Other values (MINIMIZE, GROUND) are silently ignored - ASP_MAP
  // auto-computes bounds regardless.
  if (ui.WasEntered("DEFAULTRANGE") &&
      ui.GetString("DEFAULTRANGE") == "MAP" && !ui.WasEntered("MAP")) {
    QString msg = "DEFAULTRANGE=MAP requires a MAP file.";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  if (ui.WasEntered("LONSEAM")) {
    QString msg = "LONSEAM is not supported in ASP_MAP mode. "
      "The projection handles coordinate wrapping.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // MATCHMAP and USEPROJ are mutually exclusive (USEPROJ defines a different
  // projection, contradicting matching the map file's exact grid).
  // MAP and USEPROJ are also mutually exclusive.
  // The XML exclusions enforce this in the GUI, but check here too.
  bool useProj = ui.WasEntered("USEPROJ") && ui.GetBoolean("USEPROJ");
  bool matchMap = ui.WasEntered("MATCHMAP") && ui.GetBoolean("MATCHMAP");
  if (matchMap && useProj) {
    QString msg = "MATCHMAP and USEPROJ cannot both be true.";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  if (ui.WasEntered("MAP") && useProj) {
    QString msg = "MAP and USEPROJ cannot both be specified.";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  if (matchMap && !ui.WasEntered("MAP")) {
    QString msg = "MATCHMAP=true requires a MAP file.";
    throw IException(IException::User, msg, _FILEINFO_);
  }
}

// Project a single output pixel to input image coordinates via the camera model.
// outSamp/outLine: desired mapprojected image pixel (1-based) to mapproject at.
// outCol/outRow: resulting input raw camera image pixel (0-based), or NaN on failure.
void projectPixel(Camera *cam,
                  const GeoRef &targetGeoRef,
                  const GeoRef &demGeoRef,
                  RasterReader &demReader,
                  int outSamp, int outLine,
                  int imgCols, int imgRows,
                  double &outCol, double &outRow) {
  const double nan = std::numeric_limits<double>::quiet_NaN();
  outCol = nan;
  outRow = nan;

  // Output pixel -> lon/lat via targetGeoRef (pixel -> proj -> lonlat)
  double lon = 0.0, lat = 0.0;
  try {
    targetGeoRef.pixel_to_lonlat(outSamp - 1, outLine - 1, lon, lat);
  } catch (...) {
    return;
  }

  // Lon/lat -> DEM pixel via GDAL
  double demCol = 0.0, demRow = 0.0;
  demGeoRef.lonlat_to_pixel(lon, lat, demCol, demRow);

  // Bicubic DEM interpolation (VW kernel, Portal-based)
  double height = demReader.bicubicVal(demCol, demRow);
  if (std::isnan(height))
    return;

  // Geodetic to ECEF
  double ex = 0.0, ey = 0.0, ez = 0.0;
  demGeoRef.geodetic_to_cartesian(lon, lat, height, ex, ey, ez);

  // Project through ISIS camera model
  double radius = sqrt(ex * ex + ey * ey + ez * ez);
  SurfacePoint surfPt(Latitude(lat, Angle::Degrees),
                      Longitude(lon, Angle::Degrees),
                      Distance(radius, Distance::Meters));

  try {
    if (!cam->SetGround(surfPt))
      return;
  } catch (...) {
    return;
  }

  // ISIS returns 1-based pixels; convert to 0-based for image lookup
  double col0 = cam->Sample() - 1.0;
  double row0 = cam->Line() - 1.0;

  // Check input image bounds (0-based)
  if (col0 < 0.0 || row0 < 0.0 || col0 >= imgCols || row0 >= imgRows)
    return;

  outCol = col0;
  outRow = row0;
}

// Render the mapprojected image by iterating over output pixels, projecting
// each through the camera model to find the corresponding input pixel, and
// writing the result to the output cube. For band-independent cameras,
// projection is computed once per pixel and reused for all bands.
void renderMapprojectedImage(Camera *cam,
                             const GeoRef &targetGeoRef,
                             const GeoRef &demGeoRef,
                             RasterReader &demReader,
                             const std::string &inputFile,
                             int outSamples, int outLines,
                             Cube &outCube, int numBands) {

  // Portal-based image reader (tiled, no full memory load)
  RasterReader imgReader(inputFile);
  int imgCols = imgReader.cols();
  int imgRows = imgReader.rows();

  // For some multi-band cameras, the camera model changes per band (e.g.,
  // THEMIS IR, CRISM). Need to set the camera and reproject per band in that
  // case.
  bool bandDependentCam = !cam->IsBandIndependent();

  // Cache input image coords for one line (avoids re-projection per band)
  const double nan = std::numeric_limits<double>::quiet_NaN();
  std::vector<double> cachedCol(outSamples, nan);
  std::vector<double> cachedRow(outSamples, nan);

  // Progress indicator. Use ISIS Progress if the user has ProgressBar = On
  // in their preferences (feeds the ISIS GUI too). Otherwise, use our own
  // simple console printout so progress is always visible.
  PvlGroup &uiGroup =
    Preference::Preferences().findGroup("UserInterface");
  QString pbar = (QString) uiGroup["ProgressBar"];
  bool useIsisProgress = (pbar.toUpper() == "ON");

  Progress isisProgress;
  int nextPercent = 0;
  int percentStep = 5;

  if (useIsisProgress) {
    isisProgress.SetText("Projecting");
    isisProgress.SetMaximumSteps(outLines);
    isisProgress.CheckStatus();
  }

  LineManager outManager(outCube);
  for (int out_line = 1; out_line <= outLines; out_line++) {

    // Iterate over bands for this line
    for (int band = 1; band <= numBands; band++) {

      // Project the row. For band-dependent cameras, re-project per band
      // because SetBand changes the camera model. For band-independent cameras,
      // or if only one band, project only once (band 1) and reuse the cached
      // coords. Results go into cachedCol/cachedRow.
      cam->SetBand(band);
      if (bandDependentCam || band == 1)
        for (int out_samp = 1; out_samp <= outSamples; out_samp++)
          projectPixel(cam, targetGeoRef, demGeoRef, demReader,
                       out_samp, out_line, imgCols, imgRows,
                       cachedCol[out_samp - 1], cachedRow[out_samp - 1]);

      // Do interpolation in the image and write the line for this band
      outManager.SetLine(out_line, band);
      for (int out_samp = 1; out_samp <= outSamples; out_samp++) {
        outManager[out_samp - 1] = Null; // default to Null if projection failed
        if (!std::isnan(cachedCol[out_samp - 1])) {
          double val = imgReader.bicubicVal(cachedCol[out_samp - 1],
                                            cachedRow[out_samp - 1], band);
          if (!std::isnan(val))
            outManager[out_samp - 1] = val;
        }
      }
      outCube.write(outManager);
    }
    
    // Update progress
    if (useIsisProgress) {
      isisProgress.CheckStatus();
    } else {
      int percent = (int)(100.0 * out_line / outLines);
      if (percent >= nextPercent && percent < 100) {
        std::cout << "\r" << percent << "%" << std::flush;
        nextPercent = percent + percentStep;
      }
    }
  }
  if (!useIsisProgress)
    std::cout << "\r100%" << std::endl;

  outCube.close();
}

// Load a CSM camera model from an ISD file and set it on the cube,
// so that cube->camera() returns a CSMCamera. Uses the CSMCamera
// constructor that takes plugin/model/state strings directly,
// avoiding the blob serialize/deserialize round-trip.
void loadCsmCamera(const QString &isdFile, Cube *cube) {
  CameraFactory::initPlugin();
  QStringList spec = CameraFactory::getModelSpecFromIsd(isdFile);
  csm::Model *model = CameraFactory::constructModelFromIsd(
      isdFile, spec[0], spec[1], spec[2]);
  std::string stateStr = model->getModelState();
  delete model;

  Camera *cam = new CSMCamera(*cube, spec[0], spec[1],
                               QString::fromStdString(stateStr));
  cube->setCamera(cam);
}

void mapproject(Cube *inCube, const UserInterface &ui) {

  std::cout << "Running ASP-compatible map projection\n";
  validateUi(ui);
  std::cout << "Writing: " << ui.GetCubeName("TO").toStdString() << "\n";

  if (ui.WasEntered("ISD"))
    loadCsmCamera(ui.GetFileName("ISD"), inCube);

  Camera *cam = inCube->camera();
  std::string demFile = ui.GetFileName("DEM").toStdString();
  std::string inputFile = ui.GetCubeName("FROM").toStdString();

  // Portal-based DEM reader (tiled, no full memory load)
  RasterReader demReader(demFile);

  // Create georef from DEM GeoTIFF
  GeoRef demGeoRef(demFile);

  // The target georef comes from the input DEM, map file, or PROJ string
  GeoRef targetGeoRef = demGeoRef;
  if (ui.WasEntered("MAP")) {
    std::string mapFile = ui.GetFileName("MAP").toStdString();
    targetGeoRef = geoRefFromMapFile(mapFile,
                                     demGeoRef.semi_major_axis(),
                                     demGeoRef.semi_minor_axis());
  } else if (ui.WasEntered("USEPROJ") && ui.GetBoolean("USEPROJ")) {
    std::string projStr = ui.GetAsString("PROJString").toStdString();
    if (!projStr.empty())
      targetGeoRef = GeoRef(projStr, true);
  }

  // ISIS does not support geographic (lon/lat) CRS in .cub files.
  if (targetGeoRef.isGeographic()) {
    QString msg = "Geographic (lon/lat) CRS is not supported for .cub output. "
      "ISIS requires a projected CRS (e.g., Equirectangular). "
      "Use projstring=\"+proj=eqc ...\" instead of \"+proj=longlat ...\".";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Compute output grid: origin (upper-left), GSD, and dimensions.
  // matchmap=true reads the exact grid from the map file (no snapping).
  // Otherwise, auto-compute from camera footprint + DEM, with overrides.
  double snapUlx = 0.0, snapUly = 0.0, gsd = 0.0;
  int outSamples = 0, outLines = 0;
  bool matchMap = ui.WasEntered("MATCHMAP") && ui.GetBoolean("MATCHMAP");

  if (matchMap) {
    // matchmap=true: read the exact grid from the map file (no snapping).
    // Try GDAL first (.cub, .tif), fall back to PVL (.map text files).
    std::string mapFile = ui.GetFileName("MAP").toStdString();
    bool success = readGridFromImage(mapFile, snapUlx, snapUly, gsd,
                                     outSamples, outLines);
    if (!success)
      readGridFromPvl(mapFile, targetGeoRef, snapUlx, snapUly, gsd,
                      outSamples, outLines);
    std::cout << "matchmap=true: using exact grid from map file\n";
  } else {
    // Auto-compute grid from camera footprint and DEM
    double bboxMinX = 0.0, bboxMinY = 0.0, bboxMaxX = 0.0, bboxMaxY = 0.0;
    gsd = 0.0;
    computeAutoGrid(cam, demGeoRef, targetGeoRef, demReader,
                    bboxMinX, bboxMinY, bboxMaxX, bboxMaxY, gsd);

    // Read GSD and/or pixel-edge bounds from map= file.
    // When defaultrange=MAP, boundsFromMap is true and bboxMin/Max hold
    // pixel-edge bounds (used directly, no grid snapping needed).
    bool boundsFromMap = handleMapFile(ui, targetGeoRef, gsd,
                                      bboxMinX, bboxMinY,
                                      bboxMaxX, bboxMaxY);

    // Override GSD if user specified resolution
    handleMppPpd(ui, targetGeoRef, gsd);

    // Apply user overrides for bounds (minlat/maxlat/minlon/maxlon).
    // Mutually exclusive with defaultrange=MAP (XML enforces this).
    bool userBounds = ui.WasEntered("MINLAT") || ui.WasEntered("MAXLAT") ||
                      ui.WasEntered("MINLON") || ui.WasEntered("MAXLON");
    handleLonLatBounds(ui, targetGeoRef,
                       bboxMinX, bboxMinY, bboxMaxX, bboxMaxY);

    // User lon/lat bounds specify geographic extent (pixel edges).
    // Inset by half a pixel to get grid centers before snapping,
    // matching ASP mapproject's --t_projwin handling.
    if (userBounds) {
      double half = 0.5 * gsd;
      bboxMinX += half;
      bboxMaxX -= half;
      bboxMinY += half;
      bboxMaxY -= half;
    }

    if (boundsFromMap) {
      // defaultrange=MAP: use the map file's pixel-edge extent directly.
      // No grid snapping - the map file defines the exact geographic area.
      snapUlx = bboxMinX;
      snapUly = bboxMaxY;
      outSamples = (int)round((bboxMaxX - bboxMinX) / gsd);
      outLines   = (int)round((bboxMaxY - bboxMinY) / gsd);
    } else {
      // Auto-computed bounds: snap grid centers to GSD multiples
      snapGridCalcImageSize(gsd, bboxMinX, bboxMinY, bboxMaxX, bboxMaxY,
                            snapUlx, snapUly, outSamples, outLines);
    }
  }
  // Update targetGeoRef to match the output grid
  double gt[6] = {snapUlx, gsd, 0.0, snapUly, 0.0, -gsd};
  targetGeoRef.setGeoTransform(gt);

  // Create output cube
  QString outFile = ui.GetCubeName("TO");
  // GetOutputAttribute is not const-correct in ISIS, so cast is needed
  CubeAttributeOutput outAttr =
    const_cast<UserInterface &>(ui).GetOutputAttribute("TO");
  Cube outCube;
  outCube.setDimensions(outSamples, outLines, inCube->bandCount());
  outCube.create(outFile, outAttr);

  // Build the Mapping PVL group for the output cube
  PvlGroup mapGrp = buildMappingGroup(cam, targetGeoRef, gsd,
                                      snapUlx, snapUly);
  outCube.putGroup(mapGrp);

  // Write ASP-compatible metadata
  std::string isdFile;
  if (ui.WasEntered("ISD"))
    isdFile = ui.GetFileName("ISD").toStdString();
  PvlGroup aspGrp = buildAspGroup(inputFile, demFile, isdFile);
  outCube.putGroup(aspGrp);

  // Render the mapprojected image
  renderMapprojectedImage(cam, targetGeoRef, demGeoRef,
                          demReader, inputFile, outSamples, outLines,
                          outCube, inCube->bandCount());
}

}} // end namespace Isis::asp
