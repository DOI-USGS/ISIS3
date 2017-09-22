/**
 * @file
 * $Revision: 1.43 $
 * $Date: 2010/05/24 22:57:44 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#ifndef Constants_h
#define Constants_h

#include <string>

namespace Isis {
  /**
   * @brief Sets some basic constants for use in ISIS programming
   *
   * Sets constants used in ISIS applications and objects such as PI and E.
   *
   * @ingroup Utility
   *
   * @author 2003-01-22 Tracie Sucharski
   *
   * @internal
   *   @history 2003-02-11 Stuart Sides - Documented and created an object
   *                                      unitTest.
   *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                                      isis.astrogeology...
   *   @history 2006-05-17 Elizabeth Miller - Removed .xml file and added
   *                                          documentation to .h file
   *   @history 2009-03-16 Tracie Sucharski - Updated for release 3.2.0.
   *   @history 2010-03-18 Tracie Sucharski - Updated to 3.2.1beta.
   *   @history 2010-05-24 Travis Addair - Added DEG2RAD and
   *                                       RAD2DEG
   *   @history 2010-11-29 Steven Lambright - Isis version constant removed
   *   @history 2012-06-22 Orrin Thomas - Added TWOPI and coverted DEG2RAD and RAD2DEG
   *                                      two decimal constants (from division expresions)
   *   @history 2017-09-22 Cole Neubauer - Fixed documentation. References #4708
   */

  const double E = 2.7182818284590452354; //!<The mathematical constant E
  const double PI = 3.14159265358979323846;   //!<The mathematical constant PI
  const double HALFPI = 1.57079632679489661923;   //!<The mathematical constant PI/2
  const double TWOPI = 6.2831853071795864769253;  //!<Two * PI, a complete revolution
  const double DEG2RAD = 0.017453292519943295769237;   //!<Multiplier for converting from degrees to radians
  const double RAD2DEG = 57.29577951308232087679815481;   //!<Multiplier for converting from radians to degrees

#if (defined(__SunOS__) || defined(__x86_64))
  typedef long int BigInt;
#else
  typedef long long int BigInt; //!<Big int
#endif
}

#endif
