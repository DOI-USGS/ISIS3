/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "NirsDetectorMap.h"

namespace Isis {

  /**
   * Constructs a NirsDetectorMap object.
   *
   * @param exposureDuration The time for the observations.
   * @param parent The parent camera that uses the detector map.
   */
  NirsDetectorMap::NirsDetectorMap(double exposureDuration, Camera *parent = 0):
                                   CameraDetectorMap(parent) {
    m_exposureDuration = exposureDuration;
  }


  /**
   * Destroys a NirsDetectorMap object.
   */
  NirsDetectorMap::~NirsDetectorMap() {

  }


  /**
   * Sets the exposure duration
   *
   * @param exposureDuration The time for the observations.
   */
  void NirsDetectorMap::setExposureDuration(double exposureDuration) {
    m_exposureDuration = exposureDuration;
  }


  /**
   * Returns the exposure duration for a given pixel.
   *
   * @param sample The sample location of the pixel.
   * @param line The line location of the pixel.
   * @param band The band location of the pixel.
   *
   * @return @b double The exposure duration for the pixel in seconds.
   */
  double NirsDetectorMap::exposureDuration(const double sample,
                                           const double line,
                                           const int band) const {
    return m_exposureDuration;
  }
}
