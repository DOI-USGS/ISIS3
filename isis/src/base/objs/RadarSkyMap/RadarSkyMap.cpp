/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2009/07/09 16:41:34 $
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

#include "RadarSkyMap.h"

namespace Isis {
  /** Constructor a map between focal plane x/y and right acension/declination
   *
   * @param parent  parent camera which will use this map
   *
   */
  RadarSkyMap::RadarSkyMap(Camera *parent) : CameraSkyMap(parent) {
  }

  /** Compute ra/dec from slant range
   *
   * Radar can't paint a star will always return false
   *
   * @param ux distorted focal plane x in millimeters
   * @param uy distorted focal plane y in millimeters
   * @param uz distorted focal plane z in millimeters
   *
   * @return conversion was successful
   */
  bool RadarSkyMap::SetFocalPlane(const double ux, const double uy,
                                   double uz) {
    return false;
  }

  /**
   * Compute slant range from ra/dec.
   *
   * Radar can't paint a star will always return false
   *
   * @param ra The right ascension angle
   * @param dec The declination
   *
   */
  bool RadarSkyMap::SetSky(const double ra, const double dec) {
    return false;
  }
}
