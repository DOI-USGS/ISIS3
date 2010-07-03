/**
 * @file
 * $Revision: 1.4 $
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
#include "Sinusoidal.h"
#include "iException.h"
#include "Constants.h"

using namespace std;
namespace Isis {

 /**
  * Constructs a Sinusoidal object.
  *
  * @param label This argument must be a Label containing the proper mapping
  *              information as indicated in the Projection class. Additionally,
  *              the sinusoidal projection requires the center longitude to be
  *              defined in the keyword CenterLongitude.
  *
  * @param allowDefaults If set to false the constructor expects that a keyword
  *                      of CenterLongitude will be in the label. Otherwise it
  *                      will attempt to compute the center longitude using the
  *                      middle of the longitude range specified in the labels.
  *                      Defaults to false
  *
  * @throws Isis::iException::Io
  */
  Sinusoidal::Sinusoidal(Isis::Pvl &label, bool allowDefaults) :
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

      // Get the center longitude
      p_centerLongitude = mapGroup["CenterLongitude"];

      // convert to radians, adjust for longitude direction
      p_centerLongitude *= Isis::PI / 180.0;
      if (p_longitudeDirection == PositiveWest) p_centerLongitude *= -1.0;
    }
    catch (Isis::iException &e) {
      string message = "Invalid label group [Mapping]";
      throw Isis::iException::Message(Isis::iException::Io,message,_FILEINFO_);
    }
  }

  //! Destroys the Sinusoidal object
  Sinusoidal::~Sinusoidal() {
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
  bool Sinusoidal::SetGround(const double lat,const double lon) {
    // Convert to radians
    p_latitude = lat;
    p_longitude = lon;
    double latRadians = lat * Isis::PI / 180.0;
    double lonRadians = lon * Isis::PI / 180.0;
    if (p_longitudeDirection == PositiveWest) lonRadians *= -1.0;

    // Compute the coordinate
    double deltaLon = (lonRadians - p_centerLongitude);
    double x = p_equatorialRadius * deltaLon * cos(latRadians);
    double y = p_equatorialRadius * latRadians;
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
  bool Sinusoidal::SetCoordinate(const double x, const double y) {
    // Save the coordinate
    SetXY(x,y);

    // Compute latitude and make sure it is not above 90
    p_latitude = GetY() / p_equatorialRadius;
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
      p_longitude = p_centerLongitude + GetX() / (p_equatorialRadius * coslat);
    }

    // Convert to degrees
    p_latitude *= 180.0 / Isis::PI;
    p_longitude *= 180.0 / Isis::PI;

    // Cleanup the longitude
    if (p_longitudeDirection == PositiveWest) p_longitude *= -1.0;
  // These need to be done for circular type projections
  //  p_longitude = To360Domain (p_longitude);
  //  if (p_longitudeDomain == 180) p_longitude = To180Domain(p_longitude);

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
  bool Sinusoidal::XYRange(double &minX, double &maxX,
                               double &minY, double&maxY) {
    // Check the corners of the lat/lon range
    XYRangeCheck (p_minimumLatitude,p_minimumLongitude);
    XYRangeCheck (p_maximumLatitude,p_minimumLongitude);
    XYRangeCheck (p_minimumLatitude,p_maximumLongitude);
    XYRangeCheck (p_maximumLatitude,p_maximumLongitude);

    // If the latitude crosses the equator check there
    if ((p_minimumLatitude < 0.0) && (p_maximumLatitude > 0.0)) {
      XYRangeCheck (0.0,p_minimumLongitude);
      XYRangeCheck (0.0,p_maximumLongitude);
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
  PvlGroup Sinusoidal::Mapping()  {
    PvlGroup mapping = Projection::Mapping();

    mapping += p_mappingGrp["CenterLongitude"];

    return mapping;
  }

  /**
   * This function returns the latitude keywords that this projection uses
   * 
   * @return PvlGroup The latitude keywords that this projection uses
   */
  PvlGroup Sinusoidal::MappingLatitudes() {
    PvlGroup mapping = Projection::MappingLatitudes();

    return mapping;
  }

  /**
   * This function returns the longitude keywords that this projection uses
   * 
   * @return PvlGroup The longitude keywords that this projection uses
   */
  PvlGroup Sinusoidal::MappingLongitudes() {
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
  bool Sinusoidal::operator== (const Isis::Projection &proj) {
    if (!Isis::Projection::operator==(proj)) return false;
  // dont do the below it is a recusive plunge
  //  if (Isis::Projection::operator!=(proj)) return false;
    Sinusoidal *sinu = (Sinusoidal *) &proj;
    if (sinu->p_centerLongitude != this->p_centerLongitude) return false;
    return true;
  }
} // end namespace isis

extern "C" Isis::Projection *SinusoidalPlugin (Isis::Pvl &lab,
                                             bool allowDefaults) {
 return new Isis::Sinusoidal(lab,allowDefaults);
}
