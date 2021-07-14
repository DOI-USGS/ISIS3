#ifndef HayabusaNirsCamera_h
#define HayabusaNirsCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

namespace Isis {
  /**
   * This is the camera model for the Hayabusa NIRS camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Hayabusa
   *
   * @author 2016-12-30 Jesse Mapel
   *
   * @internal
   *   @history 2016-12-30 Jesse Mapel - Original version.
   *                           Modified from AmicaCamera. Fixes #4576.
   */
  class HayabusaNirsCamera : public FramingCamera {
    public:
      HayabusaNirsCamera(Cube &cube);
      ~HayabusaNirsCamera();

      /**
       * CK frame ID - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-130000); }

      /**
       * CK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (1); }

      /**
       * SPK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }

      virtual std::pair<iTime, iTime> ShutterOpenCloseTimes(double time,
                                                            double exposureDuration);

      virtual QList<QPointF> PixelIfovOffsets();
  };
};
#endif
