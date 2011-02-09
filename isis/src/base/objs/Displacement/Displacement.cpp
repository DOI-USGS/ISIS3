/**
 * @file
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

#include "Displacement.h"

#include "Distance.h"
#include "iException.h"
#include "iString.h"
#include "SpecialPixel.h"

namespace Isis {

  /**
   * This initializes the displacement to an invalid state. You must set the
   *   displacement later on with operator= or one of the Set methods.
   */
  Displacement::Displacement() {
    SetDisplacement(Null, Meters);
  }


  /**
   * This creates a displacement equal to a distance.
   *
   * @param distanceToCopy This is the distance we are duplicating
   */
  Displacement::Displacement(const Distance &distanceToCopy) {
    // Use meters because it is the stored format, no precision loss
    SetDisplacement(distanceToCopy.GetMeters(), Meters);
  }


  /**
   * This is the general purpose constructor for Displacement. This will 
   *   initialize with the given displacement.
   *
   * @param displacement The initial displacement; must be in units of
   *     displacementUnit
   * @param displacementUnit The unit of displacement; can be any value in
   *     displacement::Units
   */
  Displacement::Displacement(double displacement, Units displacementUnit) {
    SetDisplacement(displacement, displacementUnit);
  }


  /**
   * Get the displacement in meters.
   *
   * @return Current displacement, in meters.
   */
  double 
    Displacement::GetMeters() const {
    return GetDisplacement(Meters);
  }


  /**
   * Set the displacement in meters.
   *
   * @param displacementInMeters This is the value to set this displacement to, 
   *     given in meters.
   */
  void Displacement::SetMeters(double displacementInMeters) {
    SetDisplacement(displacementInMeters, Meters);
  }


  /**
   * Get the displacement in kilometers.
   *
   * @return Current displacement, in kilometers
   */
  double Displacement::GetKilometers() const {
    return GetDisplacement(Kilometers);
  }


  /**
   * Set the displacement in kilometers.
   *
   * @param displacementInKilometers This is the value to set as the 
   *     displacement, given in kilometers. 
   */
  void Displacement::SetKilometers(double displacementInKilometers) {
    SetDisplacement(displacementInKilometers, Kilometers);
  }


  /**
   * Test if this displacement has been initialized or not
   *
   * @return True if this displacement has been initialized.
   */
  bool Displacement::Valid() const {
    return GetDisplacement(Meters) != Null;
  }


  /**
    * Compare two displacements with the greater than operator.
    *
    * @param otherdisplacement This is the displacement we're comparing to, i.e. 
    *     it is on the right-hand-side of the operator when used
    * @return True if the length of this displacement is greater than the length 
    *     of the given displacement
    */
  bool Displacement::operator >(const Displacement &otherdisplacement) const {
    if(!Valid() || !otherdisplacement.Valid()) {
      iString msg = "Displacement has not been initialized, you must initialize "
          "it first before comparing with another displacement using [>]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return GetMeters() > otherdisplacement.GetMeters();
  }


  /**
    * Compare two displacements with the less than operator.
    *
    * @param otherdisplacement This is the displacement we're comparing to, i.e. 
    *     on the right-hand-side of the operator when used
    * @return True if the length of the displacement is less than the length of 
    *     the given displacement
    */
  bool Displacement::operator <(const Displacement &otherdisplacement) const {
    if(!Valid() || !otherdisplacement.Valid()) {
      iString msg = "Displacement has not been initialized, you must initialize "
          "it first before comparing with another displacement using [<]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return GetMeters() < otherdisplacement.GetMeters();
  }


  /**
   * Add another displacement to this displacement (1km + 5m = 1005m)
   *
   * @param displacementToAdd This is the displacement we are adding to ourselves
   * @return Resulting displacement, self not modified
   */
  Displacement Displacement::operator 
                              +(const Displacement &displacementToAdd) const {
    if(!Valid() || !displacementToAdd.Valid()) return Displacement();

    return Displacement(GetMeters() + displacementToAdd.GetMeters(), Meters);
  }


  /**
   * Subtract another displacement from this displacement (1km - 5m = 995m).
   *
   * @param displacementToSub This is the displacement we are subtracting from 
   *      ourself
   * @return Resulting displacement, self not modified
   */
  Displacement Displacement::operator 
                               -(const Displacement &displacementToSub) const {
    Displacement result(GetMeters() - displacementToSub.GetMeters(), Meters);
    return result;
  }


  /**
   * Subtract a distance from this displacement (1km - 5m = 995m).
   *
   * @param displacementToSub This is the displacement we are subtracting from 
   *      ourself
   * @return Resulting displacement, self not modified
   */
  Displacement Displacement::operator
                               -(const Distance &distanceToSub) const {
    Displacement result(GetMeters() - distanceToSub.GetMeters(), Meters);
    return result;
  }


  /**
   * Divide another displacement into this displacement (5m / 1m = 5).
   *
   * @param displacementToDiv This is the divisor displacement (denominator)
   * @return Resulting value
   */
  double Displacement::operator /(const Displacement &displacementToDiv) const {
    double result = GetMeters() / displacementToDiv.GetMeters();
    return result;
  }


  /**
   * Divide a value from this displacement (5m / 2 = 2.5m).
   *
   * @param valueToDiv This is the divisor displacement (denominator)
   * @return Resulting value
   */
  Displacement Displacement::operator /(const double &valueToDiv) const {
    Displacement result = Displacement(GetMeters() / valueToDiv, Meters);
    return result;
  }


  /**
   * Multiply this displacement by a value (5m * 2 = 10m).
   *
   * @param valueToMult This is the value to multiply by
   * @return Resulting value
   */
  Displacement Displacement::operator *(const double &valueToMult) const {
    Displacement result = Displacement(GetMeters() * valueToMult, Meters);
    return result;
  }


  /**
   * Multiply displacement by a value (5m * 2 = 10m).
   *
   * @param mult This is the value to multiply by
   * @param dist This is the distance to multiply into
   * @return Resulting value
   */
  Displacement operator *(double mult, Displacement displacement) {
    Displacement result = displacement * mult;
    return result;
  }


  /**
   * Add and assign the given displacement to ourselves.
   *
   * @param displacementToAdd This is the displacement we are to add
   */
  void Displacement::operator +=(const Displacement &displacementToAdd) {
    SetDisplacement(GetMeters() + displacementToAdd.GetMeters(), Meters);
  }


  /**
   * Subtract the given displacement from ourself and assign.
   *
   * @param displacementToSub This is the displacement we are to subtract
   */
  void Displacement::operator -=(const Displacement &displacementToSub) {
    SetDisplacement(GetMeters() - displacementToSub.GetMeters(), Meters);
  }


  /**
   * Subtract the given distance from ourself and assign.
   *
   * @param distanceToSub This is the distance we are to subtract
   */
  void Displacement::operator -=(const Distance &distanceToSub) {
    SetDisplacement(GetMeters() - distanceToSub.GetMeters(), Meters);
  }


  /**
   * Divide this displacement by a value and assign the result to ourself.
   *
   * @param valueToDiv This is the value we are going to divide by
   */
  void Displacement::operator /=(const double &valueToDiv) {
    SetDisplacement(GetMeters() / valueToDiv, Meters);
  }


  /**
   * Multiply this displacement by a value and assign the result to ourself.
   *
   * @param valueToMult This is the value we are going to multiply by
   */
  void Displacement::operator *=(const double &valueToMult) {
    SetDisplacement(GetMeters() * valueToMult, Meters);
  }


  /**
   * This is a helper method to access displacements in a universal manner with
   *   uniform error checking.
   *
   * @param displacementUnit Unit of the return value. If this is invalid, an
   *     exception will be thrown.
   * @return The displacement in units of displacementUnit
   */
  double Displacement::GetDisplacement(Units displacementUnit) const {
    double displacementInMeters = p_displacementInMeters;
    double resultingDisplacement = Null;

    if(p_displacementInMeters == Null) return Null;

    switch(displacementUnit) {
      case Meters:
        resultingDisplacement = displacementInMeters;
        break;

      case Kilometers:
        resultingDisplacement = displacementInMeters / 1000.0;
        break;
    }

    if(resultingDisplacement == Null) {
      iString msg = "Displacement does not understand the enumerated value [" +
        iString(displacementUnit) + "] as a unit";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return resultingDisplacement;
  }


  /**
   * This is a helper method to set displacements in a universal manner with
   *   uniform error checking.
   *
   * @param displacement The displacement, in units of displacementUnit, to set this class
   *     to. If this is negative an exception will be thrown and the state
   *     unmodified.
   * @param displacementUnit Unit of displacement. If this is invalid, an
   *     exception will be thrown and the state left unmodified.
   */
  void Displacement::SetDisplacement(const double &displacement, Units displacementUnit) {
    double displacementInMeters = Null;

    if(IsSpecial(displacement)) {
      p_displacementInMeters = Null;
      return;
    }

    switch(displacementUnit) {
      case Meters:
        displacementInMeters = displacement;
        break;

      case Kilometers:
        displacementInMeters = displacement * 1000.0;
        break;
    }

    if(displacementInMeters == Null) {
      iString msg = "Displacement does not understand the enumerated value [" +
        iString(displacementUnit) + "] as a unit";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    p_displacementInMeters = displacementInMeters;
  }


  /**
   * This method is a helper method for the comparison operators which operate
   *   on the length of the displacemnt.
   *
   * @return 
   *     exception will be thrown and the state left unmodified.
   */
  double Displacement::GetLength() const {
    return (fabs(p_displacementInMeters));
  }

}
