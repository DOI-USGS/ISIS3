#ifndef Angle_h
#define Angle_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QtGlobal>
#include <QGlobalStatic>

class QString;

namespace Isis {

  /**
   * @brief Defines an angle and provides unit conversions
   *
   * @author 2010-10-09 Debbie A. Cook
   *
   * @internal
   *   @history 2010-11-01 Steven Lambright - Added methods setRadians and SetDegrees
   *   @history 2012-02-16 Steven Lambright - Brought up to method and member naming standards.
   *   @history 2012-03-22 Steven Lambright - Renamed text() to toString().
   *   @history 2012-06-29 Steven Lambright and Kimberly Oyama - Removed duplicate code from the
   *                           constructor. Added the fullRotation() method which creates
   *                           an angle of 360 degrees and modified the unit test to
   *                           exercise this. References #958.
   *   @history 2012-07-26 Steven Lambright and Kimberly Oyama - Modified the < and >
   *                           operators to make sure they do not return true if the
   *                           two operands are the same. Updated the unitTest to exercise
   *                           this change. References #604.
   *   @history 2013-02-01 Steven Lambright - Added QDebug() printout capabilities.
   *                           References #1060.
   *   @history 2014-07-28 Kristin Berry - Added a constructor which accepts a Qstring of the form:
   *                           "dd mm ss.ss" for angles provided in this format. 
   *   @history 2014-08-21 Jeannie Backer - Modified isValid to utilize the IsValidPixel()
   *                           method in the SpecialPixel header. References #1659.
   *   @history 2017-07-03 Makayla Shepherd - Updated documentation. References #4807.
   *   @history 2018-11-13 Kristin Berry - Made protected methods public as part of testing refactor.
   *   @history 2019-03-11 Kaitlyn Lee - Added comments to > and < operators to clarify why
   *                           the != comparison was needed.
   */
  class Angle {

  public:
    //! The set of usable angle measurement units
    enum Units {
      /**
       * Degrees are generally considered more human readable,
       *   0-360 is one circle, however most math does not use this unit.
       *   Use these for displaying to the user and accepting input from the
       *   user.
       */
      Degrees,
      /**
       * Radians are generally used in mathematical equations, 0-2*PI is one
       *   circle, however these are more difficult for users to understand when
       *   displayed since they have PI in them. This is the default unit and
       *   is necessary for most math library calls.
       */
      Radians
    };

    Angle();
    Angle(double angle, Units unit);
    Angle(const Angle& angle);
    Angle(QString angle);

    virtual ~Angle();

    // Class member operator functions
    bool isValid() const;
    static Angle fullRotation();

    /**
     * Assign angle object equal to another
     *
     * @param angle2 The angle value to copy
     * @return The new angle value
     */
    Angle& operator=(const Angle& angle2) {
      setAngle(angle2.radians(), Radians); return *this;
    }


    Angle operator+(const Angle& angle2) const;
    Angle operator-(const Angle& angle2) const;
    Angle operator*(double value) const;
    friend Angle operator *(double mult, Angle angle);
    Angle operator/(double value) const;
    double operator/(Angle value) const;
    bool operator<(const Angle& angle2) const;
    bool operator>(const Angle& angle2) const;


    /**
     * Add angle value to another as double and replace original
     *
     * @param angle2 The angle to add to this angle
     * @return sum angle, replaces original
     */
    void operator+=(const Angle&  angle2) {
      *this = *this + angle2;
    };


    /**
     * Subtract angle value from another and set this instance to the
     *   resulting angle.
     *
     * @param angle2 The angle to subtract from this angle
     */
    void operator-=(const Angle& angle2) {
      *this = *this - angle2;
    };


    /**
     * Multiply this angle by an integer and return the resulting angle.
     *
     * @param value The integer value to multiply with this angle
     * @return Product of the angles
     */
    Angle operator*(int value) const {
      return *this * (double)value;
    }


    /**
     * Multiply this angle by a double and set this instance to the resulting
     *   angle.
     *
     * @param value The double value to multiply with this angle
     */
    void operator*=(double value) {
      *this = *this * value;
    }


    /**
     * Divide this angle by an integer and return the resulting angle.
     *
     * @param value The double value to use as the divisor
     * @return Quotient of the angles
     */
    Angle operator/(int value) const {
      return *this / (double)value;
    }


    /**
     * Divide this angle by a double and return the resulting angle.
     *
     * @param value The double value to use as the divisor
     */
    void operator/=(double value) {
      *this = *this / value;
    }

    // Relational operator functions

    /**
     * Test if another angle is equal to this angle. This does not compensate
     *   for going around a circle:
     *     Angle(360, Angle::Degrees) does not equal Angle(0, Angle::Degrees)
     *
     * Invalid angles are equal to each other.
     *
     * @param angle2 The comparison angle (on right-hand-side of == operator)
     * @return true if the angle equals the comparision angle
     */
    bool operator==(const Angle& angle2) const {
      return qFuzzyCompare(angle(Radians), angle2.angle(Radians));
    }


    /**
     * Test if another angle is not equal to this angle. Takes the negated
     * result of the '==' operator.
     *
     * @param angle2 The comparison angle (on right-hand-side of != operator)
     * @return true if the angle does not equal the comparision angle
     */
    bool operator!=(const Angle& angle2) const {
      return !(*this == angle2);
    }


    /**
     * Test if the other angle is less than or equal to the current angle
     *
     * @param angle2 The comparison angle (on right-hand-side of < operator)
     * @return true if the angle is less than or equal to the comparision angle
     */
    bool operator<=(const Angle& angle2) const {
      return *this < angle2 || *this == angle2;
    }


    /**
     * Test if the other angle is greater than or equal to the current angle
     *
     * @param angle2 The comparison angle (on right-hand-side of < operator)
     * @return true if the angle is greater than or equal to the comparision
     *          angle
     */
    bool operator>=(const Angle& angle2) const {
      return *this > angle2 || *this == angle2;
    }


    /**
     * Convert an angle to a double. This will return the radians version of
     *   the angle.
     *
     * @return The angle value in radians

    operator double() const { return angle(Radians); }*/

    /**
     * Get the angle in units of Radians.
     * @return A double-value representing this angle. Null if not valid.
     */
    double radians() const { return angle(Radians); }

    /**
     * Get the angle in units of Degrees.
     * @return A double-value representing this angle. Null if not valid.
     */
    double degrees() const { return angle(Degrees); }

    /**
     * Set the angle in units of Radians.
     *
     * @param radians The new angle value, Null for invalid angle
     */
    void setRadians(double radians) { setAngle(radians, Radians); }

    /**
     * Set the angle in units of Degrees.
     *
     * @param degrees The new angle value, Null for invalid angle
     */
    void setDegrees(double degrees) { setAngle(degrees, Degrees); }

    virtual QString toString(bool includeUnits = true) const;
    double unitWrapValue(const Units& unit) const;
    virtual double angle(const Units& unit) const;
    virtual void setAngle(const double &angle, const Units& unit);

  private:
    /**
     * The angle measure, always stored in radians. If degrees are requested
     *   then a conversion is done on the fly.
     */
    double m_radians;
  };
}

QDebug operator<<(QDebug dbg, const Isis::Angle &angleToPrint);

#endif
