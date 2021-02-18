#ifndef HiriseCamera_h
#define HiriseCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCamera.h"

namespace Isis {
  /**
   * @brief Hirise Camera Model
   *
   * This class is the implementation of the camera model
   * for the MRO HiRISE instrument.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsReconnaissanceOrbiter
   *
   * @author  2005-02-22 Jim Torson
   *
   * @internal
   *  @history 2005-10-03 Elizabeth Miller - Modified file to support Doxygen
   *                           documentation
   *  @history 2008-08-08 Steven Lambright Made the unit test work with a Sensor
   *                           change. Also, now using the new LoadCache(...) method instead of
   *                           createCache(...).
   *  @history 2009-03-08 Debbie A. Cook Removed reference to obsolete LineScanCameraDetectorMap
   *                           method SetXAxisTimeDependent
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *                           methods. Added NAIF error check. Removed Mro namespace.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   */
  class HiriseCamera : public LineScanCamera {
    public:
      // Constructs a HiriseCamera object
      HiriseCamera(Cube &cube);

      // Destroys the HiriseCamera object
      ~HiriseCamera();

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-74000); }

      /**
       *  CK Reference ID - MRO_MME_OF_DATE
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (-74900); }

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
