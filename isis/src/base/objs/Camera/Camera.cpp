/**
 * @file
 * $Revision: 1.31 $
 * $Date: 2010/06/16 18:22:48 $
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

#include "Projection.h"
#include "Constants.h"
#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "iException.h"
#include "iString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifStatus.h"
#include "ProjectionFactory.h"
//#include "SurfacePoint.h"

using namespace std;

namespace Isis {
  /**
   * Constructs the Camera object
   *
   * @param lab Pvl label used to create the Camera object
   */
  Camera::Camera(Isis::Pvl &lab) : Isis::Sensor(lab) {
    // Get the image size which can be different than the alpha cube size
    Isis::PvlGroup &dims = lab.FindObject("IsisCube")
                           .FindObject("Core")
                           .FindGroup("Dimensions");
    p_lines = dims["Lines"];
    p_samples = dims["Samples"];
    p_bands = dims["Bands"];

    SetGeometricTilingHint();

    // Get the AlphaCube information
    p_alphaCube = new Isis::AlphaCube(lab);

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
    p_ckFrameId = 0;
    p_ckReferenceId = 0;
    p_ckwriteReady = false;

    p_distortionMap = NULL;
    p_focalPlaneMap = NULL;
    p_detectorMap = NULL;
    p_groundMap = NULL;
    p_skyMap = NULL;

    // See if we have a reference band
    Isis::PvlGroup &inst = lab.FindObject("IsisCube").FindGroup("Instrument");
    if(inst.HasKeyword("ReferenceBand")) {
      p_referenceBand = inst["ReferenceBand"];
    }

    // Set the FrameId and the ReferenceFrameId
    SetCkFrameId();
    SetCkReferenceId();

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
   * Sets the sample/line values of the to get the lat/lon values
   *
   * @param sample Sample coordinate of the cube
   *
   * @param line Line coordinate of the cube
   *
   * @return bool Returns true if the image was set successfully and false if it
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
            p_hasIntersection =  p_groundMap->SetFocalPlane(x, y, z);
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
        if(SetUniversalGround(p_projection->UniversalLatitude(),
                              p_projection->UniversalLongitude())) {
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
   * @return bool Returns true if the Universal Ground was set successfully and
   *              false if it was not
   */
  bool Camera::SetUniversalGround(const double latitude, const double longitude) {
    // Convert lat/lon to undistorted focal plane x/y
    if(p_groundMap->SetGround(latitude, longitude)) {
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
    return SetUniversalGround(latitude.GetDegrees(), longitude.GetDegrees());
  }


  /**
   * Sets the lat/lon/radius values to get the sample/line values
   *
   * @param latitude Latitude coordinate of the point
   * @param longitude Longitude coordinate of the point
   *
   * @return bool Returns true if the Universal Ground was set successfully and
   *              false if it was not
   *
  bool Camera::SetGround(SurfacePoint surfacePt) {
    return SetUniversalGround(surfacePt.GetLatitude().GetDegrees(),
                              surfacePt.GetLongitude().GetDegrees(),
                              surfacePt.GetLocalRadius().GetMeters());
  }*/



  /**
   * Computes the image coordinate for the current universal ground point
   *
   *
   * @return bool Returns true if image coordinate was computed successfully and
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
  *
  * @param longitude Longitude coordinate of the cube
  *
  * @param radius Radius coordinate of the cube
  *
  * @return bool Returns true if the Universal Ground was set successfully and
  *              false if it was not
  */
  bool Camera::SetUniversalGround(const double latitude, const double longitude,
                                  const double radius) {
    // Convert lat/lon to undistorted focal plane x/y
    if(p_groundMap->SetGround(latitude, longitude, radius)) {
      return RawFocalPlanetoImage();  // sets p_hasIntersection
    }

    p_hasIntersection = false;
    return p_hasIntersection;
  }

  /**
   * Returns the detector resolution at the current position
   *
   * @return double The detector resolution
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
   * @return double The sample resolution
   */
  double Camera::SampleResolution() {
    return DetectorResolution() * p_detectorMap->SampleScaleFactor();
  }

  /**
   * Returns the line resolution at the current position
   *
   * @return double The line resolution
   */
  double Camera::LineResolution() {
    return DetectorResolution() * p_detectorMap->LineScaleFactor();
  }

  /**
   * Returns the pixel resolution at the current position in m/pix
   *
   * @return double The pixel resolution
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
   * @return double The lowest/worst resolution in the image
   */
  double Camera::LowestImageResolution() {
    GroundRangeResolution();
    return p_maxres;
  }

  /**
   * Returns the highest/best resolution in the entire image
   *
   * @return double The highest/best resolution in the entire image
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
      if(SetUniversalGround(lat, lon)) {
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
      if(SetUniversalGround(90.0, 0.0)) {
        if(Sample() >= 0.5 && Line() >= 0.5 &&
            Sample() <= p_samples + 0.5 && Line() <= p_lines + 0.5) {
          p_maxlat = 90.0;
          p_minlon = 0.0;
          p_maxlon = 360.0;
          p_minlon180 = -180.0;
          p_maxlon180 = 180.0;
        }
      }

      if(SetUniversalGround(-90.0, 0.0)) {
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
      for(double lat = p_minlat; lat <= p_maxlat; lat += (p_maxlat - p_minlat) / 10.0) {
        if(SetUniversalGround(lat, 0.0)) {
          if(Sample() >= 0.5 && Line() >= 0.5 &&
              Sample() <= p_samples + 0.5 && Line() <= p_lines + 0.5) {
            p_minlon = 0.0;
            p_maxlon = 360.0;
            break;
          }
        }
      }

      // Another special test for ground range as we could have the
      // -180-180 seam running right through the image so
      // test it as well (the increment may not be fine enough !!!)
      for(double lat = p_minlat; lat <= p_maxlat; lat += (p_maxlat - p_minlat) / 10.0) {
        if(SetUniversalGround(lat, 180.0)) {
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
   * @return bool Returns true if the range intersects the longitude domain, and
   *              false if it does not
   */
  bool Camera::IntersectsLongitudeDomain(Isis::Pvl &pvl) {
    double minlat, minlon, maxlat, maxlon;
    return GroundRange(minlat, maxlat, minlon, maxlon, pvl);
  }

  /**
   * Computes the Ground Range
   *
   * @param minlat The minimum latitude
   *
   * @param maxlat The maximum latitude
   *
   * @param minlon The minimum longitude
   *
   * @param maxlon The maximum longitude
   *
   * @param pvl The pvl file used for ground range calculations
   *
   * @return bool Returns true if it crosses the longitude domain boundary and
   *              false if it does not
   */
  bool Camera::GroundRange(double &minlat, double &maxlat,
                           double &minlon, double &maxlon,
                           Isis::Pvl &pvl) {
    // Compute the ground range and resolution
    GroundRangeResolution();

    // Get the default radii
    double radii[3];
    Radii(radii);
    double a = radii[0] * 1000.0;
    double b = radii[2] * 1000.0;

    // See if the PVL overrides the radii
    Isis::PvlGroup map = pvl.FindGroup("Mapping", Isis::Pvl::Traverse);
    if(map.HasKeyword("EquatorialRadius")) a = map["EquatorialRadius"];
    if(map.HasKeyword("PolarRadius")) b = map["PolarRadius"];

    // Convert to planetographic if necessary
    minlat = p_minlat;
    maxlat = p_maxlat;
    if(map.HasKeyword("LatitudeType")) {
      Isis::iString latType = (string) map["LatitudeType"];
      if(latType.UpCase() == "PLANETOGRAPHIC") {
        if(abs(minlat) < 90.0) {  // So tan doesn't fail
          minlat *= Isis::PI / 180.0;
          minlat = atan(tan(minlat) * (a / b) * (a / b));
          minlat *= 180.0 / Isis::PI;
        }

        if(abs(maxlat) < 90.0) {  // So tan doesn't fail
          maxlat *= Isis::PI / 180.0;
          maxlat = atan(tan(maxlat) * (a / b) * (a / b));
          maxlat *= 180.0 / Isis::PI;
        }
      }
    }

    // Assume 0 to 360 domain but change it if necessary
    minlon = p_minlon;
    maxlon = p_maxlon;
    bool domain360 = true;
    if(map.HasKeyword("LongitudeDomain")) {
      Isis::iString lonDomain = (string) map["LongitudeDomain"];
      if(lonDomain.UpCase() == "180") {
        minlon = p_minlon180;
        maxlon = p_maxlon180;
        domain360 = false;
      }
    }

    // Convert to the proper longitude direction
    if(map.HasKeyword("LongitudeDirection")) {
      Isis::iString lonDirection = (string) map["LongitudeDirection"];
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
  void Camera::BasicMapping(Isis::Pvl &pvl) {
    Isis::PvlGroup map("Mapping");
    map += Isis::PvlKeyword("TargetName", Target());
    map += Isis::PvlKeyword("EquatorialRadius", p_radii[0] * 1000.0, "meters");
    map += Isis::PvlKeyword("PolarRadius", p_radii[2] * 1000.0, "meters");
    map += Isis::PvlKeyword("LatitudeType", "Planetocentric");
    map += Isis::PvlKeyword("LongitudeDirection", "PositiveEast");
    map += Isis::PvlKeyword("LongitudeDomain", "360");

    GroundRangeResolution();
    map += Isis::PvlKeyword("MinimumLatitude", p_minlat);
    map += Isis::PvlKeyword("MaximumLatitude", p_maxlat);
    map += Isis::PvlKeyword("MinimumLongitude", p_minlon);
    map += Isis::PvlKeyword("MaximumLongitude", p_maxlon);
    map += Isis::PvlKeyword("PixelResolution", p_minres);

    map += Isis::PvlKeyword("ProjectionName", "Sinusoidal");
    pvl.AddGroup(map);
  }

  //! Reads the focal length from the instrument kernel
  void Camera::SetFocalLength() {
    int code = NaifIkCode();
    string key = "INS" + Isis::iString(code) + "_FOCAL_LENGTH";
    SetFocalLength(Isis::Spice::GetDouble(key));
  }

  //! Reads the Pixel Pitch from the instrument kernel
  void Camera::SetPixelPitch() {
    int code = NaifIkCode();
    string key = "INS" + Isis::iString(code) + "_PIXEL_PITCH";
    SetPixelPitch(Isis::Spice::GetDouble(key));
  }

  /**
   * Sets the right ascension declination
   *
   * @param ra
   *
   * @param dec
   *
   * @return bool Returns true if the declination was set successfully and false
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
   * Computes the RaDec range
   *
   * @param minra
   *
   * @param maxra
   *
   * @param mindec
   *
   * @param maxdec
   *
   * @return bool Returns true if the range computation was successful and false
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
   *
   * @return double The resutant RaDec resolution
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
   * @return double North Azimuth
   */
  double Camera::NorthAzimuth() {
    double lat = UniversalLatitude();
    if (lat >= 0.0) {
      return ComputeAzimuth(LocalRadius() / 1000.0, 90.0, 0.0);
    } else {
      double azimuth = ComputeAzimuth(LocalRadius() / 1000.0, -90.0, 0.0) + 180.0;
      if (azimuth > 360.0) azimuth = azimuth - 360.0;
      return azimuth;
    }
  }

  /**
   * Returns the Sun Azimuth
   *
   * @return double Sun Azimuth
   *
   * @todo Get appropriate radius at the subsolar point
   */
  double Camera::SunAzimuth() {
    double lat, lon;
    SubSolarPoint(lat, lon);
    return ComputeAzimuth(LocalRadius() / 1000.0, lat, lon);
  }

  /**
   * Return the Spacecraft Azimuth
   *
   * @return double Spacecraft Azimuth
   *
   * @todo Get appropriate radius at the subscraft point
   */
  double Camera::SpacecraftAzimuth() {
    double lat, lon;
    SubSpacecraftPoint(lat, lon);
    return ComputeAzimuth(LocalRadius() / 1000.0, lat, lon);
  }

  /**
   * Computes the Azimuth value at specified lat/lon with the entered radius
   *
   * @param radius The Radius
   *
   * @param lat The Latitude
   *
   * @param lon The Longitude
   *
   * @return double Azimuth value
   *
   * @history 2009-09-23  Tracie Sucharski - Convert negative
   *                         longitudes coming out of reclat.
   * @history 2010-09-28  Janet Barrett - Added Randy's updated method
   *                         for calculating the azimuth.
   *
   * @todo Write PushState and PopState method to ensure the
   * internals of the class are set based on SetImage or SetGround
   */
  double Camera::ComputeAzimuth(const double radius,
                                const double lat, const double lon) {
    if(!HasSurfaceIntersection()) return -1.0;

    bool computed = p_pointComputed;
    double originalSample = Sample();
    double originalLine = Line();

    NaifStatus::CheckErrors();

    // Convert the point to x/y/z in body-fixed
    SpiceDouble pB[3];
    latrec_c(radius, lon * Isis::PI / 180.0, lat * Isis::PI / 180.0, pB);

    // Get the origin point
    SpiceDouble oB[3];
    Coordinate(oB);

    // Get the difference unit vector
    SpiceDouble poB[3],upoB[3];
    vsub_c(pB, oB, poB);
    vhat_c(poB, upoB);

    // Scale to be within a pixel (km)
    double scale = (PixelResolution() / 1000.0) / 2.0;
    SpiceDouble hpoB[3];
    SpiceDouble spoB[3];
    vperp_c(upoB, oB, hpoB);
    vscl_c(scale, hpoB, spoB);

    // Compute the new point in body fixed.  This point will be within
    // a pixel of the origin but in the same direction as the
    // requested lat/lon
    SpiceDouble nB[3];
    vadd_c(oB, spoB, nB);

    // Get the origin image coordinate
    double osample = Sample();
    double oline = Line();

    // Convert the point to a lat/lon and find out its image coordinate
    double nrad, nlon, nlat;
    reclat_c(nB, &nrad, &nlon, &nlat);
    nlat = nlat * 180.0 / Isis::PI;
    nlon = nlon * 180.0 / Isis::PI;
    if(nlon < 0) nlon += 360.0;
    SetUniversalGround(nlat, nlon);
    double nsample = Sample();
    double nline = Line();

    // TODO:  Write PushState and PopState method to ensure the
    // internals of the class are set based on SetImage or SetGround
    SetImage(osample, oline);

    double deltaSample = nsample - osample;
    double deltaLine = nline - oline;

    // Compute the angle
    double azimuth = 0.0;
    if(deltaSample != 0.0 || deltaLine != 0.0) {
      azimuth = atan2(deltaLine, deltaSample);
      azimuth *= 180.0 / Isis::PI;
    }
    if(azimuth < 0.0) azimuth += 360.0;
    if(azimuth > 360.0) azimuth -= 360.0;

    NaifStatus::CheckErrors();

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
   * @return double Off Nadir Angle
   */
  double Camera::OffNadirAngle() {
    NaifStatus::CheckErrors();

    // Get the xyz coordinates for the spacecraft and point we are interested in
    double coord[3], spCoord[3];
    Coordinate(coord);
    InstrumentPosition(spCoord);

    // Get the angle between the 2 points and convert to degrees
    double a = vsep_c(coord, spCoord) * 180.0 / Isis::PI;
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
   * @return double The azimuth in degrees
   */
  double Camera::GroundAzimuth(double glat, double glon,
                               double slat, double slon) {
    double a = (90.0 - slat) * Isis::PI / 180.0;
    double b = (90.0 - glat) * Isis::PI / 180.0;
    double c = (glon - slon) * Isis::PI / 180.0;
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
    double azimuthA = A * 180.0 / Isis::PI + 90.0;
    double azimuthB = 90.0 - B * 180.0 / Isis::PI;
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
   * Computes and returns the distance between two latitude/longitude points in
   * meters, given the radius of the sphere.  The method uses the haversine
   * formula to compute the distance.
   *
   * @param lat1 The first latitude value
   * @param lon1 The first longitude value
   * @param lat2 The second latitude value
   * @param lon2 The second longitude value
   * @param radius The radius of the sphere (in meters)
   *
   * @return double The distance between the two points
   */
  double Camera::Distance(double lat1, double lon1,
                          double lat2, double lon2, double radius) {
    // Convert lat/lon values to radians
    double latRad1 = lat1 * Isis::PI / 180.0;
    double latRad2 = lat2 * Isis::PI / 180.0;
    double lonRad1 = lon1 * Isis::PI / 180.0;
    double lonRad2 = lon2 * Isis::PI / 180.0;

    double deltaLat = latRad2 - latRad1;
    double deltaLon = lonRad2 - lonRad1;
    double a = (sin(deltaLat / 2) * sin(deltaLat / 2)) + cos(latRad1) *
               cos(latRad2) * (sin(deltaLon / 2) * sin(deltaLon / 2));
    double c = 2 * atan(sqrt(a) / sqrt(1 - a));
    double dist = radius * c;
    return dist;
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
   * @param cacheSize The size of the spice cache. Should be >1 or not entered.
   *
   */
  void Camera::LoadCache(int cacheSize) {
    // We want to stay in unprojected space for this process
    bool projIgnored = p_ignoreProjection;
    p_ignoreProjection = true;

    double etStart = 0.0, etEnd = 0.0;

    for(int band = 1; band <= Bands(); band++) {
      SetBand(band);
      SetImage(0.5, 0.5);
      double etStartTmp = EphemerisTime();
      SetImage(p_alphaCube->BetaSamples() + 0.5, p_alphaCube->BetaLines() + 0.5);
      double etEndTmp = EphemerisTime();

      if(band == 1) {
        etStart = min(etStartTmp, etEndTmp);
        etEnd   = max(etStartTmp, etEndTmp);
      }

      etStart = min(etStart, min(etStartTmp, etEndTmp));
      etEnd   = max(etEnd,   max(etStartTmp, etEndTmp));
    }

    if(cacheSize <= 0) {
      // BetaLines() + 1 so we get at least 2 points for interpolation
      cacheSize = p_alphaCube->BetaLines() + 1;

      if(etStart == etEnd) {
        cacheSize = 1;
      }
    }

    if(etStart == -DBL_MAX || etEnd == -DBL_MAX) {
      string msg = "Unable to find time range for the spice kernels";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // Set a position in the image so that the PixelResolution can be calculated
    SetImage(p_alphaCube->BetaSamples() / 2, p_alphaCube->BetaLines() / 2);
    double tol = PixelResolution() / 100.; //meters/pix/100.

    if(tol < 0.) {
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

    Spice::CreateCache(etStart, etEnd, cacheSize, tol);

    SetEphemerisTime(etStart);

    // Reset to band 1
    SetBand(1);
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
   * @return bool Point was extrapolated
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

  //! Reads the ck frame id from the instrument kernel
  void Camera::SetCkFrameId() {
    int code = NaifIkCode();
    string key = "INS" + Isis::iString(code) + "_CK_FRAME_ID";
    try {
      p_ckFrameId = Isis::Spice::GetInteger(key, 0);
      p_ckwriteReady = true;
    }
    catch(iException &e) {
      p_ckwriteReady = false;
      e.Clear();
    }
  }

  //! Returns the ck frame id if defined
  int Camera::CkFrameId() {
    if(p_ckwriteReady) {
      return p_ckFrameId;
    }
    else {
      std::string msg = "Unable to find CK_FRAME_ID keyword for instrument";
      throw Isis::iException::Message(iException::Camera, msg, _FILEINFO_);
    }
  }


  //! Reads the ck reference frame id if known
  void Camera::SetCkReferenceId() {
    int code = NaifIkCode();
    string key = "INS" + Isis::iString(code) + "_CK_REFERENCE_ID";
    try {
      p_ckReferenceId = Isis::Spice::GetInteger(key);
      p_ckwriteReady = true;
    }
    catch(iException &e) {
      p_ckwriteReady = false;
      e.Clear();
    }
  }


  //! Returns the ck reference frame id if known
  int Camera::CkReferenceId() {
    if(p_ckwriteReady) {
      return p_ckReferenceId;
    }
    else {
      std::string msg = "Unable to find CK_REFERENCE_ID keyword for instrument";
      throw Isis::iException::Message(iException::Camera, msg, _FILEINFO_);
    }
  }
// end namespace isis
}
