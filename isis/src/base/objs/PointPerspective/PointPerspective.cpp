/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2008/06/13 19:35:00 $
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
#include "PointPerspective.h"
#include "iException.h"
#include "Constants.h"

using namespace std;
namespace Isis {
 /**
  * Constructs an PointPerspective object
  *
  * @param label This argument must be a Label containing the proper mapping
  *              information as indicated in the Projection class. Additionally,
  *              the point perspective projection requires the center longitude
  *              to be defined in the keyword CenterLongitude.
  *
  * @param allowDefaults If set to false the constructor expects that a keyword
  *                      of CenterLongitude will be in the label. Otherwise it
  *                      will attempt to compute the center longitude using the
  *                      middle of the longitude range specified in the labels.
  *                      Defaults to false.
  *
  * @throws Isis::iException::Io
  */
  PointPerspective::PointPerspective(Isis::Pvl &label, bool allowDefaults) :
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

      // convert to radians, adjust for longitude direction
      p_centerLongitude *= Isis::PI / 180.0;
      p_centerLatitude *= Isis::PI / 180.0;
      if (p_longitudeDirection == PositiveWest) p_centerLongitude *= -1.0;

      // Calculate sine & cosine of center latitude
      sinph0 = sin(p_centerLatitude);
      cosph0 = cos(p_centerLatitude);

      // Get the distance above planet center (the point of perspective from
      // the center of planet), and calculate P
      p_distance = mapGroup["Distance"];
      p_distance *= 1000.;
      p_P = 1.0 + (p_distance / p_equatorialRadius);

    }
    catch (Isis::iException &e) {
      string message = "Invalid label group [Mapping]";
      throw Isis::iException::Message(Isis::iException::Io,message,_FILEINFO_);
    }
  }

  //! Destroys the PointPerspective object
  PointPerspective::~PointPerspective() {
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
  bool PointPerspective::SetGround(const double lat,const double lon) {
    // Convert longitude to radians & clean up
    p_longitude = lon;
    double lonRadians = lon * Isis::PI / 180.0;
    if (p_longitudeDirection == PositiveWest) lonRadians *= -1.0;

    // Now convert latitude to radians & clean up ... it must be planetographic
    p_latitude = lat;
    double latRadians = lat;
    if (IsPlanetocentric()) latRadians = ToPlanetographic(latRadians);
    latRadians *= Isis::PI / 180.0;

    // Compute helper variables
    double deltaLon = (lonRadians - p_centerLongitude);
    double sinphi = sin(latRadians);
    double cosphi = cos(latRadians);
    double coslon = cos(deltaLon);

    // Lat/Lon cannot be projected
    double g =  sinph0*sinphi + cosph0*cosphi*coslon;
    if ( g < (1.0/p_P) ) {
      p_good = false;
      return p_good;
    }

    // Compute the coordinates
    double ksp = (p_P - 1.0) / (p_P - g);
    double x = p_equatorialRadius * ksp * cosphi * sin(deltaLon);
    double y = p_equatorialRadius * ksp * 
               (cosph0 * sinphi - sinph0 * cosphi * coslon);
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
  bool PointPerspective::SetCoordinate(const double x, const double y) {
    // Save the coordinate
    SetXY(x,y);

    // Declare instance variables and calculate rho
    double rho,rp,con,com,z,sinz,cosz;
    const double epsilon = 1.0e-10;
    rho = sqrt(GetX()*GetX() + GetY()*GetY());
    rp = rho / p_equatorialRadius;
    con = p_P - 1.0;
    com = p_P + 1.0;

    // Error calculating rho - should be less than equatorial radius
    if (rp > (sqrt(con/com))) {
      p_good = false;
      return p_good;
    }
    
    // Calculate the latitude and longitude
    p_longitude = p_centerLongitude;
    if (fabs(rho) <= epsilon) {
      p_latitude = p_centerLatitude;
    }
    else {
      if (rp <= epsilon) {
        sinz = 0.0;
      }
      else {
        sinz = (p_P - sqrt(1.0-rp*rp*com / con)) / (con / rp + rp / con);
      }
      z = asin(sinz);
      sinz = sin(z);
      cosz = cos(z);
      con = cosz*sinph0 + GetY()*sinz*cosph0 / rho;
      if (con > 1.0) con = 1.0;
      if (con < -1.0) con = -1.0;
      p_latitude = asin(con);

      con = fabs(p_centerLatitude) - Isis::HALFPI;
      if (fabs(con) <= epsilon) {
        if(p_centerLatitude >= 0.0) {
          p_longitude += atan2(GetX(), -GetY());
        }
        else {
          p_longitude += atan2(-GetX(), GetY());
        }
      }
      else {
        con = cosz - sinph0 * sin(p_latitude);
        if ((fabs(con) >= epsilon) || (fabs(GetX()) >= epsilon)) {
          p_longitude += atan2(GetX()*sinz*cosph0, con*rho);
        }
      }
    }

    // Convert to degrees
    p_latitude *= 180.0 / Isis::PI;
    p_longitude *= 180.0 / Isis::PI;

    // Cleanup the longitude
    if (p_longitudeDirection == PositiveWest) p_longitude *= -1.0;
  // These need to be done for circular type projections
    p_longitude = To360Domain (p_longitude);
    if (p_longitudeDomain == 180) p_longitude = To180Domain(p_longitude);

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
  */
  bool PointPerspective::XYRange(double &minX, double &maxX, double &minY, double&maxY){
    double lat, lon;

    // Check the corners of the lat/lon range
    XYRangeCheck (p_minimumLatitude,p_minimumLongitude);
    XYRangeCheck (p_maximumLatitude,p_minimumLongitude);
    XYRangeCheck (p_minimumLatitude,p_maximumLongitude);
    XYRangeCheck (p_maximumLatitude,p_maximumLongitude);

    // Walk top and bottom edges
    for (lat=p_minimumLatitude; lat<=p_maximumLatitude; lat+=0.01) {
      lat = lat;
      lon = p_minimumLongitude;
      XYRangeCheck(lat,lon);

      lat = lat;
      lon = p_maximumLongitude;
      XYRangeCheck(lat,lon);
    }

    // Walk left and right edges
    for (lon=p_minimumLongitude; lon<=p_maximumLongitude; lon+=0.01) {
      lat = p_minimumLatitude;
      lon = lon;
      XYRangeCheck(lat,lon);

      lat = p_maximumLatitude;
      lon = lon;
      XYRangeCheck(lat,lon);
    }

    // Walk the limb
    for (double angle=0.0; angle<=360.0; angle+=0.01) {
      double x = p_equatorialRadius * cos(angle * Isis::PI / 180.0);
      double y = p_equatorialRadius * sin(angle * Isis::PI / 180.0);
      if (SetCoordinate(x,y) == 0) {
        if (p_latitude > p_maximumLatitude) continue;
        if (p_longitude > p_maximumLongitude) continue;
        if (p_latitude < p_minimumLatitude) continue;
        if (p_longitude < p_minimumLongitude) continue;
        XYRangeCheck(p_latitude, p_longitude);
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
  PvlGroup PointPerspective::Mapping() {
    PvlGroup mapping = Projection::Mapping();

    mapping += p_mappingGrp["CenterLatitude"];
    mapping += p_mappingGrp["CenterLongitude"];
    mapping += p_mappingGrp["Distance"];

    return mapping;
  }

  /**
   * This function returns the latitude keywords that this projection uses
   * 
   * @return PvlGroup The latitude keywords that this projection uses
   */
  PvlGroup PointPerspective::MappingLatitudes() {
    PvlGroup mapping = Projection::MappingLatitudes();

    mapping += p_mappingGrp["CenterLatitude"];

    return mapping;
  }

  /**
   * This function returns the longitude keywords that this projection uses
   * 
   * @return PvlGroup The longitude keywords that this projection uses
   */
  PvlGroup PointPerspective::MappingLongitudes() {
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
  bool PointPerspective::operator== (const Isis::Projection &proj) {
    if (!Isis::Projection::operator==(proj)) return false;
  // dont do the below it is a recusive plunge
  //  if (Isis::Projection::operator!=(proj)) return false;
    PointPerspective *point = (PointPerspective *) &proj;
    if ((point->p_centerLongitude != this->p_centerLongitude) ||
        (point->p_centerLatitude != this->p_centerLatitude) ||
        (point->p_distance != this->p_distance)) return false;
    return true;
  }
} // end namespace isis

extern "C" Isis::Projection *PointPerspectivePlugin (Isis::Pvl &lab,
                                             bool allowDefaults) {
 return new Isis::PointPerspective(lab,allowDefaults);
}

