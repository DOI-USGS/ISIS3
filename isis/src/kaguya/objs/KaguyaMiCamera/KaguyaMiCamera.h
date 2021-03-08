#ifndef KaguyaMiCamera_h
#define KaguyaMiCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCamera.h"

namespace Isis {
  /**
   * @brief Kaguya MI Camera Model
   *
   * This is the camera model for the Kaguya Multiband pushbroom imagers (VIR and NIR)
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Kaguya
   *
   * @author 2009-02-20 Jacob Danton
   *
   * @internal
   *   @history 2012-06-14 Orrin Thomas - original version
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-01 Ian Humphrey and Makayla Shepherd - Updated check for determining
   *                           instrument names to check all valid MI camera naif codes.
   *                           References #2335.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   */
  class KaguyaMiCamera : public LineScanCamera {
    public:
      KaguyaMiCamera(Cube &cube);

      //! Destroys the LroNarrowAngleCamera object
      ~KaguyaMiCamera() {};

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-131000); }

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
  };
};
#endif
