#ifndef PUSHFRAMECAMERA_H
#define PUSHFRAMECAMERA_H
/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2009/08/31 15:11:49 $
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
