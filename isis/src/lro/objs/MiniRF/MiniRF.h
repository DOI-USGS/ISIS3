#ifndef MiniRF_h
#define MiniRF_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "RadarCamera.h"

namespace Isis {
  /**
   * @brief LRO Mini-RF SAR and Chandrayaan 1 Mini-RF SAR
   *
   * This is the camera model for both LRO Mini-RF SAR and
   * Chandrayaan 1 Mini-RF SAR radar systems.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup LunarReconnaissanceOrbiter
   *
   * @author 2008-07-01 Jeff Anderson
   *
   * @internal
   *   @history 2009-07-01 Janet Barrett - Updated with code to handle
   *                           weighting of slant range and Doppler shift; fixed so that
   *                           the frequency and wavelength of the instrument are made
   *                           available to Radar classes.
   *   @history 2009-07-31 Debbie A. Cook and Jeannie Walldren - Added
   *                           new tolerance argument to LoadCache call to be compatible
   *                           with update to Spice class
   *   @history 2009-08-05 Debbie A. Cook - corrected altitude in tolerance
   *                           calculation
   *   @history 2009-10-16 Debbie A. Cook - fixed the rotation of the velocity vector
   *   @history 2009-11-03 Debbie A. Cook - added RadarGroundMap method calls to set radar
   *                           parameters
   *   @history 2010-03-19 Debbie A. Cook - removed unit change of rangeResolution so range_sigma
   *                           works on meters instead of km
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *                           methods. Updated documentation. Removed Lro namespace wrap
   *                           inside Isis namespace wrap. Added Isis Disclaimer to files.
   *                           Added NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   *   @history 2016-08-01 Kristin Berry - Added to unitTest to test camera model performance on
   *                           level 2 cubes and to test RA/DEC values. References #2400.
   */
  class MiniRF : public RadarCamera {
    public:
      MiniRF(Cube &cube);

      //! Destroys the MiniRF object
      ~MiniRF() {};

      virtual int CkFrameId() const;

      virtual int CkReferenceId() const;

      /**
       *  SPK Target Body ID - Lunar Reconnaissance Orbiter spacecraft
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Target ID
       */
      virtual int SpkTargetId() const { return (-85); }

      /**
       *  SPK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }
  };
};
#endif
