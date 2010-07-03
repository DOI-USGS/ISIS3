#include "SsiCamera.h"
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
namespace Galileo {
  SsiCamera::SsiCamera (Pvl &lab) : FramingCamera(lab) {
    // Get the camera characteristics
    double k1;

    iTime removeCoverDate("1994/04/01 00:00:00");
    iTime imageDate(lab.FindKeyword("StartTime",Isis::PvlObject::Traverse)[0]);
    /*
    * Change the Focal Length and K1 constant based on whether or not the protective cover is on 
    * See "The Direction of the North Pole and the Control Network of Asteroid 951 Gaspra"  Icarus 107, 18-22 (1994) 
    */
    if(imageDate < removeCoverDate) {
      int code = NaifIkCode();
      string key = "INS" + Isis::iString(code) + "_FOCAL_LENGTH_COVER";
      SetFocalLength(Isis::Spice::GetDouble(key));
      k1 = Spice::GetDouble("INS" + (iString)(int)NaifIkCode() + "_K1_COVER");
    }
    else {
      SetFocalLength ();
      k1 = Spice::GetDouble("INS" + (iString)(int)NaifIkCode() + "_K1");
    }

    SetPixelPitch ();

    // Get the start time in et
    PvlGroup inst = lab.FindGroup ("Instrument",Pvl::Traverse);
    string stime = inst["StartTime"];
    double et; 
    str2et_c(stime.c_str(),&et);

    // Get summation mode
    double sumMode = inst["Summing"];

    // Setup detector map
    CameraDetectorMap *detectorMap = new CameraDetectorMap(this);
    detectorMap->SetDetectorSampleSumming(sumMode);
    detectorMap->SetDetectorLineSumming(sumMode);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this,NaifIkCode());

    focalMap->SetDetectorOrigin (Spice::GetDouble("INS" + (iString)(int)NaifIkCode() + "_BORESIGHT_SAMPLE"), 
                                 Spice::GetDouble("INS" + (iString)(int)NaifIkCode() + "_BORESIGHT_LINE"));

    // Setup distortion map
    new RadialDistortionMap(this, k1);

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);
  
    SetEphemerisTime(et);
    LoadCache();
  }
}

extern "C" Camera *SsiCameraPlugin(Pvl &lab) {
  return new Galileo::SsiCamera(lab);
}
