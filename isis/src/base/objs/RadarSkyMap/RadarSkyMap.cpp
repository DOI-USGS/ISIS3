/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
