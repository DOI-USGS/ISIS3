#ifndef JunoCamera_h
#define JunoCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

#include <QString>

namespace Isis {
  /**
   * @brief Juno's JNC (JunoCam) camera model
   *
   * This is the camera model for the JunoCam instrument. This
   * instrument is technically a pushframe instrument, but it is treated as a
   * framing instrument. This is
   * also a more flexible camera model since it will make controlling the
   * individual framelets alot easier.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Juno
   * @author 2017-07-22 Jeannie Backer
   *
   * @internal
   *   @history 2017-07-22 Jeannie Backer - Original version.
   *   @history 2018-04-25 Jesse Mapel - Modified frame start calculation to not
   *                           use exposure duration. Fixes #5236.
   */
  class JunoCamera : public FramingCamera {
    public:
      JunoCamera(Cube &cube);
      ~JunoCamera();

      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);
      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkTargetId() const;
      virtual int SpkReferenceId() const;

  };
};
#endif
