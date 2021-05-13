#ifndef ReseauDistortionMap_h
#define ReseauDistortionMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <vector>
#include "CameraDistortionMap.h"

namespace Isis {
  /**
   * Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of the camera.
   *
   * @ingroup Camera
   *
   * @author 2005-06-08 Elizabeth Ribelin
   *
   * @internal
   *   @history 2005-12-07 Elizabeth Miller - Added check for colinearity in
   *                  closest reseaus to fix a bug
   */
  class ReseauDistortionMap : public CameraDistortionMap {
    public:
      ReseauDistortionMap(Camera *parent, Pvl &labels, const QString &fname);

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

    private:
      std::vector<double> p_rlines, p_rsamps;        //!<Refined Reseau Locations
      std::vector<double> p_mlines, p_msamps;        //!<Master Reseau Locations
      double p_distortedLines, p_distortedSamps;     /**<Dimensions of distorted
                                                        cube*/
      double p_undistortedLines, p_undistortedSamps; /**<Dimensions of
                                                        undistorted cube*/
      int p_numRes;                                  //!<Number of Reseaus
      double p_pixelPitch;                           /**<Pixel Pitch of parent
                                                        Camera*/
  };
};
#endif

