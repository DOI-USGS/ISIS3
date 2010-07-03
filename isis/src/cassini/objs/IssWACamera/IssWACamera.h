// $Id: IssWACamera.h,v 1.7 2009/08/31 15:12:29 slambright Exp $
#ifndef IssWACamera_h
#define IssWACamera_h

#include "FramingCamera.h"

namespace Cassini { 
  /**                                                                       
   * @brief Cassini ISS Wide Angle Camera class
   *
   * This is the camera class for the IssWACamera
   *
   * @ingroup Cassini
   *
   * @author  2007-07-10 Steven Koechle
   *
   * @internal
   *   @history 2007-07-10 Steven Koechle - Original Version
   *   @history 2007-07-10 Steven Koechle - Removed hardcoding of
   *            NAIF Instrument number
   *   @history 2007-07-11 Steven Koechle - casted NaifIkCode to
   *           int before iString to fix problem on Linux 32bit
   *   @history 2008-08-08 Steven Lambright Now using the new LoadCache(...)
   *            method instead of CreateCache(...).
   *   @history 2009-01-22 Kris Becker Added new frame rotation to the CK frame
   *            hierarchy to convert to detector coordinates.  This is
   *            essentially a 180 degree rotation.  The frame definition is
   *            actually contained in the IAK.
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *            inherit directly from Camera
   */
  class IssWACamera : public Isis::FramingCamera {
    public:
      IssWACamera (Isis::Pvl &lab);
      ~IssWACamera () {};      
  };
};
#endif
