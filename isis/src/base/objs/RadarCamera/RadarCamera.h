#ifndef RadarCamera_h
#define RadarCamera_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"

namespace Isis {
  /**
   * @brief Generic class for Radar Cameras
   *
   * This class is used to abstract out radar camera functionality from children
   * classes.
   *  
   * @ingroup SpiceInstrumentsAndCameras
   *
   * @author 2009-08-26 Steven Lambright
   *
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added documentation and Isis Disclaimer to files.
   *   @history 2015-09-01 Ian Humphrey and Makayla Shepherd - Modified unit test to override 
   *                           Sensor's pure virtual methods.
   *  
   *   @todo Implement more functionality in this class and abstract away from the children
   */

  class RadarCamera : public Camera {
    public:
      RadarCamera(Cube &cube);

      virtual CameraType GetCameraType() const {
        return Radar;
      }

    private:
      //! Copying cameras is not allowed
      RadarCamera(const RadarCamera &);
      //! Assigning cameras is not allowed
      RadarCamera &operator=(const RadarCamera &);
  };
};

#endif
