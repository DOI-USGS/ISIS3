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

#include <QDebug>

#include "Constants.h"
#include "IString.h"
#include "IException.h"
#include "SpecialPixel.h"

namespace Isis {
  /**
   * Constructs a blank angle object which needs a value to be set in order
   *   to do any calculations.
   */
  Angle::Angle() {
    m_radians = Null;
  }

  /**
   * Constructs an angle object with the entered value and unit
   *
   * @param angle The initial angle value in units of the unit parameter
   * @param unit  The unit of the initial angle (see Angle::Units)
   */
  Angle::Angle(double angle, Units unit) {
    setAngle(angle, unit);
  }


  /**
   * Constructs an angle object from another Angle object
   *
   * @param fromAngle The angle object to copy on initialization
   */
  Angle::Angle(const Angle& fromAngle) {
    m_radians = fromAngle.m_radians;
  }


  /**
   * Constructs an angle object with units of Angle::Degrees from 
   * a QString of the general form "dd mm ss.ss" (there can be more than 2 digits per piece.)
   *  
   * @param angle The value of the angle in degrees, as a QString of the form: "dd mm ss.ss"
   */
  Angle::Angle(QString angle) {
    QString::SectionFlag flag = QString::SectionSkipEmpty;
    bool degreesSucceeded, minutesSucceeded, secondsSucceeded; 

    double degrees = angle.section(' ', 0, 0, flag).toDouble(&degreesSucceeded);
    double minutes = angle.section(' ', 1, 1, flag).toDouble(&minutesSucceeded);
    double seconds = angle.section(' ', 2, 2, flag).toDouble(&secondsSucceeded);

    if (!(degreesSucceeded && minutesSucceeded && secondsSucceeded) ) {
      QString msg = QObject::tr("[%1] is not a vaid input to Angle. It needs to be of the form: "
                    "\"dd mm ss.ss\"").arg(angle);
      throw IException(IException::Programmer, msg, _FILEINFO_); 
    }

    //if the first digit is '-', everything should be negative
    if (degrees < 0) { 
      minutes = -minutes;
      seconds = -seconds; 
    }

    double decimalDegrees = degrees + minutes/60.0 + seconds/3600.0;
    setAngle(decimalDegrees, Angle::Degrees);
  }


  /**
   * Destroys the angle object
   *
   */
  Angle::~Angle() {
    // Help prevent any accidental reuse of an Angle object
    m_radians = Null;
  }


  /**
   * This indicates whether we have a legitimate angle stored or are in an
   *   unset, or invalid, state.
   *
   * @returns True if we have a legitimate angle value, false if not initialized
   */
  bool Angle::isValid() const {
    return m_radians != Isis::Null; // returns false if the value is Null or any other special value
//    return IsValidPixel(m_radians); // returns false if the value is Null or any other special value
  }


  /**
   * Makes an angle to represent a full rotation (0-360 or 0-2pi).
   *
   * @return the angle of 1 full rotation
   */
  Angle Angle::fullRotation() {
    return Angle(360, Degrees);
  }


  /**
   * Add angle value to another. If either of these are invalid angles, then
   *   the result will be an invalid angle.
   *
   * @param angle2 The angle to add to this angle
   * @return sum angle
   */
  Angle Angle::operator+(const Angle& angle2) const {
    if(!isValid() || !angle2.isValid()) return Angle();

    double ourAngle   = radians();
    double otherAngle = angle2.radians();

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
    if(!isValid() || !angle2.isValid()) return Angle();

    double ourAngle   = radians();
    double otherAngle = angle2.radians();

    return Angle(ourAngle - otherAngle, Radians);
  }


  /**
   * Multiply this angle by a double and return the resulting angle. If
   *   this is an invalid angle, then the result will be an invalid
   *   angle.
   *
   * @param value The value to multiply to this angle
   * @return Multiplied angle
   */
  Angle Angle::operator*(double value) const {
    if(!isValid()) return Angle();

    return Angle(radians() * value, Radians);
  }


  /**
   * Multiply this angle by a double and return the resulting angle. If
   *   this is an invalid angle, then the result will be an invalid
   *   angle.
   *
   * @param mult The value to multiply to this angle
   * @param angle The angle being multiplied by mult
   * @return Multiplied angle
   */
  Angle operator *(double mult, Angle angle) {
    return angle * mult;
  }


  /**
    * Divide this angle by a double
    *
    * @param value The double value to use as the divisor
    * @return Quotient of the angles
    */
  Angle Angle::operator/(double value) const {
    if(!isValid()) return Angle();

    return Angle(radians() / value, Radians);
  }


  /**
    * Divide this angle by another angle and return the ratio.
    *
    * @param value The ratio, Null if invalid
    * @return Quotient of the angles
    */
  double Angle::operator/(Angle value) const {
    if(!isValid() || !value.isValid()) return Null;

    return radians() / value.radians();
  }


  /**
   * Test if the other angle is less than the current angle. If either is
   *   invalid, then an exception will be thrown.
   *
   * @param angle2 The comparison angle (on right-hand-side of < operator)
   * @return True if the angle is less than the comparision angle
   */
  bool Angle::operator<(const Angle& angle2) const {
    if(!isValid() || !angle2.isValid()) {
      IString msg = "Cannot compare a invalid angles with the < operator";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    // != comparison allows for angles that are considered equal to be treated
    // as being equal. == operator override uses qFuzzyCompare().
    return (angle(Radians) < angle2.angle(Radians)) && *this != angle2;
  }


  /**
   * Test if the other angle is greater than the current angle. If either is
   *   invalid, then an exception will be thrown.
   *
   * @param angle2 The comparison angle (on right-hand-side of > operator)
   * @return True if the angle is greater than the comparision angle
   */
  bool Angle::operator>(const Angle& angle2) const {
    if(!isValid() || !angle2.isValid()) {
      IString msg = "Cannot compare a invalid angles with the > operator";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    // != comparison allows for angles that are considered equal to be treated
    // as being equal. == operator override uses qFuzzyCompare().
    return (angle(Radians) > angle2.angle(Radians)) && *this != angle2;
  }


  /**
   * Get the angle in human-readable form.
   *
   * @param includeUnits Include the angle's units in the text.
   * @return A user-displayable angle string.
   */
  QString Angle::toString(bool includeUnits) const {
    QString textResult = "";

    if (isValid()) {
      textResult = Isis::toString(degrees());

      if (includeUnits)
        textResult += " degrees";
    }

    return textResult;
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
  double Angle::unitWrapValue(const Units& unit) const {
    switch (unit) {
      case Radians:
        return PI * 2.;
        break;

      case Degrees:
        return 360.;
        break;
    }

    IString msg = "Angle can not interpret the enumerated value ["  +
      IString(unit) + "] as a unit";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Return angle value in desired units
   *
   * @param unit Desired units of the angle (see Angle::Units)
   * @return  angle value in specified units
   */
  double Angle::angle(const Units& unit) const {
    // Don't do math on special pixels
    if(m_radians == Null) {
      return Null;
    }

    double angleValue = Null;

    switch (unit) {
      case Radians:
        angleValue = m_radians;
        break;

      case Degrees:
        angleValue = m_radians * RAD2DEG;
        break;
    }

    if(angleValue == Null) {
      IString msg = "Angle can not interpret the enumerated value ["  +
        IString(unit) + "] as a unit";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return angleValue;
  }


  /**
   * Set angle value in desired units
   *
   * @param angle The angle value in units of the unit parameter
   * @param unit  Desired units of the angle (see Angle::Units)
   */
  void Angle::setAngle(const double &angle,const Units& unit) {
    // Don't allow non-Null special pixels, Null means no value
    if (IsSpecial(angle) && angle != Null) {
      IString msg = "Angle cannot be a non-Null special pixel";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Don't do math on special pixels
    if(angle == Null) {
      m_radians = Null;
      return;
    }

    switch (unit) {
      case Radians:
        m_radians = angle;
        break;

      case Degrees:
        m_radians = angle * DEG2RAD;
        break;

      default:
        IString msg = "Angle can not interpret the enumerated value ["  +
          IString(unit) + "] as a unit";
        throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }

}


/**
 * Display an Angle for a debugging statement.
 *
 * Example:
 * @code
 *   qDebug() << Angle(90, Angle::Degrees);
 * @endcode
 *
 * @param dbg The QDebug we're printing into
 * @param angleToPrint The Angle to display
 * 
 * @return QDebug
 */
QDebug operator<<(QDebug dbg, const Isis::Angle &angleToPrint) {
  dbg.nospace() << angleToPrint.radians() << " <radians> ("
                << angleToPrint.degrees() << " <degrees>)";

  return dbg.space();
}
