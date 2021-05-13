#ifndef NewHorizonsMvicTdiCamera_h
#define NewHorizonsMvicTdiCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCamera.h"

namespace Isis {

  /**
   * @brief New Horizons Mvic Camera, Tdi mode
   *
   * This is the camera model for the New Horizons Mvic camera operating in Tdi mode
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup NewHorizons
   *
   * @author  2014-02-12 Tracie Sucharski
   *
   * @internal
   *   @history 2015-08-11 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           added methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   *   @history 2015-10-21 Stuart Sides - Changed the throw in the SetUndistortedFocalPlane
   *                           mamber to a return false. The cases were the member was throwing
   *                           were way outside the array, so this is not expexted to cause any
   *                           problems. These routines should not ever throw. The calling
   *   @history 2016-10-21 Kristin Berry - Updated unitTest. References #4476.
   */
  class NewHorizonsMvicTdiCamera : public LineScanCamera {
    public:
      // constructor
      NewHorizonsMvicTdiCamera(Cube &cube);

      //! Destroys the NewHorizonsMvicTdiCamera object.
      ~NewHorizonsMvicTdiCamera() {};

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-98000); }

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
      double m_etStart;
      double m_lineRate;
  };
};
#endif
