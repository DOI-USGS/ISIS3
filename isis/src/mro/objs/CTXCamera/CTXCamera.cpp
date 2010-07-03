#include "CTXCamera.h"
#include "iString.h"
#include "iException.h"
#include "LineScanCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"

using namespace std;
namespace Isis {
  namespace Mro {
    // constructors
    CTXCamera::CTXCamera (Isis::Pvl &lab) : Isis::LineScanCamera(lab) {
      // Set up the camera info from ik/iak kernels
      SetFocalLength();
      SetPixelPitch();

      // Get the start time from labels
      Isis::PvlGroup &inst = lab.FindGroup ("Instrument",Isis::Pvl::Traverse);
      string stime = inst["SpacecraftClockCount"];
      SpiceDouble etStart;
      scs2e_c (NaifSpkCode(),stime.c_str(),&etStart);

      // Get other info from labels
      double csum = inst["SpatialSumming"];
      double lineRate = (double) inst["LineExposureDuration"] / 1000.0;
//      lineRate *= csum;
      double ss = inst["SampleFirstPixel"];
      ss += 1.0;

      // Setup detector map
      LineScanCameraDetectorMap *detectorMap =
        new LineScanCameraDetectorMap(this,etStart,lineRate);
      detectorMap->SetDetectorSampleSumming(csum);
      detectorMap->SetStartingDetectorSample(ss);

      // Setup focal plane map
      CameraFocalPlaneMap *focalMap =new CameraFocalPlaneMap(this,NaifIkCode());

      //  Retrieve boresight location from instrument kernel (IK) (addendum?)
      iString ikernKey = "INS" + iString((int)NaifIkCode()) + "_BORESIGHT_SAMPLE";
      double sampleBoreSight = GetDouble(ikernKey);

      ikernKey = "INS" + iString((int)NaifIkCode()) + "_BORESIGHT_LINE";
      double lineBoreSight = GetDouble(ikernKey);

      focalMap->SetDetectorOrigin(sampleBoreSight, lineBoreSight);
      focalMap->SetDetectorOffset(0.0,0.0);

      // Setup distortion map
      CameraDistortionMap *distMap = new CameraDistortionMap(this);
      distMap->SetDistortion(NaifIkCode());


      // Setup the ground and sky map
      new LineScanCameraGroundMap(this);
      new LineScanCameraSkyMap(this);

      LoadCache();
    }
  }
}

extern "C" Isis::Camera *CTXCameraPlugin (Isis::Pvl &lab) {
  return new Isis::Mro::CTXCamera(lab);
}
