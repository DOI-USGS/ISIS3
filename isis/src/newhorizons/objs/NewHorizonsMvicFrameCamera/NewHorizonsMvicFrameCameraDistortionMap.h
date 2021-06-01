#ifndef NewHorizonsMvicFrameCameraDistortionMap_h
#define NewHorizonsMvicFrameCameraDistortionMap_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>
#include "CameraDistortionMap.h"

using namespace std;

namespace Isis {

  /**
   *  Distort/undistort focal plane coordinates for New Horizons/MVIC frame sensor
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera for the New Horizons/MVIC frame sensor.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Mvic
   *
   * @see MvicCamera
   *
   * @author 2014-05-02 Ken Edmundson
   * @internal
   *   @history 2014-05-02 Ken Edmundson - Original Version
   */
  class NewHorizonsMvicFrameCameraDistortionMap : public CameraDistortionMap {
    public:
      NewHorizonsMvicFrameCameraDistortionMap(Camera *parent, vector<double> xDistortionCoeffs,
                                   vector<double> yDistortionCoeffs);

      ~NewHorizonsMvicFrameCameraDistortionMap();

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

      bool outputDeltas(); // for debugging

    private:
      bool computeDistortionCorrections(const double xscaled, const double yscaled, double &deltax,
                                        double &deltay);

    private:
      std::vector<double> m_xDistortionCoeffs; //!< distortion coefficients in x and y as determined
      std::vector<double> m_yDistortionCoeffs; //!< by Keith Harrison (Interface Control Document
                                               //!< section 10.3.1.2)

      double m_focalPlaneHalf_x;               //!< half of focal plane x and y dimensions in mm
      double m_focalPlaneHalf_y;
  };
};
#endif
