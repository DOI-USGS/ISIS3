#ifndef NewHorizonsMvicTdiCameraDistortionMap_h
#define NewHorizonsMvicTdiCameraDistortionMap_h

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
   *  Distort/undistort focal plane coordinates for New Horizons/MVIC
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera for the New Horizons/MVIC instrument.
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
  class NewHorizonsMvicTdiCameraDistortionMap : public CameraDistortionMap {
    public:
    NewHorizonsMvicTdiCameraDistortionMap(Camera *parent,
                               vector<double> xDistortionCoeffs,
                               vector<double> yDistortionCoeffs,
                               vector<double> residualColDistCoeffs,
                               vector<double> residualRowDistCoeffs);

      ~NewHorizonsMvicTdiCameraDistortionMap();

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

//      bool outputResidualDeltas(); // for debugging

  private:
      bool computeDistortionCorrections(const double xscaled, const double yscaled, double &deltax);
      void computeResidualDistortionCorrections(const double dx, double &residualDeltax,
                                                double &residualDeltay);

    private:
      std::vector<double> m_xDistortionCoeffs; //!< distortion coefficients in x and y as determined
      std::vector<double> m_yDistortionCoeffs; //!< by Keith Harrison (Interface Control Document
                                               //!< section 10.3.1.2)

      vector<double> m_residualColDistCoeffs;  //!< residual distortion coefficients as determined
      vector<double> m_residualRowDistCoeffs;  //!< by Jason Cook, SWRI (MVIC Distortion)

      double m_focalPlaneHalf_x;               //!< half of focal plane x and y dimensions in mm
  };
};
#endif
