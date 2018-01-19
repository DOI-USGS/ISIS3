#ifndef SAMPLESCANCAMERA_H
#define SAMPLESCANCAMERA_H
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
  class SampleScanCameraGroundMap;
  class SampleScanCameraDetectorMap;
  class SampleScanCameraSkyMap;

  /**
   * @brief Generic class for Sample Scan Cameras
   *
   * This class is used to abstract out sample scan camera functionality from
   * children classes.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @author 2016-09-12 Ken Edmundson
   *
   * @internal
   *   @history 2016-09-12 Ken Edmundson - Original version.
   *  
   *   @todo Implement more functionality in this class and abstract away from the children
   */

  class SampleScanCamera : public Camera {
    public:
      SampleScanCamera(Isis::Cube &cube);

      /** 
       * Returns the SampleScan type of camera, as enumerated in the Camera
       * class. 
       * @return @b CameraType SampleScan camera type.
       */
      virtual CameraType GetCameraType() const {
        return SampleScan;
      }

      /**
       * Returns a pointer to the SampleScanCameraGroundMap object
       *
       * @return SampleScanCameraGroundMap*
       */
      SampleScanCameraGroundMap *GroundMap() {
        return (SampleScanCameraGroundMap *)Camera::GroundMap();
      };

      /**
       * Returns a pointer to the SampleScanCameraSkyMap object
       *
       * @return SampleScanCameraSkyMap*
       */
      SampleScanCameraSkyMap *SkyMap() {
        return (SampleScanCameraSkyMap *)Camera::SkyMap();
      };

      /**
       * Returns a pointer to the SampleScanCameraDetectorMap object
       *
       * @return SampleScanCameraDetectorMap*
       */
      SampleScanCameraDetectorMap *DetectorMap() {
        return (SampleScanCameraDetectorMap *)Camera::DetectorMap();
      };

    private:
      //! Copying cameras is not allowed
      SampleScanCamera(const SampleScanCamera &);
      //! Assigning cameras is not allowed
      SampleScanCamera &operator=(const SampleScanCamera &);
  };
};

#endif
