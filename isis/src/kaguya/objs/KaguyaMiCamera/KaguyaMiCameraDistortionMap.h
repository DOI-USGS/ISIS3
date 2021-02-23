#ifndef KaguyaMiCameraDistortionMap_h
#define KaguyaMiCameraDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>
#include "CameraDistortionMap.h"

namespace Isis {

  /**
   *  Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Kaguya
   *
   * @see KaguyaMiCamera
   *
   * @author 2012-06-20 Orrin Thomas
   * @internal
   *   @history 2012-06-20 Orrin Thomas - Original Version
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                          coding standards. References #972.
   *   @history 2012-08-24 Tyler J. Wilson, Changed the date of the first internal
   *                          history entry to 2012-06-20 (originally 2020-06-20)
   */
  class KaguyaMiCameraDistortionMap : public CameraDistortionMap {
    public:
      KaguyaMiCameraDistortionMap(Camera *parent);

      //! Destroys the KaguyaMiCameraDistortionMap object.
      virtual ~KaguyaMiCameraDistortionMap() {};

      void SetDistortion(const int naifIkCode);

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);
    private:
      double m_distCoefX[4], m_distCoefY[4];
      double m_boreX, m_boreY;
      int m_numDistCoef;
  };
};
#endif
