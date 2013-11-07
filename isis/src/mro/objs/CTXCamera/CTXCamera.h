#ifndef CTXCamera_h
#define CTXCamera_h
/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
   *            InverseCoefficients
   *   @history 2008-02-21 Steven Lambright Boresight, focal length, pixel pitch
   *            keywords now loaded from kernels instead of being hard-coded.
   *            The distortion map is now being used.
   *   @history 2008-08-08 Steven Lambright Made the unit test work with a Sensor
   *            change. Also, now using the new LoadCache(...) method instead of
   *            createCache(...).
   *   @history 2009-03-07 Debbie A. Cook Removed obsolute CameraDetectorMap methods
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *            inherit directly from Camera
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *            methods. Updated documentation. Removed Mro namespace wrap
   *            inside Isis namespace. Added Isis Disclaimer to files. Added
   *            NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *            coding standards. References #972.
   *   @history 2012-12-27 Tracie Sucharski, Fixed bug for images with a SpatialSumming=2.
   *            The images were compressed in the y-direction.  There was a line of code commented
   *            out, "lineRate *= csum;".  From the MRO_ctx_pds_sis.pdf, "Note that CTX implements
   *            downtrack summing by increasing the line time; for example, a 2X2 summed image has
   *            an actual line time twice that given by this field.".  Uncommenting the line fixed
   *            the y-direction scale problem.  Fixes #826.
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
