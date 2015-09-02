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

#include "NewHorizonsMvicFrameCamera.h"

#include <QDebug>
#include <QString>

#include "Camera.h"
#include "CameraDetectorMap.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "IString.h"
#include "iTime.h"
#include "NewHorizonsMvicFrameCameraDistortionMap.h"
#include "NaifStatus.h"

using namespace std;

namespace Isis {

  /**
   * Constructs a New Horizons MVIC Framing Camera object. The MVIC push-frame camera operates 
   * in "staring" mode, so it has been implemented as a framing camera rather than a push-frame.
   * The test images show the same part of the planet in each framelet, so the push-frame 
   * implementation will not work since the same lat/lon values are located in possibly every 
   * framelet. 
   *
   * @param lab Pvl label from a New Horizons MVIC Framing Camera image.
   *
   * @author Tracie Sucharski
   * @internal
   */
  NewHorizonsMvicFrameCamera::NewHorizonsMvicFrameCamera(Cube &cube) : FramingCamera(cube) {
    m_instrumentNameLong = "Multispectral Visible Imaging Framing Camera";
    m_instrumentNameShort = "MVIC FRAMING";
    m_spacecraftNameLong = "New Horizons";
    m_spacecraftNameShort = "NewHorizons";
    
    NaifStatus::CheckErrors();

    SetFocalLength();
    SetPixelPitch();

    // Get the start time from labels
    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    m_exposure = inst["ExposureDuration"];
    QString stime = inst["SpacecraftClockStartCount"];
    // **  TODO  **  Need an offset time added to labels at ingestion??  The 0.125 value is 
    //     the value in DELTAT00.
    double offset = 0.125;
    m_etStart = getClockTime(stime).Et() + offset;
    SpiceChar utc[30];
    et2utc_c(m_etStart, "ISOC", 3, 30, utc);
//  qDebug()<<"\n\nspacecraftClockStartCount + "<<offset<<" (offset) = "<<utc;

    // If bands have been extracted from the original image then we
    // need to read the band bin group so we can map from the cube band
    // number to the instrument band number.  Also save times of each framelet which are stored
    // in the BandBin group.
    PvlGroup &bandBin = lab.findGroup("BandBin", Pvl::Traverse);
    PvlKeyword &origBand = bandBin["OriginalBand"];
    PvlKeyword &utcTime = bandBin["UtcTime"];
    for(int i = 0; i < origBand.size(); i++) {
      m_originalBand.push_back(toInt(origBand[i]));
      m_utcTime.push_back(utcTime[i]);
    }

    CameraDetectorMap *detectorMap = new CameraDetectorMap(this);
    detectorMap->SetDetectorSampleSumming(1);
    detectorMap->SetDetectorLineSumming(1);

    // Setup focal plane map. The class will read data from the instrument addendum kernel to pull
    // out the affine transforms from detector samp,line to focal plane x,y.
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());
    focalMap->SetDetectorOrigin(2500.5, 64.5);

    // Read distortion coefficients and boresight offsets from the instrument kernels. Then
    // construct the distortion map.
    //read the distortion coefs from the NAIF Kernels
    QString naifXKey = "INS-98900_DISTORTION_COEF_X";
    QString naifYKey = "INS-98900_DISTORTION_COEF_Y";
    QString naifppKey = "INS-98900_PP_OFFSET";
    vector<double> distCoefX;
    vector<double> distCoefY;

    for (int i=0; i < 20; i++) {
      distCoefX.push_back(getDouble(naifXKey,i));
      distCoefY.push_back(getDouble(naifYKey,i));
    }

    new NewHorizonsMvicFrameCameraDistortionMap(this, distCoefX, distCoefY);

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    // Internalize all the NAIF SPICE information into memory.
    LoadCache();
    NaifStatus::CheckErrors();
  }


  /**
   * Sets the band in the camera model
   *
   * @param band The band number to set
   */
  void NewHorizonsMvicFrameCamera::SetBand(const int vband) {

    if(vband > (int) m_originalBand.size()) {
      QString msg = QObject::tr("Band number out of array bounds in NewHorizonsMvicFrameCamera::SetBand legal "
                                "bands are [1-%1], input was [%2]").
                    arg(m_originalBand.size()).arg(vband);
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    iTime time(m_utcTime[vband-1]);
    double et = time.Et();
   
    SpiceChar utc[30];
    et2utc_c(et, "ISOC", 3, 30, utc);
    Camera::setTime(et);
    pair<iTime, iTime> shuttertimes = ShutterOpenCloseTimes(et, m_exposure);

    //  Set up valid band access
    Camera::SetBand(vband);
    
  }


  /**
   * Returns the shutter open and close times. The user should pass in the
   * ExposureDuration keyword value and the StartTime keyword value, converted
   * to ephemeris time. The  StartTime keyword value from the labels represents
   * the shutter center time of the observation. To find the shutter open and
   * close times, half of the exposure duration is subtracted from and added to
   * the input time parameter, respectively. This method overrides the
   * FramingCamera class method.
   *
   * @param exposureDuration ExposureDuration keyword value from the labels, in
   *                         seconds.
   * @param time The StartTime keyword value from the labels, converted to
   *             ephemeris time.
   *
   * @return @b pair < @b iTime, @b iTime > The first value is the shutter
   *         open time and the second is the shutter close time.
   *
   * @author 2014-01-01 Tracie Sucharski
   */
  pair<iTime, iTime> NewHorizonsMvicFrameCamera::ShutterOpenCloseTimes(double time, double exposureDuration) {

    return FramingCamera::ShutterOpenCloseTimes(time, exposureDuration);
  }
  
  
  /**
   * This method returns the full instrument name.
   *
   * @return QString
   */
  QString NewHorizonsMvicFrameCamera::instrumentNameLong() const {
    return m_instrumentNameLong;
  }
  
  
  /**
   * This method returns the shortened instrument name.
   *
   * @return QString
   */
  QString NewHorizonsMvicFrameCamera::instrumentNameShort() const {
    return m_instrumentNameShort;
  }
  
  
  /**
   * This method returns the full spacecraft name.
   * 
   * @return QString
   */
  QString NewHorizonsMvicFrameCamera::spacecraftNameLong() const {
    return m_spacecraftNameLong;
  }
  
  
  /**
   * This method returns the shortened spacecraft name.
   *
   * @return QString
   */
  QString NewHorizonsMvicFrameCamera::spacecraftNameShort() const {
    return m_spacecraftNameShort;
  }
}


/**
 * This is the function that is called in order to instantiate a NewHorizonsMvicFrameCamera
 * object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* NewHorizonsMvicFrameCamera
 * @internal
 */
extern "C" Isis::Camera *NewHorizonsMvicFrameCameraPlugin(Isis::Cube &cube) {
  return new Isis::NewHorizonsMvicFrameCamera(cube);
}
