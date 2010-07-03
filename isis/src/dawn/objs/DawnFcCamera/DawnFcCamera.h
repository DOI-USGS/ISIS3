#ifndef DawnFcCamera_h
#define DawnFcCamera_h

#include "FramingCamera.h"

namespace Dawn {
  /**
   * @ingroup SpiceInstrumentsAndCameras
   *
   * @brief Camera class
   *
   * This is the camera class for the FC camera
   *
   * @ingroup Dawn
   *
   * @author  Janet Barrett
   *
   */
  class DawnFcCamera : public Isis::FramingCamera {
    public:
      DawnFcCamera(Isis::Pvl &lab);
      ~DawnFcCamera() {};
  };
};
#endif
