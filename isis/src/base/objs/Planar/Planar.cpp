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

#include "Planar.h"

#include <cmath>
#include <cfloat>

#include "Constants.h"
#include "IException.h"
#include "Projection.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"

using namespace std;
namespace Isis {
  /**
   * TODO: correct documentation in this file
   *
   * Constructs an Planar object
   *
   * @param label This argument must be a Label containing the proper mapping
   *              information as indicated in the Projection class. Additionally,
   *              the orthographic projection requires the center longitude to be
   *              defined in the keyword CenterLongitude.
   *
   * @param allowDefaults If set to false the constructor expects that a keyword
   *                      of CenterLongitude will be in the label. Otherwise it
   *                      will attempt to compute the center longitude using the
   *                      middle of the longitude range specified in the labels.
   *                      Defaults to false.
   *
   * @throws IException
   */
  Planar::Planar(Pvl &label, bool allowDefaults) :
    Projection::Projection(label) {

    // latitude in ring plane is always zero
    m_latitude = 0.0;
    m_minimumLatitude = 0.0;
    m_maximumLatitude = 1.0;

    if (m_mappingGrp.HasKeyword("MinimumRingRadius"))
      m_minimumRingRadius = m_mappingGrp["MinimumRingRadius"];
    if (m_mappingGrp.HasKeyword("MaximumRingRadius"))
      m_maximumRingRadius = m_mappingGrp["MaximumRingRadius"];

    try {
      // Try to read the mapping group
      PvlGroup &mapGroup = label.FindGroup("Mapping", Pvl::Traverse);

      // Compute and write the default center longitude if allowed and
      // necessary
      if ((allowDefaults) && (!mapGroup.HasKeyword("CenterLongitude"))) {
        double lon = (m_minimumLongitude + m_maximumLongitude) / 2.0;
        mapGroup += PvlKeyword("CenterLongitude", lon);
      }

      // Compute and write the default center radius if allowed and
      // necessary
      if ((allowDefaults) && (!mapGroup.HasKeyword("CenterRadius"))) {
        double radius = (m_minimumRingRadius + m_maximumRingRadius) / 2.0;
        mapGroup += PvlKeyword("CenterRadius", radius);
      }

      // Get the center longitude  & radius
      m_centerLongitude = mapGroup["CenterLongitude"];
      m_centerRadius = mapGroup["CenterRadius"];

      // convert to radians, adjust for longitude direction
      m_centerLongitude *= DEG2RAD;
      if (m_longitudeDirection == PositiveWest) m_centerLongitude *= -1.0;
/*
      // Calculate sine & cosine of center latitude
      m_sinph0 = sin(m_centerLatitude);
      m_cosph0 = cos(m_centerLatitude);

      // Let's apply this equation at the extremes to minimize our lat/lon range
      double sinphi, cosphi, coslon;

      // Can we project at the minlat, center lon? If not, then we should move
      // up the min lat to be inside the image.
      sinphi = sin(m_minimumLatitude * DEG2RAD);
      cosphi = cos(m_minimumLatitude * DEG2RAD);
      coslon = 1.0; // at lon=centerLon: cos(lon-centerLon) = cos(0) = 1
      if (m_sinph0 * sinphi + m_cosph0 * cosphi * coslon < 1E-10) {
        // solve for x: a * sin(x) + b * cos(x) * 1 = 0
        // a * sin(x) + b * cos(x) = 0
        // a * sin(x) = - b * cos(x)
        // -(a * sin(x)) / b = cos(x)
        // -(a / b) = cos(x) / sin(x)
        // -(b / a) = sin(x) / cos(x)
        // -(b / a) = tan(x)
        // arctan(-(b / a)) = x
        // arctan(-(m_cosph0 / m_sinph0)) = x
        double newMin = atan2(- m_cosph0, m_sinph0) * 180.0 / PI;
        if (newMin > m_minimumLatitude) {
          m_minimumLatitude = newMin;
        } // else something else is off (i.e. longitude range)
      }

      sinphi = sin(m_minimumLatitude * PI / 180.0);
      cosphi = cos(m_minimumLatitude * PI / 180.0);

      // Can we project at the maxlat, center lon? If not, then we should move
      // down the max lat to be inside the image.
      sinphi = sin(m_maximumLatitude * PI / 180.0);
      cosphi = cos(m_maximumLatitude * PI / 180.0);
      coslon = 1.0; // at lon=centerLon: cos(lon-centerLon) = cos(0) = 1
      if (m_sinph0 * sinphi + m_cosph0 * cosphi * coslon < 1E-10) {
        // see above equations for latitude
        double newMax = atan2(- m_cosph0, m_sinph0) * 180.8 / PI;
        if (newMax < m_maximumLatitude && newMax > m_minimumLatitude) {
          m_maximumLatitude = newMax;
        } // else something else is off (i.e. longitude range)
      }

      // If we are looking at the side of the planet (clat = 0), then make sure
      // the longitude range is limited to 90 degrees to either direction
      if (m_centerLatitude == 0.0) {
        if (m_maximumLongitude - m_centerLongitude * 180.0 / PI > 90) {
          m_maximumLongitude = (m_centerLongitude * 180.0 / PI) + 90;
        }

        if (m_centerLongitude * 180.0 / PI - m_minimumLongitude > 90) {
          m_minimumLongitude = (m_centerLongitude * 180.0 / PI) - 90;
        }
      }
*/
    }
    catch(IException &e) {
      string message = "Invalid label group [Mapping]";
      throw IException(e, IException::Io, message, _FILEINFO_);
    }
  }

  //! Destroys the Planar object
  Planar::~Planar() {
  }

  /**
   * Compares two Projection objects to see if they are equal
   *
   * @param proj Projection object to do comparison on
   *
   * @return bool Returns true if the Projection objects are equal, and false if
   *              they are not
   */
  bool Planar::operator== (const Projection &proj) {
    if (!Projection::operator==(proj)) return false;
    // dont do the below it is a recusive plunge
    //  if (Projection::operator!=(proj)) return false;
    Planar *planar = (Planar *) &proj;
    if ((planar->m_centerLongitude != m_centerLongitude) ||
        (planar->m_centerRadius != m_centerRadius)) return false;
    return true;
  }

  /**
    * Returns the name of the map projection, "Planar"
    *
    * @return string Name of projection, "Planar"
    */
   string Planar::Name() const {
     return "Planar";
   }

   /**
    * Returns the center latitude, in degrees.
    *
    * TODO: Correct this comment for planar projection, assuming right now scale
    * at center of projection.
    * (believe scale is uniform across planar projection)
    *
    * **NOTE** In the case of Planar projections, there is NO latitude
    * that is entirely true to scale. The only true scale for this projection is
    * at the single point, (center latitude, center longitude).
    *
    * @return double The center latitude.
    */
//   double Planar::TrueScaleLatitude() const {
//     return m_centerLongitude * RAD2DEG;
//   }

   /**
    * Returns the version of the map projection
    *
    *
    * @return string Version number
    */
   string Planar::Version() const {
     return "1.0";
   }

   /**
    * This returns a radius with correct type as specified in the label object.
    * The method can only be used if SetGround, SetCoordinate,
    * SetUniversalGround, or SetWorld return with success. Success can also
    * be checked using the IsGood method.
    *
    * @return double
    */
   double Planar::Radius() const {
     return m_radius;
   }


  /**
   * This method is used to set the radius/longitude (assumed to be of the
   * correct LatitudeType, LongitudeDirection, a nd LongitudeDomain. The Set
   * forces an attempted calculation of the projection X/Y values. This may or
   * may not be successful and a status is returned as such.
   *
   * @param radius Radius value to project
   *
   * @param lon Longitude value to project
   *
   * @return bool
   */
  bool Planar::SetGround(const double radius, const double lon) {

    // Convert longitude to radians & clean up
    m_longitude = lon;
    double lonRadians = lon * DEG2RAD;
    if (m_longitudeDirection == PositiveWest) lonRadians *= -1.0;


    // Compute helper variables
    double deltaLon = (lonRadians - m_centerLongitude);
//  double coslon = cos(deltaLon);

    // Lat/Lon cannot be projected
//    double g =  m_sinph0 * sinphi + m_cosph0 * cosphi * coslon;
//    if ((g <= 0.0) && (fabs(g) > 1.0e-10)) {
//      m_good = false;
//      return m_good;
//    }

    // Compute the coordinates
    double x = radius * cos(deltaLon);
    double y = radius * sin(deltaLon);
//    double x = radius;
//    double y = deltaLon;

    SetComputedXY(x, y);
    m_good = true;
    return m_good;
  }

  /**
   * This method is used to set the projection x/y. The Set forces an attempted
   * calculation of the corresponding radius/longitude position. This may or
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
  bool Planar::SetCoordinate(const double x, const double y) {

    // Save the coordinate
    SetXY(x, y);

    // compute radius and longitude
    m_radius = sqrt(x*x + y*y);
    m_longitude = atan2(y,x) * RAD2DEG;
    if ( m_longitude < 0.0 )
      m_longitude += 360.0;

    // Cleanup the longitude
    if (m_longitudeDirection == PositiveWest) m_longitude *= -1.0;

    // These need to be done for circular type projections
    m_longitude = To360Domain(m_longitude);
    if (m_longitudeDomain == 180)
      m_longitude = To180Domain(m_longitude);

    m_good = true;
    return m_good;
  }

  /**
   * This method is used to determine the x/y range which completely covers the
   * area of interest specified by the radius/longitude range. This range may be
   * obtained from the labels. The purpose of this method is to return the x/y
   * range so it can be used to compute how large a map may need to be. For
   * example, how big a piece of paper is needed or how large of an image needs
   * to be created. The method may fail as indicated by its return value.
   *
   * @param minX Minimum x projection coordinate which covers the radius/
   *             longitude range specified in the labels.
   *
   * @param maxX Maximum x projection coordinate which covers the radius/
   *             longitude range specified in the labels.
   *
   * @param minY Minimum y projection coordinate which covers the radius/
   *             longitude range specified in the labels.
   *
   * @param maxY Maximum y projection coordinate which covers the radius/
   *             longitude range specified in the labels.
   *
   * @return bool
   */
/*
  bool Planar::XYRange(double &minX, double &maxX,
                             double &minY, double &maxY) {
    double lat, lon;

    // Check the corners of the lat/lon range
    XYRangeCheck(m_minimumLatitude, m_minimumLongitude);
    XYRangeCheck(m_maximumLatitude, m_minimumLongitude);
    XYRangeCheck(m_minimumLatitude, m_maximumLongitude);
    XYRangeCheck(m_maximumLatitude, m_maximumLongitude);

//cout << " ************ WALK LATITUDE ******************\n";
//cout << "MIN LAT: " << m_minimumLatitude << " MAX LAT: " << m_maximumLatitude << "\n";
    // Walk top and bottom edges
    for (lat = m_minimumLatitude; lat <= m_maximumLatitude; lat += 0.01) {
//cout << "WALKED A STEP - lat: " << lat << "\n";
      lat = lat;
      lon = m_minimumLongitude;
      XYRangeCheck(lat, lon);

      lat = lat;
      lon = m_maximumLongitude;
      XYRangeCheck(lat, lon);
//cout << "MIN LAT: " << m_minimumLatitude << " MAX LAT: " << m_maximumLatitude << "\n";
    }

//cout << " ************ WALK LONGITUDE ******************\n";
    // Walk left and right edges
    for (lon = m_minimumLongitude; lon <= m_maximumLongitude; lon += 0.01) {
      lat = m_minimumLatitude;
      lon = lon;
      XYRangeCheck(lat, lon);

      lat = m_maximumLatitude;
      lon = lon;
      XYRangeCheck(lat, lon);
    }

    // Walk the limb
    for (double angle = 0.0; angle <= 360.0; angle += 0.01) {
      double x = m_equatorialRadius * cos(angle * PI / 180.0);
      double y = m_equatorialRadius * sin(angle * PI / 180.0);
      if (SetCoordinate(x, y) == 0) {
        if (m_latitude > m_maximumLatitude) {
          continue;
        }
        if (m_longitude > m_maximumLongitude) {
          continue;
        }
        if (m_latitude < m_minimumLatitude) {
          continue;
        }
        if (m_longitude < m_minimumLongitude) {
          continue;
        }

        if (m_minimumX > x) m_minimumX = x;
        if (m_maximumX < x) m_maximumX = x;
        if (m_minimumY > y) m_minimumY = y;
        if (m_maximumY < y) m_maximumY = y;
        XYRangeCheck(m_latitude, m_longitude);
      }
    }

    // Make sure everything is ordered
    if (m_minimumX >= m_maximumX) return false;
    if (m_minimumY >= m_maximumY) return false;

    // Return X/Y min/maxs
    minX = m_minimumX;
    maxX = m_maximumX;
    minY = m_minimumY;
    maxY = m_maximumY;
    return true;
  }
*/


  /**
   * This method is used to determine the x/y range which completely covers the
   * area of interest specified by the radius/longitude range. This range may be
   * obtained from the labels. The purpose of this method is to return the x/y
   * range so it can be used to compute how large a map may need to be. For
   * example, how big a piece of paper is needed or how large of an image needs
   * to be created. The method may fail as indicated by its return value.
   *
   * @param minX Minimum x projection coordinate which covers the radius/
   *             longitude range specified in the labels.
   *
   * @param maxX Maximum x projection coordinate which covers the radius/
   *             longitude range specified in the labels.
   *
   * @param minY Minimum y projection coordinate which covers the radius/
   *             longitude range specified in the labels.
   *
   * @param maxY Maximum y projection coordinate which covers the radius/
   *             longitude range specified in the labels.
   *
   * @return bool
   */
  bool Planar::XYRange(double &minX, double &maxX,
                             double &minY, double &maxY) {
    /*
    double lat, lon;

    // Check the corners of the lat/lon range
    XYRangeCheck(m_minimumLatitude, m_minimumLongitude);
    XYRangeCheck(m_maximumLatitude, m_minimumLongitude);
    XYRangeCheck(m_minimumLatitude, m_maximumLongitude);
    XYRangeCheck(m_maximumLatitude, m_maximumLongitude);

//cout << " ************ WALK LATITUDE ******************\n";
//cout << "MIN LAT: " << m_minimumLatitude << " MAX LAT: " << m_maximumLatitude << "\n";
    // Walk top and bottom edges
    for (lat = m_minimumLatitude; lat <= m_maximumLatitude; lat += 0.01) {
//cout << "WALKED A STEP - lat: " << lat << "\n";
      lat = lat;
      lon = m_minimumLongitude;
      XYRangeCheck(lat, lon);

      lat = lat;
      lon = m_maximumLongitude;
      XYRangeCheck(lat, lon);
//cout << "MIN LAT: " << m_minimumLatitude << " MAX LAT: " << m_maximumLatitude << "\n";
    }

//cout << " ************ WALK LONGITUDE ******************\n";
    // Walk left and right edges
    for (lon = m_minimumLongitude; lon <= m_maximumLongitude; lon += 0.01) {
      lat = m_minimumLatitude;
      lon = lon;
      XYRangeCheck(lat, lon);

      lat = m_maximumLatitude;
      lon = lon;
      XYRangeCheck(lat, lon);
    }

    // Walk the limb
    for (double angle = 0.0; angle <= 360.0; angle += 0.01) {
      double x = m_equatorialRadius * cos(angle * PI / 180.0);
      double y = m_equatorialRadius * sin(angle * PI / 180.0);
      if (SetCoordinate(x, y) == 0) {
        if (m_latitude > m_maximumLatitude) {
          continue;
        }
        if (m_longitude > m_maximumLongitude) {
          continue;
        }
        if (m_latitude < m_minimumLatitude) {
          continue;
        }
        if (m_longitude < m_minimumLongitude) {
          continue;
        }

        if (m_minimumX > x) m_minimumX = x;
        if (m_maximumX < x) m_maximumX = x;
        if (m_minimumY > y) m_minimumY = y;
        if (m_maximumY < y) m_maximumY = y;
        XYRangeCheck(m_latitude, m_longitude);
      }
    }

    // Make sure everything is ordered
    if (m_minimumX >= m_maximumX) return false;
    if (m_minimumY >= m_maximumY) return false;
*/
    // Return X/Y min/maxs
    // TODO: get rid of hard-coding

    m_maximumX = m_maximumRingRadius*cos(m_maximumLongitude);
    m_minimumX = -m_maximumX;
    m_maximumY = m_maximumRingRadius*sin(m_maximumLongitude);
    m_minimumY = -m_maximumY;

    minX = m_minimumX;
    maxX = m_maximumX;
    minY = m_minimumY;
    maxY = m_maximumY;

    return true;
  }


  /**
   * This function returns the keywords that this projection uses.
   *
   * @return PvlGroup The keywords that this projection uses
   */
  PvlGroup Planar::Mapping() {
    PvlGroup mapping = Projection::ringMapping();

    mapping += PvlKeyword("CenterRadius", m_centerRadius);
    mapping += PvlKeyword("CenterLongitude", m_centerLongitude);

    return mapping;
  }

  /**
   * This function returns the radius keywords that this projection uses
   *
   * @return PvlGroup The radius keywords that this projection uses
   */
  PvlGroup Planar::MappingRadii() {
    PvlGroup mapping("Mapping");

    if (HasGroundRange()) {
      mapping += m_mappingGrp["MinimumRadius"];
      mapping += m_mappingGrp["MaximumRadius"];
    }

    return mapping;
  }


  /**
   * This function returns the longitude keywords that this projection uses
   *
   * @return PvlGroup The longitude keywords that this projection uses
   */
  PvlGroup Planar::MappingLongitudes() {
    PvlGroup mapping = Projection::MappingLongitudes();

    mapping += m_mappingGrp["CenterLongitude"];

    return mapping;
  }

  /**
   * This method returns the local radius in meters at the specified longitude.
   * For this method, the local radius is defined as the distance
   * from the center of the planet to the given longitude in the ring plane.
   *
   * @param longitude A longitude in degrees (assumed to be of the correct
   *                 LongitudeType).
   *
   * @throw IException::Unknown - "The given longitude is invalid."
   *
   * @return double The value for the local radius, in meters, at the given
   *                latitude.
   */
  /*
  double Planar::LocalRadius(double longitude) const {

    if (longitude == Null) {
      throw IException(IException::Unknown,
                       "Unable to calculate local radius. The given latitude value ["
                       + iString(latitude) + "] is invalid.",
                       _FILEINFO_);
    }

    double a = m_equatorialRadius;
    double c = m_polarRadius;
    // to save calculations, if the target is spherical, return the eq. rad
    if (a - c < DBL_EPSILON) {
      return a;
    }
    else {
      double lat = latitude * PI / 180.0;
      return  a * c / sqrt(pow(c * cos(lat), 2) + pow(a * sin(lat), 2));
    }
  }
*/

} // end namespace isis

/** 
 * This is the function that is called in order to instantiate an 
 * Planar object.
 *  
 * @param lab Cube labels with appropriate Mapping information.
 *  
 * @param allowDefaults If the label does not contain the value for 
 *                      CenterLongitude, this method indicates
 *                      whether the constructor should compute this value.
 * 
 * @return @b Isis::Projection* Pointer to an Planar projection
 *         object.
 */
extern "C" Isis::Projection *PlanarPlugin(Isis::Pvl &lab,
    bool allowDefaults) {
  return new Isis::Planar(lab, allowDefaults);
}

