#ifndef Angle_h
#define Angle_h
/**
 * @file
 * $Revision: 1.14 $
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

namespace Isis {
  /**
   *  @brief Defines an angle and provides unit conversions
   *
   *  @author 2010-10-09 Debbie A. Cook
   *
   *  @internal
   *    @history 2010-11-01 Steven Lambright - Added methods SetRadians and
   *                          SetDegrees
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
    Angle(double angle, Units unit=Radians);
    Angle(const Angle& angle);

    virtual ~Angle();


    // Class member operator functions 
    bool Valid() const;

    /**
     * Assign angle object equal to another
     *
     * @param angle2 The angle value to copy
     * @return The new angle value 
     */
    Angle& operator=(const Angle& angle2) { 
      SetAngle(angle2.GetRadians(), Radians); return *this;
    }


    Angle operator+(const Angle& angle2) const;
    Angle operator-(const Angle& angle2) const;
    Angle operator*(double value) const;
    Angle operator/(double value) const;
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
      return (GetAngle(Radians) == angle2.GetAngle(Radians));
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
     */
    operator double() const { return GetAngle(Radians); }

    /**
     * Get the angle in units of Radians.
     */
    double GetRadians() const { return GetAngle(Radians); }

    /**
     * Get the angle in units of Degrees.
     */
    double GetDegrees() const { return GetAngle(Degrees); }

    /**
     * Set the angle in units of Radians.
     *
     * @param radians The new angle value, Null for invalid angle
     */
    void SetRadians(double radians) { SetAngle(radians, Radians); }

    /**
     * Set the angle in units of Degrees.
     *
     * @param degrees The new angle value, Null for invalid angle
     */
    void SetDegrees(double degrees) { SetAngle(degrees, Degrees); }

  protected:
    double UnitWrapValue(const Units& unit) const;
    virtual double GetAngle(const Units& unit) const;
    virtual void SetAngle(const double &angle, const Units& unit);

  private:
    /**
     * The angle measure, always stored in radians. If degrees are requested
     *   then a conversion is done on the fly.
     */
    double p_radians;
  };
}

#endif
