#include "LoHighCamera.h"
#include "LoCameraFiducialMap.h"
#include "LoHighDistortionMap.h"
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
    LoHighCamera::LoHighCamera (Pvl &lab) : FramingCamera(lab) {
      // Get the Instrument label information needed to define the camera for this frame
      PvlGroup inst = lab.FindGroup ("Instrument",Pvl::Traverse);
      iString spacecraft = (string)inst["SpacecraftName"];
      iString instId = (string)inst["InstrumentId"];

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

      LoCameraFiducialMap fid( inst, NaifIkCode());

      // Setup detector map
      new CameraDetectorMap(this);

      // Setup focalplane map
      CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this,NaifIkCode());
      // Try (0.,0.)
      focalMap->SetDetectorOrigin(0.0,0.0);

      // Setup distortion map
      LoHighDistortionMap *distortionMap = new LoHighDistortionMap(this);
      distortionMap->SetDistortion(NaifIkCode());
      // Setup the ground and sky map
      new CameraGroundMap(this);
      new CameraSkyMap(this);

      SetEphemerisTime(time);
      LoadCache();
    }
  }
}

extern "C" Isis::Camera *LoHighCameraPlugin(Isis::Pvl &lab) {
   return new Isis::Lo::LoHighCamera(lab);
}
