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

#include "MvicTdiCamera.h"

#include <QDebug>

#include "MvicTdiCameraDistortionMap.h"
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
  MvicTdiCamera::MvicTdiCamera(Cube &cube) : LineScanCamera(cube) {
    NaifStatus::CheckErrors();

    // Set the pixel pitch, focal length and row offset from Mvic frame transfer array
    SetPixelPitch();
    SetFocalLength();
//    SetRowOffset();

//  qDebug()<<"NaifId = "<<naifIkCode()<<"  FocalLength = "<<FocalLength()<<"  PixelPitch = "<<PixelPitch();

    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString stime = inst["SpacecraftClockStartCount"];
//  stime = "1/0034948318:06600"; // Jupiter Spacecraft clock timestamp (SPCSCLK0)
//  stime = "1/0034948341:03300"; // Jupiter mid-observ
//  stime = "1/0034829038:06600"; // Io  Spacecraft clock timestamp (SPCSCLK0)
//  stime = "1/0034829043:28300"; // Io mid-observ
//  qDebug()<<"sclk = "<<stime;

    m_etStart = getClockTime(stime).Et();
//  qDebug()<<"et = "<<QString::number(m_etStart, 'f', 12);
//  iTime time(m_etStart);
//  qDebug()<<"utc = "<<time.UTC();
//  SpiceChar jd[30];
//  et2utc_c(m_etStart, "J", 7, 30, jd);
//  qDebug()<<"jd = "<<jd;

//  m_lineRate = (double)inst["ExposureDuration"];
//  m_lineRate = (double)inst["ExposureDuration"] / 32.0;
    m_lineRate = 1.0 / (double)inst["TdiRate"];
//  qDebug()<<"Line rate = "<<m_lineRate;

    // The detector map tells us how to convert from image coordinates to
    // detector coordinates.  In our case, a (sample,line) to a (sample,time)
    new LineScanCameraDetectorMap(this, m_etStart, m_lineRate);

    // The focal plane map tells us how to go from detector position
    // to focal plane x/y (distorted).  That is, (sample,time) to (x,y).
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());
//    focalMap->SetDetectorOrigin(2512.5, 0.5);
    focalMap->SetDetectorOrigin(2500.5, -16.5); // NOTE: TRACIE says this works...

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

    new MvicTdiCameraDistortionMap(this, distCoefX, distCoefY, residualColumnDistCoefs,
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
 * MvicTdiCamera object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* MvicTdiCamera
 * @internal
 */
extern "C" Isis::Camera *MvicTdiCameraPlugin(Isis::Cube &cube) {
  return new Isis::MvicTdiCamera(cube);
}
