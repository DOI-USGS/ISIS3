#ifndef VoyagerCamera_h
#define VoyagerCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "FramingCamera.h"

namespace Isis {
  /**
   * @brief Voyager Camera Model
   *
   * This is the camera model for Voyager 1 and 2 wide and narrow
   * angle cameras.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Voyager
   *
   *
   * @see
   *      http://pds-imaging.jpl.nasa.gov/data/vg2-n-iss-2-edr-v1.0/vg_0009/document/volinfo.txt
   * @see http://voyager.jpl.nasa.gov
   * @see http://pds-imaging.jpl.nasa.gov/portal/voyager_mission.html
   * @see http://astrogeology.usgs.gov/Missions/Voyager
   *
   * @author 2010-07-19 Mackenzie Boyd
   *
   * @internal
   *   @history 2010-07-19 Mackenzie Boyd - Original Version
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                           pure virtual in Camera, implemented in mission
   *                           specific cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2011-05-03 Jeannie Walldren - Added ShutterOpenCloseTimes()
   *                           method. Updated unitTest to test for new methods.
   *                           Updated documentation. Added Isis Disclaimer to
   *                           files. Added NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2015-08-14 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test for
   *                           name methods and added new data for Voyager1 WAC, Voyager2 NAC and
   *                           and WAC.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   */
  class VoyagerCamera : public FramingCamera {
    public:
      VoyagerCamera (Cube &cube);
      //! Destroys the VoyagerCamera object.
      ~VoyagerCamera () {};
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);
      /**
       * CK frame ID -
       * Voyager 1 instrument code (VG1_SCAN_PLATFORM) = -31100
       * Voyager 2 instrument code (VG1_SCAN_PLATFORM) [sic] = -32100
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return p_ckFrameId; }

      /**
       * CK Reference ID - B1950
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (2); }

      /**
       * SPK Target Body ID -
       * VOYAGER 1 = -31
       * VOYAGER 2 = -32
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Target ID
       */
      virtual int SpkTargetId() const { return p_spkTargetId; }

      /**
       * SPK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }

    private:
      int p_ckFrameId;       //!< "Camera-matrix" Kernel Frame ID
      int p_spkTargetId;     //!< Spacecraft Kernel Target ID
  };
};
#endif
