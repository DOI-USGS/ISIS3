#ifndef Displacement_h
#define Displacement_h

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
  class Distance;

  /**
   * @brief Displacement is a signed length, usually in meters
   *
   * This class is designed to encapsulate the concept of a displacement.  A 
   *   displacement has a distance and a sense of direction indicated by a sign
   *   (+ or -).  It is typically used for vectors or coordinates, but is also
   *   available as a general purpose class. This class does accept both
   *   positive and negative values.
   *
   * @ingroup Utility
   *
   * @author 2010-10-12 Steven Lambright and Debbie A. Cook
   *
   * @internal
   *   @history 2012-02-16 Steven Lambright - Brought up to method and member
   *                           naming standards.
   */
  class Displacement {
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

      Displacement();
      Displacement(double displacement, Units distanceUnit);
      Displacement(double displacementInPixels, double pixelsPerMeter);
      Displacement(const Distance &distanceToCopy);

      /**
       * Free the memory allocated by this instance of the displacement class.
       */
      ~Displacement() {};

      double meters() const;
      void setMeters(double displacementInMeters);

      double kilometers() const;
      void setKilometers(double displacementInKilometers);

      double pixels(double pixelsPerMeter = 1.0) const;
      void setPixels(double distanceInPixels, double pixelsPerMeter = 1.0);

      bool isValid() const;

      /**
       * Get the displacement in meters. This is equivalent to meters()
       *
       * @return The displacement, as a number, in units of Meters

      operator double() const {
        return meters();
      }*/

      bool operator >(const Displacement &otherDisplacement) const;
      bool operator <(const Displacement &otherDisplacement) const;


      /**
       * Compare the distances of 2 displacements with the >= operator.
       *
       * @param otherDisplacement This is the displacement we are comparing to, 
       *     i.e. it is on the right-hand-side of the operator when used
       * @return True if the length (unsigned) of the displacement 
       *     is greater than or equal to the length of the given displacement
       */
      bool operator >=(const Displacement &otherDisplacement) const {
        return *this > otherDisplacement  ||  *this == otherDisplacement;
      }


      /**
       * Compare the lengths of 2 displacements with the <= operator.
       *
       * @param otherDisplacement This is the displacement we are comparing
       *   i.e. it is on the right-hand-side of the operator when used
       * @return True if the length of this displacement is less than or 
       *   equal to the length of the given displacement.
       */
      bool operator <=(const Displacement &otherDisplacement) const {
        return *this < otherDisplacement  ||  *this == otherDisplacement;
      }

      /**
       * Compare the lengths of 2 displacements with the != operator.
       *   Two uninitialized displacements are equal to each other.
       *
       * @param otherDisplacement This is the displacement we are comparing to,
       *     i.e. it is on the right-hand-side of the operator when used
       * @return True if the length of this displacement is not equal to the
       *     given displacement
       *
       */
      bool operator !=(const Displacement &otherDisplacement) const {
        return !(*this == otherDisplacement);
      }

      /**
       * Compare the lengths of 2 displacements with the == operator. 
       *   Two uninitialized displacements are equal to each other.
       *
       * @param otherDisplacement This is the displacement we are comparing to, 
       *     i.e. it is on the right-hand-side of the operator when used
       * @return True if the length of this displacement is equal to the  
       *     given displacement 
       * 
       */
      bool operator ==(const Displacement &otherDisplacement) const {
        return m_displacementInMeters ==
               otherDisplacement.m_displacementInMeters;
      }

      Displacement operator +(const Displacement &displacementToAdd) const;
      Displacement operator -(const Displacement &displacementToSub) const;
      Displacement operator -(const Distance &distanceToSub) const;
      double operator /(const Displacement &displacementToDiv) const;
      Displacement operator /(const double &valueToDiv) const;
      Displacement operator *(const double &valueToMult) const;
      friend Displacement operator *(double mult, Displacement displacement);
      void operator +=(const Displacement &displacementToAdd);
      void operator -=(const Displacement &displacementToSub);
      void operator -=(const Distance &distanceToSub);
      void operator /=(const double &valueToDiv);
      void operator *=(const double &valueToMult);

    protected:
      double displacement(Units displacementUnit) const;
      void setDisplacement(const double &displacement, Units displacementUnit);

    private:
      /**
       * This is the displacement value that this class is encapsulating, always
       *   stored in meters.
       */
      double m_displacementInMeters;
  };
}

#endif
