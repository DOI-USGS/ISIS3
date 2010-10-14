/**
 * @file
 * $Revision: 1.15 $
 * $Date: 2010/03/19 20:38:01 $
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
#include "Angle.h"

#include <cmath>

#include "Constants.h"
#include "iString.h"
#include "iException.h"

namespace Isis {

  /**
   * Constructs an angle object with the entered value and unit
   *  
   * @param angle The initial angle value in units of the unit parameter 
   * @param unit  The unit of the initial angle (see Angle::Units) 
   */
  Angle::Angle(double angle, Units unit) {
    switch (unit) {
      case Radians:
        p_radians = angle;
        break;

      case Degrees: 
        p_radians = angle * Isis::DEG2RAD;
        break;

      default:
        iString msg = "Angle can not interpret the enumerated value ["  +
          iString(unit) + "] as a unit";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Constructs an angle object from another Angle object 
   *  
   * @param fromAngle The angle object to copy on initialization 
   */
  Angle::Angle(const Angle& fromAngle) {
    p_radians = fromAngle.p_radians;
  }


  /**
   * Destroys the angle object
   *  
   */
  Angle::~Angle() {
    // Prevent any accidental reuse of an Angle object
    p_radians = 0.0;
  }


  /**
   * Return wrap value in desired units. The 'wrap' value is the value where
   *   one circle occurs - angles greater than this are conceptually 'wrapping'
   *   back to zero. For example, this is 2*PI in radians because 2*PI == 0 on
   *   a circle. Please keep in mind we still differentiate those two angles.
   *
   * @param unit Desired units of the Angle wrap constant (see Angle::Units)
   * @return  Wrap value in specified units
   */
  double Angle::UnitWrapValue(const Units& unit) const {
    switch (unit) {
      case Radians:
        return PI * 2.;
        break;

      case Degrees:
        return 360.;
        break;

      default:
        iString msg = "Angle can not interpret the enumerated value ["  +
          iString(unit) + "] as a unit";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Return angle value in desired units
   *  
   * @param unit Desired units of the angle (see Angle::Units)
   * @return  angle value in specified units
   */
  double Angle::GetAngle(const Units& unit) const{
    switch (unit) {
      case Radians:
        return p_radians;
        break;

      case Degrees:
        return p_radians * Isis::RAD2DEG;
        break;

      default:
        iString msg = "Angle can not interpret the enumerated value ["  +
          iString(unit) + "] as a unit";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Set angle value in desired units
   *  
   * @param angle The angle value in units of the unit parameter 
   * @param unit  Desired units of the angle (see Angle::Units)
   */
  void Angle::SetAngle(const double &angle,const Units& unit) {

    switch (unit) {
      case Radians:
        p_radians = angle;
        break;

      case Degrees:
        p_radians  =  angle * Isis::DEG2RAD;
        break;

      default:
        iString msg = "Angle can not interpret the enumerated value ["  +
          iString(unit) + "] as a unit";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }

}

