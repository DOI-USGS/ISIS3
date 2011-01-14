#ifndef MocNarrowAngleCamera_h
#define MocNarrowAngleCamera_h

#include "LineScanCamera.h"

namespace Isis {
  namespace Mgs {
    /**
     * @internal
     *   @history 2008-08-08 Steven Lambright Now using the new LoadCache(...)
     *            method instead of CreateCache(...).
     *   @history 2009-03-07 Debbie A. Cook Removed reference to obsolute
     *            CameraDetectorMap methods
     *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
     *            inherit directly from Camera
     */
    class MocNarrowAngleCamera : public LineScanCamera {
      public:
        MocNarrowAngleCamera(Isis::Pvl &lab);

        ~MocNarrowAngleCamera() {};

        /** CK Frame ID - Instrument Code from spacit run on CK */
        virtual int CkFrameId() const { return (-94000); }

        /** CK Reference ID - J2000 */
        virtual int CkReferenceId() const { return (1); }

        /** SPK Reference ID - J2000 */
        virtual int SpkReferenceId() const { return (1); }
    };
  };
};

#endif
