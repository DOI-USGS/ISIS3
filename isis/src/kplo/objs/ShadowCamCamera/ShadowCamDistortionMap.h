#ifndef ShadowCamDistortionMap_h
#define ShadowCamDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>
#include <cmath>
#include <string.h>
#include "IString.h"
#include "IException.h"
#include "Application.h"
#include "CameraDistortionMap.h"

namespace Isis {

  /**
   *  Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.
   *
   * This class may throw exceptions during error conditions. 
   * It also logs important steps and errors using ISIS logging.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup KoreaPathfinderLunarOrbiter
   *
   * @see ShadowCamCamera
   *
   * @author 2022-10-12 Victor Silva
   * @internal
   */
  class ShadowCamDistortionMap : public CameraDistortionMap {
    public:
      ShadowCamDistortionMap(Camera *parent);

      //! Destroys the ShadowCamDistortionMap object.
      virtual ~ShadowCamDistortionMap() {};

      /**
       * Sets the distortion coefficients for the camera.
       * @param naifIkCode The NAIF IK Code to retrieve the distortion coefficients.
       * @throws IException if error occurs during retrieval.
       */
      void SetDistortion(const int naifIkCode);

      /**
       * Computes the undistorted focal plane coordinates given the distorted ones.
       * @param dx Distorted focal plane x-coordinate.
       * @param dy Distorted focal plane y-coordinate.
       * @returns true if conversion was successful, false otherwise.
       * @throws IException if error occurs during computation.
       */
      virtual bool SetFocalPlane(const double dx, const double dy);

      /**
       * Computes the distorted focal plane coordinates given the undistorted ones.
       * @param ux Undistorted focal plane x-coordinate.
       * @param uy Undistorted focal plane y-coordinate.
       * @returns true if conversion was successful, false otherwise.
       * @throws IException if error occurs during computation.
       */
      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

  };
};
#endif
