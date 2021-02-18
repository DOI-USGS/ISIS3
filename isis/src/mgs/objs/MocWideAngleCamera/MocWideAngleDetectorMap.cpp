/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "MocWideAngleDetectorMap.h"

using namespace std;
namespace Isis {
  /** Constructor for MocWideAngleDetectorMap class
   *
   * @param sample
   * @param line
   */
  bool MocWideAngleDetectorMap::SetDetector(const double sample,
      const double line) {
    if(!LineScanCameraDetectorMap::SetDetector(sample, line)) return false;

    if((p_moclab->CrosstrackSumming() == 13) ||
        (p_moclab->CrosstrackSumming() == 27)) {
      int detector = (int) sample;
      if(detector < 1) detector = 1;
      if(detector >= p_moclab->Detectors()) detector = p_moclab->Detectors() - 1;
      double samp1 = p_moclab->Sample(detector - 1);
      double samp2 = p_moclab->Sample(detector);
      if(samp1 < 0.0) return false;
      if(samp2 < 0.0) return false;

      double m = (samp2 - samp1);
      p_parentSample = m * (sample - detector) + samp2;
    }

    return true;
  }


  /**
   * @param sample
   * @param line
   *
   * @return @b bool
   */
  bool MocWideAngleDetectorMap::SetParent(const double sample,
                                          const double line) {
    if(!LineScanCameraDetectorMap::SetParent(sample, line)) return false;

    // Handle variable summing if necessary
    if((p_moclab->CrosstrackSumming() == 13) ||
        (p_moclab->CrosstrackSumming() == 27)) {
      int isamp = (int) sample;
      if(isamp < 2) {
        p_detectorSample = p_moclab->StartDetector(1);
        p_detectorSample += sample - 1.0;
      }
      else if(isamp > p_camera->Samples()) {
        p_detectorSample = p_moclab->StartDetector(p_camera->Samples());
        p_detectorSample += sample - p_camera->Samples();
      }
      else {
        int ss = p_moclab->StartDetector(isamp - 1);
        int es = p_moclab->EndDetector(isamp - 1);
        double samp1 = (ss + es) / 2.0;

        ss = p_moclab->StartDetector(isamp);
        es = p_moclab->EndDetector(isamp);
        double samp2 = (ss + es) / 2.0;

        double m = (samp2 - samp1);
        p_detectorSample = m * (sample - isamp) + samp1;
      }
    }
    return true;
  }
}
