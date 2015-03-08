#ifndef NewHorizonsMvicTdiCameraDistortionMap_h
#define NewHorizonsMvicTdiCameraDistortionMap_h
/** 
 * @file 
 *  
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
