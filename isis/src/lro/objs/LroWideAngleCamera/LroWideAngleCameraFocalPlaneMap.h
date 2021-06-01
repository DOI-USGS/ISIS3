#ifndef LroWideAngleCameraFocalPlaneMap_h
#define LroWideAngleCameraFocalPlaneMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QVector>
#include "CameraFocalPlaneMap.h"

namespace Isis {
  /**
   *  Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup LunarReconnaissanceOrbiter
   *
   * @see LroWideAngleCamera
   *
   * @author 2013-03-07 Kris Becker
   *
   * @internal
   *   @history 2013-03-07 Kris Becker - Implements new focal plane map allowing
   *            for band independent distortions.
   */
  class LroWideAngleCameraFocalPlaneMap : public CameraFocalPlaneMap {
    public:
      LroWideAngleCameraFocalPlaneMap(Camera *parent, int naifIkCode);

      //! Destroys the LroWideAngleCameraFocalPlaneMap object
      virtual ~LroWideAngleCameraFocalPlaneMap() { }

      void addFilter(int naifIkCode);
      void setBand(int vband);

    private:
      struct TranslationParameters {
        double m_transx[3];
        double m_transy[3];
        double m_itranss[3];
        double m_itransl[3];
      };
      QVector<TranslationParameters> m_transparms;
  };
}
#endif
