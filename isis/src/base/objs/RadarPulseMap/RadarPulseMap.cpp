/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "RadarPulseMap.h"
#include "CameraFocalPlaneMap.h"
#include "iTime.h"

namespace Isis {
  /** Compute alpha position from a detector coordinate
   *
   * This method will compute a alpha sample given a detector
   * coordinate.  The alpha line will be computed using the the
   * time in the parent camera
   *
   * @param sample Sample number in the detector
   * @param line Line number in the detector (ignored)
   *
   * @return conversion successful
   */
  bool RadarPulseMap::SetDetector(const double sample,
                                  const double line) {
    if(!CameraDetectorMap::SetDetector(sample, line)) return false;
    double etDiff = p_camera->time().Et() - p_etStart;
    p_parentLine = etDiff / p_lineRate + 1.0;
    return true;
  }

  /** Compute radar (sample/time)from a alpha image coordinate
   *
   * This method will compute the radar position from the alpha
   * line/sample coordinate.  The alpha line will be used to set
   * the appropriate time in the parent camera.
   *
   * @param sample Sample number in the alpha image
   * @param line Line number in the alpha image
   *
   * @return conversion successful
   */
  bool RadarPulseMap::SetParent(const double sample,
                                const double line) {
    // Apply base class summing/first sample corrections
    if(!CameraDetectorMap::SetParent(sample, line)) return false;

    // line is really a function of time so set detector line to zero
    p_detectorLine = 0;
    double etLine = p_etStart + p_lineRate * (line - 1.0);
    p_camera->setTime(etLine);
    return true;
  }
}
