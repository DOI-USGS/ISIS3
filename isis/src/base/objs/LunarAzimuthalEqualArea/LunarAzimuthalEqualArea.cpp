/**
* @file
* $Revision: 1.1 $
* $Date: 2009/08/07 22:52:23 $
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
#include "LunarAzimuthalEqualArea.h"
#include "iException.h"
#include "Constants.h"

using namespace std;
namespace Isis
{
  /**
  * Constructs a LunarAzimuthalEqualArea object
  *
  * @param label This argument must be a Label containing the proper mapping
  *              information as indicated in the Projection class. Additionally,
  *              the LunarAzimuthalEqualArea projection requires the
  *              center longitude to be defined in the keyword CenterLongitude.
  *
  * @throws Isis::iException::Io
  */
  LunarAzimuthalEqualArea::LunarAzimuthalEqualArea(
      Isis::Pvl & label) : Isis::Projection::Projection(label)
  {
    try
    {
      // Try to read the mapping group
      Isis::PvlGroup & mapGroup = label.FindGroup("Mapping",
          Isis::Pvl::Traverse);
  
      // Get the max libration
      p_maxLibration = mapGroup["MaximumLibration"];
      p_maxLibration *= PI / 180.0;
    }
    catch (Isis::iException & e)
    {
      string message = "Invalid label group [Mapping]";
      throw Isis::iException::Message(Isis::iException::Io, message,
          _FILEINFO_);
    }
  }
  
  //! Destroys the LunarAzimuthalEqualArea object
  LunarAzimuthalEqualArea::~LunarAzimuthalEqualArea()
  {
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
  bool LunarAzimuthalEqualArea::SetGround(const double lat,
      const double lon)
  {
    // Convert longitude to radians
    p_longitude = lon;
    double lonRadians = lon * Isis::PI / 180.0;
    if (p_longitudeDirection == PositiveWest)
      lonRadians *= -1.0;
  
    // Now convert latitude to radians... it must be planetographic
    p_latitude = lat;
    double latRadians = lat;
    if (IsPlanetocentric())
      latRadians = ToPlanetographic(latRadians);
    latRadians *= Isis::PI / 180.0;

    double x, y;
    if (lonRadians == 0.0 && latRadians == 0.0)
    {
      x = 0.0;
      y = 0.0;
      SetComputedXY(x, y);
      p_good = true;
      return true;
    }
    
    double E = acos(cos(latRadians) * cos(lonRadians));
    double test = (sin(lonRadians) * cos(latRadians)) / sin(E);
    
    if (test > 1.0) test = 1.0;
    else if (test < -1.0) test = -1.0;
    
    double D = HALFPI - asin(test);
    if (latRadians < 0.0)
      D = -D;
    
    double radius = p_equatorialRadius;
    double PFAC = (HALFPI + p_maxLibration) / HALFPI;
    double RP = radius * sin(E / PFAC);
    
    x = RP * cos(D);
    y = RP * sin(D);
    
    SetComputedXY(x, y);
    p_good = true;
    return true;
    
  } // of SetGround


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
  bool LunarAzimuthalEqualArea::SetCoordinate(const double x,
      const double y)
  {
    // Save the coordinate
    SetXY(x, y);
    
    double RP = sqrt((x * x) + (y * y));
    
    double lat, lon;
    if (y == 0.0 && x == 0.0)
    {
      lat = 0.0;
      lon = 0.0;
      return true;
    }
    
    double radius = p_equatorialRadius;
    
    double D = atan2(y, x);
    double test = RP / radius;
    double IERROR;
    if (abs(test) > 1.0)
    {
      IERROR = 1001;  // throw exception or something ??
      return false;
    }
    
    double EPSILON = 0.0000000001;
    double PFAC = (HALFPI + p_maxLibration) / HALFPI;
    double E = PFAC * asin(RP / radius);
    
    lat = HALFPI - (acos(sin(D) * sin(E)));
    
    if (abs(HALFPI - abs(lat)) <= EPSILON)
    {
      lon = 0.0;
    }
    else
    {
      test = sin(E) * cos(D) / sin(HALFPI - lat);
      if (test > 1.0) test = 1.0;
      else if (test < -1.0) test = -1.0;

      lon = asin(test);
    }
    
    if (E >= HALFPI)
    {
      if (lon <= 0.0) lon = -PI - lon;
      else lon = PI - lon;
    }
    
    // Convert to degrees
    p_latitude = lat * 180.0 / Isis::PI;
    p_longitude = lon * 180.0 / Isis::PI;
    
    // Cleanup the latitude
    if (IsPlanetocentric())
      p_latitude = ToPlanetocentric(p_latitude);
    
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
  bool LunarAzimuthalEqualArea::XYRange(double & minX, double & maxX,
      double & minY, double & maxY)
  {
    // Check the corners of the lat/lon range
    XYRangeCheck (p_minimumLatitude,p_minimumLongitude);
    XYRangeCheck (p_maximumLatitude,p_minimumLongitude);
    XYRangeCheck (p_minimumLatitude,p_maximumLongitude);
    XYRangeCheck (p_maximumLatitude,p_maximumLongitude);
  
    // If the latitude range contains 0
    if ((p_minimumLatitude < 0.0) && (p_maximumLatitude > 0.0))
    {
      XYRangeCheck (0.0, p_minimumLongitude);
      XYRangeCheck (0.0, p_maximumLongitude);
    }
    
    // If the longitude range contains 0
    if ((p_minimumLongitude < 0.0) && (p_maximumLongitude > 0.0))
    {
      XYRangeCheck (p_minimumLatitude, 0.0);
      XYRangeCheck (p_maximumLatitude, 0.0);
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
  PvlGroup LunarAzimuthalEqualArea::Mapping()
  {
    PvlGroup mapping = Projection::Mapping();
    mapping += p_mappingGrp["MaximumLibration"];
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
  bool LunarAzimuthalEqualArea::operator== (const Isis::Projection & proj)
  {
    if (!Isis::Projection::operator==(proj))
      return false;
    // dont use != (it is a recusive plunge)
    //  if (Isis::Projection::operator!=(proj)) return false;
    
    LunarAzimuthalEqualArea * LKAEA = (LunarAzimuthalEqualArea *) &proj;
    if (LKAEA->p_maxLibration != this->p_maxLibration)
      return false;
    return true;
  }

  
} // end namespace isis

extern "C" Isis::Projection *LunarAzimuthalEqualAreaPlugin (Isis::Pvl &lab,
    bool allowDefaults)
{
  return new Isis::LunarAzimuthalEqualArea(lab);
}
