#ifndef Time_h
#define Time_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2009/12/28 19:27:12 $
 *
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

#include <string>

#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"

#include "FileName.h"

namespace Isis {
  class IString;

  /**
   * @brief Parse and return pieces of a time string
   *
   * This class parses a date/time string into individual components. The input
   * time string can be in a variety of formats (see the NAIF routine str2et_c).
   * The components and different representation can then be returned using the
   * member functions.
   *
   * @ingroup Parsing
   *
   * @author 2003-10-24 Stuart Sides
   *
   * @internal
   *  @history 2003-11-05 Stuart Sides - Fixed error in documentation
   *  @history 2003-12-03 Stuart Sides - Added comparison operators for (>=, <=,
   *                                     >, <, ==, and !=)
   *  @history 2003-12-09 Stuart Sides - Modified so all iTime objects will share
   *                                     the same leap second kernel. This means
   *                                     the leapsecond kernel will be loaded once
   *                                     and never unloaded. Other objects which
   *                                     use the leapsecond kernel should not
   *                                     unload it either.
   *  @history 2005-02-22 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *  @history 2006-03-31 Elizabeth Miller - added UTC method
   *  @history 2006-10-02 Brendan George - Added CurrentLocalTime and CurrentGMT
   *                                       methods
   *  @history 2007-12-07 Kris Becker - Modifed the SecondString() method to
   *           always return fixed notation for fractions of a second of no more
   *           than 8 digits after the decimal point.  It was returning scientific
   *           notation causing parsing errors (in NAIF, PostgreSQL TIMESTAMP
   *           fields, etc...)
   *  @history 2011-05-25 Janet Barrett and Steven Lambright - Added setUtc,
   *           setEt and addition operators
   */
  class iTime {
    public:
      // constructor
      iTime();
      iTime(const std::string &time);

      /**
      * Constructs a iTime object and initializes it to the time from the argument.
      *
      * @param time An ephemeris time (ET).
      */
      iTime(const double time) {
        if(!p_lpInitialized)
          LoadLeapSecondKernel();

        p_et = time;
      }

      // destructor
      ~iTime() {};

      void operator=(const std::string &time);
      void operator=(const char *time);
      void operator=(const double time);

      bool operator>=(const iTime &time);
      bool operator<=(const iTime &time);
      bool operator>(const iTime &time);
      bool operator<(const iTime &time);
      bool operator!=(const iTime &time);
      bool operator==(const iTime &time);

      iTime operator +(const double &secondsToAdd) const;
      void operator +=(const double &secondsToAdd);
      friend iTime operator +(const double &secondsToAdd, iTime time);

      // Return the year
      std::string YearString() const;
      int Year() const;
      std::string MonthString() const;
      int Month() const;
      std::string DayString() const;
      int Day() const;
      std::string HourString() const;
      int Hour() const;
      std::string MinuteString() const;
      int Minute() const;
      std::string SecondString() const;
      double Second() const;
      std::string DayOfYearString() const;
      int DayOfYear() const;
      std::string EtString() const;

      /**
       * Returns the ephemeris time (TDB) representation of the time as a double
       */
      double Et() const {
        return p_et;
      }

      std::string UTC() const;
      static std::string CurrentGMT();
      static std::string CurrentLocalTime();
      
      void setEt(double et);
      void setUtc(IString utcString);

    private:
      double p_et;     /**<The ephemeris representaion of the original string
                           passed into the constructor or the operator= member*/

      void LoadLeapSecondKernel();
      void UnloadLeapSecondKernel();

      static bool p_lpInitialized;
  };
};

#endif

