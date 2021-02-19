#ifndef PUSHFRAMECAMERA_H
#define PUSHFRAMECAMERA_H
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"

namespace Isis {
  class PushFrameCameraGroundMap;
  class PushFrameCameraDetectorMap;

  /**
   * @brief Generic class for Push Frame Cameras
   *
   * This class is used to abstract out push frame camera functionality from
   * children classes.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @author 2009-08-26 Steven Lambright
   *
   * @internal
   *   @history 2010-08-04 Jeannie Walldren - Fixed the \#ifndef identifier to
   *                          define PushFrameCamera_h.  Updated documentation.
   *   @history 2011-05-03 Jeannie Walldren - Added Isis Disclaimer to files.
   *   @history 2015-09-01 Ian Humphrey and Makayla Shepherd - Modified unit test to override 
   *                           Sensor's pure virtual methods.
   *  
   *   @todo Implement more functionality in this class and abstract away from the children
   */

  class PushFrameCamera : public Camera {
    public:
      PushFrameCamera(Isis::Cube &cube);

      /** 
       * Returns the PushFrame type of camera, as enumerated in the Camera 
       * class. 
       * @return @b CameraType PushFrame camera type.
       */
      virtual CameraType GetCameraType() const {
        return PushFrame;
      }

      /**
       * Returns a pointer to the PushFrameCameraGroundMap object
       *
       * @return PushFrameCameraGroundMap*
       */
      PushFrameCameraGroundMap *GroundMap() {
        return (PushFrameCameraGroundMap *)Camera::GroundMap();
      };

      /**
       * Returns a pointer to the PushFrameCameraDetectorMap object
       *
       * @return PushFrameCameraDetectorMap*
       */
      PushFrameCameraDetectorMap *DetectorMap() {
        return (PushFrameCameraDetectorMap *)Camera::DetectorMap();
      };

    private:
      //! Copying cameras is not allowed
      PushFrameCamera(const PushFrameCamera &);
      //! Assigning cameras is not allowed
      PushFrameCamera &operator=(const PushFrameCamera &);
  };
};

#endif
