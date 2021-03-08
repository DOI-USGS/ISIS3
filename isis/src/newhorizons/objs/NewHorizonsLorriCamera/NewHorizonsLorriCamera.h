#ifndef NewHorizonsLorriCamera_h
#define NewHorizonsLorriCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

#include <QString>

namespace Isis {
  /**
   * This is the camera model for the LORRI Framing Camera
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup New Horizons
   *
   * @author 2013-11-12 Stuart Sides
   *
   * @internal
   *   @history 2015-08-11 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           added methods.
   *   @history 2015-08-27 Stuart Sides - Modified to work with new Lorri IK with different unit
   *                          (mm) for the focal length. Incremented camera version to 2.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   *   @history 2016-10-21 Kristin Berry - Updated unitTest. References #4476.

   */
  class NewHorizonsLorriCamera : public FramingCamera {
    public:
      //! Create a NewHorizonsLorriCamer object
      NewHorizonsLorriCamera(Cube &cube);

      //! Destroys the NewHorizonsLorriCamera object
      ~NewHorizonsLorriCamera() {};

    /**
     * Reimplemented from FrameCamera
     *
     * @author Stuart Sides (2013/12/26)
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
      virtual int CkFrameId() const { return (-98000); }


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
