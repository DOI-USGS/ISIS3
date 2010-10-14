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
#include "SpecialPixel.h"

namespace Isis {
  /**
   * Constructs a blank angle object which needs a value to be set in order
   *   to do any calculations.
   */
  Angle::Angle() {
    p_radians = Null;
  }

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
        p_radians = angle * DEG2RAD;
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
    // Help prevent any accidental reuse of an Angle object
    p_radians = Null;
  }


  /**
   * This indicates whether we have a legitimate angle stored or are in an
   *   unset, or invalid, state.
   *
   * @returns True if we have a legitimate angle value, false if not initialized
   */
  bool Angle::Valid() const {
    return p_radians != Null;
  }


  /**
   * Add angle value to another. If either of these are invalid angles, then
   *   the result will be an invalid angle.
   *
   * @param angle2 The angle to add to this angle
   * @return sum angle 
   */
  Angle Angle::operator+(const Angle& angle2) const {
    if(!Valid() || !angle2.Valid()) return Angle();

    double ourAngle   = GetRadians();
    double otherAngle = angle2.GetRadians();

    return Angle(ourAngle + otherAngle, Radians);
  }


  /**
   * Subtract angle value from another and return the resulting angle. If
   *   either of these are invalid angles, then the result will be an invalid
   *   angle.
   *
   * @param angle2 The angle to subtract from this angle
   * @return difference angle
   */
  Angle Angle::operator-(const Angle& angle2) const {
    if(!Valid() || !angle2.Valid()) return Angle();

    double ourAngle   = GetRadians();
    double otherAngle = angle2.GetRadians();

    return Angle(ourAngle - otherAngle, Radians);
  }


  /**
   * Multiply this angle by a double and return the resulting angle. If
   *   this is an invalid angle, then the result will be an invalid
   *   angle.
   *
   * @param value The value to multiply to this angle
   * @return difference angle
   */
  Angle Angle::operator*(double value) const {
    if(!Valid()) return Angle();

    return Angle(GetRadians() * value, Radians);
  }


  /**
    * Divide this angle by a double and set this instance to the resulting
    *   angle.
    *
    * @param value The double value to use as the divisor
    * @return Quotient of the angles 
    */
  Angle Angle::operator/(double value) const {
    if(!Valid()) return Angle();

    return Angle(GetRadians() / value, Radians);
  }


  /**
   * Test if the other angle is less than the current angle. If either is
   *   invalid, then an exception will be thrown.
   *
   * @param angle2 The comparison angle (on right-hand-side of < operator) 
   * @return True if the angle is less than the comparision angle 
   */
  bool Angle::operator<(const Angle& angle2) const {
    if(!Valid() || !angle2.Valid()) {
      iString msg = "Cannot compare a invalid angles with the < operator";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    } 

    return (GetAngle(Radians) < angle2.GetAngle(Radians));
  }


  /**
   * Test if the other angle is greater than the current angle. If either is
   *   invalid, then an exception will be thrown.
   *
   * @param angle2 The comparison angle (on right-hand-side of > operator) 
   * @return True if the angle is greater than the comparision angle 
   */
  bool Angle::operator>(const Angle& angle2) const {
    if(!Valid() || !angle2.Valid()) {
      iString msg = "Cannot compare a invalid angles with the > operator";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    } 

    return (GetAngle(Radians) > angle2.GetAngle(Radians));
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
    }

    iString msg = "Angle can not interpret the enumerated value ["  +
      iString(unit) + "] as a unit";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Return angle value in desired units
   *  
   * @param unit Desired units of the angle (see Angle::Units)
   * @return  angle value in specified units
   */
  double Angle::GetAngle(const Units& unit) const {
    // Don't do math on special pixels
    if(p_radians == Null) {
      return Null;
    }

    double angleValue = Null;

    switch (unit) {
      case Radians:
        angleValue = p_radians;
        break;

      case Degrees:
        angleValue = p_radians * RAD2DEG;
        break;
    }

    if(angleValue == Null) {
      iString msg = "Angle can not interpret the enumerated value ["  +
        iString(unit) + "] as a unit";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return angleValue;
  }


  /**
   * Set angle value in desired units
   *
   * @param angle The angle value in units of the unit parameter 
   * @param unit  Desired units of the angle (see Angle::Units)
   */
  void Angle::SetAngle(const double &angle,const Units& unit) {
    // Don't allow non-Null special pixels, Null means no value
    if (IsSpecial(angle) && angle != Null) {
      iString msg = "Angle cannot be a non-Null special pixel";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // Don't do math on special pixels
    if(angle == Null) {
      p_radians = Null;
      return;
    }

    switch (unit) {
      case Radians:
        p_radians = angle;
        break;

      case Degrees:
        p_radians  =  angle * DEG2RAD;
        break;

      default:
        iString msg = "Angle can not interpret the enumerated value ["  +
          iString(unit) + "] as a unit";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }

}

