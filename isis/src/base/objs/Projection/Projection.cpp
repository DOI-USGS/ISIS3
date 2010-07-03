/**
 * @file
 * $Revision: 1.10 $
 * $Date: 2009/12/28 20:56:01 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <cfloat>
#include <cmath>
#include <sstream>
#include <iomanip>
#include "naif/SpiceUsr.h"

#include "Projection.h"
#include "iException.h"
#include "Constants.h"
#include "Filename.h"

using namespace std;
namespace Isis {
 /**
  * Constructs an empty Projection object.
  *
  * @param label A PVL object containing map projection labels. These labels
  *              are fully described in the Isis Map Projection Users Guide.
  *              A brief example follows:
  *                   @code
  *                       Group = Mapping
  *                         EquatorialRadius = 3396190.0
  *                         PolarRadius = 3376200.0
  *                         LongitudeDirection = PositiveEast
  *                         LongitudeDomain = 360
  *                         LatitudeType = Planetographic
  *                         MinimumLatitude = 10.8920539924144
  *                         MaximumLatitude = 34.7603960060206
  *                         MinimumLongitude = 219.72432466275
  *                         MaximumLongitude = 236.186050244411
  *                         PixelResolution = 1387.31209461362
  *                         ProjectionName = SimpleCylindrical
  *                         CenterLongitude = 220.0
  *                       EndGroup
  *                     End
  *                   @endcode
  *
  * @throws Isis::iException::Projection
  */
  Projection::Projection (Isis::Pvl &label) : p_mappingGrp("Mapping") {
    try {
      // Try to read the mapping group
      p_mappingGrp = label.FindGroup ("Mapping",Isis::Pvl::Traverse);

      // Get the radii from the EquatorialRadius and PolarRadius keywords
      if ((p_mappingGrp.HasKeyword("EquatorialRadius")) &&
          (p_mappingGrp.HasKeyword("PolarRadius"))) {
        p_equatorialRadius = p_mappingGrp["EquatorialRadius"];
        p_polarRadius = p_mappingGrp["PolarRadius"];
      }
      // Get the radii using the "TargetName" keyword and NAIF
      else  if (p_mappingGrp.HasKeyword("TargetName")) {
        PvlGroup radii = TargetRadii ((string)p_mappingGrp["TargetName"]);
        p_equatorialRadius = radii["EquatorialRadius"];
        p_polarRadius = radii["PolarRadius"];
      }
      else {
        string message = "No target radii available through keywords ";
        message += "[EquatorialRadius and PolarRadius] or [TargetName].";
        throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
      }

      // Check the radii for validity
      if (p_equatorialRadius <= 0.0) {
        string message = "Invalid value for keyword [EquatorialRadius] it must be ";
        message += "greater than zero";
        throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
      }
      if (p_polarRadius <= 0.0) {
        string message = "Invalid value for keyword [PolarRadius] it must be ";
        message += "greater than zero";
        throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
      }

      // Get the LatitudeType
      if ((string) p_mappingGrp["LatitudeType"] == "Planetographic") {
        p_latitudeType = Planetographic;
      }
      else if ((string) p_mappingGrp["LatitudeType"] == "Planetocentric") {
        p_latitudeType = Planetocentric;
      }
      else {
        string message = "Invalid value for keyword [LatitudeType] must be ";
        message += "[Planetographic or Planetocentric]";
        throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
      }

      // Get the LongitudeDirection
      if ((string) p_mappingGrp["LongitudeDirection"] == "PositiveWest") {
        p_longitudeDirection = PositiveWest;
      }
      else if ((string) p_mappingGrp["LongitudeDirection"] == "PositiveEast") {
        p_longitudeDirection = PositiveEast;
      }
       else {
        string message = "Invalid value for keyword [LongitudeDirection] must be ";
        message += "[PositiveWest or PositiveEast]";
        throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
      }

      // Get the LongitudeDomain
      if ((string) p_mappingGrp["LongitudeDomain"] == "360") {
        p_longitudeDomain = 360;
      }
      else if ((string) p_mappingGrp["LongitudeDomain"] == "180") {
        p_longitudeDomain = 180;
      }
       else {
        string message = "Invalid value for keyword [LongitudeDomain] must be ";
        message += "[180 or 360]";
        throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
      }

      // Get the ground range if it exists
      p_groundRangeGood = false;
      if ((p_mappingGrp.HasKeyword("MinimumLatitude")) &&
          (p_mappingGrp.HasKeyword("MaximumLatitude")) &&
          (p_mappingGrp.HasKeyword("MinimumLongitude")) &&
          (p_mappingGrp.HasKeyword("MaximumLongitude"))) {
        p_minimumLatitude  = p_mappingGrp["MinimumLatitude"];
        p_maximumLatitude  = p_mappingGrp["MaximumLatitude"];
        p_minimumLongitude = p_mappingGrp["MinimumLongitude"];
        p_maximumLongitude = p_mappingGrp["MaximumLongitude"];

        if ((p_minimumLatitude < -90.0) || (p_minimumLatitude > 90.0)) {
          string msg = "[MinimumLatitude] of [" + iString(p_minimumLatitude);
          msg += "] is outside the range of [-90:90]";
          throw Isis::iException::Message(Isis::iException::Projection,msg,_FILEINFO_);
        }

        if ((p_maximumLatitude < -90.0) || (p_maximumLatitude > 90.0)) {
          string msg = "[MaximumLatitude] of [" + iString(p_maximumLatitude);
          msg += "] is outside the range of [-90:90]";
          throw Isis::iException::Message(Isis::iException::Projection,msg,_FILEINFO_);
        }

        if (p_minimumLatitude >= p_maximumLatitude) {
          string msg = "[MinimumLatitude,MaximumLatitude] of [" + iString(p_minimumLatitude);
          msg += + "," + iString(p_maximumLatitude) + "] are not ";
          msg += "properly ordered";
          throw Isis::iException::Message(Isis::iException::Projection,msg,_FILEINFO_);
        }

        if (p_minimumLongitude >= p_maximumLongitude) {
          string msg = "[MinimumLongitude,MaximumLongitude] of [" + iString(p_minimumLongitude);
          msg += + "," + iString(p_maximumLongitude) + "] are not ";
          msg += "properly ordered";
          throw Isis::iException::Message(Isis::iException::Projection,msg,_FILEINFO_);
        }

        p_groundRangeGood = true;
      }

      // Get the map rotation
      p_rotation = 0.0;
      if (p_mappingGrp.HasKeyword("Rotation")) {
        p_rotation = p_mappingGrp["Rotation"];
      }


      // Initialize miscellaneous protected data elements
      p_good = false;

      p_minimumX = DBL_MAX;
      p_maximumX = -DBL_MAX;
      p_minimumY = DBL_MAX;
      p_maximumY = -DBL_MAX;

      p_eccentricity = 1.0 -
                       (p_polarRadius * p_polarRadius) /
                       (p_equatorialRadius * p_equatorialRadius);
      p_eccentricity = sqrt(p_eccentricity);

      p_mapper = NULL;

      p_sky = false;
      if (p_mappingGrp.HasKeyword("TargetName")) {
        iString str = (string) p_mappingGrp["TargetName"];
        if (str.UpCase() == "SKY") p_sky = true;
      }
    }
    catch (Isis::iException &e) {
      string message = "Invalid label group [Mapping]";
      throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
    }
  }

 /**
  * This method is used to set a world coordinate. A world coordinate is a
  * different coordinate type that has a one-to-one mapping to the projection
  * coordinate system. For example, mapping pixel samples and lines to
  * projection x's and y's. The Set forces an attempted calculation of the
  * corresponding latitude/longitude position. This may or may not be successful
  * and a status is returned as such. Note that is only applies if the
  * Projection object was given an WorldMapper object during construction. If
  * an WorldMapper was not supplied then SetWorld operates exactly the same as
  * SetCoordinate (impling that world coordinate and projection coordinate are
  * identical).
  *
  * @param worldX World X coordinate in units that are specified by the
  *               WorldMapper object (e.g., pixels, millimeters, etc)
  *
  * @param worldY World Y coordinate in units that are specified by the
  *               WorldMapper object (e.g., pixels, millimeters, etc)
  *
  * @return bool
  */
  bool Projection::SetWorld(const double worldX, const double worldY) {
    double projectionX;
    double projectionY;

    if (p_mapper != NULL) {
      projectionX = p_mapper->ProjectionX(worldX);
      projectionY = p_mapper->ProjectionY(worldY);
    }
    else {
      projectionX = worldX;
      projectionY = worldY;
    }

    return SetCoordinate(projectionX,projectionY);
  }

 /**
  * This returns the world X coordinate provided SetGround, SetCoordinate,
  * SetUniversalGround, or SetWorld returned with success. Success can also be
  * checked using the IsGood method. The units of X will be in the units as
  * specified by the WorldMapper object which was given to the SetWorldMapper
  * method. If a mapper object was not given then world coordinates are the same
  * as the projection coordinates (i.e., WorldX and XCoord will return the same
  * value).
  *
  * @return double
  */
  double Projection::WorldX () const {
    if (p_mapper != NULL) {
      return p_mapper->WorldX(p_x);
    }
    else {
      return p_x;
    }
  }

 /**
  * This returns the world Y coordinate provided SetGround, SetCoordinate,
  * SetUniversalGround, or SetWorld returned with success. Success can also be
  * checked using the IsGood method. The units of Y will be in the units as
  * specified by the WorldMapper object which was given to the SetWorldMapper.
  * If a mapper object was not given then world coordinates are the same as the
  * projection coordinates (i.e., WorldY and YCoord will return the same value).
  *
  * @return double
  */
  double Projection::WorldY () const {
    if (p_mapper != NULL) {
      return p_mapper->WorldY(p_y);
    }
    else {
      return p_y;
    }
  }

 /**
  * This returns a universal longitude (positive east in 0 to 360 domain). The
  * method can only be used if SetGround, SetCoordinate, SetUniversalGround, or
  * SetWorld return with success. Success can also be checked using the IsGood
  * method.
  *
  * @return double
  */
  double Projection::UniversalLongitude() {
    double lon = p_longitude;
    if (p_longitudeDirection == PositiveWest) lon = -lon;
    lon = To360Domain(lon);
    return lon;
  }

 /**
  * This returns a universal latitude (planetocentric). The method can only be
  * used if SetGround, SetCoordinate, SetUniversalGround, or SetWorld return
  * with success. Success can also be checked using the IsGood method.
  *
  * @return double
  */
  double Projection::UniversalLatitude() {
    double lat = p_latitude;
    if (p_latitudeType == Planetographic) lat = ToPlanetocentric(lat);
    return lat;
  }

 /**
  * This method is used to set the latitude/longitude which must be
  * Planetocentric (latitude) and PositiveEast/Domain360 (longitude). The Set
  * forces an attempted calculation of the projection X/Y values. This may or
  * may not be successful and a status is returned as such.
  *
  * @param lat Planetocentric Latitude value to project
  *
  * @param lon PositiveEast, Domain360 Longitude value to project
  *
  * @return bool
  */
  bool Projection::SetUniversalGround (const double lat, const double lon) {
    // Deal with the longitude first
    p_longitude = lon;
    if (p_longitudeDirection == PositiveWest) p_longitude = -lon;
    if (p_longitudeDomain == 180) {
      p_longitude = To180Domain(p_longitude);
    }
    else {
      // Do this because longitudeDirection could cause (-360,0)
      p_longitude = To360Domain(p_longitude);
    }

    // Deal with the latitude
    if (p_latitudeType == Planetographic) {
      p_latitude = ToPlanetographic(lat);
    }
    else {
      p_latitude = lat;
    }

    // Now the lat/lon are in user defined coordinates so set them
    return SetGround (p_latitude,p_longitude);
  }

   /**
    * This method converts a planetocentric latitude to a planetographic
    * latitude. It utilizes the equatorial and polar radii to perform the
    * computation.
    *
    * @param lat Planetocentric latitude to convert.
    *
    * @return double
    */
    double Projection::ToPlanetographic(const double lat) const {
      double mylat = lat;
      if (abs(mylat) < 90.0) { // So tan doesn't fail
        mylat *= Isis::PI / 180.0;
        mylat = atan (tan(mylat) * (p_equatorialRadius / p_polarRadius) *
                                   (p_equatorialRadius / p_polarRadius));
        mylat *= 180.0 / Isis::PI;
      }
      return mylat;
    }

  /**
   * This method converts a planetocentric latitude to a planetographic
   * latitude. It is static so that a projection object does not need to
   * exist.
   *
   * @param lat Planetocentric latitude to convert.
   * @param eRadius Equatorial radius.
   * @param pRadius Polar radius
   *
   * @return double
   */
  double Projection::ToPlanetographic(double lat,
                                      double eRadius, double pRadius) {
    double mylat = lat;
    if (abs(mylat) < 90.0) { // So tan doesn't fail
      mylat *= Isis::PI / 180.0;
      mylat = atan (tan(mylat) * (eRadius / pRadius) *
                                 (eRadius / pRadius));
      mylat *= 180.0 / Isis::PI;
    }
    return mylat;
  }

 /**
  * This method converts a planetographic latitude to a planetocentric latitude.
  * It utilizes the equatorial and polar radii to perform the computation.
  *
  * @param lat Planetographic latitude to convert.
  *
  * @return double
  */
  double Projection::ToPlanetocentric(const double lat) const {
    double mylat = lat;
    if (abs(mylat) < 90.0) { // So tan doesn't fail
      mylat *= Isis::PI / 180.0;
      mylat = atan (tan(mylat) * (p_polarRadius / p_equatorialRadius) *
                                 (p_polarRadius / p_equatorialRadius));
      mylat *= 180.0 / Isis::PI;
    }
    return mylat;
  }

 /**
  * This method converts a planetographic latitude to a planetocentric latitude.
  *
  * @param lat Planetographic latitude to convert.
  *
  * @return double
  */
  double Projection::ToPlanetocentric(const double lat,
                                      double eRadius, double pRadius) {
    double mylat = lat;
    if (abs(mylat) < 90.0) { // So tan doesn't fail
      mylat *= Isis::PI / 180.0;
      mylat = atan (tan(mylat) * (pRadius / eRadius) *
                                 (pRadius / eRadius));
      mylat *= 180.0 / Isis::PI;
    }
    return mylat;
  }

 /**
  * This method converts a longitude into the 0 to 360 domain. It will leave the
  * longitude unchanged if it is already in the domain.
  *
  * @return double
  *
  * @param lon Longitude to convert into the 0 to 360 domain.
  */
  double Projection::To360Domain(const double lon) {
    double mylon = lon;
    while (mylon < 0.0) mylon += 360.0;
    while (mylon > 360.0) mylon -= 360.0;
    return mylon;
  }

 /**
  * This method converts a longitude into the -180 to 180 domain. It will leave
  * the longitude unchanged if it is already in the domain.
  *
  * @return double
  *
  * @param lon Longitude to convert into the -180 to 180 domain.
  */
  double Projection::To180Domain(const double lon) {
    double mylon = lon;
    while (mylon < -180.0) mylon += 360.0;
    while (mylon > 180.0) mylon -= 360.0;
    return mylon;
  }

 /**
  * This method converts a longitude into the positive east direction.
  *
  * @return double
  *
  * @param lon Longitude to convert into the positive east direction.
  */
  double Projection::ToPositiveEast(const double lon, const int domain) {
    double mylon = lon;

    mylon *= -1;

    if(domain == 360) {
      mylon = To360Domain(mylon);
    }
    else if(domain == 180) {
      mylon = To180Domain(mylon);
    }
    else {
      iString err = "Domain [";
      err += domain;
      err += "] is not 180 or 360.";
      throw iException::Message(iException::Programmer, err, _FILEINFO_);
    }

    return mylon;
  }

 /**
  * This method converts a longitude into the positive west direction.
  *
  * @return double
  *
  * @param lon Longitude to convert into the positive west direction.
  */
  double Projection::ToPositiveWest(const double lon, const int domain) {
    double mylon = lon;

    mylon *= -1;

    if(domain == 360) {
      mylon = To360Domain(mylon);
    }
    else if(domain == 180) {
      mylon = To180Domain(mylon);
    }
    else {
      iString err = "Domain [";
      err += domain;
      err += "] is not 180 or 360.";
      throw iException::Message(iException::Programmer, err, _FILEINFO_);
    }

    return mylon;
  }

 /**
  * This convience function is established to assist in the development of the
  * XYRange virtual method. It allows the developer to test ground points
  * (lat/lon) to see if they produce a minimum/maximum projection coordinate.
  * For example in Sinusoidal,
  *    @code
  *       bool Sinusoidal::XYRange(double &minX, double &maxX,
  *                                    double &minY, double &maxY) {
  *        // Check the corners of the lat/lon range
  *         XYRangeCheck (p_minimumLatitude,p_minimumLongitude);
  *         XYRangeCheck (p_maximumLatitude,p_minimumLongitude);
  *         XYRangeCheck (p_minimumLatitude,p_maximumLongitude);
  *         XYRangeCheck (p_maximumLatitude,p_maximumLongitude);
  *
  *         // If the latitude crosses the equator check there
  *         if ((p_minimumLatitude < 0.0) && (p_maximumLatitude > 0.0)) {
  *           XYRangeCheck (0.0,p_minimumLongitude);
  *           XYRangeCheck (0.0,p_maximumLongitude);
  *         }
  *
  *         // Make sure everything is ordered
  *         if (p_minimumX >= p_maximumX) return false;
  *         if (p_minimumY >= p_maximumY) return false;
  *
  *         // Return X/Y min/maxs
  *         minX = p_minimumX;
  *         maxX = p_maximumX;
  *         minY = p_minimumY;
  *         maxY = p_maximumY;
  *         return true;
  *      }
  *    @endcode
  *
  *
  * @param latitude Test for min/max projection coordinates at this latitude
  *
  * @param longitude Test for min/max projection coordinates at this longitude
  */
  void Projection::XYRangeCheck(const double latitude, const double longitude) {
    SetGround(latitude,longitude);
    if (!IsGood()) return;

    if (p_x < p_minimumX) p_minimumX = p_x;
    if (p_x > p_maximumX) p_maximumX = p_x;
    if (p_y < p_minimumY) p_minimumY = p_y;
    if (p_y > p_maximumY) p_maximumY = p_y;
  }

 /**
  * This method returns the latitude type as a string. It will return either
  * Planetocentric or Planetographic.
  *
  * @return string
  */
  string Projection::LatitudeTypeString() const {
    if (p_latitudeType == Planetographic) return "Planetographic";
    return "Planetocentric";
  }

  /**
   * This method returns the longitude direction as a string. It will return
   * either PositiveEast or PositiveWest.
   *
   * @return string
   */
   string Projection::LongitudeDirectionString() const {
    if (p_longitudeDirection == PositiveEast) return "PositiveEast";
    return "PositiveWest";
  }

  /**
   * This method returns the longitude domain as a string. It will return either
   * 180 or 360.
   *
   * @return string
   */
   string Projection::LongitudeDomainString() const {
    if (p_longitudeDomain == 360) return "360";
    return "180";
  }

 /**
  * This method converts a projection x value to a world x value. For example,
  * if the world coordinate system is an image then this method converts a
  * projection x to a sample position. Note that if SetWorldMapper is not used
  * then this routine simply returns the value of the argument. That is, no
  * mapping occurs.
  *
  * @param projectionX Projection x value in meters
  *
  * @return double
  */
  double Projection::ToWorldX (const double projectionX) const {
    if (p_mapper != NULL) {
      return p_mapper->WorldX(projectionX);
    }
    else {
      return projectionX;
    }
  }

 /**
  * This method converts a projection y value to a world y value. For example,
  * if the world coordinate system is an image then this method converts a
  * projection y to a line position. Note that if SetWorldMapper is not used
  * then this routine simply returns the value of the argument. That is, no
  * mapping occurs.
  *
  * @param projectionY Projection y value in meters
  *
  * @return double
  */
  double Projection::ToWorldY (const double projectionY) const {
    if (p_mapper != NULL) {
      return p_mapper->WorldY(projectionY);
    }
    else {
      return projectionY;
    }
  }

 /**
  * This method converts a world x value to a projection x value. For example,
  * if the world coordinate system is an image then this method converts a
  * sample position to a projection x value. Note that if SetWorldMapper is not
  * used then this routine simply returns the value of the argument. That is,
  * no mapping occurs.
  *
  * @param worldX World x coordinate
  *
  * @return double
  */
  double Projection::ToProjectionX (const double worldX) const {
    if (p_mapper != NULL) {
      return p_mapper->ProjectionX(worldX);
    }
    else {
      return worldX;
    }
  }

 /**
  * This method converts a world y value to a projection y value. For example,
  * if the world coordinate system is an image then this method converts a line
  * position to a projection y value. Note that if SetWorldMapper is not used
  * then this routine simply returns the value of the argument. That is, no
  * mapping occurs.
  *
  * @param worldY World y coordinate
  *
  * @return double
  */
  double Projection::ToProjectionY (const double worldY) const {
    if (p_mapper != NULL) {
      return p_mapper->ProjectionY(worldY);
    }
    else {
      return worldY;
    }
  }

 /**
  * This method returns the resolution for mapping world coordinates into
  * projection coordinates. For example, if the world coordinate system is an
  * image then this routine returns the number of meters per pixel. Likewise,
  * if the world coordinate system is a piece of paper, it might return the
  * number of meters per inch of paper. If the SetWorldMapper method is not
  * invoked then this method returns 1.0
  *
  * @return double
  */
  double Projection::Resolution () const {
    if (p_mapper != NULL) {
      return p_mapper->Resolution();
    }
    else {
      return 1.0;
    }
  }

 /**
  * This method returns if two map projection objects are equal. That is, they
  * have the same radii, latitude type, longitude direction, projection name,
  * and projection specific parameters.
  *
  * @return bool
  */
  bool Projection::operator== (const Projection &proj) {
    if (this->EquatorialRadius() != proj.EquatorialRadius()) return false;
    if (this->PolarRadius() != proj.PolarRadius()) return false;
    if (this->IsPlanetocentric() != proj.IsPlanetocentric()) return false;
    if (this->IsPositiveWest() != proj.IsPositiveWest()) return false;
    if (this->Resolution() != proj.Resolution()) return false;
    if (this->Name() != proj.Name()) return false;
    return true;
  }

 /**
  * This method returns if two map projection objects are not equal. That is, they have at least
  * some differences in the radii, latitude type, longitude direction, projection name, and
  * projection specific parameters.
  *
  * @return bool
  */
  bool Projection::operator!= (const Projection &proj) {
    return !(*this == proj);
  }

 /**
  * This method returns the scale for mapping world coordinates into projection
  * coordinates. For example, if the world coordinate system is an image then
  * this routine returns the number of pixels per degree. Likewise, if the
  * world coordinate system is a piece of paper, it might return the number of
  * inches of paper per degree. If the SetWorldMapper method is not invoked
  * then this method returns 1.0
  *
  * @return double
  */
  double Projection::Scale () const {
    if (p_mapper != NULL) {
      double lat = TrueScaleLatitude() * Isis::PI / 180.0;
      double a = p_polarRadius * cos(lat);
      double b = p_equatorialRadius * sin(lat);
      double localRadius = p_equatorialRadius * p_polarRadius /
                           sqrt(a*a + b*b);

      return localRadius / p_mapper->Resolution();
    }
    else {
      return 1.0;
    }
  }

 /**
  * This method returns the latitude of true scale. It is a virtual function and
  * if it is not overriden the latitude of true scale is 0. Otherwise it is
  * projection specific.For example, the center latitude for Mercator,
  * Equidistant, or a parallel for conic projections. This method is used by the
  * Scale routine to ensure the local radius is used in the computation.
  *
  * @return double
  */
  double Projection::TrueScaleLatitude () const {
    return 0.0;
  }

  /**
   * This function returns the keywords that this projection uses.
   * 
   * @return PvlGroup The keywords that this projection uses
   */
  PvlGroup Projection::Mapping() {
    PvlGroup mapping("Mapping");

    if(p_mappingGrp.HasKeyword("TargetName")) {
      mapping += p_mappingGrp["TargetName"];
    }

    mapping += p_mappingGrp["ProjectionName"];
    mapping += p_mappingGrp["EquatorialRadius"];
    mapping += p_mappingGrp["PolarRadius"];
    mapping += p_mappingGrp["LatitudeType"];
    mapping += p_mappingGrp["LongitudeDirection"];
    mapping += p_mappingGrp["LongitudeDomain"];

    if(HasGroundRange()) {
      mapping += p_mappingGrp["MinimumLatitude"];
      mapping += p_mappingGrp["MaximumLatitude"];
      mapping += p_mappingGrp["MinimumLongitude"];
      mapping += p_mappingGrp["MaximumLongitude"];
    }

    if(p_mappingGrp.HasKeyword("Rotation")) {
      mapping += p_mappingGrp["Rotation"];
    }

    return mapping;
  }

  /**
   * This function returns the latitude keywords that this projection uses
   * 
   * @return PvlGroup The latitude keywords that this projection uses
   */
  PvlGroup Projection::MappingLatitudes() {
    PvlGroup mapping("Mapping");

    if(HasGroundRange()) {
      mapping += p_mappingGrp["MinimumLatitude"];
      mapping += p_mappingGrp["MaximumLatitude"];
    }

    return mapping;
  }

  /**
   * This function returns the longitude keywords that this projection uses
   * 
   * @return PvlGroup The longitude keywords that this projection uses
   */
  PvlGroup Projection::MappingLongitudes() {
    PvlGroup mapping("Mapping");

    if(HasGroundRange()) {
      mapping += p_mappingGrp["MinimumLongitude"];
      mapping += p_mappingGrp["MaximumLongitude"];
    }

    return mapping;
  }

 /**
  * This method is used to set the latitude/longitude (assumed to be of the
  * correct LatitudeType, LongitudeDirection, and LongitudeDomain. The Set
  * forces an attempted calculation of the projection X/Y values. This may or
  * may not be successful and a status is returned as such. Usually this method
  * is overridden in a dervied class, for example, Sinusoidal. If not the
  * default method simply copies lat/lon to x/y which is no projection.
  *
  * @param lat Latitude value to project
  *
  * @param lon Longitude value to project
  *
  * @return bool
  */
  bool Projection::SetGround (const double lat, const double lon) {
    p_latitude = lat;
    p_longitude = lon;
    SetComputedXY(lon,lat);
    return true;
  }

 /**
  * This method is used to set the projection x/y. The Set forces an attempted
  * calculation of the corresponding latitude/longitude position. This may or
  * may not be successful and a status is returned as such.  Usually this method
  * is overridden in a dervied class, for example, Sinusoidal. If not the
  * default method simply copies x/y to  lat/lon to x/y which is no projection.
  *
  * @param x X coordinate of the projection in units that are the same as the
  *          radii in the label
  *
  * @param y Y coordinate of the projection in units that are the same as the
  *          radii in the label
  *
  * @return bool
  */
  bool Projection::SetCoordinate (const double x, const double y) {
    SetXY(x,y);
    p_latitude = p_y;
    p_longitude = p_x;
    return true;
  }

 /**
  * This method is used to determine the x/y range which completely covers the
  * area of interest specified by the lat/lon range. The latitude/longitude
  * range may be obtained from the labels. This method should not be used if
  * HasGroundRange is false. The purpose of this method is to return the x/y
  * range so it can be used to compute how large a map may need to be. For
  * example, how big a piece of paper is needed or how large of an image needs
  * to be created. This is method and therefore must be written by the derived
  * class (e.g., Sinusoidal). The method may fail as indicated by its return
  * value.
  *
  * @param minX Minimum x projection coordinate which covers the latitude
  *             longitude range specified in the labels.
  *
  * @param maxX Maximum x projection coordinate which covers the latitude
  *             longitude range specified in the labels.
  *
  * @param minY Minimum y projection coordinate which covers the latitude
  *             longitude range specified in the labels.
  *
  * @param maxY Maximum y projection coordinate which covers the latitude
  *             longitude range specified in the labels.
  *
  * @return bool
  */
  bool Projection::XYRange (double &minX,double &maxX,double &minY,double &maxY) {
    if (p_groundRangeGood) {
      minX = p_minimumLongitude;
      maxX = p_maximumLongitude;
      minY = p_minimumLatitude;
      maxY = p_maximumLatitude;
      return true;
    }
    return false;
  }

 /**
  * This method returns the local radius in meters at the current latitude
  * position. This is only usable if the use of SetGround or SetCoordinate was
  * successful.
  *
  * @return double
  */
  double Projection::LocalRadius () const {
    return LocalRadius(p_latitude);
  }

 /**
  * This method returns the local radius in meters at the specified latitude
  * position.
  *
  * @param latitude A latitude in degrees (assumed to be of the correct
  *                 LatitudeType).
  *
  * @return double
  */
  double Projection::LocalRadius (double latitude) const {
    double a = p_equatorialRadius;
    double c = p_polarRadius;
    double lat = latitude * Isis::PI / 180.0;
    return  a*c / sqrt(pow(c*cos(lat),2) + pow(a*sin(lat),2));
  }

 /**
  * A convience method to compute m = cosphi/sqrt(1-(ecc*sinphi)**2)
  *
  * @param sinphi sine of phi
  *
  * @param cosphi cosine of phi
  *
  * @return double
  */
  double Projection::mCompute(const double sinphi, const double cosphi) const {
    double eccTimesSinphi = Eccentricity() * sinphi;
    double denominator = sqrt(1.0 - eccTimesSinphi * eccTimesSinphi);
    return cosphi / denominator;
  }

 /**
  * A convience method to compute e4 = sqrt(1+ecc)**(1+ecc)*(1-ecc)**(1-ecc))
  *
  * @return double
  */
  double Projection::e4Compute() const {
    double onePlusEcc = 1.0 + Eccentricity();
    double oneMinusEcc = 1.0 - Eccentricity();

    return sqrt(pow(onePlusEcc,onePlusEcc) *
                pow(oneMinusEcc,oneMinusEcc));
  }

 /**
  * A convience method to compute:
  * t = tan(.5*(.5PI-phi)/((1-ecc*sinphi)/(1+ecc*sinphi))**(.5*ecc).
  *
  * @param phi  phi
  *
  * @param sinphi sin of phi
  *
  * @return double
  */
  double Projection::tCompute(const double phi, const double sinphi) const {
    if ((Isis::HALFPI) - fabs(phi) < DBL_EPSILON) return 0.0;

    double eccTimesSinphi = Eccentricity() * sinphi;
    double denominator  = pow( (1.0 - eccTimesSinphi) /
                               (1.0 + eccTimesSinphi),
                               0.5*Eccentricity() );
    return tan(0.5 * (Isis::HALFPI - phi)) / denominator;
  }

 /**
  * A convience method to compute latitude angle phi2 given small t
  *
  * @param t small t
  *
  * @return double
  */
  double Projection::phi2Compute(const double t) const {
    double localPhi = Isis::HALFPI - 2.0 * atan(t);
    double halfEcc = 0.5 * Eccentricity();
    double difference = DBL_MAX;
    int iteration = 0;

    while ((iteration < 15) && (difference > 0.0000000001))  {
      double eccTimesSinphi = Eccentricity() * sin(localPhi);
      double newPhi = Isis::HALFPI -
                      2.0 * atan(t * pow((1.0-eccTimesSinphi) /
                                         (1.0+eccTimesSinphi), halfEcc));
      difference = fabs(newPhi - localPhi);
      localPhi = newPhi;
      iteration++;
    }

    if (iteration >= 15) {
      string msg = "Failed to converge in Projection::tCompute";
      throw Isis::iException::Message(Isis::iException::Projection,msg,_FILEINFO_);
    }

    return localPhi;
  }

 /**
  * Converts angle(in degrees) to hours
  *
  * @param angle Angle in degrees to be converted to hours
  *
  * @return double
  */
  double Projection::ToHours(double angle) {
     return angle/15.0;
  }

 /**
  * Converts angle(in degrees) to degrees, minutes, seconds. Outputs in
  * the form xxx yym zz.zzzs, for example, 206.291 degrees is
  * 206 17m 27.6s
  *
  * @param angle Angle in degrees to be converted to degrees, minutes, seconds
  *
  * @return string Degrees, minutes, seconds
  */
  string Projection::ToDMS(double angle) {
    int iangle = (int)angle;
    double mins = abs(angle-iangle)*60.0;
    int imins = (int)mins;
    double secs = (mins-imins)*60.0;
    int isecs = (int)secs;
    double frac = (secs-isecs)*1000.0;
    if (frac >= 1000.0) {
      frac-=1000.0;
      isecs++;
    }
    if (isecs >= 60) {
      isecs-=60;
      imins++;
    }
    if (imins >= 60) {
      imins-=60;
      iangle++;
    }
    stringstream s;
    s << iangle << " " << setw(2) << setfill('0')
      << imins << "m " << setw(2) << setfill('0') << isecs << "."  <<
      setprecision(3) << frac << "s";
    return s.str();
  }

 /**
  * Converts angle(in degrees) to hours, minutes, seconds. Outputs in
  * the form xxh yym zz.zzzs  For example, 206.291 will be
  * 13h 45m 09.84s
  *
  * @param angle Angle in degrees to be converted to hours, minutes, seconds
  *
  * @return string Hours, minutes, seconds
  */
  string Projection::ToHMS(double angle) {
    double tangle = angle;
    while (tangle < 0.0) tangle += 360.0;
    while (tangle > 360.0) tangle -=360.0;
    double hrs = ToHours(tangle);
    int ihrs = (int)(hrs);
    double mins = (hrs-ihrs)*60.0;
    int imins = (int)(mins);
    double secs = (mins-imins)*60.0;
    int isecs = (int)(secs);
    double msecs = (secs-isecs)*1000.0;
    int imsecs = (int)(msecs + 0.5);
    if (imsecs >= 1000) {
      imsecs-=1000;
      isecs++;
    }
    if (isecs >= 60) {
      isecs-=60;
      imins++;
    }
    if (imins >= 60) {
      imins-=60;
      ihrs++;
    }
    stringstream s;
    s << setw(2) << setfill('0') << ihrs << "h " << setw(2) << setfill('0') <<
      imins << "m " <<setw(2) << setfill('0') << isecs << "." << imsecs << "s";
    return s.str();
  }

  /**
   * Creates a Pvl Group with keywords TargetName,
   * EquitorialRadius, and PolarRadius. The values for the radii
   * will be retrieved from the most recent Target Attitude and
   * Shape Naif kernel available in the Isis data area.
   *
   * @param target The name of the body for which the radii will
   *             be retrieved.
   *
   * @return PvlGroup Group named "Mapping" with keywords
   *         TargetName, EquatorialRadius, and PolarRadius.
   */

  PvlGroup Projection::TargetRadii (std::string target) {
    // Convert the target name to a NAIF code
    SpiceInt code;
    SpiceBoolean found;
    bodn2c_c (target.c_str(), &code, &found);
    if (!found) {
      string msg = "Could not convert target name [" + target + "] to NAIF code";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }

    // Load the most recent target attitude and shape kernel for NAIF
    Filename kern("$Base/kernels/pck/pck?????.tpc");
    kern.HighestVersion();
    string kernName(kern.Expanded());
    furnsh_c (kernName.c_str());

    // Get the radii from NAIF
    SpiceInt n;
    SpiceDouble radii[3];
    bodvar_c (code, "RADII", &n, radii);
    unload_c (kernName.c_str());

    PvlGroup mapping ("Mapping");
    mapping += PvlKeyword ("TargetName",  target);
    mapping += PvlKeyword ("EquatorialRadius",  radii[0]*1000.0, "meters");
    mapping += PvlKeyword ("PolarRadius", radii[2]*1000.0, "meters");

    return mapping;
  }


  /**
   * Convenience method 
   * See method above for more details. 
   * 
   * 
   * @param cubeLab 
   * @param mapGroup 
   * 
   * @return PvlGroup 
   */
  PvlGroup Projection::TargetRadii(Pvl &cubeLab, PvlGroup &mapGroup){
    //Check to see if the mapGroup already has the target radii.
    //If BOTH radii are already in the mapGroup then just return back the mapGroup.
    if(mapGroup.HasKeyword("EquatorialRadius") && mapGroup.HasKeyword("PolarRadius")) {
      return mapGroup;
    }
    //If the mapping group only has one or the other of the radii keywords, then
    //we are going to replace both, so delete which ever one it does have.
    if(mapGroup.HasKeyword("EquatorialRadius") && !mapGroup.HasKeyword("PolarRadius")) {
      mapGroup.DeleteKeyword("EquatorialRadius");
    }
    if(!mapGroup.HasKeyword("EquatorialRadius") && mapGroup.HasKeyword("PolarRadius")) {
      mapGroup.DeleteKeyword("PolarRadius");
    }

    PvlGroup inst = cubeLab.FindGroup("Instrument", Pvl::Traverse);
    string target = inst["TargetName"];
    PvlGroup radii = Projection::TargetRadii(target);
    //Now INSERT the EquatorialRadius and PolorRadius into the mapGroup pvl.
    mapGroup += PvlKeyword ("EquatorialRadius",  radii.FindKeyword("EquatorialRadius")[0], "meters");
    mapGroup += PvlKeyword ("PolarRadius", radii.FindKeyword("PolarRadius")[0], "meters");

    return mapGroup;
  }

  /**
   * This protected method is a helper for derived classes.  It
   * takes an unrotated x,y and rotates using p_rotation storing
   * the results in p_x and p_y.
   *
   * @param x unrotated x coordinate
   * @param y unrotated y coordinate
   */
  void Projection::SetComputedXY (double x, double y) {
    if (p_rotation == 0.0) {
      p_x = x;
      p_y = y;
    }
    else {
      double rot = p_rotation * Isis::PI / 180.0;
      p_x = x*cos(rot) + y*sin(rot);
      p_y = y*cos(rot) - x*sin(rot);
    }
  }

  /**
   * This protected method is a helper for derived classes.  It
   * takes a rotated x,y and stores them in p_x and p_y.
   *
   * @param x rotated x coordinate
   * @param y rotated y coordinate
   */
  void Projection::SetXY (double x, double y) {
    p_x = x;
    p_y = y;
  }

  /**
   * Return the unrotated form of p_x
   *
   * @return double
   */
  double Projection::GetX () const {
    if (p_rotation == 0.0) return p_x;
    double rot = p_rotation * Isis::PI / 180.0;
    return p_x*cos(rot) - p_y*sin(rot);
  }

  /**
   * Returh the unrotated form of p_y
   *
   * @return double
   */
  double Projection::GetY () const {
    if (p_rotation == 0.0) return p_y;
    double rot = p_rotation * Isis::PI / 180.0;
    return p_y*cos(rot) + p_x*sin(rot);
  }


} //end namespace isis


