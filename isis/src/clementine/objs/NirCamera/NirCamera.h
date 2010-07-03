#ifndef NirCamera_h
#define NirCamera_h

#include "FramingCamera.h"

namespace Clementine { 
  /**                                                                       
   * @brief Camera class
   *
   * This is the camera class for the NirCamera
   *
   * @ingroup Clementine
   *
   * @author  2007-07-10 Steven Lambright
   *
   * @internal
   *   @history 2007-07-10 Steven Lambright - Original Version
   *   @history 2007-07-11 Steven Koechle - casted NaifIkCode to int before
   *            istring to fix Linux 32bit build error
   *  @history 2008-08-08 Steven Lambright Made the unit test work with a Sensor
   *           change. Also, now using the new LoadCache(...) method instead of
   *           CreateCache(...).
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *            inherit directly from Camera
   */
  class NirCamera : public Isis::FramingCamera {
    public:
      NirCamera (Isis::Pvl &lab);
      ~NirCamera () {};      
  };
};
#endif
