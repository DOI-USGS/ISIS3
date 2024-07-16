/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "IrregularBodyCameraGroundMap.h"

#include "SurfacePoint.h"

using namespace std;

namespace Isis {

  /** 
   * Constructor
   *
   * @param parent Pointer to camera to be used for mapping with ground
   */
  IrregularBodyCameraGroundMap::IrregularBodyCameraGroundMap(Camera *parent,
      const bool clip_emission_angles) : CameraGroundMap(parent),
      m_clip_emission(clip_emission_angles) {
  }

  /** 
   * This method computes the undistorted focal plane coordinates for a ground
   * position, using the current Spice settings (time and kernels) without
   * resetting the current point values for lat/lon/radius/x/y. The class value
   * value for m_pB and m_lookJ are set by this method.
   * 
   * This method has been reimplemented from the CameraGroundMap class to never
   * perform the emission angle check. This is because it uses the ellipsoid for
   * the check which is in general not adequate for irregular bodies.
   *
   * @param point Surface point (ground position) 
   * @param cudx [out] Pointer to computed undistored x focal plane coordinate
   * @param cudy [out] Pointer to computed undistored y focal plane coordinate
   *
   * @return @b bool If conversion was successful
   */
  bool IrregularBodyCameraGroundMap::GetXY(const SurfacePoint &point, 
                                           double *cudx, double *cudy) {

      return CameraGroundMap::GetXY(point, cudx, cudy, m_clip_emission);
  }      
}
