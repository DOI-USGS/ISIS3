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
    double raRad = ra * DEG2RAD;
    double decRad = dec * DEG2RAD;

    // Make the radius bigger, some multiple of the body radius -or- use sensor position at the reference point
    SensorUtilities::GroundPt3D sphericalPt = {decRad, raRad, 10e12};
    SensorUtilities::Vec rectPt = SensorUtilities::sphericalToRect(sphericalPt);

    double f = p_camera->FocalLength();
    double m[3][3];
    double x = p_camera->getParameterValue(3);
    double y = p_camera->getParameterValue(4);
    double z = p_camera->getParameterValue(5);
    double w = p_camera->getParameterValue(6);

    double norm = sqrt(x * x + y * y + z * z + w * w);
    x /= norm;
    y /= norm;
    w /= norm;
    z /= norm;

    m[0][0] = w * w + x * x - y * y - z * z;
    m[0][1] = 2 * (x * y - w * z);
    m[0][2] = 2 * (w * y + x * z);
    m[1][0] = 2 * (x * y + w * z);
    m[1][1] = w * w - x * x + y * y - z * z;
    m[1][2] = 2 * (y * z - w * x);
    m[2][0] = 2 * (x * z - w * y);
    m[2][1] = 2 * (w * x + y * z);
    m[2][2] = w * w - x * x - y * y + z * z;

    // Sensor position
    double undistortedx, undistortedy, denom;
    denom = m[0][2] * xo + m[1][2] * yo + m[2][2] * zo;
    p_focalPlaneX = (f * (m[0][0] * xo + m[1][0] * yo + m[2][0] * zo) / denom);
    p_focalPlaneY = (f * (m[0][1] * xo + m[1][1] * yo + m[2][1] * zo) / denom);
    return true;
  }

};