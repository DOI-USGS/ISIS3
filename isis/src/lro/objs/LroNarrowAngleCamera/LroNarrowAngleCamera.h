#ifndef LroNarrowAngleCamera_h
#define LroNarrowAngleCamera_h

#include "LineScanCamera.h"

namespace Isis {
  namespace Lro {
    /**
     * @internal
     *   @history 2009-02-20  Jacob Danton, Original Object
     *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
     *            inherit directly from Camera
     */
    class LroNarrowAngleCamera : public LineScanCamera {
      public:
        LroNarrowAngleCamera(Isis::Pvl &lab);

        ~LroNarrowAngleCamera() {};

        /** CK Frame ID - Instrument Code from spacit run on CK */
        virtual int CkFrameId() const { return (-85000); }

        /** CK Reference ID - J2000 */
        virtual int CkReferenceId() const { return (1); }

        /** SPK Reference ID - J2000 */
        virtual int SpkReferenceId() const { return (1); }
    };
  };
};

#endif
