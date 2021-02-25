/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "RadarGroundRangeMap.h"

namespace Isis {
  /** Construct mapping between detectors and focal plane x/y
   *
   * @param parent      parent camera that will use this map
   * @param naifIkCode  code of the naif instrument for reading coefficients
   *
   */
  RadarGroundRangeMap::RadarGroundRangeMap(Camera *parent, const int naifIkCode)
    : CameraFocalPlaneMap(parent, naifIkCode) {
  }

  void RadarGroundRangeMap::setTransform(int naifIkCode,
                                         double groundRangeResolution,
                                         int samples, Radar::LookDirection ldir) {
    // Setup map from radar(sample,time) to radar(groundRange,time)
    double transx[3], transy[3];
    double transs[3], transl[3];

    // There is no change for Left and Right look because the RangeCoefficientSet
    // takes the look direction into account
    transx[0] = -1.0 * groundRangeResolution;
    transx[1] = groundRangeResolution;
    transx[2] = 0.0;

    transs[0] = 1.0;
    transs[1] = 1.0 / groundRangeResolution;
    transs[2] = 0.0;

    transy[0] = 0.0;
    transy[1] = 0.0;
    transy[2] = 0.0;

    transl[0] = 0.0;
    transl[1] = 0.0;
    transl[2] = 0.0;

    std::string icode = "INS" + IString(naifIkCode);
    pdpool_c((icode + "_TRANSX").c_str(), 3, transx);
    pdpool_c((icode + "_TRANSY").c_str(), 3, transy);
    pdpool_c((icode + "_ITRANSS").c_str(), 3, transs);
    pdpool_c((icode + "_ITRANSL").c_str(), 3, transl);
  }
}

