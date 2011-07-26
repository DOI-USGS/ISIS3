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

#include "Longitude.h"

#include <cmath>

#include "Constants.h"
#include "iException.h"
#include "iString.h"
#include "PvlGroup.h"
#include "SpecialPixel.h"

namespace Isis {
  /**
   * Create a blank Longitude object with 0-360 domain.
   */
  Longitude::Longitude() : Angle() {
    p_currentDomain = Domain360;
  }


  /**
   * Create and initialize a Longitude value. This value can wrap the planet
   *   any number of times regardless of the domain. The longitude domain and
   *   direction are read from the mapping group.
   *
   * @param longitude The longitude value this instance will represent
   * @param mapping The mapping group containing the longitude direction and
   *   domain
   * @param longitudeUnits The angular units of the longitude value (degs, rads)
   */
  Longitude::Longitude(double longitude, PvlGroup mapping,
                       Angle::Units longitudeUnits) :
      Angle(longitude, longitudeUnits) {
    if(mapping["LongitudeDomain"][0] == "360") {
      p_currentDomain = Domain360;
    }
    else if(mapping["LongitudeDomain"][0] == "180") {
      p_currentDomain = Domain180;
    }
    else {
      iString msg = "Longitude domain [" +
          iString(mapping["LongitudeDomain"][0]) + "] not recognized";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if(mapping["LongitudeDirection"][0] == "PositiveEast") {
      SetPositiveEast(longitude, longitudeUnits);
    }
    else if(mapping["LongitudeDirection"][0] == "PositiveWest") {
      SetPositiveWest(longitude, longitudeUnits);
    }
    else {
      iString msg = "Longitude direction [" +
          iString(mapping["LongitudeDirection"][0]) + "] not recognized";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Create and initialize a Longitude value. This value can wrap the planet
   *   any number of times regardless of the domain.
   *
   * @param longitude The longitude value this instance will represent
   * @param lonDir True if moving east means an increase in longitude
   * @param lonDomain Domain of the given longitude value
   */
  Longitude::Longitude(Angle longitude,
                       Direction lonDir, Domain lonDomain) :
      Angle(longitude) {
    p_currentDomain = lonDomain;

    if(lonDir == PositiveEast) {
      SetPositiveEast(longitude.GetRadians(), Radians);
    }
    else if(lonDir == PositiveWest) {
      SetPositiveWest(longitude.GetRadians(), Radians);
    }
    else {
      iString msg = "Longitude direction [" + iString(lonDir) + "] not valid";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Create and initialize a Longitude value. This value can wrap the planet
   *   any number of times regardless of the domain.
   *
   * @param longitude The longitude value this instance will represent
   * @param longitudeUnits The angular units of the longitude value (degs, rads)
   * @param lonDir True if moving east means an increase in longitude
   * @param lonDomain Domain of the given longitude value
   */
  Longitude::Longitude(double longitude, Angle::Units longitudeUnits,
                       Direction lonDir, Domain lonDomain) :
      Angle(longitude, longitudeUnits) {
    p_currentDomain = lonDomain;

    if(lonDir == PositiveEast) {
      SetPositiveEast(longitude, longitudeUnits);
    }
    else if(lonDir == PositiveWest) {
      SetPositiveWest(longitude, longitudeUnits);
    }
    else {
      iString msg = "Longitude direction [" + iString(lonDir) + "] not valid";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * This copies the given longitude exactly.
   *
   * @param longitudeToCopy The latitude we're duplicating
   */
  Longitude::Longitude(const Longitude &longitudeToCopy)
      : Angle(longitudeToCopy) {
    p_currentDomain = longitudeToCopy.p_currentDomain;
  }


  /**
   * This cleans up the Longitude class.
   */
  Longitude::~Longitude() {
  }


  /**
   * Get the longitude in the PositiveEast coordinate system.
   *
   * @param units The angular units to get the longitude in
   * @see Direction
   * @return The PositiveEast longitude value
   */
  double Longitude::GetPositiveEast(Angle::Units units) const {
    return GetAngle(units);
  }


  /**
   * Get the longitude in the PositiveWest coordinate system.
   *
   * @param units The angular units to get the longitude in
   * @see Direction
   * @return The PositiveWest longitude value
   */
  double Longitude::GetPositiveWest(Angle::Units units) const {
    double longitude = GetAngle(units);

    if(p_currentDomain == Domain360) {
      double wrapPoint = UnitWrapValue(units);
      double halfWrap = wrapPoint / 2.0;

      int numPlanetWraps = (int)(longitude / wrapPoint);

      // being negative needs an extra increment here to get the value into
      //   the positive 360 world
      if (numPlanetWraps < 0) numPlanetWraps --;

      longitude -= numPlanetWraps * wrapPoint;
      longitude = -(longitude - halfWrap) + halfWrap;
      longitude += numPlanetWraps * wrapPoint;
    }
    else {
      // 180 domain is just a negation in this conversion,
      //   no more work needs done
      longitude = -1 * longitude;
    }

    return longitude;
  }


  /**
   * Set the longitude given a value in the PositiveEast longitude system
   *
   * @param longitude The positive east longitude to set ourselves to
   * @param units The angular units longitude is in
   */
  void Longitude::SetPositiveEast(double longitude, Angle::Units units) {
    SetAngle(longitude, units);
  }


  /**
   * Set the longitude given a value in the PositiveWest longitude system
   *
   * @param longitude The positive west longitude to set ourselves to
   * @param units The angular units longitude is in
   */
  void Longitude::SetPositiveWest(double longitude, Angle::Units units) {
    if(!IsSpecial(longitude)) {
      if(p_currentDomain == Domain360) {
        // Same as GetPositiveWest
        double wrapPoint = UnitWrapValue(units);
        double halfWrap = wrapPoint / 2.0;

        int numPlanetWraps = (int)(longitude / wrapPoint);

        // being negative needs an extra increment here to get the value into
        //   the positive 360 world
        if (numPlanetWraps < 0) numPlanetWraps --;

        longitude -= numPlanetWraps * wrapPoint;
        longitude = -(longitude - halfWrap) + halfWrap;
        longitude += numPlanetWraps * wrapPoint;
      }
      else {
        // 180 domain is just a negation in this conversion,
        //   no more work needs done
        longitude = -1 * longitude;
      }
    }

    SetAngle(longitude, units);
  }


  /**
   * Assign the values in the given longitude to ourselves. This will make an
   *   exact duplicate.
   *
   * @param longitudeToCopy The longitude we're duplicating with the current
   *     instance
   */
  Longitude& Longitude::operator=(const Longitude & longitudeToCopy) {
    if(this == &longitudeToCopy) return *this;

    p_currentDomain = longitudeToCopy.p_currentDomain;
    SetPositiveEast(longitudeToCopy.GetPositiveEast());

    return *this;
  }


  /**
   * This returns a longitude that is constricted to 0-360 degrees.
   *
   * @return Longitude with an angular value between 0 and 360 inclusive
   */
  Longitude Longitude::Force360Domain() const {
    if(!Valid()) return Longitude();

    double resultantLongitude = GetAngle(Angle::Degrees);

    // Bring the number in the 0 to 360 range
    resultantLongitude -= 360 * floor(resultantLongitude / 360);

    return Longitude(resultantLongitude, Angle::Degrees);
  }


  /**
   * This returns a longitude that is constricted to -180 to 180 degrees.
   *
   * @return Longitude with an angular value between -180 and 180 inclusive
   */
  Longitude Longitude::Force180Domain() const {
    if(!Valid()) return Longitude();

    Longitude forced = Force360Domain();

    if(forced.GetDegrees() > 180.0)
      forced -= Angle(360.0, Angle::Degrees);

    return forced;
  }


  /**
   * Checks if this longitude value is within the given range.  Defines the
   * range as the change from the minimum longitude to the maximum longitude (an
   * angle), and returns whether the change from the minimum longitude to this
   * longitude is less than or equal to the maximum change allowed (the range).
   *
   * All longitude values are restricted to a 0-360 range for the sake of
   * comparison.  If the provided min and max longitude values are nominally
   * different, but resolve to the same value when clamped to the 0-360 range,
   * (for example: min=0 and max=360 => adjustedMin=0 and adjustedMax=0), then
   * every point will be considered valid (because the whole planet is the
   * range).
   *
   * @param min The beginning of the valid longitude range
   * @param max The end of the valid longitude range
   *
   * @return Whether the longitude is in the given range
   */
  bool Longitude::IsInRange(Longitude min, Longitude max) const {
    // Get around the "wrapping around the planet" problem by clamping the
    // longitudes to a strict 0-360 range.  Now we're simply dealing with angles
    // on a circle from an origin.
    Longitude adjustedMin = min.Force360Domain();
    Longitude adjustedMax = max.Force360Domain();

    bool inRange = false;
    if (adjustedMin == adjustedMax && min != max) {
      // The beginning and end of the range is the same point, but the
      // originally provided begin and end were nominally different, implying
      // that the entire 0-360 range is valid.  For example, if the user entered
      // 0-0 for the range, we assume they mean that only longitude 0 is valid.
      // If, however, 0-360 was entered, then forcing the 360 results in 0-0,
      // but we assume that they intended to cover the entire planet, because
      // 0-360 is inherently an exclusive range in concept, but not in practice.
      inRange = true;
    }
    else {
      // Define the angle 0 degrees as the origin
      Angle origin = Angle(0.0, Angle::Degrees);

      // Define the angle 360 degrees as the maximal, "boundary" angle
      Angle boundary = Angle(360.0, Angle::Degrees);

      // Find the change in angle from the min to the max
      Longitude deltaMax = adjustedMax - adjustedMin;

      // Add back the boundary value if the change is less than 0 (the range
      // crossed over the origin, so offset it to account for this)
      if (deltaMax < origin)
        deltaMax += boundary;

      // Add a small amount of wriggle room to the maximum change to account for
      // precision error
      deltaMax += Angle(DBL_EPSILON, Angle::Degrees);

      // Clamp this longitude to the same 0-360 domain
      Longitude adjusted = Force360Domain();

      // Apply the same adjustment for the change from the min longitude to this
      // longitude as was applied to the maximal change
      Longitude delta = adjusted - adjustedMin;
      if (delta < origin)
        delta += boundary;

      // If the change in angle is less than or equal to the maximal change, it
      // is within the valid range
      inRange = delta <= deltaMax;
    }

    return inRange;
  }
}
