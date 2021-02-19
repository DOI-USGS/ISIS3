#ifndef MexHrscSrcCamera_h
#define MexHrscSrcCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

#include <QString>

namespace Isis {
  /**
   * This is the camera model for the Mex HRSC SRC Framing Camera
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Mex
   *
   * @author 2020-04-02 Stuart Sides
   *
   * @internal
   *   @history 2020-04-01 Stuart Sides - Initial version

   */
  class MexHrscSrcCamera : public FramingCamera {
    public:
      //! Create a MexHrscSrcCamera object
      MexHrscSrcCamera(Cube &cube);

      //! Destroys the MexHrscSrcCamera object
      ~MexHrscSrcCamera() {};

     /**
      * Reimplemented from FrameCamera
      *
      * @param time Start time of the observation
      * @param exposureDuration The exposure duration of the observation
      *
      * @return std::pair<iTime,iTime> The start and end times of the observation
      */
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-41001); }


      /**
       * CK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (1); }


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
