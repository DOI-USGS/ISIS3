/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CameraSkyMap.h"
#include "NaifStatus.h"

namespace Isis {
  /** Constructor a map between focal plane x/y and right acension/declination
   *
   * @param parent  parent camera which will use this map
   *
   */
  CameraSkyMap::CameraSkyMap(Camera *parent) {
    p_camera = parent;
    p_camera->SetSkyMap(this);
  }

  /** Compute ra/dec from focal plane coordinate
   *
   * This method will compute the right ascension and declination given an
   * undistorted focal plane coordinate.  Note that the ra/dec values
   * can be obtained from the parent camera class passed into the constructor.
   *
   * @param ux distorted focal plane x in millimeters
   * @param uy distorted focal plane y in millimeters
   * @param uz distorted focal plane z in millimeters
   *
   * @return conversion was successful
   */
  bool CameraSkyMap::SetFocalPlane(const double ux, const double uy,
                                   double uz) {
    NaifStatus::CheckErrors();

    SpiceDouble lookC[3];
    lookC[0] = ux;
    lookC[1] = uy;
    lookC[2] = uz;

    SpiceDouble unitLookC[3];
    vhat_c(lookC, unitLookC);
    p_camera->SetLookDirection(unitLookC);

    NaifStatus::CheckErrors();

    return true;
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
  bool CameraSkyMap::SetSky(const double ra, const double dec) {
    p_camera->Sensor::SetRightAscensionDeclination(ra, dec);
    double lookC[3];
    p_camera->Sensor::LookDirection(lookC);
    double scale = p_camera->FocalLength() / lookC[2];
    p_focalPlaneX = lookC[0] * scale;
    p_focalPlaneY = lookC[1] * scale;
    return true;
  }
}
