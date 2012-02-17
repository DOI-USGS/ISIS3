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
  class Displacement;

  /**
   * @brief Distance measurement, usually in meters
   *
   * This class is designed to encapsulate the concept of a distance. This is
   *   typically used for Radius and XYZ values but is also available as a
   *   general purpose class. This class does not accept negative values.
   *
   * @ingroup Utility
   *
   * @author 2010-10-12 Steven Lambright
   *
   * @internal
   *   @history 2011-04-18 Steven Lambright Improved special pixel math handling
   *   @history 2012-02-16 Steven Lambright - Brought up to method and member
   *                           naming standards.
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
        Kilometers,
        //! The distance is being specified in pixels
        Pixels
      };

      Distance();
      Distance(double distance, Units distanceUnit);
      Distance(double distanceInPixels, double pixelsPerMeter);
      Distance(const Distance &distanceToCopy);
      virtual ~Distance();

      double meters() const;
      void setMeters(double distanceInMeters);

      double kilometers() const;
      void setKilometers(double distanceInKilometers);

      double pixels(double pixelsPerMeter = 1.0) const;
      void setPixels(double distanceInPixels, double pixelsPerMeter = 1.0);

      bool isValid() const;

      bool operator >(const Distance &otherDistance) const;
      bool operator <(const Distance &otherDistance) const;

      /**
       * Compare two distances with the greater than or equal to operator.
       *
       * @param otherDistance This is the distance we're comparing to, i.e. on
       *     the right hand side of the operator when used
       * @return True if this distance is greater than or equal to the given
       *     distance
       */
      bool operator >=(const Distance &otherDistance) const {
        return *this > otherDistance || *this == otherDistance;
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
        return *this < otherDistance || *this == otherDistance;
      }


      /**
       * Compare two distances with the == operator. Two uninitialized distances
       *   are equal to each other.
       *
       * @param otherDistance This is the distance we're comparing to, i.e. on
       *     the right hand side of the operator when used
       * @return True if this distance is equal to the given distance
       */
      bool operator ==(const Distance &otherDistance) const {
        return meters() == otherDistance.meters();
      }


      /**
       * Compare two distances with the != operator. Two uninitialized distances
       *   are equal to each other.
       *
       * @param otherDistance This is the distance we're comparing to, i.e. on
       *     the right hand side of the operator when used
       * @return True if this distance is not equal to the given distance
       */
      bool operator !=(const Distance &otherDistance) const {
        return meters() != otherDistance.meters();
      }


      Distance &operator =(const Distance &distanceToCopy);
      Distance operator +(const Distance &distanceToAdd) const;
      Displacement operator -(const Distance &distanceToSub) const;
      double operator /(const Distance &distanceToDiv) const;
      Distance operator /(const double &valueToDiv) const;
      Distance operator *(const double &valueToMult) const;
      friend Distance operator *(double mult, Distance dist);
      void operator +=(const Distance &distanceToAdd);
      void operator -=(const Distance &distanceToSub);
      void operator /=(const double &valueToDiv);
      void operator *=(const double &valueToMult);

    protected:
      virtual double distance(Units distanceUnit) const;
      virtual void setDistance(const double &distance, Units distanceUnit);

    private:
      /**
       * This is the distance value that this class is encapsulating, always
       *   stored in meters.
       */
      double m_distanceInMeters;
  };
}

#endif
