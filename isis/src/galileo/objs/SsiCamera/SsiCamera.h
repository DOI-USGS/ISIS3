#ifndef SsiCamera_h
#define SsiCamera_h

#include "FramingCamera.h"

namespace Galileo { 
  /**
   * @ingroup SpiceInstrumentsAndCameras
   * 
   * @brief Camera class
   *
   * This is the camera class for the IssNACamera
   *
   * @ingroup Galileo
   *
   * @author  Jeff Anderson
   *
   * @internal
   *   @history 2007-10-25 Steven Koechle - Fixed so that it works
   *            in Isis3
   *   @history 2008-08-08 Steven Lambright Now using the new LoadCache(...)
   *            method instead of CreateCache(...).
   *   @history 2009-05-04 Steven Koechle - Fixed to grab appropriate FocalLength
   *            based on image time.
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *            inherit directly from Camera
   */
  class SsiCamera : public Isis::FramingCamera {
    public:
      SsiCamera (Isis::Pvl &lab);
      ~SsiCamera () {};
  };
};
#endif
