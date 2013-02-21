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

#include "RingCylindrical.h"

#include <cmath>
#include <cfloat>

#include "Constants.h"
#include "IException.h"
#include "Projection.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "RingPlaneProjection.h"
#include "SpecialPixel.h"

using namespace std;
namespace Isis {

  /**
   * Constructs a RingCylindrical object.
   *
   * @param label This argument must be a Label containing the proper mapping
   *              information as indicated in the Projection class.
   *              Additionally, the ring cylindrical projection requires the
   *              center azimuth to be defined in the keyword CenterAzimuth.
   *
   * @param allowDefaults If set to false, the constructor requires that the
   *                      keyword CenterAzimuth exist in the label. Otherwise
   *                      if it does not exist it will be computed and written
   *                      to the label using the middle of the azimuth range
   *                      as specified in the labels. Defaults to false
   *
   * @throws IException
   */
  RingCylindrical::RingCylindrical(Pvl &label, bool allowDefaults) :
    RingPlaneProjection::RingPlaneProjection(label) {
    try {
      // Try to read the mapping group
      PvlGroup &mapGroup = label.FindGroup("Mapping", Pvl::Traverse);

      // Compute the default value if allowed and needed
      if ((allowDefaults) && (!mapGroup.HasKeyword("CenterAzimuth"))) {
        double az = (m_minimumAzimuth + m_maximumAzimuth) / 2.0;
        mapGroup += PvlKeyword("CenterAzimuth", toString(az));
      }

      // Compute and write the default center radius if allowed and
      // necessary
      if ((allowDefaults) && (!mapGroup.HasKeyword("CenterRadius"))) {
        double radius = (m_minimumRadius + m_maximumRadius) / 2.0;
        mapGroup += PvlKeyword("CenterRadius", toString(radius));
      }

      // Get the center radius and center azimuth. 
      m_centerAzimuth = mapGroup["CenterAzimuth"];
      m_centerRadius = mapGroup["CenterRadius"];

      //  Convert to radians, adjust for azimuth direction
      m_centerAzimuth *= PI / 180.0;
      if (m_azimuthDirection == CounterClockwise) m_centerAzimuth *= -1.0;
    }
    catch(IException &e) {
      string message = "Invalid label group [Mapping]";
      throw IException(e, IException::Io, message, _FILEINFO_);
    }
  }

  //! Destroys the RingCylindrical object
  RingCylindrical::~RingCylindrical() {
  }

  /**
   * Compares two Projection objects to see if they are equal
   *
   * @param proj Projection object to do comparison on
   *
   * @return bool Returns true if the Projection objects are equal, and false if
   *              they are not
   */
  bool RingCylindrical::operator== (const Projection &proj) {
    if (!Projection::operator==(proj)) return false;
    // dont do the below it is a recusive plunge
    //  if (Projection::operator!=(proj)) return false;
    RingCylindrical *ringCyl = (RingCylindrical *) &proj;
    if ((ringCyl->m_centerAzimuth != m_centerAzimuth) ||
        (ringCyl->m_centerRadius != m_centerRadius)) return false;
    return true;
  }

  /**
   * Returns the name of the map projection, "RingCylindrical"
   *
   * @return string Name of projection, "RingCylindrical"
   */
  QString RingCylindrical::Name() const {
    return "RingCylindrical";
  }

  /**
   * Returns the version of the map projection
   *
   * @return std::string Version number
   */
  QString RingCylindrical::Version() const {
    return "1.0";
  }

   /**
    * Returns the center radius, in meters.
    *
    * TODO: Correct this comment for planar projection, assuming right now scale
    * at center of projection.
    * (believe scale is uniform across planar projection)
    *
    * **NOTE** In the case of Planar projections, there is NO radius
    * that is entirely true to scale. The only true scale for this projection is
    * at the single point, (center radius, center azimuth).
    *
    * @return double The center radius.
    */
  double RingCylindrical::TrueScaleRadius() const {
    return m_centerRadius;
  }


   /**
    * Returns the center azimuth, in degrees.
    *
    * @return double The center azimuth.
    */
  double RingCylindrical::CenterAzimuth() const {
    return m_centerAzimuth;
  }


   /**
    * Returns the center radius, in meters.
    *
    * @return double The center radius.
    */
  double RingCylindrical::CenterRadius() const {
    return m_centerRadius;
  }


  /**
   * This method is used to set the radius/longitude (assumed to be of the
   * correct AzimuthDirection, and AzimuthDomain. The Set
   * forces an attempted calculation of the projection X/Y values. This may or
   * may not be successful and a status is returned as such.
   *
   * @param lat Azimuth value to project
   *
   * @param lon Azimuth value to project
   *
   * @return bool
   */
  bool RingCylindrical::SetGround(const double radius, const double az) {
    //TODO Add  scalar to make azimuth distance equivalent to radius distance at center rad
    // Convert to azimuth to radians and adjust
    m_azimuth = az;
    double azRadians = az * DEG2RAD;
    if (m_azimuthDirection == CounterClockwise) azRadians *= -1.0;

    // Check to make sure radius is valid
    if (radius < 0) {
      throw IException(IException::Unknown,
                       "Unable to set radius. The given radius value ["
                       + IString(radius) + "] is invalid.",
                       _FILEINFO_);
    }
    m_radius = radius;

    // Compute the coordinate
    double deltaAz = (azRadians - m_centerAzimuth);
    double x = m_centerRadius * deltaAz;
    double y = radius - m_centerRadius;
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
  bool RingCylindrical::SetCoordinate(const double x, const double y) {
    // Save the coordinate
    SetXY(x, y);

    // Compute radius and make sure it is valid
    m_radius = GetY() + m_centerRadius;
    if (m_radius < m_minimumRadius || m_radius > m_maximumRadius) {
      m_good = false;
      return m_good;
    }

    // Compute azimuth
    m_azimuth = m_centerAzimuth + GetX() / m_centerRadius;

    // Convert to degrees
    m_azimuth *= 180.0 / PI;

    // Cleanup the azimuth
    if (m_azimuthDirection == CounterClockwise) m_azimuth *= -1.0;
    // Do these if the projection is circular
     m_azimuth = To360Domain (m_azimuth);
     if (m_azimuthDomain == 180) m_azimuth = To180Domain(m_azimuth);

    m_good = true;
    return m_good;
  }

  /**
   * This method is used to determine the x/y range which completely covers the
   * area of interest specified by the radus/lon range. The radius/longitude
   * range may be obtained from the labels. This method should not be used if
   * HasGroundRange is false. The purpose of this method is to return the x/y
   * range so it can be used to compute how large a map may need to be. For
   * example, how big a piece of paper is needed or how large of an image needs
   * to be created. This is method and therefore must be written by the derived
   * class (e.g., RingCylindrical). The method may fail as indicated by its return
   * value.
   *
   * 
   * @param &minX Reference to the address where the minimum x 
   *             coordinate value will be written.  The Minimum x projection
   *             coordinate calculated by this method covers the
   *             radius/longitude range specified in the labels.
   *
   * @param &maxX Reference to the address where the maximum x 
   *             coordinate value will be written.  The Maximum x projection
   *             coordinate calculated by this method covers the
   *             radius/longitude range specified in the labels.
   *
   * @param &minY Reference to the address where the minimum y 
   *             coordinate value will be written.  The Minimum y projection
   *             coordinate calculated by this method covers the
   *             radius/longitude range specified in the labels.
   *
   * @param &maxY Reference to the address where the maximum y 
   *             coordinate value will be written.  The Maximum y projection
   *             coordinate calculated by this method covers the
   *             radius/longitude range specified in the labels.
   * 
   * @return bool Indicates whether the method was able to determine the X/Y 
   *              Range of the projection.  If yes, minX, maxX, minY, maxY will
   *              be set with these values.
   *
   */
  bool RingCylindrical::XYRange(double &minX, double &maxX, 
                           double &minY, double &maxY) {
    if (minX == Null || maxX == Null || minY == Null || maxY == Null) {
      return false;
    }
    if (m_groundRangeGood) {
      minX = m_minimumAzimuth;
      maxX = m_maximumAzimuth;
      minY = m_minimumRadius;
      maxY = m_maximumRadius;
      return true;
    }
    return false;
  }


  /**
   * This function returns the keywords that this projection uses.
   *
   * @return PvlGroup The keywords that this projection uses
   */
  PvlGroup RingCylindrical::Mapping() {
    PvlGroup mapping = RingPlaneProjection::Mapping();

    mapping += PvlKeyword("CenterRadius", toString(m_centerRadius));
    mapping += PvlKeyword("CenterAzimuth", toString(m_centerAzimuth));

    return mapping;
  }

  /**
   * This function returns the radii keywords that this projection uses
   *
   * @return PvlGroup The latitude keywords that this projection uses
   */
  PvlGroup RingCylindrical::MappingRadii() {
    PvlGroup mapping = RingPlaneProjection::MappingRadii();

    if (HasGroundRange()) 
      mapping += m_mappingGrp["CenterRadius"];

    return mapping;
  }


  /**
   * This function returns the longitude keywords that this projection uses
   *
   * @return PvlGroup The azimuth keywords that this projection uses
   */
  PvlGroup RingCylindrical::MappingAzimuths() {
    PvlGroup mapping = RingPlaneProjection::MappingAzimuths();

    if (HasGroundRange()) 
      mapping += m_mappingGrp["CenterAzimuth"];

    return mapping;
  }

} // end namespace isis

/** 
 * This is the function that is called in order to instantiate a 
 * RingCylindrical object.
 *  
 * @param lab Cube labels with appropriate Mapping information.
 *  
 * @param allowDefaults Indicates whether CenterLongitude is allowed to be 
 *                      computed.
 * 
 * @return @b Isis::Projection* Pointer to a RingCylindrical 
 *                              projection object.
 */
extern "C" Isis::Projection *RingCylindricalPlugin(Isis::Pvl &lab,
    bool allowDefaults) {
  return new Isis::RingCylindrical(lab, allowDefaults);
}
