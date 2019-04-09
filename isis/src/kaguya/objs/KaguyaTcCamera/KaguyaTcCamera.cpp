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
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
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

    // set variables startTime and exposureDuration
    double time = iTime((QString)inst["StartTime"]).Et();

    // divide exposure duration keyword value by 1000 to convert to seconds
    // needed? 
    double exposureDuration = ((double) inst["ExposureDuration"]) / 1000.0;

    // Setup detector map
    double lineRate = (double) inst["LineSamplingInterval"] / 1000.0;

    new LineScanCameraDetectorMap(this, time, lineRate);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());
//    focalMap->SetDetectorOrigin( Samples() / 2.0 + 0.5, 1.0); //Lines() / 2.0 + 0.5);
    focalMap->SetDetectorOrigin( Samples() / 2.0 + 0.5, Lines() / 2.0 + 0.5);
/*
    focalMap->SetDetectorOrigin(
      Spice::getDouble("INS" + toString(naifIkCode()) +
                       "_BORESIGHT_SAMPLE"),
      Spice::getDouble("INS" + toString(naifIkCode()) +
                       "_BORESIGHT_LINE"));
*/
    // Setup distortion map
    new KaguyaTcCameraDistortionMap(this, naifIkCode());

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

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
