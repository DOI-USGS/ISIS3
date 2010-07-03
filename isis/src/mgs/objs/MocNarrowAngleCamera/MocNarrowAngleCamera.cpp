#include "MocNarrowAngleCamera.h"
#include "iString.h"
#include "iException.h"
#include "LineScanCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"

using namespace std;
namespace Isis {
  namespace Mgs {
    // constructors
    MocNarrowAngleCamera::MocNarrowAngleCamera (Isis::Pvl &lab) : Isis::LineScanCamera(lab) {
      // Set up the camera info from ik/iak kernels
//      LoadEulerMounting();
      SetFocalLength();
      SetPixelPitch();
      InstrumentRotation()->SetTimeBias(-1.15);

      // Get the start time from labels
      Isis::PvlGroup &inst = lab.FindGroup ("Instrument",Isis::Pvl::Traverse);
      string stime = inst["SpacecraftClockCount"];
      SpiceDouble etStart;
      scs2e_c (NaifSpkCode(),stime.c_str(),&etStart);

      // Get other info from labels
      double csum = inst["CrosstrackSumming"];
      double dsum = inst["DowntrackSumming"];
      double lineRate = (double) inst["LineExposureDuration"] / 1000.0;
      lineRate *= dsum;
      double ss = inst["FirstLineSample"];

      // Setup detector map
      LineScanCameraDetectorMap *detectorMap =
        new LineScanCameraDetectorMap(this,etStart,lineRate);
      detectorMap->SetDetectorSampleSumming(csum);
      detectorMap->SetDetectorLineSumming(dsum);
      detectorMap->SetStartingDetectorSample(ss);

      // Setup focal plane map
      CameraFocalPlaneMap *focalMap =
        new CameraFocalPlaneMap(this,NaifIkCode());
      focalMap->SetDetectorOrigin(1024.5,0.0);
      focalMap->SetDetectorOffset(0.0,0.0);

      // Setup distortion map
      new CameraDistortionMap(this);

      // Setup the ground and sky map
      new LineScanCameraGroundMap(this);
      new LineScanCameraSkyMap(this);

      LoadCache();
    }
  }
}

extern "C" Isis::Camera *MocNarrowAngleCameraPlugin (Isis::Pvl &lab) {
  return new Isis::Mgs::MocNarrowAngleCamera(lab);
}
