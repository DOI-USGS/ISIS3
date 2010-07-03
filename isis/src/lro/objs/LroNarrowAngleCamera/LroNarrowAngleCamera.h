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
    	  LroNarrowAngleCamera (Isis::Pvl &lab);

        ~LroNarrowAngleCamera () {};
    };
  };
};

#endif
