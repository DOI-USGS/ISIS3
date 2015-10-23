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

#include "NewHorizonsMvicTdiCamera.h"

#include <QDebug>
#include <QString>

#include "NewHorizonsMvicTdiCameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "IException.h"
#include "iTime.h"
#include "LineScanCameraDetectorMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "NaifStatus.h"

using namespace std;
namespace Isis {
  /**
   * Constructor for the New Horizons Mvic/Tdi Camera Model
   *
   * @param lab Pvl label from a New Horizons Mvic/Tdi image.
   *
   * @internal
   */
  NewHorizonsMvicTdiCamera::NewHorizonsMvicTdiCamera(Cube &cube) : LineScanCamera(cube) {
    m_instrumentNameLong = "Multispectral Visible Imaging TDI Camera";
    m_instrumentNameShort = "MVIC TDI";
    m_spacecraftNameLong = "New Horizons";
    m_spacecraftNameShort = "NewHorizons";
    
    NaifStatus::CheckErrors();

    // Set the pixel pitch, focal length and row offset from Mvic frame transfer array
    SetPixelPitch();
    SetFocalLength();

    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString stime = inst["SpacecraftClockStartCount"];

    m_etStart = getClockTime(stime).Et();
    m_lineRate = 1.0 / (double)inst["TdiRate"];

    // The detector map tells us how to convert from image coordinates to
    // detector coordinates.  In our case, a (sample,line) to a (sample,time)
    new LineScanCameraDetectorMap(this, m_etStart, m_lineRate);

    // The focal plane map tells us how to go from detector position
    // to focal plane x/y (distorted).  That is, (sample,time) to (x,y).
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    // This origin does not use 5024/2 because we strip off the leading and trailing 12 pixels
    focalMap->SetDetectorOrigin(2500.5, -16.5);

    // Read legendre polynomial distortion coefficients and boresight offsets from the instrument
    // kernels. Then construct the distortion map.

    // read legendre polynomial distortion coefs from the NAIF Kernels
    QString naifXKey = "INS-98900_DISTORTION_COEF_X";
    QString naifYKey = "INS-98900_DISTORTION_COEF_Y";
    QString naifppKey = "INS-98900_PP_OFFSET";
    vector<double> distCoefX;
    vector<double> distCoefY;
    vector<double> residualColumnDistCoefs;
    vector<double> residualRowDistCoefs;

    for (int i=0; i < 20; i++) {
      distCoefX.push_back(getDouble(naifXKey,i));
      distCoefY.push_back(getDouble(naifYKey,i));
    }

    // read residual polynomial distortion coefs from the NAIF Kernels
    int code = naifIkCode();
    QString naifCOLKey = "INS" + toString(code) + "_RESIDUAL_COL_DIST_COEF";
    QString naifROWKey = "INS" + toString(code) + "_RESIDUAL_ROW_DIST_COEF";

    for (int i=0; i < 6; i++) {
      residualColumnDistCoefs.push_back(getDouble(naifCOLKey,i));
      residualRowDistCoefs.push_back(getDouble(naifROWKey,i));
    }

    new NewHorizonsMvicTdiCameraDistortionMap(this, distCoefX, distCoefY, residualColumnDistCoefs,
                                   residualRowDistCoefs);

    // Setup the ground and sky map
    new LineScanCameraGroundMap(this);
    new LineScanCameraSkyMap(this);

    LoadCache();
    NaifStatus::CheckErrors();
  }
}


// Plugin
/**
 * This is the function that is called in order to instantiate a
 * NewHorizonsMvicTdiCamera object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* NewHorizonsMvicTdiCamera
 * @internal
 */
extern "C" Isis::Camera *NewHorizonsMvicTdiCameraPlugin(Isis::Cube &cube) {
  return new Isis::NewHorizonsMvicTdiCamera(cube);
}
