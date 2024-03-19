#ifndef IrregularBodyCameraGroundMap_h
#define IrregularBodyCameraGroundMap_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"
#include "CameraGroundMap.h"
#include "SurfacePoint.h"

namespace Isis {
  /** 
   * Convert between undistorted focal plane and ground coordinates
   * 
   * This class is derived from CameraGroundMap to support the special case of
   * irregular bodies. Particularly, only the GetXY() method is reimplemented
   * here to never perform the "emission angle" (in places called "back-of-the-
   * planet") test. This is because the test uses the ellipsoidd to validate
   * ground point visibility along the look vector to the surface. This is not
   * adequate for most irregular bodies.
   *
   * @ingroup Camera
   *
   * @see CameraGroundMap
   *
   * @author 2018-07-26 UA/OSIRIS-REx IPWG Team 
   *
   * @internal
   *  @history 2018-07-26 UA/OSIRIS-REx IPWG Team  - Developed to support control
   *                         of irregular bodies
   *  @history 2024-03-10 Ken Edmundson - Modified per Kris Becker such that the
   *                         emission angle (or back-of-the-planet) test is never
   *                         performed in the GetXY method (see class description
   *                         above). 
   */
  class IrregularBodyCameraGroundMap : public CameraGroundMap {
    public:
      IrregularBodyCameraGroundMap(Camera *parent, const bool clip_emission_angles = false);
      
      //! Destructor
      virtual ~IrregularBodyCameraGroundMap() {};

      virtual bool GetXY(const SurfacePoint &spoint, double *cudx, double *cudy);

    protected:
      bool   m_clip_emission;   /**! Test for emission angles? */

  };
};
#endif
