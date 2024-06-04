#ifndef CSMSkyMap_h
#define CSMSkyMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CSMCamera.h"
#include "CameraSkyMap.h"

namespace Isis {


  /** Convert between undistorted focal plane and ra/dec coordinates
   *
   * This base class is used to convert between undistorted focal plane
   * coordinates (x/y) in millimeters and sky (ra/dec).  This
   * class handles the case of framing cameras.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   */
  class CSMSkyMap : public CameraSkyMap {
    public:
      
      CSMSkyMap(Camera *parent);

      virtual bool SetSky(const double ra, const double dec);
  };
};

#endif
