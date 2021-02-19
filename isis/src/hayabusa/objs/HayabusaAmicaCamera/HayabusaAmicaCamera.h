#ifndef HayabusaAmicaCamera_h
#define HayabusaAmicaCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

namespace Isis {
  /**
   * This is the camera model for the Hayabusa AMICA camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Hayabusa
   *
   * @author  2013-11-27 Kris Becker
   *
   * @internal
   *   @history 2013-11-27 Kris Becker - Original version
   *   @history 2015-02-26 Kris Becker - Implement starting detector specs; add
   *                           summing and AlphaCube support.
   *   @history 2015-03-11 Kris Becker - Fixed timing error - was using the
   *                           UTC StartTime rather than the
   *                           SpacecraftClockStartCount. References #2180.
   *   @history 2015-08-11 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these added methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   *   @history 2016-09-14 Kelvin Rodriguez - Enforced the order in which BORESIGHT_LINE and
   *                           BORESIGHT_SAMPLE are added to the PVL. Part of porting to
   *                           OSX 10.11
   *   @history 2017-01-03 Jeannie Backer - Renamed from AmicaCamera to HayabusaAmicaCamera.
   *   @history 2017-01-03 Jeannie Backer - Fixed bug in constructor. When setting the detector
   *                           start line, the camera model was not taking into account the image
   *                           flip on ingestion. Added subframe example to test. Fixes #.
   */
  class HayabusaAmicaCamera : public FramingCamera {
    public:
      HayabusaAmicaCamera(Cube &cube);

      ~HayabusaAmicaCamera();

      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);

      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;
  };
};
#endif
