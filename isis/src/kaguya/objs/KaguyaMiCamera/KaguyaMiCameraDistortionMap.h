#ifndef KaguyaMiCameraDistortionMap_h
#define KaguyaMiCameraDistortionMap_h
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
