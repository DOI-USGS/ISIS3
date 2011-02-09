#include "DawnFcCamera.h"
#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "RadialDistortionMap.h"
#include "iString.h"
#include "iTime.h"

using namespace std;
using namespace Isis;
namespace Dawn {
  DawnFcCamera::DawnFcCamera(Pvl &lab) : FramingCamera(lab) {
    PvlGroup bandBin = lab.FindGroup("BandBin", Pvl::Traverse);

    // Get the camera characteristics
    iString key = string("INS" + (iString)(int)NaifIkCode() + "_F") +
                  (string)bandBin["FilterNumber"] + "_FOCAL_LENGTH";
    key = key.Convert("/", '_');
    double focalLength = Spice::GetDouble(key);
    SetFocalLength(focalLength);
    SetPixelPitch();

    // Get the start time in et
    PvlGroup inst = lab.FindGroup("Instrument", Pvl::Traverse);
    string stime = inst["StartTime"];
    double et;
    str2et_c(stime.c_str(), &et);
    double exposureDuration = (double)inst["ExposureDuration"] / 1000.0;
    et += exposureDuration / 2.0;

    // Setup detector map
    CameraDetectorMap *detectorMap = new CameraDetectorMap(this);
    detectorMap->SetDetectorLineSumming(1);
    detectorMap->SetDetectorSampleSumming(1);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, NaifIkCode());

    focalMap->SetDetectorOrigin(Spice::GetDouble("INS" + (iString)(int)NaifIkCode() + "_BORESIGHT_SAMPLE"),
                                Spice::GetDouble("INS" + (iString)(int)NaifIkCode() + "_BORESIGHT_LINE"));

    // Setup distortion map
    new CameraDistortionMap(this);

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    SetTime(et);
    LoadCache();
  }
}

extern "C" Camera *DawnFcCameraPlugin(Pvl &lab) {
  return new Dawn::DawnFcCamera(lab);
}
