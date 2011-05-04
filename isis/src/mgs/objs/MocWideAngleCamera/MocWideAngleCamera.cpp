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
#include "MocWideAngleCamera.h"
#include "MocWideAngleDetectorMap.h"
#include "MocWideAngleDistortionMap.h"
#include "MocLabels.h"

#include "CameraFocalPlaneMap.h"
#include "iException.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "NaifStatus.h"

using namespace std;
namespace Isis {
  // constructors
  /**
   * Constructor for the Mgs MOC Wide Angle Camera Model
   *
   * @param lab Pvl label from an MOC WAC image. 
   *
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Replaced reference
   *          to MocLabels IsWideAngleRed() with MocLabels
   *          WideAngleRed().
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check.
   */
  MocWideAngleCamera::MocWideAngleCamera(Pvl &lab) : LineScanCamera(lab) {
    NaifStatus::CheckErrors();
    // See if we have a moc camera
    MocLabels *moclab = new MocLabels(lab);
    double lineRate = moclab->LineRate();
    double csum = moclab->CrosstrackSumming();
    double dsum = moclab->DowntrackSumming();
    double ss = moclab->FirstLineSample();
    bool isRed = moclab->WideAngleRed();

    // Set up the camera info from ik/iak kernels
    // LoadEulerMounting();
    SetFocalLength();
    SetPixelPitch();

    if(PixelPitch() == 1) {
      throw iException::Message(iException::User,
                                "Cube file needs to be spiceinit'd with updated iak", _FILEINFO_);
    }
    InstrumentRotation()->SetTimeBias(-1.15);

    // Get the start time from labels
    PvlGroup &inst = lab.FindGroup("Instrument", Pvl::Traverse);
    string stime = inst["SpacecraftClockCount"];
    SpiceDouble etStart;
    scs2e_c(NaifSpkCode(), stime.c_str(), &etStart);

    // Setup detector map
    MocWideAngleDetectorMap *detectorMap =
      new MocWideAngleDetectorMap(this, etStart, lineRate, moclab);
    detectorMap->SetDetectorSampleSumming(csum);
    detectorMap->SetDetectorLineSumming(dsum);
    detectorMap->SetStartingDetectorSample(ss);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap =
      new CameraFocalPlaneMap(this, NaifIkCode());
    if(isRed) {
      focalMap->SetDetectorOrigin(1674.65, 0.0);
      focalMap->SetDetectorOffset(0.0, 6.7785);
    }
    else {
      focalMap->SetDetectorOrigin(1688.58, 0.0);
      focalMap->SetDetectorOffset(0.0, -0.8486);
    }

    // Setup distortion map
    new MocWideAngleDistortionMap(this, isRed);

    // Setup the ground and sky map
    new LineScanCameraGroundMap(this);
    new LineScanCameraSkyMap(this);

    LoadCache();
    NaifStatus::CheckErrors();
  }
}

/**
 * This is the function that is called in order to instantiate a 
 * MocWideAngleCamera object. 
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* MocWideAngleCamera
 * @internal
 *   @history 2011-05-03 Jeannie Walldren - Removed Mgs namespace.
 */
extern "C" Isis::Camera *MocWideAngleCameraPlugin(Isis::Pvl &lab) {
  return new Isis::MocWideAngleCamera(lab);
}
