#ifndef TgoCassisCamera_h
#define TgoCassisCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

#include <QString>

namespace Isis {
  /**
   * @brief TGO Cassis camera model
   *
   * This is the camera model for the Trace Gas Orbiter CaSSIS instrument. This
   * instrument is technically a pushframe instrument, but it is treated as a
   * framing instrument because the framelet size is 256 lines or more. This is
   * also a more flexible camera model since it will make controlling the
   * individual framelets alot easier.
   *
   * The CaSSIS frame hierarchy is as follows:
   * <pre>
   *         J2000
   *           |
   *           | ck
   *           |
   *           V
   *     TGO_SPACECRAFT
   *           |
   *           | fixed
   *           |
   *           V
   *     TGO_CASSIS_CRU
   *           |
   *           | ck
   *           |
   *           V
   *     TGO_CASSIS_TEL
   *           |
   *           | fixed
   *           |
   *           V
   *     TGO_CASSIS_FSA
   * </pre>
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Tgo
   * @author 2017-01-26 Kris Becker
   *
   * @internal
   *   @history 2017-01-26 Kris Becker - Original implementation. Fixes #4593.
   *   @history 2017-02-06 Jesse Mapel & Kristin Berry - Updated ck frame and
   *                           documentation. Added a unitTest. References #4593.
   *   @history 2017-09-15 Jesse Mapel - Removed setting the detector start line
   *                           because it is now being handled by the alpha
   *                           cube group. Fixes #5156.
   *   @history 2018-01-11 Christopher Combs - Added try/catch around creation of
   *                           cameras distortion map to prevent segfault when
   *                           destructing. Fixes #5163.
   */
  class TgoCassisCamera : public FramingCamera {
    public:
      TgoCassisCamera(Cube &cube);
      ~TgoCassisCamera();

      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);
      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkTargetId() const;
      virtual int SpkReferenceId() const;

  };
};
#endif
