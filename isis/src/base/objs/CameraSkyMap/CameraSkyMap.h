#ifndef CameraSkyMap_h
#define CameraSkyMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"

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
   * @author 2005-02-08 Jeff Anderson
   *
   * @internal
   *  @history 2005-02-08 Jeff Anderson Original version
   *  @history 2008-07-14 Steven Lambright Added NaifStatus calls
   *  @history 2017-08-30 Summer Stapleton - Updated documentation. References #4807.
   *
   */
  class CameraSkyMap {
    public:
      CameraSkyMap() = default;
      CameraSkyMap(Camera *parent);

      //! Destructor
      virtual ~CameraSkyMap() {};

      virtual bool SetFocalPlane(const double ux, const double uy,
                                 const double uz);

      virtual bool SetSky(const double ra, const double dec);

      /**
       * @returns The undistorted focal plane x
       */
      inline double FocalPlaneX() const {
        return p_focalPlaneX;
      };

      /**
       * @returns The undistorted focal plane y
       */
      inline double FocalPlaneY() const {
        return p_focalPlaneY;
      };

    protected:
      Camera *p_camera;       //!< The main camera to calculate distortions on
      double p_focalPlaneX;   //!< Undistorted x value for the focal plane
      double p_focalPlaneY;   //!< Undistorted y value for the focal plane
  };
};
#endif
