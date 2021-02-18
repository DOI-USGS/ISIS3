#ifndef NirsDetectorMap_h
#define NirsDetectorMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CameraDetectorMap.h"

namespace Isis {
  /**
   * @brief The detector map class for the Hayabusa NIRS camera.
   *
   * The detector map class to allow for exposure duration storage and retrieval
   * in the Hayabusa NIRS camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Hayabusa
   *
   * @author 2017-01-04 Jesse Mapel
   *
   * @internal
   *   @history 2017-01-04 Jesse Mapel - Original version. Fixes #4576.
   */
  class NirsDetectorMap : public CameraDetectorMap {
    public:
      NirsDetectorMap(double exposureDuration, Camera *parent);

      ~NirsDetectorMap();

      void setExposureDuration(double exposureDuration);

      virtual double exposureDuration(const double sample,
                                      const double line,
                                      const int band) const;
    protected:
      double m_exposureDuration; //!< The total time for the observation
  };
};
#endif
