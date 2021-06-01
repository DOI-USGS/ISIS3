/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Displacement.h"

#include "Distance.h"
#include "IException.h"
#include "IString.h"
#include "SpecialPixel.h"

namespace Isis {

  /**
   * This initializes the displacement to an invalid state. You must set the
   *   displacement later on with operator= or one of the Set methods.
   */
  Displacement::Displacement() {
    setDisplacement(Null, Meters);
  }


  /**
   * This creates a displacement equal to a distance.
   *
   * @param distanceToCopy This is the distance we are duplicating
   */
  Displacement::Displacement(const Distance &distanceToCopy) {
    // Use meters because it is the stored format, no precision loss
    setDisplacement(distanceToCopy.meters(), Meters);
  }


  /**
   * This is the general purpose constructor for Displacement. This will
   *   initialize with the given displacement. If Pixels are supplied as the
   *   units, then a default pixels/meter = 1.0 will be used.
   *
   * @param displacement The initial displacement; must be in units of
   *     displacementUnit
   * @param displacementUnit The unit of displacement; can be any value in
   *     displacement::Units
   */
  Displacement::Displacement(double displacement, Units displacementUnit) {
    if(displacementUnit == Pixels)
      displacementUnit = Meters;

    setDisplacement(displacement, displacementUnit);
  }


  /**
   * This is a constructor for Displacement given pixels with a conversion
   *   ratio. This will initialize with the pixels converted to meters.
   *
   * @param displacementInPixels The displacement to initialize with, must be in
   *     units of pixels and should not be negative
   * @param pixelsPerMeter The pixels/meter conversion factor
   */
  Displacement::Displacement(double displacementInPixels,
                             double pixelsPerMeter) {
    setDisplacement(displacementInPixels / pixelsPerMeter, Meters);
  }


  /**
   * Get the displacement in meters.
   *
   * @return Current displacement, in meters.
   */
  double Displacement::meters() const {
    return displacement(Meters);
  }


  /**
   * Set the displacement in meters.
   *
   * @param displacementInMeters This is the value to set this displacement to,
   *     given in meters.
   */
  void Displacement::setMeters(double displacementInMeters) {
    setDisplacement(displacementInMeters, Meters);
  }


  /**
   * Get the displacement in kilometers.
   *
   * @return Current displacement, in kilometers
   */
  double Displacement::kilometers() const {
    return displacement(Kilometers);
  }


  /**
   * Set the displacement in kilometers.
   *
   * @param displacementInKilometers This is the value to set as the
   *     displacement, given in kilometers.
   */
  void Displacement::setKilometers(double displacementInKilometers) {
    setDisplacement(displacementInKilometers, Kilometers);
  }


  /**
   * Get the displacement in pixels using the given conversion ratio.
   *
   * @param pixelsPerMeter Pixels/Meters conversion ratio to use, stored data
   *         is always in meters
   * @return Current displacement, in pixels
   */
  double Displacement::pixels(double pixelsPerMeter) const {
    return displacement(Meters) * pixelsPerMeter;
  }


  /**
   * Set the displacement in pixels.
   *
   * @param displacementInPixels This is the value to set this displacement to,
   *     given in pixels.
   * @param pixelsPerMeter Pixels/Meters conversion ratio to use, stored data
   *         is always in meters
   */
  void Displacement::setPixels(double displacementInPixels,
                               double pixelsPerMeter) {
    setDisplacement(displacementInPixels / pixelsPerMeter, Meters);
  }


  /**
   * Test if this displacement has been initialized or not
   *
   * @return True if this displacement has been initialized.
   */
  bool Displacement::isValid() const {
    return displacement(Meters) != Null;
  }


  /**
    * Compare two displacements with the greater than operator.
    *
    * @param otherdisplacement This is the displacement we're comparing to, i.e.
    *     it is on the right-hand-side of the operator when used
    * @return True if the length of this displacement is greater than the length
    *     of the given displacement
    */
  bool Displacement::operator >(const Displacement &otherDisplacement) const {
    if(!isValid() || !otherDisplacement.isValid()) {
      IString msg = "Displacement has not been initialized, you must initialize "
          "it first before comparing with another displacement using [>]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return meters() > otherDisplacement.meters();
  }


  /**
    * Compare two displacements with the less than operator.
    *
    * @param otherdisplacement This is the displacement we're comparing to, i.e.
    *     on the right-hand-side of the operator when used
    * @return True if the length of the displacement is less than the length of
    *     the given displacement
    */
  bool Displacement::operator <(const Displacement &otherDisplacement) const {
    if(!isValid() || !otherDisplacement.isValid()) {
      IString msg = "Displacement has not been initialized, you must initialize "
          "it first before comparing with another displacement using [<]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return meters() < otherDisplacement.meters();
  }


  /**
   * Add another displacement to this displacement (1km + 5m = 1005m)
   *
   * @param displacementToAdd This is the displacement we are adding to ourselves
   * @return Resulting displacement, self not modified
   */
  Displacement Displacement::operator
                              +(const Displacement &displacementToAdd) const {
    if(!isValid() || !displacementToAdd.isValid()) return Displacement();

    return Displacement(meters() + displacementToAdd.meters(), Meters);
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
    if(!isValid() || !displacementToSub.isValid()) return Displacement();

    Displacement result(meters() - displacementToSub.meters(), Meters);
    return result;
  }


  /**
   * Subtract a distance from this displacement (1km - 5m = 995m).
   *
   * @param distanceToSub This is the displacement we are subtracting from
   *      ourself
   * @return Resulting displacement, self not modified
   */
  Displacement Displacement::operator
                               -(const Distance &distanceToSub) const {
    if(!isValid() || !distanceToSub.isValid()) return Displacement();

    Displacement result(meters() - distanceToSub.meters(), Meters);
    return result;
  }


  /**
   * Divide another displacement into this displacement (5m / 1m = 5).
   *
   * @param displacementToDiv This is the divisor displacement (denominator)
   * @return Resulting value
   */
  double Displacement::operator /(const Displacement &displacementToDiv) const {
    if(!isValid() || !displacementToDiv.isValid()) return Null;

    double result = meters() / displacementToDiv.meters();
    return result;
  }


  /**
   * Divide a value from this displacement (5m / 2 = 2.5m).
   *
   * @param valueToDiv This is the divisor displacement (denominator)
   * @return Resulting value
   */
  Displacement Displacement::operator /(const double &valueToDiv) const {
    if(!isValid() || IsSpecial(valueToDiv)) return Displacement();

    Displacement result = Displacement(meters() / valueToDiv, Meters);
    return result;
  }


  /**
   * Multiply this displacement by a value (5m * 2 = 10m).
   *
   * @param valueToMult This is the value to multiply by
   * @return Resulting value
   */
  Displacement Displacement::operator *(const double &valueToMult) const {
    if(!isValid() || IsSpecial(valueToMult)) return Displacement();

    Displacement result = Displacement(meters() * valueToMult, Meters);
    return result;
  }


  /**
   * Multiply displacement by a value (5m * 2 = 10m).
   *
   * @param mult This is the value to multiply by
   * @param displacement This is the distance to multiply into
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
    if(!isValid() || !displacementToAdd.isValid())
      setDisplacement(Null, Meters);
    else
      setDisplacement(meters() + displacementToAdd.meters(), Meters);
  }


  /**
   * Subtract the given displacement from ourself and assign.
   *
   * @param displacementToSub This is the displacement we are to subtract
   */
  void Displacement::operator -=(const Displacement &displacementToSub) {
    if(!isValid() || !displacementToSub.isValid())
      setDisplacement(Null, Meters);
    else
      setDisplacement(meters() - displacementToSub.meters(), Meters);
  }


  /**
   * Subtract the given distance from ourself and assign.
   *
   * @param distanceToSub This is the distance we are to subtract
   */
  void Displacement::operator -=(const Distance &distanceToSub) {
    if(!isValid() || !distanceToSub.isValid())
      setDisplacement(Null, Meters);
    else
      setDisplacement(meters() - distanceToSub.meters(), Meters);
  }


  /**
   * Divide this displacement by a value and assign the result to ourself.
   *
   * @param valueToDiv This is the value we are going to divide by
   */
  void Displacement::operator /=(const double &valueToDiv) {
    if(!isValid() || IsSpecial(valueToDiv))
      setDisplacement(Null, Meters);
    else
      setDisplacement(meters() / valueToDiv, Meters);
  }


  /**
   * Multiply this displacement by a value and assign the result to ourself.
   *
   * @param valueToMult This is the value we are going to multiply by
   */
  void Displacement::operator *=(const double &valueToMult) {
    if(!isValid() || IsSpecial(valueToMult))
      setDisplacement(Null, Meters);
    else
      setDisplacement(meters() * valueToMult, Meters);
  }


  /**
   * This is a helper method to access displacements in a universal manner with
   *   uniform error checking.
   *
   * @param displacementUnit Unit of the return value. If this is invalid, an
   *     exception will be thrown.
   * @return The displacement in units of displacementUnit
   */
  double Displacement::displacement(Units displacementUnit) const {
    double displacementInMeters = m_displacementInMeters;
    double resultingDisplacement = Null;

    if(m_displacementInMeters == Null) return Null;

    switch(displacementUnit) {
      case Meters:
        resultingDisplacement = displacementInMeters;
        break;

      case Kilometers:
        resultingDisplacement = displacementInMeters / 1000.0;
        break;

      case Pixels:
        IString msg = "Cannot call displacement with pixels, ask for another "
            "unit";
        throw IException(IException::Programmer, msg, _FILEINFO_);
        break;
    }

    if(resultingDisplacement == Null) {
      IString msg = "Displacement does not understand the enumerated value [" +
        IString(displacementUnit) + "] as a unit";
      throw IException(IException::Programmer, msg, _FILEINFO_);
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
  void Displacement::setDisplacement(const double &displacement, Units displacementUnit) {
    double displacementInMeters = Null;

    if(IsSpecial(displacement)) {
      m_displacementInMeters = Null;
      return;
    }

    switch(displacementUnit) {
      case Meters:
        displacementInMeters = displacement;
        break;

      case Kilometers:
        displacementInMeters = displacement * 1000.0;
        break;

      case Pixels:
        IString msg = "Cannot setDisplacement with pixels, must convert to "
            "another unit first";
        throw IException(IException::Programmer, msg, _FILEINFO_);
        break;
    }

    if(displacementInMeters == Null) {
      IString msg = "Displacement does not understand the enumerated value [" +
        IString(displacementUnit) + "] as a unit";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_displacementInMeters = displacementInMeters;
  }
}
