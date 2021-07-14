#ifndef VikingCamera_h
#define VikingCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "FramingCamera.h"

namespace Isis {
  /**
   * @brief Viking Camera Model
   *
   * This is the camera model for both viking orbiter 1 and viking orbiter 2,
   * both cameras A and B.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Viking
   *
   * @see http://nssdc.gsfc.nasa.gov/nmc/masterCatalog.do?sc=1975-075A
   * @see http://nssdc.gsfc.nasa.gov/nmc/masterCatalog.do?sc=1975-083A
   *
   * @author 2005-06-09 Elizabeth Ribelin
   *
   * @internal
   *   @history 2005-11-15 Elizabeth Miller - Fixed problems caused by viking
   *                           data area split
   *   @history 2006-05-17 Elizabeth Miller - Depricated CameraManager to
   *                           CameraFactory
   *   @history 2006-06-14 Elizabeth Miller - Changed format of unitTest to
   *                           fix problems with minor naif changes
   *   @history 2008-08-08 Steven Lambright Now using the new LoadCache(...)
   *                           method instead of CreateCache(...).
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera
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
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods and added data for Viking1 VISA, Viking1 VISB, and
   *                           Viking2 VISA.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   */
  class VikingCamera : public FramingCamera {
    public:
      VikingCamera(Cube &cube);
      //! Destroys the VikingCamera Object
      ~VikingCamera() {};
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);

      /**
       * CK frame ID -
       * Viking1 instrument code (VO1_PLATFORM) = -27000
       * Viking2 instrument code (VO2_PLATFORM) = -30000
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       *
       */
      virtual int CkFrameId() const { return p_ckFrameId; }

      /**
       * CK Reference ID -
       * B1950 or J2000 depending on the ck used.  The mdim2.0_rand ck is in
       * J2000.  Here we use B1950 (code = 2) because it was the reference
       * frame for the original spice
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       *
       */
      virtual int CkReferenceId() const { return (2); }

      /**
       * SPK Target Body ID -
       * VIKING 1 ORBITER = -27
       * VIKING 2 ORBITER = -30
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Target ID
       */
      virtual int SpkTargetId() const { return p_spkTargetId; }

      /**
       * SPK Reference ID - B1950
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (2); }

    private:
      int p_ckFrameId;       //!< "Camera-matrix" Kernel Frame ID
      int p_spkTargetId;     //!< Spacecraft Kernel Target ID
  };
};
#endif
