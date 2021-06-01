#ifndef RollingShutterCamera_h
#define RollingShutterCamera_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"

/**
 * @brief Generic class for Rolling Shutter Cameras
 *
 * This class is used to abstract out framing camera functionality from children
 * classes.
 *
 * @ingroup SpiceInstrumentsAndCameras
 *
 * @author Makayla Shepherd 2018-04-02
 *
 * @internal
 *   @history 2018-04-09 Ian Humphrey - Updated some doxygen documentation and coding standards.
 */
namespace Isis {
  class Cube;
  class RollingShutterCameraDetectorMap;

  /**
   * @brief Generic class for Rolling Shutter Cameras
   *
   * This class is used to abstract out rolling shutter camera functionality from
   * children classes.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @author 2018-04-02 Makayla Shepherd
   */
  class RollingShutterCamera : public Camera {
    public:
      RollingShutterCamera(Cube &cube);

      /**
       * Returns the RollingShutter type of camera, as enumerated in the Camera
       * class.
       * @return @b CameraType RollingShutter camera type.
       */
      virtual CameraType GetCameraType() const {
        return RollingShutter;
      };

      /**
       * Returns a pointer to the RollingShutterCameraDetectorMap object
       *
       * @return RollingShutterCameraDetectorMap*
       */
      RollingShutterCameraDetectorMap *DetectorMap() {
        return (RollingShutterCameraDetectorMap *)Camera::DetectorMap();
      };

    private:
      /** Copying cameras is not allowed */
      RollingShutterCamera(const RollingShutterCamera &);
      /** Assigning cameras is not allowed */
      RollingShutterCamera &operator=(const RollingShutterCamera &);
  };
};

#endif
