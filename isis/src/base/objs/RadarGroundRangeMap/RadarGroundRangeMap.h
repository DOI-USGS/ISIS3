#ifndef RadarGroundRangeMap_h
#define RadarGroundRangeMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"
#include "CameraFocalPlaneMap.h"

namespace Isis {

#ifndef RADAR_LOOK_DIR
  namespace Radar {
    enum LookDirection { Left, Right };
  }
#define RADAR_LOOK_DIR
#endif

  /**
   * Construct a mapping between image sample and Radar ground range
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2008-06-17 Jeff Anderson
   * Original version
   *
   * @internal
   *
   * @history 2009-07-01 Janet Barrett - Corrected the transformation
   *                     calculations
   *
   */
  class RadarGroundRangeMap : public CameraFocalPlaneMap {
    public:
      RadarGroundRangeMap(Camera *parent, const int naifIkCode);

      static void setTransform(int naifIkCode, double groundRangeResolution,
                               int samples, Radar::LookDirection ldir);
  };
};
#endif
