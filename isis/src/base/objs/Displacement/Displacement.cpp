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
   * This is the copy constructor for Displacement. The displacement passed in 
   *   will be exactly duplicated.
   *
   * @param displacementToCopy This is the displacement we are duplicating
   *    of
   */
  Displacement::Displacement(const Displacement &displacementToCopy) {
    // Use meters because it is the stored format, no precision loss
    SetDisplacement(displacementToCopy.GetMeters(), Meters);
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
   * Free the memory allocated by this instance of the displacement class.
   */
  Displacement::~Displacement() {
    // This will help debug memory problems, better to reset to obviously bad
    //   values in case we're used after we're deleted. 
    p_displacementInMeters = Null;
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
   * Assign this displacement to the value of another displacement.
   *
   * @param displacementToCopy This is the displacement we are to duplicate 
   *      exactly
   * @return Resulting displacement, a reference to this displacement after 
   *      assignment
   */
  Displacement &Displacement::operator =(const Displacement &displacementToCopy) {
    if(this == &displacementToCopy) return *this;

    SetDisplacement(displacementToCopy.GetMeters(), Meters);

    return *this;
  }


  /**
   * Add another displacement to this displacement (1km + 1m = 1005m)
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
   * Subtract another displacement from this displacement (1km - 1m = 995m).
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
   * Add and assign the given displacement to ourselves.
   *
   * @param displacementToAdd This is the displacement we are to duplicate 
   *      exactly
   * @return Resulting displacement, a reference to this displacement after 
   *      assignment
   */
  void Displacement::operator +=(const Displacement &displacementToAdd) {
    SetDisplacement(GetMeters() + displacementToAdd.GetMeters(), Meters);
  }


  /**
   * Subtract and assign the given displacement from ourself. This could throw
   *   an exception if the result is negative, in which case the new value is
   *   never applied.
   *
   * @param DisplacementToSub This is the displacement we are to duplicate exactly
   * @return Resulting displacement, a reference to this displacement after assignment
   */
  void Displacement::operator -=(const Displacement &displacementToSub) {
    SetDisplacement(GetMeters() - displacementToSub.GetMeters(), Meters);
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

    if(displacement == Null) {
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
