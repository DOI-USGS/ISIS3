/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2008/05/09 18:49:25 $
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
#include "Mercator.h"
#include "iException.h"
#include "Constants.h"

using namespace std;
namespace Isis {
 /**
  * Constructs a Mercator object
  *
  * @param label This argument must be a Label containing the proper mapping
  *              information as indicated in the Projection class. Additionally,
  *              the mercator projection requires the center longitude to be
  *              defined in the keyword CenterLongitude.
  *
  * @param allowDefaults If set to false the constructor expects that a keyword
  *                      of CenterLongitude will be in the label. Otherwise it
  *                      will attempt to compute the center longitude using the
  *                      middle of the longitude range specified in the labels.
  *                      Defaults to false.
  *
  * @throws Isis::iException::Io
  */
  Mercator::Mercator(Isis::Pvl &label, bool allowDefaults) :
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

      // Compute the scale factor
      double cos_clat = cos(p_centerLatitude);
      double sin_clat = sin(p_centerLatitude);
      double p_eccsq = Eccentricity() * Eccentricity();
      p_scalefactor = cos_clat / sqrt(1.0 - p_eccsq * sin_clat * sin_clat);
    }
    catch (Isis::iException &e) {
      string message = "Invalid label group [Mapping]";
      throw Isis::iException::Message(Isis::iException::Io,message,_FILEINFO_);
    }
  }

  //! Destroys the Mercator object
  Mercator::~Mercator() {
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
  bool Mercator::SetGround(const double lat,const double lon) {
    // Convert longitude to radians & clean up
    p_longitude = lon;
    double lonRadians = lon * Isis::PI / 180.0;
    if (p_longitudeDirection == PositiveWest) lonRadians *= -1.0;

    // Now convert latitude to radians & clean up ... it must be planetographic
    p_latitude = lat;
    double latRadians = lat;
    if (IsPlanetocentric()) latRadians = ToPlanetographic(latRadians);
    latRadians *= Isis::PI / 180.0;

    // Make sure latitude value is not too close to either pole
    if (fabs( fabs(p_latitude) - 90.0) <= DBL_EPSILON) {
      p_good = false;
      return p_good;
    }

    // Compute the coordinate
    double deltaLon = (lonRadians - p_centerLongitude);
    double x = p_equatorialRadius * deltaLon * p_scalefactor;
    double sinphi = sin(latRadians);
    double t = tCompute(latRadians, sinphi);
    double y = -p_equatorialRadius * p_scalefactor * log(t);
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
  bool Mercator::SetCoordinate(const double x, const double y) {
    // Save the coordinate
    SetXY(x,y);

    // Compute Snyder's t
    double snyders_t = exp(-GetY()/(p_equatorialRadius * p_scalefactor));

    // Compute latitude and make sure it is not above 90
    p_latitude = phi2Compute(snyders_t);
    if (fabs(p_latitude) > Isis::HALFPI) {
      if (fabs(Isis::HALFPI - fabs(p_latitude)) > DBL_EPSILON) {
        p_good = false;
        return p_good;
      }
      else if (p_latitude < 0.0) {
        p_latitude = -Isis::HALFPI;
      }
      else {
        p_latitude = Isis::HALFPI;
      }
    }

    // Compute longitude
    double coslat = cos(p_latitude);
    if (coslat <= DBL_EPSILON) {
      p_longitude = p_centerLongitude;
    }
    else {
      p_longitude = p_centerLongitude + GetX() /
        (p_equatorialRadius * p_scalefactor);
    }

    // Convert to degrees
    p_latitude *= 180.0 / Isis::PI;
    p_longitude *= 180.0 / Isis::PI;

    // Cleanup the longitude
    if (p_longitudeDirection == PositiveWest) p_longitude *= -1.0;
  // These need to be done for circular type projections
   // p_longitude = To360Domain (p_longitude);
   // if (p_longitudeDomain == 180) p_longitude = To180Domain(p_longitude);

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
  bool Mercator::XYRange(double &minX, double &maxX, double &minY, double&maxY){
    // Check the corners of the lat/lon range
    XYRangeCheck (p_minimumLatitude,p_minimumLongitude);
    XYRangeCheck (p_maximumLatitude,p_minimumLongitude);
    XYRangeCheck (p_minimumLatitude,p_maximumLongitude);
    XYRangeCheck (p_maximumLatitude,p_maximumLongitude);

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
  PvlGroup Mercator::Mapping() {
    PvlGroup mapping = Projection::Mapping();

    mapping += p_mappingGrp["CenterLatitude"];
    mapping += p_mappingGrp["CenterLongitude"];

    return mapping;
  }

  /**
   * This function returns the latitude keywords that this projection uses
   * 
   * @return PvlGroup The latitude keywords that this projection uses
   */
  PvlGroup Mercator::MappingLatitudes() {
    PvlGroup mapping = Projection::MappingLatitudes();

    mapping += p_mappingGrp["CenterLatitude"];

    return mapping;
  }

  /**
   * This function returns the longitude keywords that this projection uses
   * 
   * @return PvlGroup The longitude keywords that this projection uses
   */
  PvlGroup Mercator::MappingLongitudes() {
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
  bool Mercator::operator== (const Isis::Projection &proj) {
    if (!Isis::Projection::operator==(proj)) return false;
  // dont do the below it is a recusive plunge
  //  if (Isis::Projection::operator!=(proj)) return false;
    Mercator *merc = (Mercator *) &proj;
    if ((merc->p_centerLongitude != this->p_centerLongitude) ||
        (merc->p_centerLatitude != this->p_centerLatitude)) return false;
    return true;
  }
} // end namespace isis

extern "C" Isis::Projection *MercatorPlugin (Isis::Pvl &lab,
                                             bool allowDefaults) {
 return new Isis::Mercator(lab,allowDefaults);
}


