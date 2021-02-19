#ifndef FramingCamera_h
#define FramingCamera_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"

namespace Isis {
  /**
   * @brief Generic class for Framing Cameras
   *
   * This class is used to abstract out framing camera functionality from children
   * classes.   The ShutterOpenCloseTimes() method is pure virtual and must be 
   * overridden by all framing camera models that extend this class. 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @author 2009-08-26 Steven Lambright
   *
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added destructor and
   *                           ShutterOpenCloseTimes() method. Updated unitTest.
   *   @history 2015-09-01 Ian Humphrey and Makayla Shepherd - Modified unit test to override 
   *                           Sensor's pure virtual methods.
   *  
   *   @todo Implement more functionality in this class and abstract away from the children
   */

  class FramingCamera : public Camera {
    public:
      FramingCamera(Cube &cube);
      //! Destroys the FramingCamera Object
      ~FramingCamera() {};

      /**
       * This method returns Framing camera type.
       * 
       * @return @b CameraType Type of camera, Framing.
       */
      virtual CameraType GetCameraType() const {
        return Framing;
      }
      virtual std::pair<iTime, iTime> ShutterOpenCloseTimes(double time,
                                                            double exposureDuration) = 0;
    private:
      //! Copying cameras is not allowed
      FramingCamera(const FramingCamera &);
      //! Assigning cameras is not allowed
      FramingCamera &operator=(const FramingCamera &);
  };
};

#endif
