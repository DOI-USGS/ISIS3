#ifndef Distance_h
#define Distance_h

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

namespace Isis {

  /**
   * @brief Distance measurement, usually in meters
   *
   * This class is designed to encapsulate the concept of a distance. This is
   *   typically used for Radius and XYZ values but is also available as a
   *   general purpose class. This class does not accept negative values.
   *
   * The empty constructor is purposefully disallowed, I strongly discourage
   *   constructing these with a 'known' bad distance and testing for that
   *   later on.
   *
   * @ingroup Utility
   *
   * @author 2010-10-12 Steven Lambright
   *
   * @internal
   */
  class Distance {
    public:
      /**
       * This is a list of available units to access and store Distances in.
       *   These values can be passed to the constructor to specify which
       *   unit the double you are passing in is in.
       */
      enum Units {
        //! The distance is being specified in meters
        Meters,
        //! The distance is being specified in kilometers
        Kilometers
      };

      Distance(const Distance &distanceToCopy);
      Distance(double distance, Units distanceUnit = Meters);
      ~Distance();

      double GetMeters() const;
      void SetMeters(double distanceInMeters);

      double GetKilometers() const;
      void SetKilometers(double distanceInKilometers);

      /**
       * Get the distance in meters. This is equivalent to GetMeters()
       *
       * @return The distance, as a number, in units of Meters
       */
      operator double() const {
        return GetMeters();
      }

      // The following comparison operators could rely in the double operator
      // to work, but I feel it's clearer what's happening and less work to
      // maintain if I manually write these and put them here.

      /**
       * Compare two distances with the greater than operator.
       *
       * @param otherDistance This is the distance we're comparing to, i.e. on
       *     the right hand side of the operator when used
       * @return True if this distance is greater than the given distance
       */
      bool operator >(const Distance &otherDistance) const {
        return GetMeters() > otherDistance.GetMeters();
      }


      /**
       * Compare two distances with the greater than or equal to operator.
       *
       * @param otherDistance This is the distance we're comparing to, i.e. on
       *     the right hand side of the operator when used
       * @return True if this distance is greater than or equal to the given
       *     distance
       */
      bool operator >=(const Distance &otherDistance) const {
        return GetMeters() >= otherDistance.GetMeters();
      }


      /**
       * Compare two distances with the less than operator.
       *
       * @param otherDistance This is the distance we're comparing to, i.e. on
       *     the right hand side of the operator when used
       * @return True if this distance is less than the given distance
       */
      bool operator <(const Distance &otherDistance) const {
        return GetMeters() < otherDistance.GetMeters();
      }


      /**
       * Compare two distances with the less than or equal to operator.
       *
       * @param otherDistance This is the distance we're comparing to, i.e. on
       *   the right hand side of the operator when used
       * @return True if this distance is less than or equal to the given
       *   distance
       */
      bool operator <=(const Distance &otherDistance) const {
        return GetMeters() <= otherDistance.GetMeters();
      }


      /**
       * Compare two distances with the == operator.
       *
       * @param otherDistance This is the distance we're comparing to, i.e. on
       *     the right hand side of the operator when used
       * @return True if this distance is equal to the given distance
       */
      bool operator ==(const Distance &otherDistance) const {
        return GetMeters() == otherDistance.GetMeters();
      }


      Distance &operator =(Distance &distanceToCopy);

    protected:
      double GetDistance(Units distanceUnit) const;
      void SetDistance(const double &distance, Units distanceUnit);

    private:
      /**
       * This is the distance value that this class is encapsulating, always
       *   stored in meters.
       */
      double p_distanceInMeters;

      //! This is a disallowed constructor.
      Distance();
  };
}

#endif
