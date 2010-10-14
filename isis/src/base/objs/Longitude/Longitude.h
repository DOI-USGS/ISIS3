#ifndef Longitude_h
#define Longitude_h

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

#include "Angle.h"

namespace Isis {

  /**
   * This class is designed to encapsulate the concept of a Longitude. This is
   *   used primary for surface points but is also a general purpose class.
   *   This class accepts values past the longitude domain limits. This adds
   *   the concept of longitude direction to the Angle class.
   *
   * The empty constructor is purposefully disallowed, I strongly discourage
   *   constructing these with a 'known' bad longitude and testing for that
   *   later on.
   *
   * @ingroup Utility
   *
   * @author 2010-10-12 Steven Lambright
   *
   * @internal
   */
  class Longitude : public Angle {
    public:
      /**
       * Possible longitude directions: Is a positive longitude towards east
       *   or towards west?
       */
      enum Direction {
        //! As the longitude increases the actual position is more east
        PositiveEast,
        //! As the longitude increases the actual position is more west
        PositiveWest
      };

      /**
       * Use LongitudeDomain360 if 0-360 is the primary range of the longitude
       *   values with 180 being the 'center'. use LongitudeDomain180 if 0 is
       *   the 'center' longitude. This is used for converting between longitude 
       *   directions, because a 'center' longitude of 0 (-180 to 180) implies
       *   negation is all that needs done to reverse the longitude direction,
       *   whereas if you are in the 360 domain, where 180 is the center, more
       *   needs done.
       */
      enum Domain {
        //! As the longitude increases the actual position is more east
        Domain360,
        //! As the longitude increases the actual position is more west
        Domain180
      };

      Longitude();
      Longitude(double longitude,
                Angle::Units longitudeUnits = Angle::Radians,
                Direction lonDir = PositiveEast,
                Domain lonDomain = Domain360);
      Longitude(const Longitude &longitudeToCopy);
      ~Longitude();

      double GetPositiveEast(Angle::Units units = Angle::Radians) const;
      void SetPositiveEast(double longitude,
                           Angle::Units units = Angle::Radians);
      double GetPositiveWest(Angle::Units units = Angle::Radians) const;
      void SetPositiveWest(double longitude,
                           Angle::Units units = Angle::Radians);

      Longitude Force180Domain() const;
      Longitude Force360Domain() const;

      /**
       * Same as GetPositiveEast.
       *
       * @see GetPositiveEast
       */
      operator double() const {
        return GetPositiveEast();
      }

      Longitude& operator=(const Longitude & longitudeToCopy);

    private:
      /**
       * This is necessary for converting to PositiveWest and back.
       */
      Domain p_currentDomain;
  };
}

#endif
