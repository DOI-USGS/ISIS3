#ifndef Time_h
#define Time_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

#include "FileName.h"

class QString;

namespace Isis {

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
   *  @history 2015-07-21 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF errors
   *           were signaled. References #2248.
   *  @history 2018-03-15 Adam Goins - Removed deprecated function iTime::UnloadLeapSecondKernel().
   *                          Fixes #5325.
   *  @history 2019-06-15 Kristin Berry - Added a precision option to UTC to specify the precision
   *           of the output value. The default (old) precision is used if no argument is specified.
   *  @history 2021-02-17 Jesse Mapel - Added the ability to pass ISO 8601 basic format time
   *                          strings to setUtc.
   */
  class iTime {
    public:
      // constructor
      iTime();
      iTime(const QString &time);

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

      void operator=(const QString &time);
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

      iTime operator -(const double &secondsToSubtract) const;
      double operator -(const iTime &iTimeToSubtract) const;
      void operator -=(const double &secondsToSubtract);
      friend iTime operator -(const double &secondsToSubtract, iTime time);

      // Return the year
      QString YearString() const;
      int Year() const;
      QString MonthString() const;
      int Month() const;
      QString DayString() const;
      int Day() const;
      QString HourString() const;
      int Hour() const;
      QString MinuteString() const;
      int Minute() const;
      QString SecondString(int precision=8) const;
      double Second() const;
      QString DayOfYearString() const;
      int DayOfYear() const;
      QString EtString() const;

      /**
       * Returns the ephemeris time (TDB) representation of the time as a double
       */
      double Et() const {
        return p_et;
      }

      QString UTC(int precision=8) const;
      static QString CurrentGMT();
      static QString CurrentLocalTime();

      void setEt(double et);
      void setUtc(QString utcString);

    private:
      double p_et;     /**<The ephemeris representaion of the original string
                           passed into the constructor or the operator= member*/

      void LoadLeapSecondKernel();

      static bool p_lpInitialized;
  };
};

#endif
