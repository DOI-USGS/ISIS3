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
      HiresCamera (Isis::Pvl &lab);
      ~HiresCamera () {};      
  };
};
#endif
