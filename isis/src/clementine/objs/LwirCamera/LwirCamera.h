#ifndef LwirCamera_h
#define LwirCamera_h

#include "FramingCamera.h"

namespace Clementine {
  /**
   * @brief LWIR Camera class
   *
   * This is the camera class for Clementine's long wavelength
   * infared camera.
   *
   * @ingroup Clementine
   *
   * @author  2009-01-16 Jeannie Walldren
   *
   * @internal
   *   @history 2009-01-16 Jeannie Walldren - Original Version
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *            inherit directly from Camera
   */
  class LwirCamera : public Isis::FramingCamera {
    public:
      LwirCamera(Isis::Pvl &lab);
      ~LwirCamera() {};

      /** CK Frame ID - Instrument Code from spacit run on CK */
      virtual int CkFrameId() const { return (-40000); }

      /** CK Reference ID - J2000 */
      virtual int CkReferenceId() const { return (1); }

      /** SPK Reference ID - J2000 */
      virtual int SpkReferenceId() const { return (1); }
  };
};
#endif
