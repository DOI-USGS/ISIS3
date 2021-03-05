#ifndef Latitude_h
#define Latitude_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Angle.h"

namespace Isis {
  class Distance;
  class PvlGroup;

  /**
   * This class is designed to encapsulate the concept of a Latitude. This is
   *   used primarily for surface points but is also a general purpose class.
   *   This class has error checking for past the poles. This adds
   *   the concept of 90/-90 and planetographic to the Angle class.
   *
   * @ingroup Utility
   *
   * @author 2010-10-13 Steven Lambright
   *
   * @internal
   *   @history 2011-01-25 Steven Lambright - Added a constructor which takes a
   *                           mapping group.
   *   @history 2012-07-26 Kimberly Oyama and Steven Lambright - Added two add methods to handle
   *                           planetographic latitudes. The first takes the angle to add and a
   *                           Pvl mapping group to determine the latitude type and add accordingly.
   *                           The second takes the angle to add, the equatorial and polar radii,
   *                           and the latitude type. References #604.
   *   @history 2013-03-06 Steven Lambright - Added support for getting Radii from TargetName
   *                           keyword. References #1534.
   *   @history 2016-04-22 Jeannie Backer - Added try/catch around calls to
   *                           Target::radiiGroup() in constructors and add(angle, mapGroup).
   *                           Appended message to caught exceptions. References #3892,3896
   *   @history 2016-07-05 Marjorie Hahn - Fixed documentation for 
   *                           Latitude::Latitude(double latitude, PvlGroup mapping, 
   *                           Angle::Units latitudeUnits, ErrorChecking errors) and added in
   *                           documentation for all exceptions thrown. Fixes #3907
   *   @history 2016-09-29 Jeannie Backer - Changed strings in error message to use Angle::toString
   *                           instead of the Isis::toString(double) method.
   *   @history 2017-04-10 Jesse Mapel - Added an accessor and mutator for ErrorChecking member.
   *                           Fixes #4766.
   *   @history 2019-03-11 Kaitlyn Lee - Added comment in SetPlanetographic() about passing in
   *                            special pixels.
   */
  class Latitude : public Angle {
    public:
      /**
       * Some user-configurable error checking parameters. This is meant to be
       *   used as a bit flag.
       *
       *  Example:
       *    Latitude(95, Angle::Degrees, Latitude::AllowPastPole)
       *    This will allow latitudes past 90 and not throw an exception.
       *
       *    Latitude(95, Angle::Degrees,
       *             Latitude::AllowPastPole | Latitude::ThrowAllErrors)
       *    This will allow latitudes past 90 still and not throw an exception.
       *
       *    Latitude(95, Angle::Degrees, Latitude::ThrowAllErrors)
       *    This will throw an exception.
       *
       *    Latitude(95, Angle::Degrees)
       *    This will throw an exception.
       */
      enum ErrorChecking {
        //! Throw an exception if any problems are found.
        ThrowAllErrors = 0,
        //! Don't throw an exception if a latitude beyond -90/90 is found
        AllowPastPole
      };

      /**
       * These are the latitude coordinate systems. The universal system is
       *   Planetocentric and this class is heavily geared towards using them.
       *   If you wish to use Planetographic, planetary radii must be provided
       *   and at the moment latitudes past 90 aren't supported in
       *   Planetographic.
       */
      enum CoordinateType {
        /**
         * This is the universal (and default) latitude coordinate system.
         * Latitudes in this system are the angle from the the equatorial plane
         * (the line at 0 degrees latitude) to the requested latitude.
         */
        Planetocentric,
        /**
         * This is a secondary coordinate system for latitudes.
         *
         * Latitudes in this system are angle between the equatorial plane and
         *   the perpendicular to the surface at the point. Conceptually you
         *   draw the perpendicular to the surface back to the equatorial plane
         *   and the angle created is the planetographic latitude. This doesn't
         *   differ from Planetocentric on a sphere. These latitudes require the
         *   planetary radii to work with them at all, since the radii affect
         *   the latitude values themselves.
         */
        Planetographic
      };

      Latitude();
      Latitude(double latitude,
               Angle::Units latitudeUnits,
               ErrorChecking errors = AllowPastPole);

      Latitude(Angle latitude, ErrorChecking errors = AllowPastPole);

      Latitude(Angle latitude,
               PvlGroup mapping,
               ErrorChecking errors = ThrowAllErrors);

      Latitude(double latitude,
               PvlGroup mapping,
               Angle::Units latitudeUnits,
               ErrorChecking errors = ThrowAllErrors);

      Latitude(double latitude, Distance equatorialRadius, Distance polarRadius,
               CoordinateType latType = Planetocentric,
               Angle::Units latitudeUnits = Angle::Radians,
               ErrorChecking errors = ThrowAllErrors);

      Latitude(const Latitude &latitudeToCopy);

      ~Latitude();

      double planetocentric(Angle::Units units = Angle::Radians) const;
      void setPlanetocentric(double latitude,
                             Angle::Units units = Angle::Radians);

      double planetographic(Angle::Units units = Angle::Radians) const;
      void setPlanetographic(double latitude,
                           Angle::Units units = Angle::Radians);

      ErrorChecking errorChecking() const;
      void setErrorChecking(ErrorChecking errors);

      bool inRange(Latitude min, Latitude max) const;

      Latitude& operator=(const Latitude & latitudeToCopy);
      Latitude add(Angle angleToAdd, PvlGroup mapping);
      Latitude add(Angle angleToAdd, Distance equatorialRadius, Distance polarRadius,
                   CoordinateType latType);

      /**
       * Same as planetocentric.
       *
       * @see planetocentric

      operator double() const {
        return planetocentric();
      }*/

    protected:
      virtual void setAngle(const double &angle, const Angle::Units &units);

    private:
      /**
       * Used for converting to Planetographic, this is the radius of the target
       *   on the equatorial plane.
       */
      Distance *m_equatorialRadius;
      /**
       * Used for converting to Planetographic, this is the radius of the target
       *   perpendicular to the equatorial plane.
       */
      Distance *m_polarRadius;

      //! This contains which exceptions should not be thrown
      ErrorChecking m_errors;
  };
}

#endif
