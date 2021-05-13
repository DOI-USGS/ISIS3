#ifndef LoMediumCamera_h
#define LoMediumCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

namespace Isis {
  /**
   * @brief Defines the Lunar Orbiter Medium Resolution camera class
   *
   * The LoMediumCamera class defines the Medium Resolution camera for the last
   * three Lunar Orbiter missions (3, 4, and 5).
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup LunarOrbiter
   *
   * @author 2007-07-17 Debbie A. Cook
   *
   * @internal
   *   @history 2007-07-17 Debbie A. Cook - Original Version
   *   @history 2008-04-07 Debbie A. Cook - Set x/y axis directions for jigsaw
   *   @history 2008-08-08 Steven Lambright Made the unit test work with a
   *                           Sensor change. Also, now using the new
   *                           LoadCache(...) method instead of CreateCache(...).
   *   @history 2009-03-07 Debbie A. Cook Removed reference to obsolute
   *                           CameraDetectorMap methods
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera
   *   @history 2010-09-16 Steven Lambright - Updated unitTest to not use a DEM.
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                           pure virtual in Camera, implemented in mission
   *                           specific cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2011-05-03 Jeannie Walldren - Added ShutterOpenCloseTimes()
   *                           method. Updated unitTest to test for new methods.
   *                           Updated documentation. Removed Lo namespace wrap
   *                           inside Isis namespace wrap. Added Isis Disclaimer
   *                           to files. Added NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2014-01-17 Kris Becker Corrected CkFrameId reference to
   *                           properly define LO 3,4,5 at runtime for generation
   *                           of CKs using ckwriter. References #1737.
   *   @history 2015-08-24 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test for these
   *                           methods and added data for LO4 and LO5.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   */
  class LoMediumCamera : public FramingCamera {
    public:
      LoMediumCamera(Cube &cube);
      //! Destroys the LoMediumCamera Object
      ~LoMediumCamera() {};
      /**
       * This enum defines the types of focal plane maps supported in this
       * class.
       */
      enum FocalPlaneMapType { Fiducial, //!< Fiducial Focal Plane Map
                               Boresight,//!< Boresight Focal Plane Map
                               None      //!< No Focal Plane Map
                             };
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (m_ckFrameId); }

      /**
       * CK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (1); }

      /**
       *  SPK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }

    private:
      int m_ckFrameId;                        //!< LO 3,4,5 CK frame id
  };
};
#endif
