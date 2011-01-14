#ifndef HiresCamera_h
#define HiresCamera_h

#include "FramingCamera.h"

namespace Clementine {
  /**
   * @brief Camera class
   *
   * This is the camera class for the HiresCamera
   *
   * @ingroup Clementine
   *
   * @author  2009-01-16 Tracie Sucharski
   *
   * @internal
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *            inherit directly from Camera
   */
  class HiresCamera : public Isis::FramingCamera {
    public:
      HiresCamera(Isis::Pvl &lab);
      ~HiresCamera() {};

      /** CK Frame ID - Instrument Code from spacit run on CK */
      virtual int CkFrameId() const { return (-40000); }

      /** CK Reference ID - J2000 */
      virtual int CkReferenceId() const { return (1); }

      /** SPK Reference ID - J2000 */
      virtual int SpkReferenceId() const { return (1); }
  };
};
#endif
