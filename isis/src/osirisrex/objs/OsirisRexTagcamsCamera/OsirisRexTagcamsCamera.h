#ifndef OsirisRexTagcamsCamera_h
#define OsirisRexTagcamsCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

#include <QString>

namespace Isis {
  /**
   * This class models the behavior and attributes of the 
   * OSIRIS-REx Navigation cameras: NavCam, NFTCam, and StowCam 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Osiris Rex
   *  
   * @author  2018-03-09 Kris Becker
   *
   * @internal
   *   @history 2018-03-09 Kris Becker UA Original Version
   *   @history 2019-01-24 Kris Becker - Removed unused variables to suppress
   *                         warnings
   *
   */
  class OsirisRexTagcamsCamera : public FramingCamera {
    public:
      OsirisRexTagcamsCamera(Cube &cube);
      ~OsirisRexTagcamsCamera();

      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time, 
                                                             double exposureDuration);

      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;

  };
};
#endif
