#include "MocWideAngleCamera.h"
#include "MocWideAngleDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "MocWideAngleDistortionMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "MocLabels.h"

using namespace std;
namespace Isis {
  namespace Mgs {
    // constructors
    /** 
     * @internal 
     *   @history 2008-11-05 Jeannie Walldren - Replaced reference
     *          to MocLabels IsWideAngleRed() with MocLabels
     *          WideAngleRed().
     */
    MocWideAngleCamera::MocWideAngleCamera (Isis::Pvl &lab) : LineScanCamera(lab) {
      // See if we have a moc camera
      MocLabels *moclab = new Isis::Mgs::MocLabels(lab);
      double lineRate = moclab->LineRate();
      double csum = moclab->CrosstrackSumming();
      double dsum = moclab->DowntrackSumming();
      double ss = moclab->FirstLineSample();
      bool isRed = moclab->WideAngleRed();

      // Set up the camera info from ik/iak kernels
      // LoadEulerMounting();
      SetFocalLength();
      SetPixelPitch();

      if ( PixelPitch() == 1) {
        throw iException::Message(iException::User,
              "Cube file needs to be spiceinit'd with updated iak",_FILEINFO_);
      }
      InstrumentRotation()->SetTimeBias(-1.15);

      // Get the start time from labels
      Isis::PvlGroup &inst = lab.FindGroup ("Instrument",Isis::Pvl::Traverse);
      string stime = inst["SpacecraftClockCount"];
      SpiceDouble etStart;
      scs2e_c (NaifSpkCode(),stime.c_str(),&etStart);

      // Setup detector map
      MocWideAngleDetectorMap *detectorMap =
        new MocWideAngleDetectorMap(this,etStart,lineRate,moclab);
      detectorMap->SetDetectorSampleSumming(csum);
      detectorMap->SetDetectorLineSumming(dsum);
      detectorMap->SetStartingDetectorSample(ss);

      // Setup focal plane map
      CameraFocalPlaneMap *focalMap =
        new CameraFocalPlaneMap(this,NaifIkCode());
      if (isRed) {
        focalMap->SetDetectorOrigin(1674.65,0.0);
        focalMap->SetDetectorOffset(0.0,6.7785);
      }
      else {
        focalMap->SetDetectorOrigin(1688.58,0.0);
        focalMap->SetDetectorOffset(0.0,-0.8486);
      }

      // Setup distortion map
      new MocWideAngleDistortionMap(this,isRed);

      // Setup the ground and sky map
      new LineScanCameraGroundMap(this);
      new LineScanCameraSkyMap(this);

      LoadCache();
    }
  }
}

extern "C" Isis::Camera *MocWideAngleCameraPlugin (Isis::Pvl &lab) {
  return new Isis::Mgs::MocWideAngleCamera(lab);
}
