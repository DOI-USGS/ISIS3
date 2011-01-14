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

      /** CK Frame ID - Instrument Code from spacit run on CK */
      virtual int CkFrameId() const { return (-203000); }

      /** CK Reference ID - J2000 */
      virtual int CkReferenceId() const { return (1); }

      /** SPK Reference ID - J2000 */
      virtual int SpkReferenceId() const { return (1); }
  };
};
#endif
