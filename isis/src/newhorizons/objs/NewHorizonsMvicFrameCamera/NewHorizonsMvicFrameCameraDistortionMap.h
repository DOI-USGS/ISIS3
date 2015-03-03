#ifndef NewHorizonsMvicFrameCameraDistortionMap_h
#define NewHorizonsMvicFrameCameraDistortionMap_h
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
