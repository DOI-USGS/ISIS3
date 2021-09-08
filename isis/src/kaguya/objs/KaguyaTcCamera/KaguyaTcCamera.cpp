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
#include "KaguyaTcCamera.h"
#include "KaguyaTcCameraDistortionMap.h"

#include <QString>

#include "LineScanCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "NaifStatus.h"

using namespace std;
namespace Isis {
  /**
   * Constructor for the Kaguya TC Camera Model
   *
   * @param lab Pvl Label to create the camera model from
   *
   * @internal
   *   @history 2018-10-02 Adam Goins & Jeannie Backer - Original Version
   */
  KaguyaTcCamera::KaguyaTcCamera(Cube &cube) : LineScanCamera(cube) {
    m_instrumentNameLong  = "Terrain Camera";
    m_instrumentNameShort = "TC";
    m_spacecraftNameLong  = "Kaguya";
    m_spacecraftNameShort = "Kaguya";

    NaifStatus::CheckErrors();
    // Get the camera characteristics
    SetFocalLength();
    SetPixelPitch();

    // Get the start time in et
    Pvl &lab = *cube.label();
    PvlGroup inst = lab.findGroup("Instrument", Pvl::Traverse);

    QString clockCount = inst["SpacecraftClockStartCount"];
    double time = getClockTime(clockCount, -1, true).Et();

    // Setup detector map
    double lineRate = (double) inst["LineSamplingInterval"] / 1000.0;
    
    // Convert between parent image coordinates and detector coordinates (detector coordinate line, detector coordinate sample)   
    LineScanCameraDetectorMap *detectorMap = new LineScanCameraDetectorMap(this, time, lineRate);

    // Detetermine what to set the starting detector sample to, based on swath mode
    QString swathMode = inst["SwathModeId"];

    double startingDetectorSample = 1; 
    if (swathMode.compare("FULL") == 0) {
      startingDetectorSample = 1; 
    }
    else if (swathMode.compare("NOMINAL") == 0) {
      startingDetectorSample = 297; 
    }
    else if (swathMode.compare("HALF") == 0) {
      startingDetectorSample = 1172;
    }

    detectorMap->SetStartingDetectorSample(startingDetectorSample);
    
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    // This sets the origin of the detector (not image samp,line). It is zero bassed.
    // The detector offsets are 0,0 because the borsight is in the center of the array
    // The origin of the detector does not depend on swath mode. 
    QString key;
    key = "INS" + toString(naifIkCode()) + "_BORESIGHT_SAMPLE";
    double sampleBoreSight = getDouble(key);

    key = "INS" + toString(naifIkCode()) + "_BORESIGHT_LINE";
    double lineBoreSight = getDouble(key);
    focalMap->SetDetectorOrigin(sampleBoreSight, lineBoreSight); 

    // Setup distortion map
    new KaguyaTcCameraDistortionMap(this, naifIkCode());

    // Setup the ground and sky map
    new LineScanCameraGroundMap(this);
    new LineScanCameraSkyMap(this);

    setTime(time);
    LoadCache();
    NaifStatus::CheckErrors();
  }

  //! Destroys the KaguyaTcCamera object.
  KaguyaTcCamera::~KaguyaTcCamera() {
  }


  /**
   * CK frame ID -  - Instrument Code from spacit run on CK
   *
   * @return @b int The appropriate instrument code for the "Camera-matrix"
   *         Kernel Frame ID
   */
  int KaguyaTcCamera::CkFrameId() const {
     return (-131000);
  }

  /**
   * CK Reference ID - J2000
   *
   * @return @b int The appropriate instrument code for the "Camera-matrix"
   *         Kernel Reference ID
   */
  int KaguyaTcCamera::CkReferenceId() const {
    return (1);
  }

  /**
   * SPK Reference ID - J2000
   *
   * @return @b int The appropriate instrument code for the Spacecraft
   *         Kernel Reference ID
   */
  int KaguyaTcCamera::SpkReferenceId() const {
    return (1);
  }

}


/**
 * This is the function that is called in order to instantiate a KaguyaCamera
 * object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* Kaguya
 */
extern "C" Isis::Camera *KaguyaTcCameraPlugin(Isis::Cube &cube) {
  return new Isis::KaguyaTcCamera(cube);
}
