/**
 * @file
 * $Revision$
 * $Date$
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include "Camera.h"

#include <cfloat>
#include <cmath>
#include <iomanip>

#include <QList>
#include <QPair>
#include <QTime>
#include <QVector>

#include "Angle.h"
#include "Constants.h"
#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "iException.h"
#include "iString.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifStatus.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "SurfacePoint.h"

using namespace std;

namespace Isis {
  /**
   * Constructs the Camera object
   *
   * @param lab Pvl label used to create the Camera object
   */
  Camera::Camera(Pvl &lab) : Sensor(lab) {
    // Get the image size which can be different than the alpha cube size

    PvlGroup &dims = lab.FindObject("IsisCube")
                           .FindObject("Core")
                           .FindGroup("Dimensions");
    p_lines = dims["Lines"];
    p_samples = dims["Samples"];
    p_bands = dims["Bands"];

    SetGeometricTilingHint();

    // Get the AlphaCube information
    p_alphaCube = new AlphaCube(lab);

    // Get the projection group if it exists
    if(lab.FindObject("IsisCube").HasGroup("Mapping")) {
      p_projection = ProjectionFactory::CreateFromCube(lab);
    }
    else {
      p_projection = NULL;
    }
    p_ignoreProjection = false;

    // Initialize stuff
    p_focalLength = 0.0;
    p_pixelPitch = 1.0;
    p_referenceBand = 0;
    p_childBand = 1;

    p_distortionMap = NULL;
    p_focalPlaneMap = NULL;
    p_detectorMap = NULL;
    p_groundMap = NULL;
    p_skyMap = NULL;

    // See if we have a reference band
    PvlGroup &inst = lab.FindObject("IsisCube").FindGroup("Instrument");
    if(inst.HasKeyword("ReferenceBand")) {
      p_referenceBand = inst["ReferenceBand"];
    }

    p_groundRangeComputed = false;
    p_raDecRangeComputed = false;
    p_pointComputed = false;
  }

  //! Destroys the Camera Object
  Camera::~Camera() {
    if(p_projection) {
      delete p_projection;
      p_projection = NULL;
    }

    if(p_alphaCube) {
      delete p_alphaCube;
      p_alphaCube = NULL;
    }

    if(p_distortionMap) {
      delete p_distortionMap;
      p_distortionMap = NULL;
    }

    if(p_focalPlaneMap) {
      delete p_focalPlaneMap;
      p_focalPlaneMap = NULL;
    }

    if(p_detectorMap) {
      delete p_detectorMap;
      p_detectorMap = NULL;
    }

    if(p_groundMap) {
      delete p_groundMap;
      p_groundMap = NULL;
    }

    if(p_skyMap) {
      delete p_skyMap;
      p_skyMap = NULL;
    }
  }

  /**
   * Sets the sample/line values of the image to get the lat/lon values
   *
   * @param sample Sample coordinate of the cube
   * @param line Line coordinate of the cube
   *
   * @return @b bool Returns true if the image was set successfully and false if it
   *              was not
   */
  bool Camera::SetImage(const double sample, const double line) {
    p_childSample = sample;
    p_childLine = line;
    p_pointComputed = true;

    // Case of no map projection
    if(p_projection == NULL || p_ignoreProjection) {
      // Convert to parent coordinate (remove crop, pad, shrink, enlarge)
      double parentSample = p_alphaCube->AlphaSample(sample);
      double parentLine = p_alphaCube->AlphaLine(line);
      // Convert from parent to detector
      if(p_detectorMap->SetParent(parentSample, parentLine)) {
        double detectorSample = p_detectorMap->DetectorSample();
        double detectorLine   = p_detectorMap->DetectorLine();

        // Now Convert from detector to distorted focal plane
        if(p_focalPlaneMap->SetDetector(detectorSample, detectorLine)) {
          double focalPlaneX = p_focalPlaneMap->FocalPlaneX();
          double focalPlaneY = p_focalPlaneMap->FocalPlaneY();

          // Remove optical distortion
          if(p_distortionMap->SetFocalPlane(focalPlaneX, focalPlaneY)) {
            // Map to the ground
            double x = p_distortionMap->UndistortedFocalPlaneX();
            double y = p_distortionMap->UndistortedFocalPlaneY();
            double z = p_distortionMap->UndistortedFocalPlaneZ();

            p_hasIntersection = p_groundMap->SetFocalPlane(x, y, z);
            return p_hasIntersection;
          }
        }
      }
    }

    // The projection is a sky map
    else if(p_projection->IsSky()) {
      if(p_projection->SetWorld(sample, line)) {
        if(SetRightAscensionDeclination(p_projection->Longitude(),
                                        p_projection->UniversalLatitude())) {
          p_childSample = sample;
          p_childLine = line;

          return HasSurfaceIntersection();
        }
      }
    }

    // We have map projected camera model
    else {
      if(p_projection->SetWorld(sample, line)) {
        Latitude lat(p_projection->UniversalLatitude(), Angle::Degrees);
        Longitude lon(p_projection->UniversalLongitude(), Angle::Degrees);
        Distance rad(LocalRadius(lat, lon));
        SurfacePoint surfPt(lat, lon, rad);
        if(SetGround(surfPt)) {
          p_childSample = sample;
          p_childLine = line;

          p_hasIntersection = true;
          return p_hasIntersection;
        }
      }
    }

    // failure
    p_hasIntersection = false;
    return p_hasIntersection;
  }

  /**
   * Sets the lat/lon values to get the sample/line values
   *
   * @param latitude Latitude coordinate of the point
   * @param longitude Longitude coordinate of the point
   *
   * @return @b bool Returns true if the Universal Ground was set successfully and
   *              false if it was not
   */
  bool Camera::SetUniversalGround(const double latitude, const double longitude) {
    // Convert lat/lon to undistorted focal plane x/y
    if(p_groundMap->SetGround(Latitude(latitude, Angle::Degrees),
                              Longitude(longitude, Angle::Degrees))) {
      return RawFocalPlanetoImage();
    }

    p_hasIntersection = false;
    return p_hasIntersection;
  }


  /**
   * Sets the lat/lon values to get the sample/line values
   *
   * @param latitude Latitude coordinate of the point
   * @param longitude Longitude coordinate of the point
   *
   * @return bool Returns true if the Universal Ground was set successfully and
   *              false if it was not
   */
  bool Camera::SetGround(Latitude latitude, Longitude longitude) {
    Distance localRadius(LocalRadius(latitude, longitude));

    if(!localRadius.Valid()) {
      p_hasIntersection = false;
      return p_hasIntersection;
    }

    return SetGround(SurfacePoint(latitude, longitude, localRadius));
  }



  /**
   * Sets the lat/lon/radius values to get the sample/line values
   *
   * @param latitude Latitude coordinate of the point
   * @param longitude Longitude coordinate of the point
   *
   * @return bool Returns true if the Universal Ground was set successfully and
   *              false if it was not
   */
  bool Camera::SetGround(const SurfacePoint & surfacePt) {
    if(!surfacePt.Valid()) {
      p_hasIntersection = false;
      return p_hasIntersection;
    }

    // Convert lat/lon to undistorted focal plane x/y
    if(p_groundMap->SetGround(surfacePt)) {
      return RawFocalPlanetoImage();
    }

    p_hasIntersection = false;
    return p_hasIntersection;
  }


  /**
   * Computes the image coordinate for the current universal ground point
   *
   *
   * @return @b bool Returns true if image coordinate was computed successfully and
   *              false if it was not
   */
  bool Camera::RawFocalPlanetoImage() {
    double ux = p_groundMap->FocalPlaneX();
    double uy = p_groundMap->FocalPlaneY();
    // Convert undistorted x/y to distorted x/y
    if(p_distortionMap->SetUndistortedFocalPlane(ux, uy)) {
      double focalPlaneX = p_distortionMap->FocalPlaneX();
      double focalPlaneY = p_distortionMap->FocalPlaneY();
      // Convert distorted x/y to detector position
      if(p_focalPlaneMap->SetFocalPlane(focalPlaneX, focalPlaneY)) {
        double detectorSample = p_focalPlaneMap->DetectorSample();
        double detectorLine = p_focalPlaneMap->DetectorLine();
        // Convert detector to parent position
        if(p_detectorMap->SetDetector(detectorSample, detectorLine)) {
          double parentSample = p_detectorMap->ParentSample();
          double parentLine = p_detectorMap->ParentLine();
          p_pointComputed = true;

          if(p_projection == NULL || p_ignoreProjection) {
            p_childSample = p_alphaCube->BetaSample(parentSample);
            p_childLine = p_alphaCube->BetaLine(parentLine);
            p_hasIntersection = true;
            return p_hasIntersection;
          }
          else if(p_projection->IsSky()) {
            if(p_projection->SetGround(Declination(), RightAscension())) {
              p_childSample = p_projection->WorldX();
              p_childLine = p_projection->WorldY();
              p_hasIntersection = true;
              return p_hasIntersection;
            }
          }
          else {
            if(p_projection->SetUniversalGround(UniversalLatitude(), UniversalLongitude())) {
              p_childSample = p_projection->WorldX();
              p_childLine = p_projection->WorldY();
              p_hasIntersection = true;
              return p_hasIntersection;
            }
          }
        }
      }
    }

    p_hasIntersection = false;
    return p_hasIntersection;
  }



  /**
  * Sets the lat/lon/radius values to get the sample/line values
  *
  * @param latitude Latitude coordinate of the cube
  * @param longitude Longitude coordinate of the cube
  * @param radius Radius coordinate of the cube
  *
  * @return @b bool Returns true if the Universal Ground was set successfully 
  *              and false if it was not
  */
  bool Camera::SetUniversalGround(const double latitude, const double longitude,
                                  const double radius) {
    // Convert lat/lon to undistorted focal plane x/y
    if(p_groundMap->SetGround(
        SurfacePoint(Latitude(latitude, Angle::Degrees),
                     Longitude(longitude, Angle::Degrees),
                     Distance(radius, Distance::Meters)))) {
      return RawFocalPlanetoImage();  // sets p_hasIntersection
    }

    p_hasIntersection = false;
    return p_hasIntersection;
  }

  /**
   * Returns the detector resolution at the current position
   *
   * @return @b double The detector resolution
   */
  double Camera::DetectorResolution() {
    if(HasSurfaceIntersection()) {
      double sB[3];
      InstrumentPosition(sB);
      double pB[3];
      Coordinate(pB);
      double a = sB[0] - pB[0];
      double b = sB[1] - pB[1];
      double c = sB[2] - pB[2];
      double dist = sqrt(a * a + b * b + c * c) * 1000.0;
      return dist / (p_focalLength / p_pixelPitch);
    }
    return -1.0;
  }

  /**
   * Returns the sample resolution at the current position
   *
   * @return @b double The sample resolution
   */
  double Camera::SampleResolution() {
    return DetectorResolution() * p_detectorMap->SampleScaleFactor();
  }

  /**
   * Returns the line resolution at the current position
   *
   * @return @b double The line resolution
   */
  double Camera::LineResolution() {
    return DetectorResolution() * p_detectorMap->LineScaleFactor();
  }

  /**
   * Returns the pixel resolution at the current position in m/pix
   *
   * @return @b double The pixel resolution
   */
  double Camera::PixelResolution() {
    double lineRes = LineResolution();
    double sampRes = SampleResolution();
    if(lineRes < 0.0) return -1.0;
    if(sampRes < 0.0) return -1.0;
    return (lineRes + sampRes) / 2.0;
  }

  /**
   * Returns the lowest/worst resolution in the entire image
   *
   * @return @b double The lowest/worst resolution in the image
   */
  double Camera::LowestImageResolution() {
    GroundRangeResolution();
    return p_maxres;
  }

  /**
   * Returns the highest/best resolution in the entire image
   *
   * @return @b double The highest/best resolution in the entire image
   */
  double Camera::HighestImageResolution() {
    GroundRangeResolution();
    return p_minres;
  }

  /**
   * Computes the ground range and min/max resolution
   */
  void Camera::GroundRangeResolution() {
    // Have we already done this
    if(p_groundRangeComputed) return;
    p_groundRangeComputed = true;

    bool computed = p_pointComputed;
    double originalSample = Sample();
    double originalLine = Line();
    int originalBand = Band();

    // Initializations
    p_minlat    = DBL_MAX;
    p_minlon    = DBL_MAX;
    p_minlon180 = DBL_MAX;
    p_maxlat    = -DBL_MAX;
    p_maxlon    = -DBL_MAX;
    p_maxlon180 = -DBL_MAX;
    p_minres    = DBL_MAX;
    p_maxres    = -DBL_MAX;

    // See if we have band dependence and loop for the appropriate number of bands
    int eband = p_bands;
    if(IsBandIndependent()) eband = 1;
    for(int band = 1; band <= eband; band++) {
      SetBand(band);

      // Loop for each line testing the left and right sides of the image
      for(int line = 1; line <= p_lines + 1; line++) {
        // Look for the first good lat/lon on the left edge of the image
        // If it is the first or last line then test the whole line
        int samp;
        for(samp = 1; samp <= p_samples + 1; samp++) {

          if(SetImage((double)samp - 0.5, (double)line - 0.5)) {
            double lat = UniversalLatitude();
            double lon = UniversalLongitude();
            if(lat < p_minlat) p_minlat = lat;
            if(lat > p_maxlat) p_maxlat = lat;
            if(lon < p_minlon) p_minlon = lon;
            if(lon > p_maxlon) p_maxlon = lon;

            if(lon > 180.0) lon -= 360.0;
            if(lon < p_minlon180) p_minlon180 = lon;
            if(lon > p_maxlon180) p_maxlon180 = lon;

            double res = PixelResolution();
            if(res > 0.0) {
              if(res < p_minres) p_minres = res;
              if(res > p_maxres) p_maxres = res;
            }
            if((line != 1) && (line != p_lines + 1)) break;
          }
        }

        //We've already checked the first and last lines.
        if(line == 1) continue;
        if(line == p_lines + 1) continue;

        // Look for the first good lat/lon on the right edge of the image
        if(samp < p_samples + 1) {
          for(samp = p_samples + 1; samp >= 1; samp--) {
            if(SetImage((double)samp - 0.5, (double)line - 0.5)) {
              double lat = UniversalLatitude();
              double lon = UniversalLongitude();
              if(lat < p_minlat) p_minlat = lat;
              if(lat > p_maxlat) p_maxlat = lat;
              if(lon < p_minlon) p_minlon = lon;
              if(lon > p_maxlon) p_maxlon = lon;

              if(lon > 180.0) lon -= 360.0;
              if(lon < p_minlon180) p_minlon180 = lon;
              if(lon > p_maxlon180) p_maxlon180 = lon;

              double res = PixelResolution();
              if(res > 0.0) {
                if(res < p_minres) p_minres = res;
                if(res > p_maxres) p_maxres = res;
              }
              break;
            }
          }
        }
      }

      // Test at the sub-spacecraft point to see if we have a
      // better resolution
      double lat, lon;

      SubSpacecraftPoint(lat, lon);
      Latitude latitude(lat, Angle::Degrees);
      Longitude longitude(lon, Angle::Degrees);
      Distance radius(LocalRadius(latitude, longitude));

      SurfacePoint testPoint(latitude, longitude, radius);

      if(SetGround(testPoint)) {
        if(Sample() >= 0.5 && Line() >= 0.5 &&
            Sample() <= p_samples + 0.5 && Line() <= p_lines + 0.5) {
          double res = PixelResolution();
          if(res > 0.0) {
            if(res < p_minres) p_minres = res;
            if(res > p_maxres) p_maxres = res;
          }
        }
      }

      // Special test for ground range to see if either pole is in the image
      latitude = Latitude(90, Angle::Degrees);
      longitude = Longitude(0.0, Angle::Degrees);
      radius = LocalRadius(latitude, longitude);

      testPoint = SurfacePoint(latitude, longitude, radius);

      if(SetGround(testPoint)) {
        if(Sample() >= 0.5 && Line() >= 0.5 &&
            Sample() <= p_samples + 0.5 && Line() <= p_lines + 0.5) {
          p_maxlat = 90.0;
          p_minlon = 0.0;
          p_maxlon = 360.0;
          p_minlon180 = -180.0;
          p_maxlon180 = 180.0;
        }
      }

      latitude = Latitude(-90, Angle::Degrees);
      radius = LocalRadius(latitude, longitude);

      testPoint = SurfacePoint(latitude, longitude, radius);
      if(SetGround(testPoint)) {
        if(Sample() >= 0.5 && Line() >= 0.5 &&
            Sample() <= p_samples + 0.5 && Line() <= p_lines + 0.5) {
          p_minlat = -90.0;
          p_minlon = 0.0;
          p_maxlon = 360.0;
          p_minlon180 = -180.0;
          p_maxlon180 = 180.0;
        }
      }

      // Another special test for ground range as we could have the
      // 0-360 seam running right through the image so
      // test it as well (the increment may not be fine enough !!!)
      for(Latitude lat = Latitude(p_minlat, Angle::Degrees);
                   lat <= Latitude(p_maxlat, Angle::Degrees);
                   lat += Angle((p_maxlat - p_minlat) / 10.0, Angle::Degrees)) {
        if(SetGround(lat, Longitude(0.0, Angle::Degrees))) {
          if(Sample() >= 0.5 && Line() >= 0.5 &&
              Sample() <= p_samples + 0.5 && Line() <= p_lines + 0.5) {
            p_minlon = 0.0;
            p_maxlon = 360.0;
            break;
          }
        }

        // Another special test for ground range as we could have the
        // -180-180 seam running right through the image so
        // test it as well (the increment may not be fine enough !!!)
        if(SetGround(lat, Longitude(180.0, Angle::Degrees))) {
          if(Sample() >= 0.5 && Line() >= 0.5 &&
              Sample() <= p_samples + 0.5 && Line() <= p_lines + 0.5) {
            p_minlon180 = -180.0;
            p_maxlon180 = 180.0;
            break;
          }
        }
      }
    }

    SetBand(originalBand);

    if(computed) {
      SetImage(originalSample, originalLine);
    }
    else {
      p_pointComputed = false;
    }

    // Checks for invalide lat/lon ranges
    if(p_minlon == DBL_MAX  ||  p_minlat == DBL_MAX  ||  p_maxlon == -DBL_MAX  ||  p_maxlat == -DBL_MAX) {
      string message = "Camera missed planet or SPICE data off.";
      throw iException::Message(iException::Camera, message, _FILEINFO_);
    }
  }

  /**
   * Checks whether the ground range intersects the longitude domain or not
   *
   * @param pvl The pvl file used to set the ground range
   *
   * @return @b bool Returns true if the range intersects the longitude domain, and
   *              false if it does not
   */
  bool Camera::IntersectsLongitudeDomain(Pvl &pvl) {
    double minlat, minlon, maxlat, maxlon;
    return GroundRange(minlat, maxlat, minlon, maxlon, pvl);
  }

  /**
   * Computes the Ground Range
   *
   * @param minlat The minimum latitude
   * @param maxlat The maximum latitude
   * @param minlon The minimum longitude
   * @param maxlon The maximum longitude
   * @param pvl The pvl file used for ground range calculations
   *
   * @return @b bool Returns true if it crosses the longitude domain boundary and
   *              false if it does not
   */
  bool Camera::GroundRange(double &minlat, double &maxlat,
                           double &minlon, double &maxlon,
                           Pvl &pvl) {
    // Compute the ground range and resolution
    GroundRangeResolution();

    // Get the default radii
    Distance radii[3];
    Radii(radii);
    Distance &a = radii[0];
    Distance &b = radii[2];

    // See if the PVL overrides the radii
    PvlGroup map = pvl.FindGroup("Mapping", Pvl::Traverse);

    if(map.HasKeyword("EquatorialRadius"))
      a = Distance(map["EquatorialRadius"][0], Distance::Meters);

    if(map.HasKeyword("PolarRadius")) 
      b = Distance(map["PolarRadius"][0], Distance::Meters);

    // Convert to planetographic if necessary
    minlat = p_minlat;
    maxlat = p_maxlat;
    if(map.HasKeyword("LatitudeType")) {
      iString latType = (string) map["LatitudeType"];
      if(latType.UpCase() == "PLANETOGRAPHIC") {
        if(abs(minlat) < 90.0) {  // So tan doesn't fail
          minlat *= PI / 180.0;
          minlat = atan(tan(minlat) * (a / b) * (a / b));
          minlat *= 180.0 / PI;
        }

        if(abs(maxlat) < 90.0) {  // So tan doesn't fail
          maxlat *= PI / 180.0;
          maxlat = atan(tan(maxlat) * (a / b) * (a / b));
          maxlat *= 180.0 / PI;
        }
      }
    }

    // Assume 0 to 360 domain but change it if necessary
    minlon = p_minlon;
    maxlon = p_maxlon;
    bool domain360 = true;
    if(map.HasKeyword("LongitudeDomain")) {
      iString lonDomain = (string) map["LongitudeDomain"];
      if(lonDomain.UpCase() == "180") {
        minlon = p_minlon180;
        maxlon = p_maxlon180;
        domain360 = false;
      }
    }

    // Convert to the proper longitude direction
    if(map.HasKeyword("LongitudeDirection")) {
      iString lonDirection = (string) map["LongitudeDirection"];
      if(lonDirection.UpCase() == "POSITIVEWEST") {
        double swap = minlon;
        minlon = -maxlon;
        maxlon = -swap;
      }
    }

    // Convert to the proper longitude domain
    if(domain360) {
      while(minlon < 0.0) {
        minlon += 360.0;
        maxlon += 360.0;
      }
      while(minlon > 360.0) {
        minlon -= 360.0;
        maxlon -= 360.0;
      }
    }
    else {
      while(minlon < -180.0) {
        minlon += 360.0;
        maxlon += 360.0;
      }
      while(minlon > 180.0) {
        minlon -= 360.0;
        maxlon -= 360.0;
      }
    }

    // Now return if it crosses the longitude domain boundary
    if((maxlon - minlon) > 359.0) return true;
    return false;
  }

  /**
   * Writes the basic mapping group to the specified Pvl.
   *
   * @param pvl Pvl to write mapping group to
   */
  void Camera::BasicMapping(Pvl &pvl) {
    PvlGroup map("Mapping");
    map += PvlKeyword("TargetName", Target());
    map += PvlKeyword("EquatorialRadius", p_radii[0].GetMeters(), "meters");
    map += PvlKeyword("PolarRadius", p_radii[2].GetMeters(), "meters");
    map += PvlKeyword("LatitudeType", "Planetocentric");
    map += PvlKeyword("LongitudeDirection", "PositiveEast");
    map += PvlKeyword("LongitudeDomain", "360");

    GroundRangeResolution();
    map += PvlKeyword("MinimumLatitude", p_minlat);
    map += PvlKeyword("MaximumLatitude", p_maxlat);
    map += PvlKeyword("MinimumLongitude", p_minlon);
    map += PvlKeyword("MaximumLongitude", p_maxlon);
    map += PvlKeyword("PixelResolution", p_minres);

    map += PvlKeyword("ProjectionName", "Sinusoidal");
    pvl.AddGroup(map);
  }

  //! Reads the focal length from the instrument kernel
  void Camera::SetFocalLength() {
    int code = NaifIkCode();
    string key = "INS" + iString(code) + "_FOCAL_LENGTH";
    SetFocalLength(Spice::GetDouble(key));
  }

  //! Reads the Pixel Pitch from the instrument kernel
  void Camera::SetPixelPitch() {
    int code = NaifIkCode();
    string key = "INS" + iString(code) + "_PIXEL_PITCH";
    SetPixelPitch(Spice::GetDouble(key));
  }

  /**
   * Sets the right ascension declination
   *
   * @param ra Right ascension value
   * @param dec Declination value
   *
   * @return @b bool Returns true if the declination was set successfully and false
   *              if it was not
   */
  bool Camera::SetRightAscensionDeclination(const double ra, const double dec) {
    if(p_skyMap->SetSky(ra, dec)) {
      double ux = p_skyMap->FocalPlaneX();
      double uy = p_skyMap->FocalPlaneY();
      if(p_distortionMap->SetUndistortedFocalPlane(ux, uy)) {
        double dx = p_distortionMap->FocalPlaneX();
        double dy = p_distortionMap->FocalPlaneY();
        if(p_focalPlaneMap->SetFocalPlane(dx, dy)) {
          double detectorSamp = p_focalPlaneMap->DetectorSample();
          double detectorLine = p_focalPlaneMap->DetectorLine();
          if(p_detectorMap->SetDetector(detectorSamp, detectorLine)) {
            double parentSample = p_detectorMap->ParentSample();
            double parentLine = p_detectorMap->ParentLine();
            p_pointComputed = true;

            if(p_projection == NULL || p_ignoreProjection) {
              p_childSample = p_alphaCube->BetaSample(parentSample);
              p_childLine = p_alphaCube->BetaLine(parentLine);
              return true;
            }
            else if(p_projection->IsSky()) {
              if(p_projection->SetGround(dec, ra)) {
                p_childSample = p_projection->WorldX();
                p_childLine = p_projection->WorldY();
                return true;
              }
            }
            else if(p_hasIntersection) {
              if(p_projection->SetUniversalGround(UniversalLatitude(),
                                                  UniversalLongitude())) {
                p_childSample = p_projection->WorldX();
                p_childLine = p_projection->WorldY();
                return true;
              }
            }
          }
        }
      }
    }

    return false;
  }
  
  
  /**
   * Sets the passed in vector to be the local normal which is calculated using
   * the DEM
   *
   * @param normal - local normal vector to be set
   *
   */
  void Camera::GetLocalNormal(double normal[3]) {
    
    // As documented in the doxygen above, the goal of this method is to
    // calculate a normal vector to the surface using the DEM instead of the
    // ellipsoid
    double samp = Sample();
    double line = Line();
    
    // order of points in vector is top, bottom, left, right
    QList< QPair< double, double > > surroundingPoints;
    surroundingPoints.append(qMakePair(samp, line - 0.5));
    surroundingPoints.append(qMakePair(samp, line + 0.5));
    surroundingPoints.append(qMakePair(samp - 0.5, line));
    surroundingPoints.append(qMakePair(samp + 0.5, line));
    
    // save state
    bool computed = p_pointComputed;
    double originalSample = samp;
    double originalLine = line;

    // now we have all four points in the image, so find the same points in the
    // dem
    QVector< double * > lookVects(4);
    for (int i = 0; i < lookVects.size(); i++)
      lookVects[i] = new double[3];
      
    for (int i = 0; i < surroundingPoints.size(); i ++) {
      if (!(SetImage(surroundingPoints[i].first, surroundingPoints[i].second))) {
        surroundingPoints[i].first = samp;
        surroundingPoints[i].second = line;
        if (!(SetImage(surroundingPoints[i].first, surroundingPoints[i].second))) {
          normal[0] = 0.;
          normal[1] = 0.;
          normal[2] = 0.;
          // restore state
          if(computed) {
            SetImage(originalSample, originalLine);
          } else {
            p_pointComputed = false;
          }

          // free memory
          for (int i = 0; i < lookVects.size(); i++)
            delete [] lookVects[i];

          return;
        }
      }

      Latitude demLat = p_surfacePoint->GetLatitude();
      Longitude demLon = p_surfacePoint->GetLongitude();
      Distance demRadius = DemRadius(demLat, demLon);

      latrec_c(demRadius.GetKilometers(), demLon.GetRadians(),
               demLat.GetRadians(), lookVects[i]);
    }
   
    if ((surroundingPoints[0].first == surroundingPoints[1].first &&
        surroundingPoints[0].second == surroundingPoints[1].second) ||
       (surroundingPoints[2].first == surroundingPoints[3].first &&
        surroundingPoints[2].second == surroundingPoints[3].second)) {
      normal[0] = 0.;
      normal[1] = 0.;
      normal[2] = 0.;
      // restore state
      if(computed) {
        SetImage(originalSample, originalLine);
      } else {
        p_pointComputed = false;
      }

      // free memory
      for (int i = 0; i < lookVects.size(); i++)
        delete [] lookVects[i];

      return;
    }

    // subtract bottom from top and left from right and store results
    double topMinusBottom[3];
    vsub_c(lookVects[0], lookVects[1], topMinusBottom);
    double rightMinusLeft[3];
    vsub_c(lookVects[3], lookVects[2], rightMinusLeft);
    
    // take cross product of subtraction results to get normal
    ucrss_c(topMinusBottom, rightMinusLeft, normal);
    
    // unitize normal (and do sanity check for magnitude)
    double mag;
    unorm_c(normal, normal, &mag); 
    if (mag == 0.0) {
      normal[0] = 0.;
      normal[1] = 0.;
      normal[2] = 0.;
      // restore state
      if(computed) {
        SetImage(originalSample, originalLine);
      } else {
        p_pointComputed = false;
      }

      // free memory
      for (int i = 0; i < lookVects.size(); i++)
        delete [] lookVects[i];
 
      return;
    }
    
    double centerLookVect[3];
    SetImage(originalSample, originalLine);
    
    // Check to make sure that the normal is pointing outward from the planet
    // surface. This is done by taking the dot product of the normal and 
    // any one of the unitized xyz vectors. If the normal is pointing inward, 
    // then negate it.

    SpiceDouble pB[3];
    pB[0] = p_surfacePoint->GetX().GetKilometers();
    pB[1] = p_surfacePoint->GetY().GetKilometers();
    pB[2] = p_surfacePoint->GetZ().GetKilometers();

    unorm_c(pB, centerLookVect, &mag);
    double dotprod = vdot_c(normal,centerLookVect);
    if (dotprod < 0.0)
      vminus_c(normal, normal);

    // restore state
    if(computed) {
      SetImage(originalSample, originalLine);
    }
    else {
      p_pointComputed = false;
    }
    
    // free memory
    for (int i = 0; i < lookVects.size(); i++)
      delete [] lookVects[i];
  }
  
  
  /**
   * Calculates LOCAL photometric angles using the DEM (not ellipsoid).  These
   * calcualtions are more expensive computationally than Sensor's angle getter
   * methods.  Furthermore, this cost is mostly in calculating the local normal
   * vector, which can be done only once for all angles using this method.
   *
   * @param phase The local phase angle to be calculated
   *
   * @param emission The local emission angle to be calculated
   *
   * @param incidence The local incidence angle to be calculated
   */
  void Camera::LocalPhotometricAngles(Angle & phase, Angle & incidence,
      Angle & emission, bool &success) {
    
    // get local normal vector
    double normal[3];
    GetLocalNormal(normal);
    success = true;

    // Check to make sure normal is valid
    SpiceDouble mag;
    unorm_c(normal,normal,&mag);
    if (mag == 0.) {
      success = false;
      return;
    }
    
    // get a normalized surface spacecraft vector
    SpiceDouble surfSpaceVect[3], unitizedSurfSpaceVect[3], dist;
    std::vector<double> sB = BodyRotation()->ReferenceVector(
        InstrumentPosition()->Coordinate());

    SpiceDouble pB[3];
    pB[0] = p_surfacePoint->GetX().GetKilometers();
    pB[1] = p_surfacePoint->GetY().GetKilometers();
    pB[2] = p_surfacePoint->GetZ().GetKilometers();

    vsub_c((SpiceDouble *) &sB[0], pB, surfSpaceVect);
    unorm_c(surfSpaceVect, unitizedSurfSpaceVect, &dist);

    // get a normalized surface sun vector
    SpiceDouble surfaceSunVect[3];
    vsub_c(p_uB, pB, surfaceSunVect);
    SpiceDouble unitizedSurfSunVect[3];
    unorm_c(surfaceSunVect, unitizedSurfSunVect, &dist);

    // use normalized surface spacecraft and surface sun vectors to calculate
    // the phase angle (in radians)
    phase = Angle(vsep_c(unitizedSurfSpaceVect, unitizedSurfSunVect),
        Angle::Radians);
    
    // use normalized surface spacecraft and local normal vectors to calculate
    // the emission angle (in radians)
    emission = Angle(vsep_c(unitizedSurfSpaceVect, normal),
        Angle::Radians);
    
    // use normalized surface sun and normal vectors to calculate the incidence
    // angle (in radians)
    incidence = Angle(vsep_c(unitizedSurfSunVect, normal),
        Angle::Radians);
  }
  
  
  /**
   * Computes the RaDec range
   *
   * @param minra Minimum right ascension value
   * @param maxra Maximum right ascension value
   * @param mindec Minimum declination value
   * @param maxdec Maximum declination value
   *  
   * @throw iException::Programmer - "Camera::RaDecRange can not calculate a 
   *  right ascension, declination range for projected images which are not
   *  projected to sky"
   * @return @b bool Returns true if the range computation was successful and false
   *              if it was not
   */
  bool Camera::RaDecRange(double &minra, double &maxra,
                          double &mindec, double &maxdec) {
    if(p_projection != NULL && !p_projection->IsSky()) {
      iString msg = "Camera::RaDecRange can not calculate a right ascension, declination range";
      msg += " for projected images which are not projected to sky";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    bool computed = p_pointComputed;
    double originalSample = Sample();
    double originalLine = Line();
    int originalBand = Band();

    // Have we already done this
    if(!p_raDecRangeComputed) {
      p_raDecRangeComputed = true;

      // Initializations
      p_mindec    = DBL_MAX;
      p_minra     = DBL_MAX;
      p_minra180  = DBL_MAX;
      p_maxdec    = -DBL_MAX;
      p_maxra     = -DBL_MAX;
      p_maxra180  = -DBL_MAX;

      // See if we have band dependence and loop for the appropriate number of bands
      int eband = p_bands;
      if(IsBandIndependent()) eband = 1;
      for(int band = 1; band <= eband; band++) {
        this->SetBand(band);

        for(int line = 1; line <= p_lines; line++) {
          // Test left, top, and bottom sides
          int samp;
          for(samp = 1; samp <= p_samples; samp++) {
            SetImage((double)samp, (double)line);
            double ra = RightAscension();
            double dec = Declination();
            if(ra < p_minra) p_minra = ra;
            if(ra > p_maxra) p_maxra = ra;
            if(dec < p_mindec) p_mindec = dec;
            if(dec > p_maxdec) p_maxdec = dec;

            if(ra > 180.0) ra -= 360.0;
            if(ra < p_minra180) p_minra180 = ra;
            if(ra > p_maxra180) p_maxra180 = ra;

            if((line != 1) && (line != p_lines)) break;
          }

          // Test right side
          if(samp < p_samples) {
            for(samp = p_samples; samp >= 1; samp--) {
              SetImage((double)samp, (double)line);
              double ra = RightAscension();
              double dec = Declination();
              if(ra < p_minra) p_minra = ra;
              if(ra > p_maxra) p_maxra = ra;
              if(dec < p_mindec) p_mindec = dec;
              if(dec > p_maxdec) p_maxdec = dec;

              if(ra > 180.0) ra -= 360.0;
              if(ra < p_minra180) p_minra180 = ra;
              if(ra > p_maxra180) p_maxra180 = ra;

              break;
            }
          }
        }

        // Special test for ground range to see if either pole is in the image
        if(SetRightAscensionDeclination(0.0, 90.0)) {
          if((Line() >= 0.5) && (Line() <= p_lines) &&
              (Sample() >= 0.5) && (Sample() <= p_samples)) {
            p_maxdec = 90.0;
            p_minra = 0.0;
            p_maxra = 360.0;
            p_minra180 = -180.0;
            p_maxra180 = 180.0;
          }
        }

        if(SetRightAscensionDeclination(0.0, -90.0)) {
          if((Line() >= 0.5) && (Line() <= p_lines) &&
              (Sample() >= 0.5) && (Sample() <= p_samples)) {
            p_mindec = -90.0;
            p_minra = 0.0;
            p_maxra = 360.0;
            p_minra180 = -180.0;
            p_maxra180 = 180.0;
          }
        }

        // Another special test for ground range as we could have the
        // 0-360 seam running right through the image so
        // test it as well (the increment may not be fine enough !!!)
        for(double dec = p_mindec; dec <= p_maxdec; dec += (p_maxdec - p_mindec) / 10.0) {
          if(SetRightAscensionDeclination(0.0, dec)) {
            if((Line() >= 0.5) && (Line() <= p_lines) &&
                (Sample() >= 0.5) && (Sample() <= p_samples)) {
              p_minra = 0.0;
              p_maxra = 360.0;
              break;
            }
          }
        }

        // Another special test for ground range as we could have the
        // 0-360 seam running right through the image so
        // test it as well (the increment may not be fine enough !!!)
        for(double dec = p_mindec; dec <= p_maxdec; dec += (p_maxdec - p_mindec) / 10.0) {
          if(SetRightAscensionDeclination(180.0, dec)) {
            if((Line() >= 0.5) && (Line() <= p_lines) &&
                (Sample() >= 0.5) && (Sample() <= p_samples)) {
              p_minra180 = -180.0;
              p_maxra180 = 180.0;
              break;
            }
          }
        }
      }
    }

    minra = p_minra;
    maxra = p_maxra;
    mindec = p_mindec;
    maxdec = p_maxdec;

    SetBand(originalBand);

    if(computed) {
      SetImage(originalSample, originalLine);
    }
    else {
      p_pointComputed = false;
    }

    return true;
  }


  /**
   * Returns the RaDec resolution
   *
   * @return @b double The resutant RaDec resolution
   */
  double Camera::RaDecResolution() {
    if(p_projection != NULL && !p_projection->IsSky()) {
      iString msg = "Camera::RaDecResolution can not calculate a right ascension, declination resolution";
      msg += " for projected images which are not projected to sky";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    bool computed = p_pointComputed;
    double originalSample = Sample();
    double originalLine = Line();
    int originalBand = Band();

    SetImage(1.0, 1.0);
    double ra1 = RightAscension();
    double dec1 = Declination();

    SetImage(1.0, (double)p_lines);
    double ra2 = RightAscension();
    double dec2 = Declination();

    double dist = (ra1 - ra2) * (ra1 - ra2) + (dec1 - dec2) * (dec1 - dec2);
    dist = sqrt(dist);
    double lineRes = dist / (p_lines - 1);

    SetImage((double)p_samples, 1.0);
    ra2 = RightAscension();
    dec2 = Declination();

    dist = (ra1 - ra2) * (ra1 - ra2) + (dec1 - dec2) * (dec1 - dec2);
    dist = sqrt(dist);
    double sampRes = dist / (p_samples - 1);

    SetBand(originalBand);

    if(computed) {
      SetImage(originalSample, originalLine);
    }
    else {
      p_pointComputed = false;
    }

    return (sampRes < lineRes) ? sampRes : lineRes;
  }

  /**
   * Returns the North Azimuth
   *
   * @return @b double North Azimuth
   */
  double Camera::NorthAzimuth() {
    // Get the latitude of your current location using the shape model
    // specified in the image Kernels
    double lat = UniversalLatitude();
    // We are in northern hemisphere
    if (lat >= 0.0) {
      return ComputeAzimuth(LocalRadius(90.0, 0.0), 90.0, 0.0);
    } 
    // We are in southern hemisphere
    else {
      double azimuth = ComputeAzimuth(LocalRadius(-90.0, 0.0),
                                      -90.0, 0.0) + 180.0;
      if (azimuth > 360.0) azimuth = azimuth - 360.0;
      return azimuth;
    }
  }

  /**
   * Returns the Sun Azimuth
   *
   * @return @b double Sun Azimuth
   *
   * @todo Get appropriate radius at the subsolar point
   */
  double Camera::SunAzimuth() {
    double lat, lon;
    SubSolarPoint(lat, lon);
    return ComputeAzimuth(LocalRadius(lat, lon), lat, lon);
  }

  /**
   * Return the Spacecraft Azimuth
   *
   * @return @b double Spacecraft Azimuth
   *
   * @todo Get appropriate radius at the subscraft point
   */
  double Camera::SpacecraftAzimuth() {
    double lat, lon;
    SubSpacecraftPoint(lat, lon);
    return ComputeAzimuth(LocalRadius(lat, lon), lat, lon);
  }

  /**
   * Computes the azimuth value from your current position (origin) to a
   * point of interest specified by the lat/lon input to this method. All
   * azimuths are measured the same way regardless of the image level (level1
   * or level2) and the shape model being used. The azimuth is an angle 
   * measured along the ground from the current postion (origin point) to a 
   * point of interest. The azimuth is measured in a positive clockwise 
   * direction from a reference line. The reference line is formed by drawing 
   * a line horizontally from the origin point to the right side of the image.
   * This is usually called the 3 o'clock reference line because the image
   * can be viewed as a clock face and the origin point as the center of the
   * clock face with the hand of the clock pointing at 3 o'clock. The azimuth
   * is measured in a positive clockwise direction because images have 
   * lines that increase downward. If lines increased upward, then the azimuth
   * would be measure in a positive counterclockwise direction.
   *
   * The algorithm works by getting the body-fixed (x,y,z) of the origin point 
   * and the body-fixed (x,y,z) of the point of interest. The vector from the 
   * origin point to the point of interest is then determined. The perpendicular
   * component of this new vector to the origin vector is determined. This gives 
   * a vector that is tangent to the planet surface at the origin point and that 
   * is in the direction of the point of interest. The tangent vector is then 
   * scaled to be within a pixel in size. A new body-fixed vector from the 
   * center of the planet to the head of the tangent vector is determined.
   * The body-fixed (x,y,z) of the new vector is used to get line,sample. So,
   * we now have the line,sample of the origin point and the line,sample of
   * a point in the direction of the point of interest and within a pixel's
   * distance from the origin point. The arctangent of (newline-originline)/
   * (newsample-originsample) is used to acquire the azimuth value.
   *
   * NOTE: All vectors in this method are body-fixed and use the radius of the
   * shape model at the origin point for doing calculations. By using the radius
   * of the shape model at the origin, we avoid problems where the DEM does
   * not completely cover the planet.
   *
   * @param radius The Radius
   * @param lat The Latitude
   * @param lon The Longitude
   *
   * @return @b double Azimuth value
   *
   * @history 2009-09-23  Tracie Sucharski - Convert negative
   *                         longitudes coming out of reclat.
   * @history 2010-09-28  Janet Barrett - Added Randy's updated method
   *                         for calculating the azimuth.
   * @history 2011-02-11  Janet Barrett - Added documentation.
   * @history 2011_02-11  Janet Barrett - There were some problems with 
   *                         calculating azimuths when a DEM shape model was
   *                         specified. One problem occurred when the DEM did
   *                         not cover the poles. The LocalRadius was returning
   *                         a NULL for the radius value in places that the DEM
   *                         did not cover (such as the poles). This was fixed
   *                         by using the radius of the origin point when 
   *                         determining the x,y,z location of the point of 
   *                         interest. The radius is not important because we just 
   *                         need to know the direction of the point of interest 
   *                         from the origin point. Another problem was also found 
   *                         with the call to SetUniversalGround when the new point 
   *                         (new point = point within a pixel of the origin point 
   *                         and in the direction of the point of interest) was being 
   *                         determined. The new point should be at the same radius as
   *                         the origin point, but this was not happening. The call to
   *                         SetUniversalGround was changed to use the radius of the 
   *                         origin point when determining the line,sample of the new 
   *                         point. Another problem was that the vector pointing from
   *                         the origin point to the point of interest was being 
   *                         unitized before its perpendicular component was being
   *                         calculated. This has been fixed.
   *
   * @todo Write PushState and PopState method to ensure the
   * internals of the class are set based on SetImage or SetGround
   */
  double Camera::ComputeAzimuth(Distance radius,
                                const double lat, const double lon) {
    // Make sure we are on the planet
    if(!HasSurfaceIntersection()) return -1.0;

    // Need to save the "state" of the camera so we can restore it when the
    // method is done
    bool computed = p_pointComputed;
    double originalSample = Sample();
    double originalLine = Line();

    NaifStatus::CheckErrors();

    // Get the origin point and its radius
    SpiceDouble oB[3];
    Coordinate(oB);
    Distance originRadius = LocalRadius();

    // Convert the point of interest to x/y/z in body-fixed and use the origin radius
    // to avoid the situation where the DEM does not cover the entire planet
    SpiceDouble pB[3];
    latrec_c(originRadius.GetKilometers(), lon * PI / 180.0,
             lat * PI / 180.0, pB);

    // Get the difference vector poB=pB-oB with its tail at oB and its head at pB
    SpiceDouble poB[3],upoB[3];
    vsub_c(pB, oB, poB);

    // Get pixel scale in km/pixel and divide by 2 to insure that we stay within
    // a pixel of the origin point
    double scale = (PixelResolution() / 1000.0) / 2.0;

    SpiceDouble hpoB[3];
    SpiceDouble spoB[3];
    // Get the component of the difference vector poB that is perpendicular to the origin 
    // point; this will result in a vector that is tangent to the surface at the origin 
    // point and is in the direction of the point of interest
    vperp_c(poB, oB, hpoB);
    // Unitize the tangent vector and then scale it to within a pixel of the origin point
    vhat_c(hpoB, upoB);
    vscl_c(scale, upoB, spoB);

    // Compute the new point in body fixed.  This point will be within a pixel of the
    // origin but in the same direction as the requested lat/lon of the point of interest
    SpiceDouble nB[3];
    vadd_c(oB, spoB, nB);

    // Get the origin image coordinate
    double osample = Sample();
    double oline = Line();

    // Convert the point to a lat/lon and find out its image coordinate
    double nrad, nlon, nlat;
    reclat_c(nB, &nrad, &nlon, &nlat);
    nlat = nlat * 180.0 / PI;
    nlon = nlon * 180.0 / PI;
    if(nlon < 0) nlon += 360.0;

    // Use the radius of the origin point to avoid the effects of topography on the
    // calculation
    SetUniversalGround(nlat, nlon, originRadius.GetMeters());
    double nsample = Sample();
    double nline = Line();

    // TODO:  Write PushState and PopState method to ensure the
    // internals of the class are set based on SetImage or SetGround

    // We now have the information needed to calculate an arctangent, so set the
    // image back to the origin point (go back to the original "state")
    SetImage(osample, oline);

    double deltaSample = nsample - osample;
    double deltaLine = nline - oline;

    // Compute the angle; the azimuth is the arctangent of the line difference
    // divided by the sample difference; the atan2 function is used because it
    // determines which quadrant we are in based on the sign of the 2 arguments;
    // the arctangent is measured in a positive clockwise direction because the
    // lines in the image increase downward; the arctangent uses the 3 o'clock
    // axis (positive sample direction) as its reference line (line of zero degrees);
    // a good place to read about the atan2 function is at http://en.wikipedia.org/wiki/Atan2
    double azimuth = 0.0;
    if(deltaSample != 0.0 || deltaLine != 0.0) {
      azimuth = atan2(deltaLine, deltaSample);
      azimuth *= 180.0 / PI;
    }
    // Azimuth is limited to the range of 0 to 360
    if(azimuth < 0.0) azimuth += 360.0;
    if(azimuth > 360.0) azimuth -= 360.0;

    NaifStatus::CheckErrors();

    // Reset "state" of camera
    if(computed) {
      SetImage(originalSample, originalLine);
    }
    else {
      p_pointComputed = false;
    }

    return azimuth;
  }

  /**
   * Return the off nadir angle in degrees.
   *
   * @return @b double Off Nadir Angle
   */
  double Camera::OffNadirAngle() {
    NaifStatus::CheckErrors();

    // Get the xyz coordinates for the spacecraft and point we are interested in
    double coord[3], spCoord[3];
    Coordinate(coord);
    InstrumentPosition(spCoord);

    // Get the angle between the 2 points and convert to degrees
    double a = vsep_c(coord, spCoord) * 180.0 / PI;
    double b = 180.0 - EmissionAngle();

    // The three angles in a triangle must add up to 180 degrees
    double c = 180.0 - (a + b);

    NaifStatus::CheckErrors();

    return c;
  }

  /**
   * Computes and returns the ground azimuth between the ground point and
   * another point of interest, such as the subspacecraft point or the
   * subsolar point. The ground azimuth is the clockwise angle on the
   * ground between a line drawn from the ground point to the North pole
   * of the body and a line drawn from the ground point to the point of
   * interest (such as the subsolar point or the subspacecraft point).
   *
   * @param glat The latitude of the ground point
   * @param glon The longitude of the ground point
   * @param slat The latitude of the subspacecraft or subsolar point
   * @param slon The longitude of the subspacecraft or subsolar point
   *
   * @return @b double The azimuth in degrees
   */
  double Camera::GroundAzimuth(double glat, double glon,
                               double slat, double slon) {
    double a = (90.0 - slat) * PI / 180.0;
    double b = (90.0 - glat) * PI / 180.0;
    double c = (glon - slon) * PI / 180.0;
    double absum = 0.5 * (a + b);
    double cosabsum = cos(absum);
    double sinabsum = sin(absum);
    double abdif = 0.5 * (a - b);
    double cosabdif = cos(abdif);
    double sinabdif = sin(abdif);
    double cotc = 1.0 / (tan(0.5 * c));
    double tanabsum = cotc * cosabdif / cosabsum;
    double tanabdif = cotc * sinabdif / sinabsum;
    double ABsum = atan(tanabsum);
    double ABdif = atan(tanabdif);
    double A = ABsum + ABdif;
    double B = ABsum - ABdif;
    double sinc = sin(c) * sin(a) / sin(A);
    c = asin(sinc);
    double azimuthA = A * 180.0 / PI + 90.0;
    double azimuthB = 90.0 - B * 180.0 / PI;
    double azimuth = 0.0;
    if((glat > slat && glon > slon) ||
        (glat < slat && glon > slon)) {
      if(azimuthA < azimuthB) {
        azimuth = 270.0 - azimuthA;
      }
      else {
        azimuth = 270.0 - azimuthB;
      }
    }
    else if((glat < slat && glon < slon) ||
            (glat > slat && glon < slon)) {
      if(azimuthA < azimuthB) {
        azimuth = 90.0 - azimuthA;
      }
      else {
        azimuth = 90.0 - azimuthB;
      }
    }
    return azimuth;
  }


  /**
   * Sets the Distortion Map. This object will take ownership of the distortion
   * map pointer.
   *
   * @param *map Pointer to a CameraDistortionMap object
   */
  void Camera::SetDistortionMap(CameraDistortionMap *map) {
    if(p_distortionMap) {
      delete p_distortionMap;
    }

    p_distortionMap = map;
  };

  /**
   * Sets the Focal Plane Map. This object will take ownership of the focal plane
   * map pointer.
   *
   * @param *map Pointer to a CameraFocalPlaneMap object
   */
  void Camera::SetFocalPlaneMap(CameraFocalPlaneMap *map) {
    if(p_focalPlaneMap) {
      delete p_focalPlaneMap;
    }

    p_focalPlaneMap = map;
  };

  /**
   * Sets the Detector Map. This object will take ownership of the detector map
   * pointer.
   *
   * @param *map Pointer to a CameraDetectorMap object
   */
  void Camera::SetDetectorMap(CameraDetectorMap *map) {
    if(p_detectorMap) {
      delete p_detectorMap;
    }

    p_detectorMap = map;
  };

  /**
   * Sets the Ground Map. This object will take ownership of the ground map
   * pointer.
   *
   * @param *map Pointer to a CameraGroundMap object
   */
  void Camera::SetGroundMap(CameraGroundMap *map) {
    if(p_groundMap) {
      delete p_groundMap;
    }

    p_groundMap = map;
  };

  /**
   * Sets the Sky Map. This object will take ownership of the sky map pointer.
   *
   * @param *map Pointer to a CameraSkyMap object
   */
  void Camera::SetSkyMap(CameraSkyMap *map) {
    if(p_skyMap) {
      delete p_skyMap;
    }

    p_skyMap = map;
  };

  /**
   * This loads the spice cache big enough for this image. The default cache size
   *   is the number of lines in the cube if the ephemeris time changes in the
   *   image, one otherwise.
   *
   * @internal 
   *   @history 2011-02-08 Jeannie Walldren - Removed unused input parameter.
   *                          Moved calculations of cache size and start/end
   *                          ephemeris times to their own methods.
   */
  void Camera::LoadCache() {
    // We want to stay in unprojected space for this process
    bool projIgnored = p_ignoreProjection;
    p_ignoreProjection = true;

    // get the cache variables
    pair<double,double> ephemerisTimes = StartEndEphemerisTimes();
    int cacheSize = CacheSize(ephemerisTimes.first, ephemerisTimes.second);

    // Set a position in the image so that the PixelResolution can be calculated
    SetImage(p_alphaCube->BetaSamples() / 2, p_alphaCube->BetaLines() / 2);
    double tol = PixelResolution() / 100.; //meters/pix/100.

    if(tol < 0.0) {
      // Alternative calculation of ground resolution of a pixel/100
      double altitudeMeters;
      if(IsSky()) {   // Use the unit sphere as the target
        altitudeMeters = 1.0;
      }
      else {
        altitudeMeters = SpacecraftAltitude() * 1000.;
      }
      tol = PixelPitch() * altitudeMeters / FocalLength() / 100.;
    }

    p_ignoreProjection = projIgnored;

    Spice::CreateCache(ephemerisTimes.first, ephemerisTimes.second, 
                       cacheSize, tol); 

    SetTime(ephemerisTimes.first);

    // Reset to band 1
    SetBand(1);

    return;
  }


  /**
   * Calculates the start and end ephemeris times. These times are found by
   * looping through the bands and finding the ephemeris times for the upper
   * left and bottom right pixels in the image. The start time (shutter open
   * time) is the minimum value of those ephemeris times. The end time (shutter
   * close time) is the maximum value of those ephemeris times. This method must
   * be called before a call to the Spice::CreateCache() method.  It is called
   * in the LoadCache() method.
   *  
   * @throw iException::Programmer - "Unable to find time range for the 
   *             spice kernels."
   * @see CreateCache()
   * @see LoadCache()
   * 
   * @author 2011-02-02 Jeannie Walldren
   * @internal
   *   @history 2011-02-02 Jeannie Walldren - Original version.
   */
  pair<double, double> Camera::StartEndEphemerisTimes() {
    pair<double,double> ephemerisTimes;
    double startTime, endTime;
    for(int band = 1; band <= Bands(); band++) {
      SetBand(band);
      SetImage(0.5, 0.5);
      double etStart = Time().Et();
      SetImage(p_alphaCube->BetaSamples() + 0.5,
               p_alphaCube->BetaLines() + 0.5);
      double etEnd = Time().Et();
      if(band == 1) {
        startTime = min(etStart, etEnd);
        endTime = max(etStart, etEnd);
      }
      startTime = min(startTime, min(etStart, etEnd));
      endTime = max(endTime, max(etStart, etEnd));
    }
    if(startTime == -DBL_MAX || endTime == -DBL_MAX) {
      string msg = "Unable to find time range for the spice kernels";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
    ephemerisTimes.first = startTime;
    ephemerisTimes.second = endTime;
    return ephemerisTimes;
  }

  /**
   * This method calculates the spice cache size. This method finds the number
   * of lines in the beta cube and adds 1, since we need at least 2 points for
   * interpolation. This method must be called before a call to the
   * Spice::CreateCache() method.  It is called in the LoadCache() method.
   *  
   * @throw iException::Programmer - "A cache has already been created."
   * @see CreateCache()
   * @see LoadCache()
   * 
   * @author 2011-02-02 Jeannie Walldren 
   * @internal
   *   @history 2011-02-02 Jeannie Walldren - Original version.
   */
  int Camera::CacheSize(double startTime, double endTime) {
    int cacheSize;
    // BetaLines() + 1 so we get at least 2 points for interpolation
    cacheSize = p_alphaCube->BetaLines() + 1;
    if(startTime == endTime) {
      cacheSize = 1;
    }
    return cacheSize;
  }


  /**
   * This method sets the best geometric tiling size for projecting from this
   * camera model. This is used by cam2map/ProcessRubberSheet. When cubes are
   * projected, an attempt is made to use linear equations to take large, square
   * chunks of data at a time to cull the amount of SetUniversalGround(...) calls
   * necessary to project a cube. If the chunk of data fails to be linear, then it
   * will be split up into 4 corners and each of the new chunks (corners) are
   * reconsidered up until endSize is reached - the endsize size will be
   * considered, it is inclusive. The startSize must be a power of 2 greater
   * than 2, and the endSize must be a power of 2 equal to or less than the
   * start size but greater than 2. If both the startSize and endSize are set to 2
   * then no geometric tiling will be enabled.
   *
   * @param startSize The tile size to start with; default 128
   * @param endSize The tile size to give up at; default 8
   */
  void Camera::SetGeometricTilingHint(int startSize, int endSize) {
    // verify the start size is a multiple of 2 greater than 2
    int powerOf2 = 2;

    // No hint if 2's are passed in
    if(startSize == 2 && endSize == 2) {
      p_geometricTilingStartSize = 2;
      p_geometricTilingEndSize = 2;
      return;
    }

    if(endSize > startSize) {
      iString message = "Camera::SetGeometricTilingHint End size must be smaller than the start size";
      throw iException::Message(iException::Programmer, message, _FILEINFO_);
    }

    if(startSize < 4) {
      iString message = "Camera::SetGeometricTilingHint Start size must be at least 4";
      throw iException::Message(iException::Programmer, message, _FILEINFO_);
    }

    bool foundEnd = false;
    while(powerOf2 > 0 && startSize != powerOf2) {
      if(powerOf2 == endSize) foundEnd = true;
      powerOf2 *= 2;
    }

    // Didnt find a solution, the integer became negative first, must not be
    //   a power of 2
    if(powerOf2 < 0) {
      iString message = "Camera::SetGeometricTilingHint Start size must be a power of 2";
      throw iException::Message(iException::Programmer, message, _FILEINFO_);
    }

    if(!foundEnd) {
      iString message = "Camera::SetGeometricTilingHint End size must be a power of 2 less than the start size, but greater than 2";
      throw iException::Message(iException::Programmer, message, _FILEINFO_);
    }

    p_geometricTilingStartSize = startSize;
    p_geometricTilingEndSize = endSize;
  }

  /**
   * This will get the geometric tiling hint; these values are typically used for
   * ProcessRubberSheet::SetTiling(...).
   *
   * @param startSize Tiling start size
   * @param endSize Tiling end size
   */
  void Camera::GetGeometricTilingHint(int &startSize, int &endSize) {
    startSize = p_geometricTilingStartSize;
    endSize = p_geometricTilingEndSize;
  }


  /**
   * This returns true if the current Sample() or Line() value
   * is outside of the cube (meaning the point must have been
   * extrapolated).
   *
   *
   * @return @b bool Point was extrapolated
   */
  bool Camera::InCube() {
    if(Sample() < 0.5 || Line() < 0.5) {
      return false;
    }

    if(Sample() > Samples() + 0.5 || Line() > Lines() + 0.5) {
      return false;
    }

    return true;
  }

// end namespace isis
}
