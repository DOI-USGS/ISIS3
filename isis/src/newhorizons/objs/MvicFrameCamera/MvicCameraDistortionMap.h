#ifndef MvicCameraDistortionMap_h
#define MvicCameraDistortionMap_h
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
  class MvicCameraDistortionMap : public CameraDistortionMap {
    public:
      MvicCameraDistortionMap(Camera *parent);

      //! Destroys the MvicMiCameraDistortionMap object.
      ~MvicCameraDistortionMap();

      void SetDistortion(const int naifIkCode);

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);
    private:
      std::vector<double> m_distCoefX;
      std::vector<double> m_distCoefY;
      double m_boreX, m_boreY;
      int m_numDistCoef;
  };
};
#endif
