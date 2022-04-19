#ifndef MsiCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

namespace Isis {
  /**
   * @brief NEAR Shoemaker MSI Camera Model
   *
   * This is the camera model for the Near Earth Asteroid Rendezvous
   * - Shoemaker Multi-Spectral Imager.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup NearEarthAsteroidRendezvousShoemaker
   *
   * @see http://nssdc.gsfc.nasa.gov/nmc/masterCatalog.do?sc=1996-008A
   * @see http://pdssbn.astro.umd.edu/data_sb/missions/near/index.shtml
   * @see http://near.jhuapl.edu/instruments/MSI/index.html
   * @see http://near.jhuapl.edu/fact_sheets/MSI.pdf
   *
   * @author  2013-03-27 Jeannie Backer
   *
   * @internal
   *   @history 2013-03-27 Jeannie Backer - Original Version.
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   *   @history 2016-09-14 Kelvin Rodriguez - Enforced the order in which BORESIGHT_LINE and
   *                           BORESIGHT_SAMPLE are added to the PVL. Part of porting to
   *                           OSX 10.11
   *   @history 2019-08-15 Kris Becker - Corrected format of the start/stop SCLK
   *                           times to make it work with NAIF kernel SCLK
   *                           conversions. Switched to using the
   *                           SpacecraftClockStartCount to establish observation
   *                           times. This makes this camera compatible with
   *                           sumspice! Changes are backward compatible even if
   *                           the SCLK keywords are not already properly
   *                           formatted (see msi2isis).
   */
  class MsiCamera : public FramingCamera {
    public:
      MsiCamera(Cube &cube);
      ~MsiCamera();
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);
      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;
  };
};
#endif
