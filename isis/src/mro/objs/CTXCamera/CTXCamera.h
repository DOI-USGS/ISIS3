#ifndef CTXCamera_h
#define CTXCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCamera.h"

namespace Isis {
  /**
   * @brief MRO CTX Camera Model
   *
   * This is the camera model for the Mars Reconnaissance Orbiter Context Camera
   * (CTX).
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsReconnaissanceOrbiter
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @todo Allow the programmer to apply scale and shear.
   *   @todo Write multiplaction method (operator*) for Affine * Affine.
   *
   *   @history 2006-08-03  Tracie Sucharski, Added Scale method
   *   @history 2007-07-12  Debbie A. Cook, Added methods Coefficients and
   *                          InverseCoefficients
   *   @history 2008-02-21 Steven Lambright Boresight, focal length, pixel pitch
   *                          keywords now loaded from kernels instead of being hard-coded.
   *                          The distortion map is now being used.
   *   @history 2008-08-08 Steven Lambright Made the unit test work with a Sensor
   *                          change. Also, now using the new LoadCache(...) method instead of
   *                          createCache(...).
   *   @history 2009-03-07 Debbie A. Cook Removed obsolute CameraDetectorMap methods
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *                           methods. Updated documentation. Removed Mro namespace wrap
   *                           inside Isis namespace. Added Isis Disclaimer to files. Added
   *                           NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2012-12-27 Tracie Sucharski, Fixed bug for images with a SpatialSumming=2.
   *                           The images were compressed in the y-direction.  There was a line of
   *                           code commented out, "lineRate *= csum;".  From the
   *                           MRO_ctx_pds_sis.pdf, "Note that CTX implements downtrack summing by
   *                           increasing the line time; for example, a 2X2 summed image has an
   *                           actual line time twice that given by this field.".  Uncommenting
   *                           the line fixed the y-direction scale problem.  Fixes #826.
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   */
  class CTXCamera : public LineScanCamera {
    public:
      CTXCamera(Cube &cube);

      //! Destroys the CTXCamera object.
      ~CTXCamera() {};

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
       * SPK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }
  };
};
#endif
