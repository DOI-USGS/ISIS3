#ifndef RadarSkyMap_h
#define RadarSkyMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"
#include "CameraSkyMap.h"

namespace Isis {
  /** Convert between slantrange/groundrange and ra/dec
   *  coordinates
   *
   * Radar can never paint a star so this routine alway returns
   * false for a sky intersection
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2008-06-17 Jeff Anderson
   *
   * @internal
   *
   *   @history 2008-06-17 Jeff Anderson - Original version
   *
   */
  class RadarSkyMap : public CameraSkyMap {
    public:
      RadarSkyMap(Camera *parent);

      //! Destructor
      virtual ~RadarSkyMap() {};

      virtual bool SetFocalPlane(const double ux, const double uy,
                                 const double uz);

      virtual bool SetSky(const double ra, const double dec);

  };
};
#endif
