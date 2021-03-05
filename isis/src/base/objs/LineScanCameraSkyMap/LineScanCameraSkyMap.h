#ifndef LineScanCameraSkyMap_h
#define LineScanCameraSkyMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "CameraSkyMap.h"

namespace Isis {
  /** Convert between undistorted focal plane and ra/dec coordinates
   *
   * This class is used to convert between undistorted focal plane
   * coordinates (x/y) in millimeters and sky (ra/dec).  This
   * class handles the case of line scan cameras.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2005-02-08 Jeff Anderson
   *
   * @internal
   *   @history 2005-10-13 Jeff Anderson - Fixed a bug.  Removed failure
   *                           comparsion test on
   *                           Sensor::SetRightAscensionDeclination calls. A
   *                           failure indicated we didn't hit the target but we
   *                           don't care here since the target is the sky.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                          coding standards. References #972.
   *
   */
  class LineScanCameraSkyMap : public CameraSkyMap {
    public:
      //! Constructor
      LineScanCameraSkyMap(Camera *parent) : CameraSkyMap(parent) {};

      //! Destructor
      virtual ~LineScanCameraSkyMap() {};

      virtual bool SetSky(const double ra, const double dec);
  };
};
#endif
