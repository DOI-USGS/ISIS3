#ifndef ThemisVisDistortionMap_h
#define ThemisVisDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CameraDistortionMap.h"

namespace Isis {
  /**
   * @brief Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of the Themis VIS camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsOdyssey
   *
   * @see ThemisVisCamera
   *
   * @author 2006-01-03 Elizabeth Miller
   *
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Fixed documentation.  Removed
   *                          Odyssey namespace wrap inside Isis wrap.
   *   @history 2014-04-17 Jeannie Backer - Updated documentation for forward/reverse directions
   *                           using ISIS2 lev1u_m01_thm_routines.c.  Added empty destructor.
   *   @history 2014-04-17 Jeannie Backer - Rewrote the reverse direction map (setFocalPlane) to
   *                           solve for the forward direction and iterate until a solution in
   *                           found. Fixes #1659
   */
  class ThemisVisDistortionMap : public CameraDistortionMap {
    public:
      ThemisVisDistortionMap(Camera *parent);
      ~ThemisVisDistortionMap();

      virtual bool SetFocalPlane(const double dx, const double dy);
      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

    private:
      double p_irPixelPitch;    //!< Pixel Pitch for Themis Ir Camera (in meters per pixel).
      double p_visPixelPitch;   //!< Pixel Pitch for Themis Vis Camera (in meters per pixel).

      double p_ir_b5_effectiveDetectorLine; /**< Effective 1-based detector line number used for
                                                 observing the Band 5, i.e., average of the 16
                                                 detector lines used for the band. Detector line
                                                 numbers increase upwards in the image. */
      double p_irBoreLine; //!< The bore line for Themis IR instrument.
  };
};
#endif
