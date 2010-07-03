#include "LoMediumCamera.h"
#include "LoCameraFiducialMap.h"
#include "LoMediumDistortionMap.h"
#include "CameraDistortionMap.h"
#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "iString.h"
#include "Affine.h"

using namespace std;
namespace Isis {
  namespace Lo {
    // constructors
    LoMediumCamera::LoMediumCamera (Pvl &lab) : FramingCamera(lab) {
      // Get the Instrument label information needed to define the camera for this frame
      PvlGroup inst = lab.FindGroup ("Instrument",Pvl::Traverse);
      iString spacecraft = (string)inst["SpacecraftName"];
      iString instId = (string)inst["InstrumentId"];

      LoMediumCamera::FocalPlaneMapType type;
      if (inst.HasKeyword("FiducialSamples")) {
        type = Fiducial;
      }
      else if (inst.HasKeyword("BoresightSample")) {
        type = Boresight;
      }
      else {
        std::string msg = "Unknown focal plane map type:  ";
         msg += "Labels must include fiducials or boresight";
         throw Isis::iException::Message(iException::User,msg,_FILEINFO_);
      }

      // Turn off the aberration corrections for the instrument position object
      InstrumentPosition()->SetAberrationCorrection("NONE");

      // Get the camera characteristics
      SetFocalLength ();
      SetPixelPitch ();

      // Get the start time in et
      string stime = inst["StartTime"];
      double time; 
      str2et_c(stime.c_str(),&time);

      // Setup focal plane map
      if (type == Fiducial) {
        LoCameraFiducialMap fid( inst, NaifIkCode());
        CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this,NaifIkCode());
        // Try (0.,0.)
        focalMap->SetDetectorOrigin(0.0,0.0);

      }
      else  {
        // Read boresight
        double boresightSample = inst["BoresightSample"];
        double boresightLine = inst["BoresightLine"];
        CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this,NaifIkCode());
        focalMap->SetDetectorOrigin(boresightSample,boresightLine);
      }

      // Setup detector map
      new CameraDetectorMap(this);

      // Setup distortion map
      LoMediumDistortionMap *distortionMap = new LoMediumDistortionMap(this);
      distortionMap->SetDistortion(NaifIkCode());
      // Setup the ground and sky map
      new CameraGroundMap(this);
      new CameraSkyMap(this);

      SetEphemerisTime(time);
      LoadCache();
    }
  }
}

extern "C" Isis::Camera *LoMediumCameraPlugin(Isis::Pvl &lab) {
   return new Isis::Lo::LoMediumCamera(lab);
}
