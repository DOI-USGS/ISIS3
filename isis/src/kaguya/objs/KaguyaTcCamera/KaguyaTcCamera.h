#ifndef KaguyaTcCamera_h
#define KaguyaTcCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCamera.h"

namespace Isis {
  /**
   * This is the camera model for the Kaguya Terrain Cameras TC1 and TC2
   *
   * @internal
   *   @history 2018-10-01 Adam Goins and Jeannie Backer - Original Version
   *
   *   @history 2019-04-26 Stuart Sides and Kristin Berry - Updates to Kaguya TC camera model
   *                        including updating to use LineScanCamera detector and ground maps, adding
   *                        detector offsets for swath modes, setting the focal plane map center to
   *                        the center of the detector, regardless of swath mode, and using the
   *                        spacecraft clock start count, rather than the StartTime for image timing.
   *                        See Git issue #3215 for more information.
   *
   */
  class KaguyaTcCamera : public LineScanCamera {
    public:
      KaguyaTcCamera(Cube &cube);
      ~KaguyaTcCamera();
      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;
  };
};
#endif
