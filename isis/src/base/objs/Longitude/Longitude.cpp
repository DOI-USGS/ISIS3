/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Longitude.h"

#include <cmath>

#include <QtCore>
#include <QDebug>

#include "Constants.h"
#include "IException.h"
#include "IString.h"
#include "PvlGroup.h"
#include "SpecialPixel.h"

namespace Isis {
  /**
   * Create a blank Longitude object with 0-360 domain.
   */
  Longitude::Longitude() : Angle() {
    m_currentDomain = Domain360;
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
      m_currentDomain = Domain360;
    }
    else if(mapping["LongitudeDomain"][0] == "180") {
      m_currentDomain = Domain180;
    }
    else {
      IString msg = "Longitude domain [" +
          IString(mapping["LongitudeDomain"][0]) + "] not recognized";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if(mapping["LongitudeDirection"][0] == "PositiveEast") {
      setPositiveEast(longitude, longitudeUnits);
    }
    else if(mapping["LongitudeDirection"][0] == "PositiveWest") {
      setPositiveWest(longitude, longitudeUnits);
    }
    else {
      IString msg = "Longitude direction [" +
          IString(mapping["LongitudeDirection"][0]) + "] not recognized";
      throw IException(IException::Programmer, msg, _FILEINFO_);
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
    m_currentDomain = lonDomain;

    if(lonDir == PositiveEast) {
      setPositiveEast(longitude.radians(), Radians);
    }
    else if(lonDir == PositiveWest) {
      setPositiveWest(longitude.radians(), Radians);
    }
    else {
      IString msg = "Longitude direction [" + IString(lonDir) + "] not valid";
      throw IException(IException::Programmer, msg, _FILEINFO_);
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
    m_currentDomain = lonDomain;

    if(lonDir == PositiveEast) {
      setPositiveEast(longitude, longitudeUnits);
    }
    else if(lonDir == PositiveWest) {
      setPositiveWest(longitude, longitudeUnits);
    }
    else {
      IString msg = "Longitude direction [" + IString(lonDir) + "] not valid";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * This copies the given longitude exactly.
   *
   * @param longitudeToCopy The latitude we're duplicating
   */
  Longitude::Longitude(const Longitude &longitudeToCopy)
      : Angle(longitudeToCopy) {
    m_currentDomain = longitudeToCopy.m_currentDomain;
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
  double Longitude::positiveEast(Angle::Units units) const {
    return angle(units);
  }


  /**
   * Get the longitude in the PositiveWest coordinate system.
   *
   * @param units The angular units to get the longitude in
   * @see Direction
   * @return The PositiveWest longitude value
   */
  double Longitude::positiveWest(Angle::Units units) const {
    double longitude = angle(units);

    if (!IsSpecial(longitude)) {
      if(m_currentDomain == Domain360) {
        double wrapPoint = unitWrapValue(units);
        double halfWrap = wrapPoint / 2.0;

        int numPlanetWraps = qFloor(longitude / wrapPoint);

        // If the input is 360, then we want numPlanetWraps == 0. Compare the input to the border
        //   case (wraps * wrapPoint == longitude) and bring the number of wraps down if it's true.
        //   If it's false, then the rounding already took care of this border case.
        if (numPlanetWraps != 0 && qFuzzyCompare(numPlanetWraps * wrapPoint, longitude)) {
          if (numPlanetWraps > 0)
            numPlanetWraps--;
          else
            numPlanetWraps++;
        }

        longitude -= numPlanetWraps * wrapPoint;
        longitude = -(longitude - halfWrap) + halfWrap;
        longitude -= numPlanetWraps * wrapPoint;
      }
      else {
        // 180 domain is just a negation in this conversion,
        //   no more work needs done
        longitude = -1 * longitude;
      }
    }

    return longitude;
  }


  /**
   * Set the longitude given a value in the PositiveEast longitude system
   *
   * @param longitude The positive east longitude to set ourselves to
   * @param units The angular units longitude is in
   */
  void Longitude::setPositiveEast(double longitude, Angle::Units units) {
    setAngle(longitude, units);
  }


  /**
   * Set the longitude given a value in the PositiveWest longitude system
   *
   * @param longitude The positive west longitude to set ourselves to
   * @param units The angular units longitude is in
   */
  void Longitude::setPositiveWest(double longitude, Angle::Units units) {
    if(!IsSpecial(longitude)) {
      if(m_currentDomain == Domain360) {
        // Same as positiveWest
        double wrapPoint = unitWrapValue(units);
        double halfWrap = wrapPoint / 2.0;

        int numPlanetWraps = qFloor(longitude / wrapPoint);

        // If the input is 360, then we want numPlanetWraps == 0. Compare the input to the border
        //   case (wraps * wrapPoint == longitude) and bring the number of wraps down if it's true.
        //   If it's false, then the rounding already took care of this border case.
        if (numPlanetWraps != 0 && qFuzzyCompare(numPlanetWraps * wrapPoint, longitude)) {
          if (numPlanetWraps > 0)
            numPlanetWraps--;
          else
            numPlanetWraps++;
        }


        longitude -= numPlanetWraps * wrapPoint;
        longitude = -(longitude - halfWrap) + halfWrap;
        longitude -= numPlanetWraps * wrapPoint;
      }
      else {
        // 180 domain is just a negation in this conversion,
        //   no more work needs done
        longitude = -1 * longitude;
      }
    }

    setAngle(longitude, units);
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

    m_currentDomain = longitudeToCopy.m_currentDomain;
    setPositiveEast(longitudeToCopy.positiveEast());

    return *this;
  }


  /**
   * This returns a longitude that is constricted to 0-360 degrees.
   *
   * @return Longitude with an angular value between 0 and 360 inclusive
   */
  Longitude Longitude::force360Domain() const {
    if(!isValid()) return Longitude();

    double resultantLongitude = angle(Angle::Degrees);

    // Bring the number in the 0 to 360 range
    if (qFuzzyCompare(degrees(), 360.0)) {
      resultantLongitude = 360.0;
    }
    else {
      resultantLongitude -= 360 * qFloor(resultantLongitude / 360);
    }

    return Longitude(resultantLongitude, Angle::Degrees);
  }


  /**
   * This returns a longitude that is constricted to -180 to 180 degrees.
   *
   * @return Longitude with an angular value between -180 and 180 inclusive
   */
  Longitude Longitude::force180Domain() const {
    if(!isValid()) return Longitude();

    Longitude forced = force360Domain();

    if(forced.degrees() > 180.0)
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
  bool Longitude::inRange(Longitude min, Longitude max) const {
    bool result = false;
    
    QList< QPair<Longitude, Longitude> > ranges = to360Range(min, max);

    Longitude thisLon = force360Domain();
    
    QPair<Longitude, Longitude> range;
    foreach (range, ranges) {
      if (thisLon >= range.first && thisLon <= range.second) {
        result = true;
      }

      double thisLonRadians = thisLon.radians();
      double rangeStartRadians = range.first.radians();
      double rangeEndRadians = range.second.radians();
      // Check equality on edges of range
      if (qFuzzyCompare(thisLonRadians, rangeStartRadians) ||
          qFuzzyCompare(thisLonRadians, rangeEndRadians)) {
        result = true;
      }

      // Be very careful at the 0-360 boundary
      if ((qFuzzyCompare(thisLonRadians, 0.0) || qFuzzyCompare(thisLonRadians, 2.0 * PI)) &&
          (qFuzzyCompare(rangeStartRadians, 0.0) ||
           qFuzzyCompare(rangeEndRadians, 2.0 * PI))) {
        result = true;
      }
    }

    return result;
  }

  /**
   * Calculates where the longitude range is in 0-360. This method will return 2 subranges if the
   *   total range intersects the 0/360 line. For instance, if the input range is -10-10, the output
   *   ranges will be 350-360 and 0-10. If the longitude range is invalid then an empty list will
   *   result.
   *
   * @param startLon the western edge of the longitude range
   * @param endLon the eastern edge of the range
   *
   * @return a list of longitude pairs (first is the start and second is the end) that represent the
   *         calculated range(s).
   */
  QList< QPair<Longitude, Longitude> > Longitude::to360Range(Longitude startLon, Longitude endLon) {

    QList< QPair<Longitude, Longitude> > range;
    
    if (startLon.isValid() && endLon.isValid() && startLon < endLon) {
        
      int multiplier = floor(startLon / fullRotation());

      startLon -= multiplier * fullRotation();
      endLon -= multiplier * fullRotation();

      if (endLon > fullRotation()) {

        Longitude startLon2(0, Angle::Degrees);
        Longitude endLon2(endLon - fullRotation());

        if (endLon2 < startLon) {
          range.append(qMakePair(startLon2, endLon2));
        }
        else {
          startLon = Longitude(0, Angle::Degrees);
        }
        endLon = Longitude(360, Angle::Degrees);
      }
      startLon = Longitude((Angle)startLon);
      endLon = Longitude((Angle)endLon);
      
      range.append(qMakePair(startLon, endLon));
    }
    return range;
  }
}
