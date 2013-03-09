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
#include "RingPlaneProjection.h"

#include <QObject>

#include <cfloat>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <vector>

#include <naif/SpiceUsr.h>

#include "Constants.h"
#include "Displacement.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Longitude.h"
#include "NaifStatus.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "SpecialPixel.h"
#include "WorldMapper.h"

using namespace std;
namespace Isis {
  /**
   * Constructs an empty RingPlaneProjection object.
   *
   * @param label A PVL object containing map projection labels. These labels
   *            are fully described in the Isis Map Projection Users Guide. A
   *            brief example follows:
   *                   @code
   *                       Group = Mapping
   *                         AzimuthDirection = PositiveEast
   *                         AzimuthDomain = 360
   *                         MinimumRadius = 10.8920539924144
   *                         MaximumRadius = 34.7603960060206
   *                         MinimumAzimuth = 219.72432466275
   *                         MaximumAzimuth = 236.186050244411
   *                         PixelResolution = 1387.31209461362
   *                         ProjectionName = Planar
   *                         CenterAzimuth = 220.0
   *                       EndGroup
   *                     End
   *                   @endcode
   *
   * @throw IException::Unknown - "Projection failed. Invalid value 
   *            for keyword [AzimuthDirection] must be [Clockwise or CounterClockwise]"
   * @throw IException::Unknown - "Projection failed. Invalid value 
   *            for keyword [AzimuthDomain] must be [180 or 360]"
   * @throw IException::Unknown - "Projection failed. [MinimumRadius] is not valid"
   * @throw IException::Unknown - "Projection failed. [MaximumRadius] is not valid"
   * @throw IException::Unknown - "Projection failed. 
   *            [MinimumRadius,MaximumRadius] are not properly ordered"
   * @throw IException::Unknown - "Projection failed. 
   *            [MinimumAzimuth,MaximumAzimuth] are not properly ordered"
   * @throw IException::Unknown - "Projection failed. Invalid label 
   *            group [Mapping]"
   * 
   */
  RingPlaneProjection::RingPlaneProjection(Pvl &label) : Projection::Projection(label) {
    try {
      // Mapping group is read by the parent (Projection)
      // Get the Azimuth Direction
      if ((QString) m_mappingGrp["AzimuthDirection"] == "Clockwise") {
        m_azimuthDirection = Clockwise;
      }
      else if ((QString) m_mappingGrp["AzimuthDirection"] == "CounterClockwise") {
        m_azimuthDirection = CounterClockwise;
      }
      else {
        QString msg = "Projection failed. Invalid value for keyword "
                      "[AzimuthDirection] must be "
                      "[Clockwise or CounterClockwise]";
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }

      // Get the AzimuthDomain
      if ((QString) m_mappingGrp["AzimuthDomain"] == "360") {
        m_azimuthDomain = 360;
      }
      else if ((QString) m_mappingGrp["AzimuthDomain"] == "180") {
        m_azimuthDomain = 180;
      }
      else {
        QString msg = "Projection failed. Invalid value for keyword "
                      "[AzimuthDomain] must be [180 or 360]";
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }

      // Get the ground range if it exists
      m_groundRangeGood = false;

      if ((m_mappingGrp.HasKeyword("MinimumAzimuth")) &&
          (m_mappingGrp.HasKeyword("MaximumAzimuth")) &&
          (m_mappingGrp.HasKeyword("MaximumRadius")) &&
          (m_mappingGrp.HasKeyword("MinimumRadius"))) {
        m_minimumAzimuth = m_mappingGrp["MinimumAzimuth"];
        m_maximumAzimuth = m_mappingGrp["MaximumAzimuth"];
        m_minimumRadius = m_mappingGrp["MinimumRadius"];
        m_maximumRadius = m_mappingGrp["MaximumRadius"];

        if (m_minimumRadius < 0) {
          IString msg = "Projection failed. "
                        "[MinimumRadius] of ["+ IString(m_minimumRadius) +  "] is not "
                        + "valid";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }

        if (m_maximumRadius < 0) {
          IString msg = "Projection failed. "
                        "[MaximumRadius] of ["+ IString(m_maximumRadius) +  "] is not "
                        + "valid";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }

        if (m_minimumRadius >= m_maximumRadius) {
          IString msg = "Projection failed. "
                        "[MinimumRadius,MaximumRadius] of ["
                        + IString(m_minimumRadius) + ","
                        + IString(m_maximumRadius) + "] are not "
                        + "properly ordered";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }

        if (m_minimumAzimuth >= m_maximumAzimuth) {
          IString msg = "Projection failed. "
                        "[MinimumAzimuth,MaximumAzimuth] of ["
                        + IString(m_minimumAzimuth) + "," 
                        + IString(m_maximumAzimuth) + "] are not "
                        + "properly ordered";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }

        m_groundRangeGood = true;
      }
      else {
        // if no ground range is given, initialize the min/max rad/az to 0
        m_minimumRadius  = 0.0;
        m_maximumRadius  = 0.0;
        m_minimumAzimuth = 0.0;
        m_maximumAzimuth = 0.0;
      }

      // Initialize miscellaneous protected data elements
      // initialize the rest of the x,y,rad,az member variables
      m_radius = Null;
      m_azimuth = Null;

      // If we made it to this point, we have what we need for a ring plane projection
      setProjectionType(RingPlane);
    }
    catch(IException &e) {
      IString msg = "Projection failed.  Invalid label group [Mapping]";
      throw IException(e, IException::Unknown, msg, _FILEINFO_);
    }
  }

  //! Destroys the Projection object
  RingPlaneProjection::~RingPlaneProjection() {
  }

  /**
   * This method determines whether two map projection objects are equal by 
   * comparing the azimuth direction, resolution, and projection name. 
   *  
   * @param proj A reference to a Projection object to which this Projection 
   *             will be compared.
   *
   * @return bool Indicates whether the Projection objects are equivalent.
   */
  bool RingPlaneProjection::operator== (const Projection &proj) {
    if (!Projection::operator==(proj)) return false;
    RingPlaneProjection *rproj = (RingPlaneProjection *) &proj;
    if (IsClockwise() != rproj->IsClockwise()) return false;
    if (Has180Domain() != rproj->Has180Domain()) return false;
    return true;
  }


  /**
   * This method returns the radius of true scale. It is a virtual function 
   * and if it is not overriden the default radius of true scale is 0 (at 
   * the equator). Otherwise it is projection specific. This method is 
   * used by the Scale routine to ensure the  local radius is used in the 
   * computation. 
   *
   * @return double The radius where the projection is not distorted.
   */
  double RingPlaneProjection::TrueScaleRadius() const {
    return 0.0;
  }


  /**
   * This indicates if the longitude direction type is positive west (as
   * opposed to postive east). The longitude type was obtained from the 
   * label during object construction.
   *
   * @return bool
   */
  bool RingPlaneProjection::IsClockwise() const {
    return m_azimuthDirection == Clockwise;
  }

  /**
   * This indicates if the longitude direction type is positive east (as
   * opposed to postive west). The longitude type was obtained from the 
   * label during object construction.
   *
   * @return bool
   */
  bool RingPlaneProjection::IsCounterClockwise() const {
    return m_azimuthDirection == CounterClockwise;
  }

  /**
   * This method converts an azimuth into the clockwise direction.
   *
   * @param az Azimuth to convert into the clockwise direction.
   * @param domain Must be an integer value of 180 (for -180 to 180) or 360 (for 
   *            0 to 360).
   *  
   * @throw IException::Unknown - "The given azimuth is invalid." 
   * @throw IException::Unknown - "Unable to convert azimuth.  Domain is
   *            not 180 or 360."
   *  
   * @return double Azimuth value, in clockwise direction.
   */
  double RingPlaneProjection::ToClockwise(const double az, const int domain) {
    if (az == Null) {
      throw IException(IException::Unknown, 
                       "Unable to convert to Clockwise. The given azimuth value [" 
                       + IString(az) + "] is invalid.", 
                       _FILEINFO_);
    }
    double myaz = az;

    myaz *= -1;

    if (domain == 360) {
      myaz = To360Domain(myaz);
    }
    else if (domain == 180) {
      myaz = To180Domain(myaz);
    }
    else {
      IString msg = "Unable to convert azimuth.  Domain [" + IString(domain) 
                    + "] is not 180 or 360.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    return myaz;
  }

  /**
   * This method converts an azimuth into the counterclockwise direction.
   *
   * @param lon Azimuth to convert into the counterclockwise direction.
   * @param domain Must be an integer value of 180 (for -180 to 180) or 360 (for 
   *            0 to 360).
   *
   * @throw IException::Unknown - "The given azimuth is invalid." 
   * @throw IException::Unknown - "Unable to convert azimuth.  Domain is
   *            not 180 or 360."
   *  
   * @return double Azimuth value, in counterclockwise direction.
   */
  double RingPlaneProjection::ToCounterClockwise(const double az, const int domain) {
    if (az == Null) {
      throw IException(IException::Unknown, 
                       "Unable to convert to CounterClockwise. The given azimuth value [" 
                       + IString(az) + "] is invalid.", 
                       _FILEINFO_);
    }
    double myaz = az;

    myaz *= -1;

    if (domain == 360) {
      myaz = To360Domain(myaz);
    }
    else if (domain == 180) {
      myaz = To180Domain(myaz);
    }
    else {
      IString msg = "Unable to convert azimuth.  Domain [" + IString(domain)
                    + "] is not 180 or 360.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    return myaz;
  }


  /**
   * This method returns the azimuth direction as a string. It will return
   * either Clockwise or CounterClockwise.
   *
   * @return string The azimuth direction, "Clockwise" or 
   *         "CounterClockwise".
   */
  string RingPlaneProjection::AzimuthDirectionString() const {
    if (m_azimuthDirection == Clockwise) return "Clockwise";
    return "CounterClockwise";
  }

  /**
   * This indicates if the longitude domain is -180 to 180 (as opposed to 0
   * to 360). The azimuth domain was obtained from the label during object
   * construction.
   *
   * @return bool
   */
  bool RingPlaneProjection::Has180Domain() const {
    return m_azimuthDomain == 180;
  }

  /**
   * This indicates if the azimuth domain is 0 to 360 (as opposed to -180
   * to 180). The azimuth domain was obtained from the label during object
   * construction.
   *
   * @return bool
   */
  bool RingPlaneProjection::Has360Domain() const {
    return m_azimuthDomain == 360;
  }

  /**
   * This method converts a longitude into the -180 to 180 domain. It will leave
   * the longitude unchanged if it is already in the domain.
   *
   * @param lon Longitude to convert into the -180 to 180 domain.
   *
   * @throw IException::Unknown - "The given longitude is invalid." 
   *  
   * @return double The longitude, converted to 180 domain.
   */
  double RingPlaneProjection::To180Domain(const double az) {
    if (az == Null) {
      throw IException(IException::Unknown, 
                       "Unable to convert to 180 degree domain. The given azimuth value [" 
                       + IString(az) + "] is invalid.", 
                       _FILEINFO_);
    }
    return Isis::Longitude(az, Angle::Degrees).force180Domain().degrees();
  }

  /**
   * This method converts an azimuth into the 0 to 360 domain. It will leave 
   * the azimuth unchanged if it is already in the domain.
   *
   * @param az Azimuth to convert into the 0 to 360 domain.
   *
   * @return double The azimuth, converted to 360 domain.
   */
  double RingPlaneProjection::To360Domain(const double az) {
    if (az == Null) {
      throw IException(IException::Unknown, 
                       "Unable to convert to 360 degree domain. The given azimuth value [" 
                       + IString(az) + "] is invalid.", 
                       _FILEINFO_);
    }
    double result = az;

    if ( (az < 0.0 || az > 360.0) &&
        !qFuzzyCompare(az, 0.0) && !qFuzzyCompare(az, 360.0)) {
     result = Isis::Longitude(az, Angle::Degrees).force360Domain().degrees();
    }

    return result;
  }

  /**
   * This method returns the longitude domain as a string. It will return either
   * 180 or 360.
   *
   * @return string The longitude domain, "180" or "360".
   */
  string RingPlaneProjection::AzimuthDomainString() const {
    if (m_azimuthDomain == 360) return "360";
    return "180";
  }

  /**
   * This returns the minimum radius of the area of interest. The value 
   * was obtained from the labels during object construction. This method 
   * can only be used if HasGroundRange returns a true.
   *
   * @return double
   */
  double RingPlaneProjection::MinimumRadius() const {
    return m_minimumRadius;
  }

  /**
   * This returns the maximum radius of the area of interest. The value 
   * was obtained from the labels during object construction. This method 
   * can only be used if HasGroundRange returns a true.
   *
   * @return double
   */
  double RingPlaneProjection::MaximumRadius() const {
    return m_maximumRadius;
  }

  /**
   * This returns the minimum azimuth of the area of interest. The value
   * was obtained from the labels during object construction. This method 
   * can only be used if HasGroundRange returns a true.
   *
   * @return double
   */
  double RingPlaneProjection::MinimumAzimuth() const {
    return m_minimumAzimuth;
  }

  /**
   * This returns the maximum azimuth of the area of interest. The value
   * was obtained from the labels during object construction. This method 
   * can only be used if HasGroundRange returns a true.
   *
   * @return double
   */
  double RingPlaneProjection::MaximumAzimuth() const {
    return m_maximumAzimuth;
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
   * @param lon Longitude value to project
   *
   * @return bool Indicates whether the method was successful.
   */
  bool RingPlaneProjection::SetGround(const double rad, const double az) {
    if (rad == Null || az == Null) {
      m_good = false;
      return m_good;
    }
    else {
      m_radius = rad;
      m_azimuth = az;
      m_good = true;
      SetComputedXY(az, rad);
    }
    return m_good;
  }

  /**
   * This method is used to set the projection x/y. The Set forces an attempted
   * calculation of the corresponding latitude/longitude position. This may or
   * may not be successful and a status is returned as such.  Usually this 
   * method is overridden in a dervied class, for example, Sinusoidal. If not 
   * the default method simply copies x/y to  lat/lon to x/y which is no 
   * projection. 
   *
   * @param x X coordinate of the projection in units that are the same as the
   *          radii in the label
   * @param y Y coordinate of the projection in units that are the same as the
   *          azimuth in the label
   *
   * @return bool Indicates whether the method was successful.
   */
  bool RingPlaneProjection::SetCoordinate(const double x, const double y) {
    if (x == Null || y == Null) {
      m_good = false;
    }
    else {
      m_good = true;
      SetXY(x, y);
      m_radius = XCoord();
      m_azimuth = YCoord();
    }
    return m_good;
  }


  /**
   * This returns a radius.  The method can only be used if SetGround, SetCoordinate,
   * SetUniversalGround, or SetWorld return with success. Success can also
   * be checked using the IsGood method.
   *
   * @return double
   */
  double RingPlaneProjection::Radius() const {
    return m_radius;
  }


  /**
   * This returns a local radius.  The method can only be used if SetGround, SetCoordinate,
   * SetUniversalGround, or SetWorld return with success. Success can also
   * be checked using the IsGood method.
   *
   * @return double
   */
  double RingPlaneProjection::LocalRadius() const {
    return m_radius;
  }

  /**
   * This returns a longitude with correct longitude direction and domain as
   * specified in the label object. The method can only be used if 
   * SetGround, SetCoordinate, SetUniversalGround, or SetWorld return with 
   * success. Success can also be checked using the IsGood method. 
   *
   * @return double
   */
  double RingPlaneProjection::Azimuth() const {
    return m_azimuth;
  }

  /**
   * This method is used to set the latitude/longitude which must be
   * Planetocentric (latitude) and PositiveEast/Domain360 (longitude). The Set
   * forces an attempted calculation of the projection X/Y values. This may or
   * may not be successful and a status is returned as such.
   *
   * @param lat Planetocentric Latitude value to project
   * @param lon PositiveEast, Domain360 Longitude value to project
   *
   * @return bool Indicates whether the method was successful.
   */
  bool RingPlaneProjection::SetUniversalGround(const double rad, const double az) {
    if (rad == Null || az == Null) {
      m_good = false;
      return m_good;
    }
    // Deal with the azimuth first
    m_azimuth = az;
    if (m_azimuthDirection == CounterClockwise) m_azimuth = -az;
    if (m_azimuthDomain == 180) {
      m_azimuth = To180Domain(m_azimuth);
    }
    else {
      // Do this because azimuthDirection could cause (-360,0)
      m_azimuth = To360Domain(m_azimuth);
    }

    // Nothing to do with radius
    m_radius = rad;

    // Now the rad/az are in user defined coordinates so set them
    return SetGround(m_radius, m_azimuth);
    }

  /**
   * This returns a universal radius, which is just the radius. 
   *
   * @return double The universal radius.
   */
  double RingPlaneProjection::UniversalRadius() {
    double rad = m_radius;
    return rad;
  }


  /**
   * This returns a universal azimuth (clockwise in 0 to 360 domain). The
   * method can only be used if SetGround, SetCoordinate, SetUniversalGround, or
   * SetWorld return with success. Success can also be checked using the IsGood
   * method.
   *
   * @return double The universal azimuth.
   */
  double RingPlaneProjection::UniversalAzimuth() {
    double az = m_azimuth;
    if (m_azimuthDirection == Clockwise) az = -az;
    az = To360Domain(az);
    return az;
  }


  /**
   * This method returns the scale for mapping world coordinates into projection
   * coordinates. For example, if the world coordinate system is an image then
   * this routine returns the number of pixels per degree. Likewise, if the
   * world coordinate system is a piece of paper, it might return the number of
   * inches of paper per degree. If the SetWorldMapper method is not invoked
   * then this method returns 1.0
   *
   * @return double The scale for mapping.
   */
  double RingPlaneProjection::Scale() const {
    if (m_mapper != NULL) {
      double localRadius = TrueScaleRadius();
      return localRadius / m_mapper->Resolution() * DEG2RAD;
     // return localRadius / m_mapper->Resolution();
    }
    else {
      return 1.0;
    }
  }


  /**
   * This method is used to determine the x/y range which completely covers the
   * area of interest specified by the rad/az range. The radius/azimuth
   * range may be obtained from the labels. This method should not be used if
   * HasGroundRange is false. The purpose of this method is to return the x/y
   * range so it can be used to compute how large a map may need to be. For
   * example, how big a piece of paper is needed or how large of an image needs
   * to be created. This is method and therefore must be written by the derived
   * class (e.g., Planar). The method may fail as indicated by its return
   * value.
   *
   * 
   * @param &minX Reference to the address where the minimum x 
   *             coordinate value will be written.  The Minimum x projection
   *             coordinate calculated by this method covers the
   *             radius/azimuth range specified in the labels.
   *
   * @param &maxX Reference to the address where the maximum x 
   *             coordinate value will be written.  The Maximum x projection
   *             coordinate calculated by this method covers the
   *             radius/azimuth range specified in the labels.
   *
   * @param &minY Reference to the address where the minimum y 
   *             coordinate value will be written.  The Minimum y projection
   *             coordinate calculated by this method covers the
   *             radius/azimuth range specified in the labels.
   *
   * @param &maxY Reference to the address where the maximum y 
   *             coordinate value will be written.  The Maximum y projection
   *             coordinate calculated by this method covers the
   *             radius/azimuth range specified in the labels.
   * 
   * @return bool Indicates whether the method was able to determine the X/Y 
   *              Range of the projection.  If yes, minX, maxX, minY, maxY will
   *              be set with these values.
   *
   */
  bool RingPlaneProjection::XYRange(double &minX, double &maxX, 
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
   * This convience function is established to assist in the development of the
   * XYRange virtual method. It allows the developer to test ground points
   * (rad/az) to see if they produce a minimum/maximum projection coordinate.
   * For example in Planar,
   *    @code
   *       bool Planar::XYRange(double &minX, double &maxX,
   *                                    double &minY, double &maxY) {
   *        // Check the corners of the rad/az range
   *         XYRangeCheck (m_minimumRadius,m_minimumAzimuth);
   *         XYRangeCheck (m_maximumRadius,m_minimumAzimuth);
   *         XYRangeCheck (m_minimumRadius,m_maximumAzimuth);
   *         XYRangeCheck (m_maximumRadius,m_maximumAzimuth);
   *
   *         // If the azimuth crosses 0/360 check there
   *         if ((m_minimumAzimuth < 0.0) && (m_maximumAzimuth > 0.0)) ||
   *             (m_minimumAzimuth < 360.0) && (m_maximumAzimuth > 360.0)) {
   *           XYRangeCheck (minimumRadius, 0. or 360.);
   *           XYRangeCheck (maximumRadius, 0 or 360.);
   *         }
   *
   *         // Make sure everything is ordered
   *         if (m_minimumX >= m_maximumX) return false;
   *         if (m_minimumY >= m_maximumY) return false;
   *
   *         // Return X/Y min/maxs
   *         minX = m_minimumX;
   *         maxX = m_maximumX;
   *         minY = m_minimumY;
   *         maxY = m_maximumY;
   *         return true;
   *      }
   *    @endcode
   *
   *
   * @param radius Test for min/max projection coordinates at this radius
   * @param azimuth Test for min/max projection coordinates at this azimuth
   */
  void RingPlaneProjection::XYRangeCheck(const double radius, const double azimuth) {
    if (radius == Null || azimuth == Null) {
      m_good = false;
      return;
    }
    SetGround(radius, azimuth);
    if (!IsGood()) return;

    if (XCoord() < m_minimumX) m_minimumX = XCoord();
    if (XCoord() > m_maximumX) m_maximumX = XCoord();
    if (YCoord() < m_minimumY) m_minimumY = YCoord();
    if (YCoord() > m_maximumY) m_maximumY = YCoord();
    return;
  }

  /** 
   * This method is used to find the XY range for oblique aspect projections 
   * (non-polar projections) by "walking" around each of the min/max lat/lon. 
   *  
   * @param minX Minimum x projection coordinate which covers the latitude
   *             longitude range specified in the labels.
   * @param maxX Maximum x projection coordinate which covers the latitude
   *             longitude range specified in the labels.
   * @param minY Minimum y projection coordinate which covers the latitude
   *             longitude range specified in the labels.
   * @param maxY Maximum y projection coordinate which covers the latitude
   *             longitude range specified in the labels.
   * 
   * @return @b bool Indicates whether the method was successful.
   * @see XYRange()
   * @author Stephen Lambright 
   * @internal
   *   @history 2011-07-02 Jeannie Backer - Moved this code from
   *                           ObliqueCylindrical class to its own method here.
   */
  // bool Projection::xyRangeOblique(double &minX, double &maxX, 
  //                                 double &minY, double &maxY) {
  //   if (minX == Null || maxX == Null || minY == Null || maxY == Null) {
  //     return false;
  //   }
  //   //For oblique, we'll have to walk all 4 sides to find out min/max x/y values
  //   if (!HasGroundRange()) return false; // Don't have min/max lat/lon, 
  //                                       //can't continue

  //   m_specialLatCases.clear();
  //   m_specialLonCases.clear();

  //   // First, search longitude for min X/Y
  //   double minFoundX1, minFoundX2;
  //   double minFoundY1, minFoundY2;

  //   // Search for minX between minlat and maxlat along minlon
  //   doSearch(MinimumLatitude(), MaximumLatitude(), 
  //            minFoundX1, MinimumLongitude(), true, true, true);
  //   // Search for minX between minlat and maxlat along maxlon
  //   doSearch(MinimumLatitude(), MaximumLatitude(), 
  //            minFoundX2, MaximumLongitude(), true, true, true);
  //   // Search for minY between minlat and maxlat along minlon
  //   doSearch(MinimumLatitude(), MaximumLatitude(), 
  //            minFoundY1, MinimumLongitude(), false, true, true);
  //   // Search for minY between minlat and maxlat along maxlon
  //   doSearch(MinimumLatitude(), MaximumLatitude(), 
  //            minFoundY2, MaximumLongitude(), false, true, true);

  //   // Second, search latitude for min X/Y
  //   double minFoundX3, minFoundX4;
  //   double minFoundY3, minFoundY4;

  //   // Search for minX between minlon and maxlon along minlat
  //   doSearch(MinimumLongitude(), MaximumLongitude(), 
  //            minFoundX3, MinimumLatitude(), true, false, true);
  //   // Search for minX between minlon and maxlon along maxlat
  //   doSearch(MinimumLongitude(), MaximumLongitude(), 
  //            minFoundX4, MaximumLatitude(), true, false, true);
  //   // Search for minY between minlon and maxlon along minlat
  //   doSearch(MinimumLongitude(), MaximumLongitude(), 
  //            minFoundY3, MinimumLatitude(), false, false, true);
  //   // Search for minY between minlon and maxlon along maxlat
  //   doSearch(MinimumLongitude(), MaximumLongitude(), 
  //            minFoundY4, MaximumLatitude(), false, false, true);

  //   // We've searched all possible minimums, go ahead and store the lowest
  //   double minFoundX5 = min(minFoundX1, minFoundX2);
  //   double minFoundX6 = min(minFoundX3, minFoundX4);
  //   m_minimumX = min(minFoundX5, minFoundX6);

  //   double minFoundY5 = min(minFoundY1, minFoundY2);
  //   double minFoundY6 = min(minFoundY3, minFoundY4);
  //   m_minimumY = min(minFoundY5, minFoundY6);

  //   // Search longitude for max X/Y
  //   double maxFoundX1, maxFoundX2;
  //   double maxFoundY1, maxFoundY2;

  //   // Search for maxX between minlat and maxlat along minlon
  //   doSearch(MinimumLatitude(), MaximumLatitude(), 
  //            maxFoundX1, MinimumLongitude(), true, true, false);
  //   // Search for maxX between minlat and maxlat along maxlon
  //   doSearch(MinimumLatitude(), MaximumLatitude(), 
  //            maxFoundX2, MaximumLongitude(), true, true, false);
  //   // Search for maxY between minlat and maxlat along minlon
  //   doSearch(MinimumLatitude(), MaximumLatitude(), 
  //            maxFoundY1, MinimumLongitude(), false, true, false);
  //   // Search for maxY between minlat and maxlat along maxlon
  //   doSearch(MinimumLatitude(), MaximumLatitude(), 
  //            maxFoundY2, MaximumLongitude(), false, true, false);

  //   // Search latitude for max X/Y
  //   double maxFoundX3, maxFoundX4;
  //   double maxFoundY3, maxFoundY4;

  //   // Search for maxX between minlon and maxlon along minlat
  //   doSearch(MinimumLongitude(), MaximumLongitude(), 
  //            maxFoundX3, MinimumLatitude(), true, false, false);
  //   // Search for maxX between minlon and maxlon along maxlat
  //   doSearch(MinimumLongitude(), MaximumLongitude(), 
  //            maxFoundX4, MaximumLatitude(), true, false, false);
  //   // Search for maxY between minlon and maxlon along minlat
  //   doSearch(MinimumLongitude(), MaximumLongitude(), 
  //            maxFoundY3, MinimumLatitude(), false, false, false);
  //   // Search for maxY between minlon and maxlon along maxlat
  //   doSearch(MinimumLongitude(), MaximumLongitude(), 
  //            maxFoundY4, MaximumLatitude(), false, false, false);

  //   // We've searched all possible maximums, go ahead and store the highest
  //   double maxFoundX5 = max(maxFoundX1, maxFoundX2);
  //   double maxFoundX6 = max(maxFoundX3, maxFoundX4);
  //   m_maximumX = max(maxFoundX5, maxFoundX6);

  //   double maxFoundY5 = max(maxFoundY1, maxFoundY2);
  //   double maxFoundY6 = max(maxFoundY3, maxFoundY4);
  //   m_maximumY = max(maxFoundY5, maxFoundY6);

  //   // Look along discontinuities for more extremes
  //   vector<double> specialLatCases = m_specialLatCases;
  //   for (unsigned int specialLatCase = 0; 
  //       specialLatCase < specialLatCases.size(); 
  //       specialLatCase ++) {
  //     double minX, maxX, minY, maxY;

  //     // Search for minX between minlon and maxlon along latitude discontinuities
  //     doSearch(MinimumLongitude(), MaximumLongitude(), 
  //              minX, specialLatCases[specialLatCase], true,  false, true);
  //     // Search for minY between minlon and maxlon along latitude discontinuities
  //     doSearch(MinimumLongitude(), MaximumLongitude(), 
  //              minY, specialLatCases[specialLatCase], false, false, true);
  //     // Search for maxX between minlon and maxlon along latitude discontinuities
  //     doSearch(MinimumLongitude(), MaximumLongitude(), 
  //              maxX, specialLatCases[specialLatCase], true,  false, false);
  //     // Search for maxX between minlon and maxlon along latitude discontinuities
  //     doSearch(MinimumLongitude(), MaximumLongitude(), 
  //              maxY, specialLatCases[specialLatCase], false, false, false);

  //     m_minimumX = min(minX, m_minimumX);
  //     m_maximumX = max(maxX, m_maximumX);
  //     m_minimumY = min(minY, m_minimumY);
  //     m_maximumY = max(maxY, m_maximumY);
  //   }

  //   vector<double> specialLonCases = m_specialLonCases;
  //   for (unsigned int specialLonCase = 0; 
  //       specialLonCase < specialLonCases.size(); 
  //       specialLonCase ++) {
  //     double minX, maxX, minY, maxY;

  //     // Search for minX between minlat and maxlat along longitude discontinuities
  //     doSearch(MinimumLatitude(), MaximumLatitude(), 
  //              minX, specialLonCases[specialLonCase], true,  true, true);
  //     // Search for minY between minlat and maxlat along longitude discontinuities
  //     doSearch(MinimumLatitude(), MaximumLatitude(), 
  //              minY, specialLonCases[specialLonCase], false, true, true);
  //     // Search for maxX between minlat and maxlat along longitude discontinuities
  //     doSearch(MinimumLatitude(), MaximumLatitude(), 
  //              maxX, specialLonCases[specialLonCase], true,  true, false);
  //     // Search for maxY between minlat and maxlat along longitude discontinuities
  //     doSearch(MinimumLatitude(), MaximumLatitude(), 
  //              maxY, specialLonCases[specialLonCase], false, true, false);

  //     m_minimumX = min(minX, m_minimumX);
  //     m_maximumX = max(maxX, m_maximumX);
  //     m_minimumY = min(minY, m_minimumY);
  //     m_maximumY = max(maxY, m_maximumY);
  //   }

  //   m_specialLatCases.clear();
  //   m_specialLonCases.clear();

  //   // Make sure everything is ordered
  //   if (m_minimumX >= m_maximumX) return false;
  //   if (m_minimumY >= m_maximumY) return false;

  //   // Return X/Y min/maxs
  //   minX = m_minimumX;
  //   maxX = m_maximumX;
  //   minY = m_minimumY;
  //   maxY = m_maximumY;

  //   return true;
  // }

  /**
   * This method searches for extreme (min/max/discontinuity) coordinate values
   * along the constBorder line between minBorder and maxBorder (that is, 
   * across latitudes/longitudes). This method locates the extrema by utilizing
   * the findExtreme() method until the coordinate values converge. Then, 
   * extremeVal parameter is updated with this value before returning. 
   *  
   * Discontinuities are stored in m_specialLatCases and m_specialLonCases so 
   * they may be checked again later, which creates significantly more accuracy 
   * in some cases. 
   *  
   * @param minBorder Minimum latitude or longitude to search between.
   * @param maxBorder Maximum latitude or longitude to search between.
   * @param extremeVal The resulting global coordinate value (min or max 
   *            value for x or y, depending on findMin and searchX) on the
   *            constBorder.
   * @param constBorder The latitude or longitude that remains constant.  The 
   *            method will step along this border.
   * @param searchX Indicates whether the method is searching for a min or max 
   *            x-coordinate.  If false the method searches for min or max
   *            y-coordinate.
   * @param searchLongitude Indicates whether the method will search 
   *            along a longitude. If true, constBorder is longitude and all
   *            other borders are latitudes. If false, the method searches a
   *            latitude (constBorder is a lat, other borders lons).
   * @param findMin Indicates whether the method is looking for a minimum 
   *            coordinate value. If false, the method is looking for a maximum
   *            value.
   * @author Steven Lambright 
   * @internal
   *   @history 2011-07-02 Jeannie Backer - Moved this code from
   *                           ObliqueCylindrical class to its own method
   *                           here. Added condition to do-while loop for
   *                           more precision. Replaced hard-coded
   *                           TOLERANCE with 1/2 pixel resolution.
   *                           (Defaults to 0.5)
   */
  // void Projection::doSearch(double minBorder, double maxBorder, 
  //                           double &extremeVal, const double constBorder, 
  //                           bool searchX, bool searchAzimuth, bool findMin) {
  //   if (minBorder == Null || maxBorder == Null || constBorder == Null) {
  //     return;
  //   }
  //   const double TOLERANCE = m_pixelResolution/2;
  //   const int NUM_ATTEMPTS = (unsigned int)DBL_DIG; // It's unsafe to go past 
  //                                                   // this precision

  //   double minBorderX, minBorderY, maxBorderX, maxBorderY;
  //   int attempts = 0;

  //   do {
  //     findExtreme(minBorder, maxBorder, minBorderX, minBorderY, maxBorderX, 
  //                 maxBorderY, constBorder, searchX, searchLongitude, findMin);
  //     if (minBorderX == Null && maxBorderX == Null 
  //         && minBorderY == Null && maxBorderY == Null ) {
  //       attempts = NUM_ATTEMPTS;
  //       continue;
  //     }
  //     attempts ++;
  //   }
  //   while ((fabs(minBorderX - maxBorderX) > TOLERANCE 
  //          || fabs(minBorderY - maxBorderY) > TOLERANCE)
  //          && (attempts < NUM_ATTEMPTS)); 
  //   // check both x and y distance in case symmetry of map
  //   // For example, if minBorderX = maxBorderX but minBorderY = -maxBorderY,
  //   // these points may not be close enough.

  //   if (attempts >= NUM_ATTEMPTS) {
  //     // We zoomed in on a discontinuity because our range never shrank, this
  //     // will need to be rechecked later. 
  //     // *min and max border should be nearly identical, so it doesn't matter
  //     //  which is used here
  //     if (searchLongitude) {
  //       m_specialLatCases.push_back(minBorder);
  //     }
  //     else {
  //       m_specialLonCases.push_back(minBorder);
  //     }
  //   }

  //   // These values will always be accurate, even over a discontinuity
  //   if (findMin) {
  //     if (searchX) extremeVal = min(minBorderX, maxBorderX);
  //     else         extremeVal = min(minBorderY, maxBorderY);
  //   }
  //   else {
  //     if (searchX) extremeVal = max(minBorderX, maxBorderX);
  //     else         extremeVal = max(minBorderY, maxBorderY);
  //   }
  //   return;
  // }

  /**
   * Searches for extreme (min/max/discontinuity) coordinate values across 
   * latitudes/longitudes. 
   *  
   * This method looks for these extrema along the constBorder between minBorder 
   * and maxBorder by stepping along constBorder (10 times) from the minBorder 
   * and maxBorder. Then, the range of this extreme value is recorded in 
   * minBorder and maxBorder and the coordinate values corresponding to these 
   * new borders are stored in minBorderX, minBorderY, maxBorderX and 
   * maxBorderY. 
   *  
   * This function should be used by calling it repeatedly until minBorderX and 
   * minBorderY do not equal maxBorderX and maxBorderY, respectively. 
   * Discontinuities will cause the minBorderX, minBorderY, maxBorderX and 
   * maxBorderY to never converge. If minBorderX never comes close to maxBorderX 
   * or minBorderY never comes close to maxBorderY, then between minBorder and 
   * maxBorder is the value of the most extreme value. In this case, either the 
   * smaller or larger of the x or y values found will be correct, depending on 
   * the values of findMin and searchX. 
   *  
   *  
   * 
   *
   * @param minBorder Minimum latitude or longitude to search between. This 
   *            value gets updated to a more precise range.
   * @param maxBorder Maximum latitude or longitude to search between. This 
   *            value gets updated to a more precise range.
   * @param minBorderX The x-value corresponding to the lower resultant 
   *            minBorder and the constBorder, which is more accurate when
   *            nearly equal to maxBorderX.
   * @param minBorderY The y-value corresponding to the lower resultant 
   *            minBorder and the constBorder, which is more accurate when
   *            nearly equal to maxBorderY.
   * @param maxBorderX The x-value corresponding to the higher resultant 
   *            maxBorder and the constBorder, which is more accurate when
   *            nearly equal to minBorderX.
   * @param maxBorderY The y-value corresponding to the higher resultant 
   *            maxBorder and the constBorder, which is more accurate when
   *            nearly equal to minBorderY.
   * @param constBorder The latitude or longitude that remains constant.  The 
   *            method will step along this border.
   * @param searchX Indicates whether the method is searching for a min or max 
   *            x-coordinate.  If false the method searches for min or max
   *            y-coordinate.
   * @param searchLongitude Indicates whether the method will search 
   *            along a longitude. If true, constBorder is longitude and all
   *            other borders are latitudes. If false, the method searches a
   *            latitude (constBorder is a lat, other borders lons).
   * @param findMin Indicates whether the method is looking for a minimum 
   *            coordinate value. If false, the method is looking for a maximum
   *            value.
   * @author Stephen Lambright 
   * @internal
   *   @history 2011-07-02 Jeannie Backer - Moved this code from
   *                           ObliqueCylindrical class to its own method here.
   *                           Replaced parameters minVal and maxVal with
   *                           minBorderX, minBorderY, maxBorderX, and
   *                           maxBorderY.
   */
  // void Projection::findExtreme(double &minBorder,  double &maxBorder,
  //                              double &minBorderX, double &minBorderY,
  //                              double &maxBorderX, double &maxBorderY, 
  //                              const double constBorder, bool searchX, 
  //                              bool searchAzimuth, bool findMin) {
  //   if (minBorder == Null || maxBorder == Null || constBorder == Null) {
  //     minBorderX = Null;
  //     minBorderY = minBorderX;
  //     minBorderX = minBorderX;
  //     minBorderY = minBorderX;
  //     return;
  //   }
  //   if (!searchAzimuth && (fabs(fabs(constBorder) - 90.0) < DBL_EPSILON)) {
  //     // it is impossible to search "along" a pole
  //     setSearchGround(minBorder, constBorder, searchAzimuth);
  //     minBorderX = XCoord();
  //     minBorderY = YCoord();
  //     maxBorderX = minBorderX;
  //     maxBorderY = minBorderY;
  //     return;
  //   }
  //   // Always do 10 steps
  //   const double STEP_SIZE = (maxBorder - minBorder) / 10.0;
  //   const double LOOP_END = maxBorder + (STEP_SIZE / 2.0); // This ensures we do 
  //                                                          // all of the steps
  //                                                          // properly
  //   double currBorderVal = minBorder;
  //   setSearchGround(minBorder, constBorder, searchAzimuth);

  //   // this makes sure that the initial currBorderVal is valid before entering
  //   // the loop below
  //   if (!m_good){
  //     // minBorder = currBorderVal+STEP_SIZE < LOOP_END until setGround is good?
  //     // then, if still not good return?
  //     while (!m_good && currBorderVal <= LOOP_END) {
  //       currBorderVal+=STEP_SIZE;
  //       if (searchAzimuth && (currBorderVal - 90.0 > DBL_EPSILON)) {
  //         currBorderVal = 90.0;
  //       }
  //       setSearchGround(currBorderVal, constBorder, searchAzimuth);
  //     }
  //     if (!m_good) {
  //       minBorderX = Null;
  //       minBorderY = minBorderX;
  //       minBorderX = minBorderX;
  //       minBorderY = minBorderX;
  //       return;
  //     }
  //   }

  //   // save the values of three consecutive steps from the minBorder towards
  //   // the maxBorder along the constBorder. initialize these three border
  //   // values (the non-constant lat or lon)
  //   double border1 = currBorderVal;
  //   double border2 = currBorderVal;
  //   double border3 = currBorderVal;

  //   // save the coordinate (x or y) values that correspond to the first
  //   // two borders that are being saved.
  //   // initialize these two coordinate values (x or y)
  //   double value1 = (searchX) ? XCoord() : YCoord();
  //   double value2 = value1;

  //   // initialize the extreme coordinate value 
  //   // -- this is the largest coordinate value found so far
  //   double extremeVal2 = value2;

  //   // initialize the extreme border values
  //   // -- these are the borders on either side of the extreme coordinate value
  //   double extremeBorder1 = minBorder;
  //   double extremeBorder3 = minBorder;

  //   while (currBorderVal <= LOOP_END) {

  //     // this conditional was added to prevent trying to SetGround with an
  //     // invalid latitude greater than 90 degrees. There is no need check for
  //     // latitude less than -90 since we start at the minBorder (already
  //     // assumed to be valid) and step forward toward (and possibly past)
  //     // maxBorder
  //     if (searchAzimuth && (currBorderVal - 90.0 > DBL_EPSILON)) {
  //       currBorderVal = 90.0;
  //     }

  //     // update the current border value along constBorder
  //     currBorderVal += STEP_SIZE;
  //     setSearchGround(currBorderVal, constBorder, searchAzimuth);
  //     if (!m_good){ 
  //       continue;
  //     } 
                     
  //     // update the border and coordinate values 
  //     border3 = border2;
  //     border2 = border1;
  //     border1 = currBorderVal;                                             
  //     value2 = value1;
  //     value1 = (searchX) ? XCoord() : YCoord();

  //     if ((findMin && value2 < extremeVal2) 
  //         || (!findMin && value2 > extremeVal2)) {
  //       // Compare the coordinate value associated with the center border with 
  //       // the current extreme. If the updated coordinate value is more extreme
  //       // (smaller or larger, depending on findMin), then we update the
  //       // extremeVal and it's borders.
  //       extremeVal2 = value2;

  //       extremeBorder3 = border3;
  //       extremeBorder1 = border1;
  //     }
  //   }

  //   // update min/max border values to the values on either side of the most 
  //   // extreme coordinate found in this call to this method
    
  //   minBorder = extremeBorder3; // Border 3 is lagging and thus smaller

  //   // since the loop steps past the original maxBorder, we want to retain 
  //   // the original maxBorder value so we don't go outside of the original
  //   // min/max range given
  //   if (extremeBorder1 <= maxBorder ) {
  //     maxBorder = extremeBorder1; // Border 1 is leading and thus larger
  //   }

  //   // update minBorder coordinate values
  //   setSearchGround(minBorder, constBorder, searchAzimuth);
  //   // if (!m_good){
  //   //   this should not happen since minBorder has already been verified in 
  //   //   the while loop above
  //   // }

  //   minBorderX = XCoord();
  //   minBorderY = YCoord();

  //   // update maxBorder coordinate values
  //   setSearchGround(maxBorder, constBorder, searchAzimuth);
  //   // if (!m_good){
  //   //   this should not happen since maxBorder has already been verified in
  //   //   the while loop above
  //   // }

  //   maxBorderX = XCoord();
  //   maxBorderY = YCoord();
  //   return;
  // }

  /**
   * This function sets the ground for the given border values.  It calls the 
   * SetGround(lat, lon) method with the appropriate lat/lon values, depending 
   * on whether variableIsLat is true. 
   *  
   * This method is used by doSearch and findExtreme in order to set the ground 
   * correctly each time. 
   *
   * @param variableBorder The latitude or longitude that is variable in the 
   *            search methods.
   * @param constBorder The latitude or longitude that is constant in the search 
   *            methods.
   * @param variableIsLat Indicates whether variableBorder is the latittude 
   *            value and constBorder is the longitude value. If false,
   *            variableBorder is the longitude value and constBorder is the
   *            latitude value.
   * @author Stephen Lambright 
   * @internal
   *   @history 2011-07-02 Jeannie Backer - Moved this code from
   *                           ObliqueCylindrical class to its own method here.
   *                           Added error.
   */
  // void Projection::setSearchGround(const double variableBorder, 
  //                                  const double constBorder, 
  //                                  bool variableIsLat) {
  //   if (variableBorder == Null || constBorder == Null) {
  //     return;
  //   }
  //   double lat, lon;
  //   if (variableIsLat) {
  //     lat = variableBorder;
  //     lon = constBorder;
  //   }
  //   else {
  //     lat = constBorder;
  //     lon = variableBorder;
  //   }
  //   SetGround(lat, lon);
  //   return;
  // }


  /**
   * This function returns the keywords that this projection uses.
   *
   * @return PvlGroup The keywords that this projection uses
   */
  PvlGroup RingPlaneProjection::Mapping() {
    PvlGroup mapping("Mapping");

    if (m_mappingGrp.HasKeyword("TargetName")) {
      mapping += m_mappingGrp["TargetName"];
    }

    mapping += m_mappingGrp["ProjectionName"];
    mapping += m_mappingGrp["AzimuthDirection"];
    mapping += m_mappingGrp["AzimuthDomain"];

    if (m_mappingGrp.HasKeyword("PixelResolution")) {
      mapping += m_mappingGrp["PixelResolution"];
    }
    if (m_mappingGrp.HasKeyword("Scale")) {
      mapping += m_mappingGrp["Scale"];
    }
    if (m_mappingGrp.HasKeyword("UpperLeftCornerX")) {
      mapping += m_mappingGrp["UpperLeftCornerX"];
    }
    if (m_mappingGrp.HasKeyword("UpperLeftCornerY")) {
      mapping += m_mappingGrp["UpperLeftCornerY"];
    }

    if (HasGroundRange()) {
      mapping += m_mappingGrp["MinimumRadius"];
      mapping += m_mappingGrp["MaximumRadius"];
      mapping += m_mappingGrp["MinimumAzimuth"];
      mapping += m_mappingGrp["MaximumAzimuth"];
    }

    if (m_mappingGrp.HasKeyword("Rotation")) {
      mapping += m_mappingGrp["Rotation"];
    }

    return mapping;
  }

  /**
   * This function returns the radius keywords that this projection uses
   *
   * @return PvlGroup The radius keywords that this projection uses
   */
  PvlGroup RingPlaneProjection::MappingRadii() {
    PvlGroup mapping("Mapping");

    if (HasGroundRange()) {
      mapping += m_mappingGrp["MinimumRadius"];
      mapping += m_mappingGrp["MaximumRadius"];
    }

    return mapping;
  }


  /**
   * This function returns the azimuth keywords that this projection uses
   *
   * @return PvlGroup The azimuth keywords that this projection uses
   */
  PvlGroup RingPlaneProjection::MappingAzimuths() {
    PvlGroup mapping("Mapping");

    if (HasGroundRange()) {
      mapping += m_mappingGrp["MinimumAzimuth"];
      mapping += m_mappingGrp["MaximumAzimuth"];
    }

    return mapping;
  }


} //end namespace isis


