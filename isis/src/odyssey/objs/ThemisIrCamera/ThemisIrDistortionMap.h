#ifndef ThemisIrDistortionMap_h
#define ThemisIrDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CameraDistortionMap.h"

namespace Isis {
  /** Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of the Themis IR camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsOdyssey
   *
   * @see ThemisIrCamera
   *
   *  @author 2005-02-01 Jeff Anderson
   *
   *  @internal
   *   @history 2005-02-01 Jeff Anderson - Original version
   *   @history 2009-03-27 Jeff Anderson - Modified to use Duxbury's distortion
   *                         model from Feb 2009 email with attached PDF
   *                         document
   *   @history 2011-05-03 Jeannie Walldren - Removed Odyssey namespace wrap.
   *
   */
  class ThemisIrDistortionMap : public CameraDistortionMap {
    public:
      ThemisIrDistortionMap(Camera *parent);

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

      void SetBand(int band);

    private:
      double p_k;
      double p_alpha1;
      double p_alpha2;
  };
};
#endif
