/**
 * @file
 * $Revision: 1.8 $
 * $Date: 2009/03/20 22:30:23 $
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

#include <cmath>
#include <cfloat>
#include "LambertConformal.h"
#include "iException.h"
#include "Constants.h"
#include "iString.h"

using namespace std;
namespace Isis {
 /**
  * Constructs a Lambert Conformal object
  *
  * @param label This argument must be a Label containing the proper mapping
  *              information as indicated in the Projection class. Additionally,
  *              the lambertconformal projection requires the center longitude
  *              to be defined in the keyword CenterLongitude, and the first and
  *              second standard parallels defined in the keywords
  *              FirstStandardParallel and SecondStandardParallel.
  *
  * @param allowDefaults If set to false the constructor expects that a keyword
  *                      of CenterLongitude, FirstStandardParallel, and
  *                      SecondStandardParallel will be in the label. Otherwise
  *                      it will attempt to compute the center longitude using
  *                      the middle of the longitude range specified in the
  *                      labels. Defaults to false.
  *
  * @throws Isis::iException::Io
  */
  LambertConformal::LambertConformal(Isis::Pvl &label, bool allowDefaults) :
    Isis::Projection::Projection (label) {
    try {
      // Try to read the mapping group
      Isis::PvlGroup &mapGroup = label.FindGroup ("Mapping",Isis::Pvl::Traverse);

      // Compute and write the default center longitude if allowed and
      // necessary
      if ((allowDefaults) && (!mapGroup.HasKeyword("CenterLongitude"))) {
        double lon = (p_minimumLongitude + p_maximumLongitude) / 2.0;
        mapGroup += Isis::PvlKeyword("CenterLongitude",lon);
      }

      // Compute and write the default center latitude if allowed and
      // necessary
      if ((allowDefaults) && (!mapGroup.HasKeyword("CenterLatitude"))) {
        double lat = (p_minimumLatitude + p_maximumLatitude) / 2.0;
        mapGroup += Isis::PvlKeyword("CenterLatitude",lat);
      }

      // Get the center longitude  & latitude
      p_centerLongitude = mapGroup["CenterLongitude"];
      p_centerLatitude = mapGroup["CenterLatitude"];
      if (this->IsPlanetocentric()) {
        p_centerLatitude = this->ToPlanetographic(p_centerLatitude);
      }

      // Test to make sure center longitude is valid
      if (fabs(p_centerLongitude) > 360.0) {
        iString message = "Central Longitude [" + iString(p_centerLongitude);
        message += "] must be between -360 and 360";
        throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
      }

      // convert to radians, adjust for longitude direction
      p_centerLongitude *= Isis::PI / 180.0;
      if (p_longitudeDirection == PositiveWest) p_centerLongitude *= -1.0;

      // Get the standard parallels & convert them to ographic
      p_par1 = mapGroup["FirstStandardParallel"];
      if (this->IsPlanetocentric()) {
        p_par1 = this->ToPlanetographic(p_par1);
      }
      p_par2 = mapGroup["SecondStandardParallel"];
      if (this->IsPlanetocentric()) {
        p_par2 = this->ToPlanetographic(p_par2);
      }

      // Test to make sure standard parallels are valid
      if (fabs(p_par1) > 90.0 || fabs(p_par2) > 90.0) {
        string message = "Standard Parallels must between -90 and 90";
        throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
      }
      if (fabs(p_par1 + p_par2) < DBL_EPSILON) {
        string message = "Standard Parallels cannot be symmetric to the equator";
        throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
      }
      // Removed because this test only works for northern hemisphere
      // Just reorder the parallels so p1 is at the larger radius of the two
      //if (p_par1 > p_par2) {
      //  string message = "Standard Parallels must be ordered";
      //  throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
      //}

      // Reorder the parallels so p1 is closer to the equator than p2
      // Therefore p2 is nearest the apex of the cone
      if (fabs(p_par1) > fabs(p_par2)) {
        double tmp = p_par2;
        p_par2 = p_par1;
        p_par1 = tmp;
      }

      // Test to make sure center latitude is valid
      // The pole opposite the apex can not be used as the clat (i.e., origin of
      // the projection) it projects to infinity
      // Given: p2 is closer to the apex than p1, and p2 must be on the same
      // side of the equator as the apex (due to the reording of p1 and p2 above)
      // Test for cone pointed south "v"
      if ((p_par2 < 0.0) && (fabs(90.0-p_centerLatitude) < DBL_EPSILON)) {
        iString message = "Center Latitude [" + iString(p_centerLatitude);
        message += "] is not valid, it projects to infinity for standard parallels [";
        message += iString(p_par1) + "," + iString(p_par2) + "]";
        throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
      }
      // Test for cone pointed north "^"
      else if ((p_par2 > 0.0) && (fabs(-90.0-p_centerLatitude) < DBL_EPSILON)) {
        iString message = "Center Latitude [" + iString(p_centerLatitude);
        message += "] is not valid, it projects to infinity for standard parallels [";
        message += iString(p_par1) + "," + iString(p_par2) + "]";
        throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
      }
      // convert clat to radians
      p_centerLatitude *= Isis::PI / 180.0;

      // Convert standard parallels to radians
      p_par1 *= Isis::PI / 180.0;
      p_par2 *= Isis::PI / 180.0;

      // Compute the Snyder's m and t values for the standard parallels and the
      // center latitude
      double sinpar1 = sin(p_par1);
      double cospar1 = cos(p_par1);
      double m1 = mCompute(sinpar1, cospar1);
      double t1 = tCompute(p_par1, sinpar1);

      double sinpar2 = sin(p_par2);
      double cospar2 = cos(p_par2);
      double m2 = mCompute(sinpar2, cospar2);
      double t2 = tCompute(p_par2, sinpar2);

      double sinclat = sin(p_centerLatitude);
      double tclat = tCompute(p_centerLatitude, sinclat);

      // Calculate Snyder's n, f, and rho
      if(fabs(p_par1 - p_par2) >= DBL_EPSILON) {
        p_n = log(m1 / m2) / log(t1 / t2);
      }
      else {
        p_n = sinpar1;
      }
      p_f = m1 / (p_n * pow(t1, p_n));
      p_rho = p_equatorialRadius * p_f * pow(tclat, p_n);
    }
    catch (Isis::iException &e) {
      string message = "Invalid label group [Mapping]";
      throw Isis::iException::Message(Isis::iException::Io,message,_FILEINFO_);
    }
  }

  //! Destroys the LambertConformal object
  LambertConformal::~LambertConformal() {
  }

 /**
  * This method is used to set the latitude/longitude (assumed to be of the
  * correct LatitudeType, LongitudeDirection, and LongitudeDomain. The Set
  * forces an attempted calculation of the projection X/Y values. This may or
  * may not be successful and a status is returned as such.
  *
  * @param lat Latitude value to project
  *
  * @param lon Longitude value to project
  *
  * @return bool
  */
  bool LambertConformal::SetGround(const double lat,const double lon) {
    // Convert longitude to radians & clean up
    p_longitude = lon;
    double lonRadians = lon * Isis::PI / 180.0;
    if (p_longitudeDirection == PositiveWest) lonRadians *= -1.0;

    // Now convert latitude to radians & clean up ... it must be planetographic
    p_latitude = lat;
    double latRadians = lat;
    if (IsPlanetocentric()) latRadians = ToPlanetographic(latRadians);
    latRadians *= Isis::PI / 180.0;

    // Check for special cases & calculate rh, theta, and snyder t
    double rh;
    if (fabs(fabs(latRadians) - Isis::HALFPI) < DBL_EPSILON) {
      // Lat/Lon point cannot be projected
      if (latRadians * p_n <= 0.0) {
        p_good = false;
        return p_good;
      }
      else rh = 0.0;
    }
    else {
      double sinlat = sin(latRadians);
      // Lat/Lon point cannot be projected
      double t = tCompute(latRadians, sinlat);
      rh = p_equatorialRadius * p_f * pow(t, p_n);
    }
    double theta = p_n * (lonRadians - p_centerLongitude);

    // Compute the coordinate
    double x = rh * sin(theta);
    double y = p_rho - rh * cos(theta);
    SetComputedXY(x,y);

    p_good = true;
    return p_good;
  }

 /**
  * This method is used to set the projection x/y. The Set forces an attempted
  * calculation of the corresponding latitude/longitude position. This may or
  * may not be successful and a status is returned as such.
  *
  * @param x X coordinate of the projection in units that are the same as the
  *          radii in the label
  *
  * @param y Y coordinate of the projection in units that are the same as the
  *          radii in the label
  *
  * @return bool
  */
  bool LambertConformal::SetCoordinate(const double x, const double y) {
    // Save the coordinate
    SetXY(x,y);

    // Get the sign of Snyder's n
    double sign;
    if (p_n >= 0) sign = 1.0;
    else sign = -1.0;

    double temp = p_rho - GetY();
    double rh = sign * sqrt(GetX() * GetX() + temp * temp);

    double theta;
    if (rh != 0) theta = atan2(sign * GetX(), sign * temp);
    else theta = 0.0;

    // Compute latitude and longitude
    if (rh != 0 || p_n > 0) {
      double t = pow(rh / (p_equatorialRadius * p_f), 1.0 / p_n);
      p_latitude = phi2Compute(t);
    }
    else p_latitude = -Isis::HALFPI;
    p_longitude = theta / p_n + p_centerLongitude;


    // Convert to degrees
    p_latitude *= 180.0 / Isis::PI;
    p_longitude *= 180.0 / Isis::PI;

    // Cleanup the longitude
    if (p_longitudeDirection == PositiveWest) p_longitude *= -1.0;
  // These need to be done for circular type projections
  //  p_longitude = To360Domain (p_longitude);
  //  if (p_longitudeDomain == 180) p_longitude = To180Domain(p_longitude);

    // Cleanup the latitude
    if (IsPlanetocentric()) p_latitude = ToPlanetocentric(p_latitude);

    p_good = true;
    return p_good;
  }

 /**
  * This method is used to determine the x/y range which completely covers the
  * area of interest specified by the lat/lon range. The latitude/longitude
  * range may be obtained from the labels. The purpose of this method is to
  * return the x/y range so it can be used to compute how large a map may need
  * to be. For example, how big a piece of paper is needed or how large of an
  * image needs to be created. The method may fail as indicated by its return
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
  *  
  * @history 2009-03-03  Tracie Sucharski, Undo the PositiveWest adjustment to 
  *                         the center longitude because it is done twice, once
  *                         in the constructor and again in SetGround.
  *  
  * @history 2009-03-20  Stuart Sides, Modified the validity check for center 
  *                      latitude. It now only tests for the pole opposite the
  *                      apex of the cone.
  */
  bool LambertConformal::XYRange(double &minX, double &maxX, double &minY, double &maxY){

    // Test the four corners
    XYRangeCheck(p_minimumLatitude, p_minimumLongitude);
    XYRangeCheck(p_minimumLatitude, p_maximumLongitude);
    XYRangeCheck(p_maximumLatitude, p_minimumLongitude);
    XYRangeCheck(p_maximumLatitude, p_maximumLongitude);

    // Decide which pole the apex of the cone is above
    // Remember p1 is now closest to the equator and p2 is closest to one of the poles
    bool north_hemi = true;
    // Stuart Sides 2008-08-15
    // This test was removed because the reordering of p1 and p2 in the
    // constructor made it incorrect 
    //if (fabs(p_par1) > fabs(p_par2)) north_hemi = false;
    if (p_par2 < 0.0) north_hemi = false;
    if ((p_par1 == p_par2) && (p_par1 < 0.0)) north_hemi = false;

    double cLonDeg = p_centerLongitude * 180.0 / Isis::PI;

    //  This is needed because the SetGround class applies the PositiveWest
    //  adjustment which was already done in the constructor.
    if (p_longitudeDirection == PositiveWest) cLonDeg = cLonDeg * -1.0;

    double pole_north, min_lat_north, max_lat_north, londiff;
    // North Pole
    if (north_hemi) {
      p_latitude = 90.0;
      p_longitude = cLonDeg;

      //Unable to project at the pole
      if (!SetGround(p_latitude,p_longitude)) {
        p_good = false;
        return p_good;
      }

      pole_north = YCoord();
      p_latitude = p_minimumLatitude;

      //Unable to project at the pole
      if (!SetGround(p_latitude,p_longitude)) {
        p_good = false;
        return p_good;
      }

      min_lat_north = YCoord();
      double y = min_lat_north + 2.0 * (pole_north - min_lat_north);

      //Unable to project opposite the center longitude
      if (!SetCoordinate(XCoord(),y)) {
        p_good = false;
        return p_good;
      }

      londiff = fabs(cLonDeg - p_longitude) / 2.0;
      p_longitude = cLonDeg - londiff;
      for (int i=0; i<3; i++) {
        if ((p_longitude >=p_minimumLongitude) && (p_longitude <= p_maximumLongitude)) {
          p_latitude = p_minimumLatitude;
          XYRangeCheck(p_latitude, p_longitude);
        }
        p_longitude += londiff;
      }

    }
    // South Pole
    else {
      p_latitude = -90.0;
      p_longitude = cLonDeg;

      //Unable to project at the pole
      if (!SetGround(p_latitude,p_longitude)) {
        p_good = false;
        return p_good;
      }

      pole_north = YCoord();
      p_latitude = p_maximumLatitude;

      //Unable to project at the pole
      if (!SetGround(p_latitude,p_longitude)) {
        p_good = false;
        return p_good;
      }

      max_lat_north = YCoord();
      double y = max_lat_north - 2.0 * (max_lat_north - pole_north);

      //Unable to project opposite the center longitude
      if (!SetCoordinate(XCoord(),y)) {
        p_good = false;
        return p_good;
      }

      londiff = fabs(cLonDeg - p_longitude) / 2.0;
      p_longitude = cLonDeg - londiff;
      for (int i=0; i<3; i++) {
        if ((p_longitude >=p_minimumLongitude) && (p_longitude <= p_maximumLongitude)) {
          p_latitude = p_maximumLatitude;
          XYRangeCheck(p_latitude, p_longitude);
        }
        p_longitude += londiff;
      }
    }

    // Make sure everything is ordered
    if (p_minimumX >= p_maximumX) return false;
    if (p_minimumY >= p_maximumY) return false;

    // Return X/Y min/maxs
    minX = p_minimumX;
    maxX = p_maximumX;
    minY = p_minimumY;
    maxY = p_maximumY;
    return true;
  }


  /**
   * This function returns the keywords that this projection uses.
   * 
   * @return PvlGroup The keywords that this projection uses
   */
  PvlGroup LambertConformal::Mapping() {
    PvlGroup mapping = Projection::Mapping();

    mapping += p_mappingGrp["CenterLatitude"];
    mapping += p_mappingGrp["CenterLongitude"];
    mapping += p_mappingGrp["FirstStandardParallel"];
    mapping += p_mappingGrp["SecondStandardParallel"];

    return mapping;
  }

  /**
   * This function returns the latitude keywords that this projection uses
   * 
   * @return PvlGroup The latitude keywords that this projection uses
   */
  PvlGroup LambertConformal::MappingLatitudes() {
    PvlGroup mapping = Projection::MappingLatitudes();

    mapping += p_mappingGrp["CenterLatitude"];
    mapping += p_mappingGrp["FirstStandardParallel"];
    mapping += p_mappingGrp["SecondStandardParallel"];

    return mapping;
  }

  /**
   * This function returns the longitude keywords that this projection uses
   * 
   * @return PvlGroup The longitude keywords that this projection uses
   */
  PvlGroup LambertConformal::MappingLongitudes() {
    PvlGroup mapping = Projection::MappingLongitudes();

    mapping += p_mappingGrp["CenterLongitude"];

    return mapping;
  }

 /**
  * Compares two Projection objects to see if they are equal
  *
  * @param proj Projection object to do comparison on
  *
  * @return bool Returns true if the Projection objects are equal, and false if
  *              they are not
  */
  bool LambertConformal::operator== (const Isis::Projection &proj) {
    if (!Isis::Projection::operator==(proj)) return false;
  // dont do the below it is a recusive plunge
  //  if (Isis::Projection::operator!=(proj)) return false;
    LambertConformal *lamb = (LambertConformal *) &proj;
    if ((lamb->p_centerLongitude != this->p_centerLongitude) ||
        (lamb->p_centerLatitude != this->p_centerLatitude)) return false;
    return true;
  }
} // end namespace isis

extern "C" Isis::Projection *LambertConformalPlugin (Isis::Pvl &lab,
                                             bool allowDefaults) {
 return new Isis::LambertConformal(lab,allowDefaults);
}

