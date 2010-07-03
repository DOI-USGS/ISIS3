#include "ApolloMetricCamera.h"
#include "ApolloMetricDistortionMap.h"
#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"
#include "NaifStatus.h"
#include "ReseauDistortionMap.h"

using namespace std;

namespace Isis {
  namespace Apollo {
    // constructors
    ApolloMetricCamera::ApolloMetricCamera (Isis::Pvl &lab) : Isis::FramingCamera(lab) {
      NaifStatus::CheckErrors();
      
      // Get the camera characteristics
      SetFocalLength();
      SetPixelPitch();

      // Setup detector map
      new CameraDetectorMap(this);

      // Setup focal plane map
      CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, NaifIkCode());
      focalMap->SetDetectorOrigin(ParentSamples()/2.0, ParentLines()/2.0);

      const PvlGroup &inst = lab.FindGroup("Instrument", Pvl::Traverse);

      iString ppKey("INS" + iString((int)NaifIkCode()) + "_PP");
      iString odkKey("INS" + iString((int)NaifIkCode()) + "_OD_K");
      iString decenterKey("INS" + iString((int)NaifIkCode()) + "_DECENTER");

      new ApolloMetricDistortionMap(this, GetDouble(ppKey, 0), 
          GetDouble(ppKey, 1), GetDouble(odkKey, 0), GetDouble(odkKey, 1),
          GetDouble(odkKey, 2), GetDouble(decenterKey, 0),
          GetDouble(decenterKey, 1), GetDouble(decenterKey, 2));

      // Setup the ground and sky map
      new CameraGroundMap(this);
      new CameraSkyMap(this);

      // Create a cache and grab spice info since it does not change for
      // a framing camera (fixed spacecraft position and pointing)
      // Get the start time in et
      string stime = inst["StartTime"];
      double time;
      str2et_c(stime.c_str(), &time);
      SetEphemerisTime(time);
      LoadCache();

      NaifStatus::CheckErrors();
    }
  }
}

extern "C" Isis::Camera *ApolloMetricCameraPlugin (Isis::Pvl &lab) {
  return new Isis::Apollo::ApolloMetricCamera(lab);
}
