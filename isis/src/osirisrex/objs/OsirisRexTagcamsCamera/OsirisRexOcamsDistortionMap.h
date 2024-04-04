#ifndef OsirisRexOcamsDistortionMap_h
#define OsirisRexOcamsDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QSharedPointer>

#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"

namespace Isis {
  /** Distort/undistort focal plane coordinates for OSIRIS REx's cameras.
   *
   * Creates a map for adding/removing optical distortions from the focal
   * plane of a camera. Modified from original version (which still exists
   * for OCams) to include a local version of the focal plane for support
   * in the use of the OpenCV distortion model.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2019-01-23 Kris Becker
   *
   * @internal
   *   @history 2019-01-23 Kris Becker - Original Version
   *
   */
  class OsirisRexOcamsDistortionMap : public CameraDistortionMap {
    public:
      OsirisRexOcamsDistortionMap(Camera *parent, double zDirection = 1.0);

      virtual ~OsirisRexOcamsDistortionMap();

      virtual void SetDistortion(int naifIkCode);

      virtual bool SetFocalPlane(double dx, double dy);

      virtual bool SetUndistortedFocalPlane(double ux, double uy);

    protected: 
      double m_pixelPitch; //!< The pixel pitch for OCAMS.
      double m_detectorOriginSample; //!< The origin of the detector's sample coordinate.
      double m_detectorOriginLine; //!< The origin of the detector's line coordinate.
      double m_distortionOriginSample; //!< The distortion's origin sample coordinate.
      double m_distortionOriginLine; //!< The distortion's origin line coordinate. 
      double p_tolerance;            //!< Convergence tolerance
      bool   p_debug;                //!< Debug the model

      QSharedPointer<CameraFocalPlaneMap> m_focalMap;  // Local focal plane map
  };
};
#endif
