// $Id: IssNACamera.cpp,v 1.6 2009/08/31 15:12:29 slambright Exp $
#include "IssNACamera.h"
#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "RadialDistortionMap.h"
#include "iString.h"

using namespace std;
using namespace Isis;
namespace Cassini {
  IssNACamera::IssNACamera (Pvl &lab) : FramingCamera(lab) {
    PvlGroup bandBin = lab.FindGroup ("BandBin",Pvl::Traverse);
    // Get the camera characteristics
    iString key = string("INS"+(iString)(int)NaifIkCode()+"_")+ (string)bandBin["FilterName"] + "_FOCAL_LENGTH";
    key = key.Convert("/",'_');
    double focalLength = Spice::GetDouble(key);
    
    SetFocalLength (focalLength);
    SetPixelPitch ();
    InstrumentRotation()->SetFrame(Spice::GetInteger("INS_"+(iString)(int)NaifIkCode()+"_FRAME_ID"));

    // Get the start time in et
    PvlGroup inst = lab.FindGroup ("Instrument",Pvl::Traverse);
    string stime = inst["StartTime"];
    double et; 
    str2et_c(stime.c_str(),&et);
    double exposureDuration = (double)inst["ExposureDuration"] /1000.0;
    et += exposureDuration / 2.0;

    // Setup detector map
    int summingMode = inst["SummingMode"];
    CameraDetectorMap *detectorMap = new CameraDetectorMap(this);
    detectorMap->SetDetectorLineSumming(summingMode);
    detectorMap->SetDetectorSampleSumming(summingMode);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this,NaifIkCode());

    focalMap->SetDetectorOrigin (Spice::GetDouble("INS" + (iString)(int)NaifIkCode() + "_BORESIGHT_SAMPLE"), 
                                 Spice::GetDouble("INS" + (iString)(int)NaifIkCode() + "_BORESIGHT_LINE"));

    // Setup distortion map
    double k1 = Spice::GetDouble("INS" + (iString)(int)NaifIkCode() + "_K1");
    new RadialDistortionMap(this, k1);

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    SetEphemerisTime(et);
    LoadCache();
  }
}

extern "C" Camera *IssNACameraPlugin(Pvl &lab) {
  return new Cassini::IssNACamera(lab);
}
