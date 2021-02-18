#ifndef Hyb2OncCamera_h
#define Hyb2OncCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

namespace Isis {
/**
   * This is the camera model for the Hayabusa2 ONC camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Hayabusa2
   *
   * @author  2017-07-07 Kristin Berry (and ... ? )
   *
   * @internal
   *   @history 2017-07-07 Kristin Berry - Original version
   *
   */
  class Hyb2OncCamera : public FramingCamera {
    public:
      Hyb2OncCamera(Cube &cube);

      ~Hyb2OncCamera();

      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);

      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;
  };
};
#endif
