#ifndef RollingShutterCamera_h
#define RollingShutterCamera_h
/**
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
