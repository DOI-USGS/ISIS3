#ifndef UvvisCamera_h
#define UvvisCamera_h

#include "FramingCamera.h"

namespace Clementine { 
  /**                                                                       
   * @brief Camera class
   *
   * This is the camera class for the UvvisCamera
   *
   * @ingroup Clementine
   *
   * @author  2007-07-10 Tracie Sucharski 
   *
   * @internal
   *   @history 2007-07-10 Steven Lambright - Imported to Isis 3
   *   @history 2007-07-11 Steven Koechle - casted NaifIkCode to int before
   *             istring to fix Linux 32bit build error
   *   @history 2008-08-08 Steven Lambright Made the unit test work with a Sensor
   *            change. Also, now using the new LoadCache(...) method instead of
   *            CreateCache(...).
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *            inherit directly from Camera
   */ 
  class UvvisCamera : public Isis::FramingCamera {
    public:
      UvvisCamera (Isis::Pvl &lab);
      ~UvvisCamera () {};      
  };
};
#endif
