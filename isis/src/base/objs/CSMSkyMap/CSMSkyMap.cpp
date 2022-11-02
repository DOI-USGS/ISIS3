/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CSMSkyMap.h"
#include "CSMCamera.h"

namespace Isis {
  /** Constructor a map between focal plane x/y and right acension/declination
   *
   * @param parent  parent camera which will use this map
   *
   */
  CSMSkyMap::CSMSkyMap(Camera *parent) {
    p_camera = parent;
    p_camera->SetSkyMap(this);
  }

  /**
   * Compute undistorted focal plane coordinate from ra/dec
   *
   * @param ra The right ascension angle
   * @param dec The declination
   *
   * @return conversion was successful
   * @todo what happens if we are looking behind the focal plane?????
   * @todo what happens if we are looking parallel to the focal plane??
   * @todo can lookC[2] == zero imply parallel
   * @todo can this all be solved by restricting the physical size of
   * the focal plane?
   */
  bool CSMSkyMap::SetSky(const double ra, const double dec) {
    ((CSMCamera*)p_camera)->SetRightAscensionDeclination(ra, dec);
    // double lookC[3];
    // ((CSMCamera*)p_camera)->LookDirection(lookC);
    // double scale = p_camera->FocalLength() / lookC[2];
    // p_focalPlaneX = lookC[0] * scale;
    // p_focalPlaneY = lookC[1] * scale;
    return true;
  }

};