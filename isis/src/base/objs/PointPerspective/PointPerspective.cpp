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

#include "PointPerspective.h"

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
   * @throws IException::Io
   */
  PointPerspective::PointPerspective(Pvl &label, bool allowDefaults) :
    Projection::Projection(label) {
    try {
      // Try to read the mapping group
      PvlGroup &mapGroup = label.FindGroup("Mapping", Pvl::Traverse);

      // Compute and write the default center longitude if allowed and
      // necessary
      if ((allowDefaults) && (!mapGroup.HasKeyword("CenterLongitude"))) {
        double lon = (m_minimumLongitude + m_maximumLongitude) / 2.0;
        mapGroup += PvlKeyword("CenterLongitude", toString(lon));
      }

      // Compute and write the default center latitude if allowed and
      // necessary
      if ((allowDefaults) && (!mapGroup.HasKeyword("CenterLatitude"))) {
        double lat = (m_minimumLatitude + m_maximumLatitude) / 2.0;
        mapGroup += PvlKeyword("CenterLatitude", toString(lat));
      }

      // Get the center longitude  & latitude
      m_centerLongitude = mapGroup["CenterLongitude"];
      m_centerLatitude = mapGroup["CenterLatitude"];
      if (IsPlanetocentric()) {
        m_centerLatitude = ToPlanetographic(m_centerLatitude);
      }

      // convert to radians, adjust for longitude direction
      m_centerLongitude *= PI / 180.0;
      m_centerLatitude *= PI / 180.0;
      if (m_longitudeDirection == PositiveWest) m_centerLongitude *= -1.0;

      // Calculate sine & cosine of center latitude
      m_sinph0 = sin(m_centerLatitude);
      m_cosph0 = cos(m_centerLatitude);

      // Get the distance above planet center (the point of perspective from
      // the center of planet), and calculate P
      m_distance = mapGroup["Distance"];
      m_distance *= 1000.;
      m_P = 1.0 + (m_distance / m_equatorialRadius);

    }
    catch(IException &e) {
      QString message = "Invalid label group [Mapping]";
      throw IException(e, IException::Io, message, _FILEINFO_);
    }
  }

  //! Destroys the PointPerspective object
  PointPerspective::~PointPerspective() {
  }

  /**
   * Compares two Projection objects to see if they are equal
   *
   * @param proj Projection object to do comparison on
   *
   * @return bool Returns true if the Projection objects are equal, and false if
   *              they are not
   */
  bool PointPerspective::operator== (const Projection &proj) {
    if (!Projection::operator==(proj)) return false;
    // dont do the below it is a recusive plunge
    //  if (Projection::operator!=(proj)) return false;
    PointPerspective *point = (PointPerspective *) &proj;
    if((point->m_centerLongitude != this->m_centerLongitude) ||
        (point->m_centerLatitude != this->m_centerLatitude) ||
        (point->m_distance != this->m_distance)) return false;
    return true;
  }

  /**
   * Returns the name of the map projection, "PointPerspective"
   *
   * @return QString Name of projection, "PointPerspective"
   */
  QString PointPerspective::Name() const {
    return "PointPerspective";
  }

  /**
   * Returns the version of the map projection
   *
   * @return std::QString Version number
   */
  QString PointPerspective::Version() const {
    return "1.0";
  }

  /**
   * Returns the latitude of true scale, in degrees.  In the case of 
   * PointPerspective it is the center latitude. 
   *
   * @return double The center latitude.
   */
  double PointPerspective::TrueScaleLatitude() const {
    return m_centerLatitude * 180.0 / PI;
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
  bool PointPerspective::SetGround(const double lat, const double lon) {
    // Convert longitude to radians & clean up
    m_longitude = lon;
    double lonRadians = lon * PI / 180.0;
    if (m_longitudeDirection == PositiveWest) lonRadians *= -1.0;

    // Now convert latitude to radians & clean up ... it must be planetographic
    m_latitude = lat;
    double latRadians = lat;
    if (IsPlanetocentric()) latRadians = ToPlanetographic(latRadians);
    latRadians *= PI / 180.0;

    // Compute helper variables
    double deltaLon = (lonRadians - m_centerLongitude);
    double sinphi = sin(latRadians);
    double cosphi = cos(latRadians);
    double coslon = cos(deltaLon);

    // Lat/Lon cannot be projected
    double g =  m_sinph0 * sinphi + m_cosph0 * cosphi * coslon;
    if (g < (1.0 / m_P)) {
      m_good = false;
      return m_good;
    }

    // Compute the coordinates
    double ksp = (m_P - 1.0) / (m_P - g);
    double x = m_equatorialRadius * ksp * cosphi * sin(deltaLon);
    double y = m_equatorialRadius * ksp *
               (m_cosph0 * sinphi - m_sinph0 * cosphi * coslon);
    SetComputedXY(x, y);
    m_good = true;
    return m_good;
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
    SetXY(x, y);

    // Declare instance variables and calculate rho
    double rho, rp, con, com, z, sinz, cosz;
    const double epsilon = 1.0e-10;
    rho = sqrt(GetX() * GetX() + GetY() * GetY());
    rp = rho / m_equatorialRadius;
    con = m_P - 1.0;
    com = m_P + 1.0;

    // Error calculating rho - should be less than equatorial radius
    if (rp > (sqrt(con / com))) {
      m_good = false;
      return m_good;
    }

    // Calculate the latitude and longitude
    m_longitude = m_centerLongitude;
    if (fabs(rho) <= epsilon) {
      m_latitude = m_centerLatitude;
    }
    else {
      if (rp <= epsilon) {
        sinz = 0.0;
      }
      else {
        sinz = (m_P - sqrt(1.0 - rp * rp * com / con)) / (con / rp + rp / con);
      }
      z = asin(sinz);
      sinz = sin(z);
      cosz = cos(z);
      con = cosz * m_sinph0 + GetY() * sinz * m_cosph0 / rho;
      if (con > 1.0) con = 1.0;
      if (con < -1.0) con = -1.0;
      m_latitude = asin(con);

      con = fabs(m_centerLatitude) - HALFPI;
      if (fabs(con) <= epsilon) {
        if (m_centerLatitude >= 0.0) {
          m_longitude += atan2(GetX(), -GetY());
        }
        else {
          m_longitude += atan2(-GetX(), GetY());
        }
      }
      else {
        con = cosz - m_sinph0 * sin(m_latitude);
        if ((fabs(con) >= epsilon) || (fabs(GetX()) >= epsilon)) {
          m_longitude += atan2(GetX() * sinz * m_cosph0, con * rho);
        }
      }
    }

    // Convert to degrees
    m_latitude *= 180.0 / PI;
    m_longitude *= 180.0 / PI;

    // Cleanup the longitude
    if (m_longitudeDirection == PositiveWest) m_longitude *= -1.0;
    // These need to be done for circular type projections
    m_longitude = To360Domain(m_longitude);
    if (m_longitudeDomain == 180) m_longitude = To180Domain(m_longitude);

    // Cleanup the latitude
    if (IsPlanetocentric()) m_latitude = ToPlanetocentric(m_latitude);

    m_good = true;
    return m_good;
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
  bool PointPerspective::XYRange(double &minX, double &maxX, double &minY, double &maxY) {
    double lat, lon;

    // Check the corners of the lat/lon range
    XYRangeCheck(m_minimumLatitude, m_minimumLongitude);
    XYRangeCheck(m_maximumLatitude, m_minimumLongitude);
    XYRangeCheck(m_minimumLatitude, m_maximumLongitude);
    XYRangeCheck(m_maximumLatitude, m_maximumLongitude);

    // Walk top and bottom edges
    for (lat = m_minimumLatitude; lat <= m_maximumLatitude; lat += 0.01) {
      lat = lat;
      lon = m_minimumLongitude;
      XYRangeCheck(lat, lon);

      lat = lat;
      lon = m_maximumLongitude;
      XYRangeCheck(lat, lon);
    }

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
        if (m_latitude > m_maximumLatitude) continue;
        if (m_longitude > m_maximumLongitude) continue;
        if (m_latitude < m_minimumLatitude) continue;
        if (m_longitude < m_minimumLongitude) continue;
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


  /**
   * This function returns the keywords that this projection uses.
   *
   * @return PvlGroup The keywords that this projection uses
   */
  PvlGroup PointPerspective::Mapping() {
    PvlGroup mapping = Projection::Mapping();

    mapping += m_mappingGrp["CenterLatitude"];
    mapping += m_mappingGrp["CenterLongitude"];
    mapping += m_mappingGrp["Distance"];

    return mapping;
  }

  /**
   * This function returns the latitude keywords that this projection uses
   *
   * @return PvlGroup The latitude keywords that this projection uses
   */
  PvlGroup PointPerspective::MappingLatitudes() {
    PvlGroup mapping = Projection::MappingLatitudes();

    mapping += m_mappingGrp["CenterLatitude"];

    return mapping;
  }

  /**
   * This function returns the longitude keywords that this projection uses
   *
   * @return PvlGroup The longitude keywords that this projection uses
   */
  PvlGroup PointPerspective::MappingLongitudes() {
    PvlGroup mapping = Projection::MappingLongitudes();

    mapping += m_mappingGrp["CenterLongitude"];

    return mapping;
  }
} // end namespace isis

/**
 * This is the function that is called in order to instantiate a 
 * PointPerspectve object. 
 *
 * @param lab The Pvl from the cube labels 
 * @param allowDefaults Indicates whether defaults are allowed. 
 *
 * @return Isis::Projection* PointPerspective
 */
extern "C" Isis::Projection *PointPerspectivePlugin(Isis::Pvl &lab,
    bool allowDefaults) {
  return new Isis::PointPerspective(lab, allowDefaults);
}

