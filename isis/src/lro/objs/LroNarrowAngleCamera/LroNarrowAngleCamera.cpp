#include "LroNarrowAngleCamera.h"
#include "iString.h"
#include "iException.h"
#include "LineScanCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "LroNarrowAngleDistortionMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"

using namespace std;
namespace Isis {
  namespace Lro {
    // constructors
    LroNarrowAngleCamera::LroNarrowAngleCamera(Isis::Pvl &lab) : Isis::LineScanCamera(lab) {
      // Set up the camera info from ik/iak kernels
      SetFocalLength();
      SetPixelPitch();

      double constantTimeOffset = 0.0,
             additionalPreroll = 0.0,
             additiveLineTimeError = 0.0,
             multiplicativeLineTimeError = 0.0;

      iString ikernKey = "INS" + iString((int)NaifIkCode()) + "_CONSTANT_TIME_OFFSET";
      constantTimeOffset = GetDouble(ikernKey);

      ikernKey = "INS" + iString((int)NaifIkCode()) + "_ADDITIONAL_PREROLL";
      additionalPreroll = GetDouble(ikernKey);

      ikernKey = "INS" + iString((int)NaifIkCode()) + "_ADDITIVE_LINE_ERROR";
      additiveLineTimeError = GetDouble(ikernKey);

      ikernKey = "INS" + iString((int)NaifIkCode()) + "_MULTIPLI_LINE_ERROR";
      multiplicativeLineTimeError = GetDouble(ikernKey);

      // Get the start time from labels
      Isis::PvlGroup &inst = lab.FindGroup("Instrument", Isis::Pvl::Traverse);
      iString stime = (string)inst["SpacecraftClockPrerollCount"];
      SpiceDouble etStart;
      if(stime != "NULL") {
        scs2e_c(NaifSclkCode(), stime.c_str(), &etStart);
      }
      else {
        stime = (string)inst["PrerollTime"];
        str2et_c(stime.c_str(), &etStart);
      }

      // Get other info from labels
      double csum = inst["SpatialSumming"];
      double lineRate = (double) inst["LineExposureDuration"] / 1000.0;
      double ss = inst["SampleFirstPixel"];
      ss += 1.0;

      lineRate *= 1.0 + multiplicativeLineTimeError;
      lineRate += additiveLineTimeError;
      etStart += additionalPreroll * lineRate;
      etStart += constantTimeOffset;

      SetEphemerisTime(etStart);

      // Setup detector map
      LineScanCameraDetectorMap *detectorMap = new LineScanCameraDetectorMap(this, etStart, lineRate);
      detectorMap->SetDetectorSampleSumming(csum);
      detectorMap->SetStartingDetectorSample(ss);

      // Setup focal plane map
      CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, NaifIkCode());

      //  Retrieve boresight location from instrument kernel (IK) (addendum?)
      ikernKey = "INS" + iString((int)NaifIkCode()) + "_BORESIGHT_SAMPLE";
      double sampleBoreSight = GetDouble(ikernKey);

      ikernKey = "INS" + iString((int)NaifIkCode()) + "_BORESIGHT_LINE";
      double lineBoreSight = GetDouble(ikernKey);

      focalMap->SetDetectorOrigin(sampleBoreSight, lineBoreSight);
      focalMap->SetDetectorOffset(0.0, 0.0);

      // Setup distortion map
      LroNarrowAngleDistortionMap *distMap = new LroNarrowAngleDistortionMap(this);
      distMap->SetDistortion(NaifIkCode());

      // Setup the ground and sky map
      new LineScanCameraGroundMap(this);
      new LineScanCameraSkyMap(this);

      LoadCache();
    }
  }
}

extern "C" Isis::Camera *LroNarrowAngleCameraPlugin(Isis::Pvl &lab) {
  return new Isis::Lro::LroNarrowAngleCamera(lab);
}
