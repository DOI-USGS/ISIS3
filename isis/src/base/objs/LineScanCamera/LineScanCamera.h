#ifndef LINESCANCAMERA_H
#define LINESCANCAMERA_H
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"

namespace Isis {
  class LineScanCameraGroundMap;
  class LineScanCameraDetectorMap;
  class LineScanCameraSkyMap;

  /**
   * @brief Generic class for Line Scan Cameras
   *
   * This class is used to abstract out line scan camera functionality from
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

  class LineScanCamera : public Camera {
    public:
      LineScanCamera(Isis::Cube &cube);

      /** 
       * Returns the LineScan type of camera, as enumerated in the Camera 
       * class. 
       * @return @b CameraType LineScan camera type.
       */
      virtual CameraType GetCameraType() const {
        return LineScan;
      }

      /**
       * Returns a pointer to the LineScanCameraGroundMap object
       *
       * @return LineScanCameraGroundMap*
       */
      LineScanCameraGroundMap *GroundMap() {
        return (LineScanCameraGroundMap *)Camera::GroundMap();
      };

      /**
       * Returns a pointer to the LineScanCameraSkyMap object
       *
       * @return LineScanCameraSkyMap*
       */
      LineScanCameraSkyMap *SkyMap() {
        return (LineScanCameraSkyMap *)Camera::SkyMap();
      };

      /**
       * Returns a pointer to the LineScanCameraDetectorMap object
       *
       * @return LineScanCameraDetectorMap*
       */
      LineScanCameraDetectorMap *DetectorMap() {
        return (LineScanCameraDetectorMap *)Camera::DetectorMap();
      };

    private:
      //! Copying cameras is not allowed
      LineScanCamera(const LineScanCamera &);
      //! Assigning cameras is not allowed
      LineScanCamera &operator=(const LineScanCamera &);
  };
};

#endif
