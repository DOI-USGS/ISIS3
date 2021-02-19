#ifndef PushFrameCameraGroundMap_h
#define PushFrameCameraGroundMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CameraGroundMap.h"

namespace Isis {
  /** Convert between undistorted focal plane and ground coordinates
   *
   * This class is used to convert between undistorted focal plane
   * coordinates (x/y) in millimeters and ground coordinates lat/lon
   * for line scan cameras.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2007-10-17 Steven Lambright and Jeff Anderson
   *
   * @internal
   *   @history 2008-06-18 Steven Lambright Fixed documentation
   *   @history 2008-10-23 Steven Lambright Added optimizations, fixed misc bugs
   *   @history 2009-11-19 Steven Lambright Removed linear search offset
   *   @history 2009-12-07 Steven Lambright Increased liklihood that our spacecraft distance
   *                           correctly minimizes for LRO
   *   @history 2014-04-17 Jeannie Backer - Added padding to bring closer to ISIS coding standards.
   *                           References #1659.
   */
  class PushFrameCameraGroundMap : public CameraGroundMap {
    public:
      /**
       * This is the constructor for the push frame ground map
       *
       * @param cam Pointer to the camera
       * @param evenFramelets True if the image contains even framelets
       */
      PushFrameCameraGroundMap(Camera *cam, bool evenFramelets) :
        CameraGroundMap(cam) {
        p_evenFramelets = evenFramelets;
      }

      //! Destructor
      virtual ~PushFrameCameraGroundMap() {};

      virtual bool SetGround(const Latitude &lat, const Longitude &lon);
      virtual bool SetGround(const SurfacePoint &surfacePt);

    private:
      double FindDistance(int framelet, const SurfacePoint &surfacePoint);
      double FindSpacecraftDistance(int framelet,
                                    const SurfacePoint &surfacePoint);

      bool   p_evenFramelets; //!< True if the file contains even framelets
  };
};
#endif
