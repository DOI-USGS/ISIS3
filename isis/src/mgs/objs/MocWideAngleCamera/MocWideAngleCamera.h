#ifndef MocWideAngleCamera_h
#define MocWideAngleCamera_h

#include "LineScanCamera.h"

namespace Isis {
  namespace Mgs {
    /**                                                                       
     * @brief Moc Wide Angle Camera Class
     *
     * @internal
     *   @history 2008-08-08 Steven Lambright Now using the new LoadCache(...)
     *            method instead of CreateCache(...).
     *   @history 2008-11-05 Jeannie Walldren - Replaced reference to 
     *          MocLabels IsWideAngleRed() with MocLabels
     *          WideAngleRed().
     *   @history 2008-11-07 Jeannie Walldren - Fixed documentation 
     *   @history 2009-03-07 Debbie A. Cook Removed reference to obsolute
     *          CameraDetectorMap methods
     *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
     *            inherit directly from Camera
     */ 
    class MocWideAngleCamera : public Isis::LineScanCamera {
      public:
        // constructors
        MocWideAngleCamera (Isis::Pvl &lab);
    
        // destructor
        ~MocWideAngleCamera () {};      
    };
  };
};

#endif
