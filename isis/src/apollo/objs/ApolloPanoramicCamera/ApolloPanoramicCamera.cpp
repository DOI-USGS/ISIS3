#include "ApolloPanoramicCamera.h"
#include "ApolloPanoramicDetectorMap.h"
#include "iString.h"
#include "iTime.h"
#include "IException.h"
#include "LineScanCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"

using namespace std;
namespace Isis {
  namespace Apollo {
  // constructors
    ApolloPanoramicCamera::ApolloPanoramicCamera(Isis::Pvl &lab) : Isis::LineScanCamera(lab) {
      printf("ApolloPan Constructor\n");
      // Set up the camera info from ik/iak kernels
      SetFocalLength(610.0);    //nominal (uncalibrated) focal length in mm from "Apollo 15 SIM Bay Photographic Equipment and Mission Summary" August, 1971
      SetPixelPitch(1.0);    //detector/focal plane units are mm

      double  constantTimeOffset = 0.0,
              additionalPreroll = 0.0,
              additiveLineTimeError = 0.0,
              multiplicativeLineTimeError = 0.0;

      //following keywords in InstrumentAddendum file
      iString ikernKey = "INS" + iString((int)naifIkCode()) + "_CONSTANT_TIME_OFFSET";
      constantTimeOffset = getDouble(ikernKey);

      ikernKey = "INS" + iString((int)naifIkCode()) + "_ADDITIONAL_PREROLL";
      additionalPreroll = getDouble(ikernKey);

      ikernKey = "INS" + iString((int)naifIkCode()) + "_ADDITIVE_LINE_ERROR";
      additiveLineTimeError = getDouble(ikernKey);

      ikernKey = "INS" + iString((int)naifIkCode()) + "_MULTIPLI_LINE_ERROR";
      multiplicativeLineTimeError = getDouble(ikernKey);
 
      Isis::PvlGroup &inst = lab.FindGroup("Instrument", Isis::Pvl::Traverse);
      iString stime = (string)inst["StartTime"];  
      SpiceDouble etStart;
      str2et_c(stime.c_str(), &etStart);
      stime = (string) inst["StopTime"];
      SpiceDouble etStop;
      str2et_c(stime.c_str(), &etStop);
      iTime isisTime( (string) inst["StartTime"]);
      
      // Get other info from labels
      double lineRate = ( (double) inst["LineExposureDuration"] );    //line exposure duration, sec/mm

      //??      ss += 1.0;

      lineRate *= 1.0 + multiplicativeLineTimeError;
      lineRate += additiveLineTimeError;
      etStart += additionalPreroll * lineRate;
      etStart += constantTimeOffset;

      setTime(isisTime);

      // Setup detector map
      ApolloPanoramicDetectorMap *detectorMap = new ApolloPanoramicDetectorMap((Camera *)this,(etStart+etStop)/2.0,(double) lineRate, &lab);  //note (etStart+etStop)/2.0 is the time in the middle of image ( line = 0 after interior orientation)
      detectorMap->SetDetectorSampleSumming(1.0);
      detectorMap->SetStartingDetectorSample(0.0);

      // Setup focal plane map
      Isis::PvlGroup &kernel = lab.FindGroup("Kernels", Isis::Pvl::Traverse);
      CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, (int) kernel["NaifFrameCode"]);  

      //  Retrieve boresight location from instrument kernel (IK) (addendum?)
      //ikernKey = "INS" + iString((int)naifIkCode()) + "_BORESIGHT_SAMPLE";
      //double sampleBoreSight = getDouble(ikernKey);
      double sampleBoreSight = 0.0;  //Presently no NAIF keywords for this sensor

      //ikernKey = "INS" + iString((int)naifIkCode()) + "_BORESIGHT_LINE";
      //double lineBoreSight = getDouble(ikernKey);
      double lineBoreSight = 0.0;  //Presently no NAIF keywords for this sensor

      focalMap->SetDetectorOrigin(sampleBoreSight, lineBoreSight);
      focalMap->SetDetectorOffset(0.0, 0.0);

      // Setup distortion map
      new CameraDistortionMap(this,-1.0);
      //distMap->SetDistortion(naifIkCode());    Presently no NAIF keywords for this sensor

      //Setup the ground and sky map
      new LineScanCameraGroundMap(this);
      new LineScanCameraSkyMap(this);

      Isis::PvlGroup &instP = lab.FindGroup("Kernels", Isis::Pvl::Traverse);
      p_CkFrameId = (int) instP["NaifFrameCode"][0];
      p_CkFrameId = -int(-p_CkFrameId/1000)*1000;

      LoadCache();
    }
  }
}

extern "C" Isis::Camera *ApolloPanoramicCameraPlugin(Isis::Pvl &lab) 
{
   return new Isis::Apollo::ApolloPanoramicCamera(lab);
}
