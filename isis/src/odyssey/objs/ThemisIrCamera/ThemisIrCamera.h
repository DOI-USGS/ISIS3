#ifndef ThemisIrCamera_h
#define ThemisIrCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCamera.h"

#include <QString>

namespace Isis {
  class ThemisIrDistortionMap;

  /**
   * @brief THEMIS IR Camera
   *
   * This is the camera model for the Thermal Emission Imaging System
   * Infrared (THEMIS IR) camera
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsOdyssey
   *
   * @author  2005-01-01 Jeff Anderson
   *
   * @internal
   *   @history 2007-07-13 Jeff Anderson Added support for spatial summing
   *   @history 2008-08-08 Steven Lambright Now using the new LoadCache(...)
   *                           method instead of CreateCache(...).
   *   @history 2009-05-12 Jeff Anderson Reworked code for changes from
   *                           Kiefer-Torson model to Duxbury model.  The majors changes
   *                           where the removal of a ~1% error in focal lenght and the
   *                           improving the spectral band registration to better than
   *                           1/20th of a pixel
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new methods.
   *                           Updated documentation. Removed Odyssey namespace
   *                           wrap inside Isis namespace. Added Isis Disclaimer
   *                           to files. Added NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   */
  class ThemisIrCamera : public LineScanCamera {
    public:
      // constructors
      ThemisIrCamera(Cube &cube);

      //! Destroys the ThemisIrCamera object.
      ~ThemisIrCamera() {};

      // Band dependent
      void SetBand(const int band);
      bool IsBandIndependent() {
        return false;
      };

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-53000); }

      /**
       * CK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (16); }

      /**
       *  SPK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }

    private:
      double p_etStart;
      double p_lineRate;
      double p_bandTimeOffset;
      QString p_tdiMode;
      std::vector<int> p_originalBand;
  };

};
#endif
