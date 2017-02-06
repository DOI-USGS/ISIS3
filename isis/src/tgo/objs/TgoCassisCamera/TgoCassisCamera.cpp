/**
 * @file
 * $Revision:$
 * $Date:$
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

#include <cmath>

#include <QByteArray>
#include <QString>
#include <QVariant>

#include "TgoCassisCamera.h"
#include "CameraDetectorMap.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "EndianSwapper.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "NaifStatus.h"

using namespace std;

namespace Isis {
  /**
   * @brief Initialize the CaSSIS camera model
   *
   *
   * @param lab Pvl label from a TGO Cassis image.
   */
  TgoCassisCamera::TgoCassisCamera(Cube &cube) : FramingCamera(cube) {

    m_instrumentNameLong = "Colour and Stereo Surface Imaging System";
    m_instrumentNameShort = "CaSSIS";

    m_spacecraftNameLong = "Trace Gas Orbiter";
    m_spacecraftNameShort = "TGO";
    
    NaifStatus::CheckErrors();

    // CaSSIS codes
    int cassisCode = naifIkCode();
    QString cassis = toString(cassisCode);

    // Get all the necessary stuff from the labels
    Pvl &lab = *cube.label();
    const PvlGroup &inst    = lab.findGroup("Instrument", Pvl::Traverse);
    const PvlGroup &bandBin = lab.findGroup("BandBin", Pvl::Traverse);

    // Filter codes
    PvlKeyword filtNames     = bandBin["FilterName"];
    PvlKeyword filterIkCodes = bandBin["NaifIkCode"];

    QString filter     = filterIkCodes[0];

    // Set up the camera characteristics
    instrumentRotation()->SetFrame( CkFrameId() );
    SetFocalLength();
    SetPixelPitch();

    // Get the Start time from the labels
    // TODO: This is currently using UTC time. Once the timestamp is figured out,
    //       this will change to use SCLK. JAM 2017-02-06
    QString stime = inst["SpacecraftClockStartCount"];
    QString startT = inst["StartTime"];
    iTime et(startT);

    // Get summing mode
    // Summing modes are:
    //   0 = 1x1 (No summing)
    //   1 = 2x2
    //   2 = 4x4
    int sumMode = toInt(inst["SummingMode"][0]);
    int summing = sumMode * 2;

    //  Setup camera detector map
    CameraDetectorMap *detMap = new CameraDetectorMap(this);
    if ( summing > 0 ) {
      detMap->SetDetectorSampleSumming(summing);
      detMap->SetDetectorLineSumming(summing);
    }
    
    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, cassisCode);

    // Get CASSIS detector boresight
    double bsSample = getDouble("INS" + cassis + "_BORESIGHT_SAMPLE");
    double bsLine = getDouble("INS" + cassis + "_BORESIGHT_LINE");
    focalMap->SetDetectorOrigin(bsSample, bsLine);

    // Set starting filter location on the detector
    detMap->SetStartingDetectorLine(getDouble("INS" + filter + "_FILTER_OFFSET"));

    // Setup distortion map
    new CameraDistortionMap(this);

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    // Set start time to center of exposure time to ensure
    // the proper SPICE data is cached.
    double p_exposureDur = toDouble(inst["ExposureDuration"]);
    iTime p_etStart = et + ( p_exposureDur / 2.0);

    setTime(p_etStart);
    LoadCache();
    NaifStatus::CheckErrors();
  }


  /**
   * Destroys the TgoCassisCamera object.
   */
  TgoCassisCamera::~TgoCassisCamera() {
  }


  /**
   * Returns the shutter open and close times.  The user should pass in the
   * ExposureDuration keyword value, converted from milliseconds to seconds, and
   * the SpacecraftClockCount keyword value, converted to ephemeris time. The 
   * StartTime keyword value from the labels represents the shutter open time of 
   * the observation. This method uses the FramingCamera class implementation, 
   * returning the given time value as the shutter open and the sum of the time 
   * value and exposure duration as the shutter close. 
   *
   * @param exposureDuration Exposure duration value from the labels, converted
   *                         to seconds.
   * @param time The SpacecraftClockCount value from the labels, converted to 
   *             ephemeris time
   * @return @b pair < @b iTime, @b iTime > The first value is the shutter
   *         open time and the second is the shutter close time.
   *
   * @see
   * @author 2017-02-01 Kris Becker
   * @internal
   */
  pair <iTime, iTime> TgoCassisCamera::ShutterOpenCloseTimes(double time,
                                                             double exposureDuration) {
    return FramingCamera::ShutterOpenCloseTimes(time, exposureDuration);
  }


  /**
   * CK frame ID -  TGO CaSSIS instrument code (TGO_CASSIS_FSA)
   *  
   * @return @b int The appropriate instrument code for the "Camera-matrix" 
   *                Kernel Frame ID.
   */
  int TgoCassisCamera::CkFrameId() const {
    return (-143420);
  }


  /** 
    * CK Reference ID - J2000
    * 
    * @return @b int The appropriate instrument code for the "Camera-matrix"
    *                Kernel Reference ID.
    */
  int TgoCassisCamera::CkReferenceId() const {
    return (1);
  }


  /**
    * SPK Target Body ID - TGO spacecraft -143
    *
    * @return @b int The appropriate instrument code for the Spacecraft
    *                Kernel Target ID.
    */
  int TgoCassisCamera::SpkTargetId() const {
    return (-143);
  }


  /** 
    *  SPK Reference ID - J2000
    *  
    * @return @b int The appropriate instrument code for the Spacecraft 
    *                Kernel Reference ID.
    */
  int TgoCassisCamera::SpkReferenceId() const {
    return (1);
  }

}

/**
 * This is the function that is called in order to instantiate a TgoCassisCamera
 * object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* TgoCassisCamera
 */
extern "C" Isis::Camera *TgoCassisCameraPlugin(Isis::Cube &cube) {
  return new Isis::TgoCassisCamera(cube);
}
