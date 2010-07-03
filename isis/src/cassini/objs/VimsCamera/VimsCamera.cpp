#include "VimsCamera.h"
#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
#include "VimsGroundMap.h"
#include "VimsSkyMap.h"
#include "iString.h"
#include "iException.h"
#include "Filename.h"
#include "SpecialPixel.h"
#include "Constants.h"

#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

namespace Isis {
  namespace Cassini {

   /**
    * Constructor for the Cassini Vims Camera Model
    *
    * @param [in] lab   (Pvl &)  Label used to create camera model 
    *  
    * @history 2007-12-12  Tracie Sucharski,  After creating spice cache with 
    *                         padding, reset et by calling SetImage(1,1) so that
    *                         et is initialized properly at beginning of image
    *                         without padding.
    *
    */
    VimsCamera::VimsCamera (Pvl &lab) : Camera(lab) {
    
      PvlGroup inst = lab.FindGroup ("Instrument",Pvl::Traverse);
      string channel = (string) inst ["Channel"];

      // Set Frame mounting

       if (channel == "VIS") {
         //LoadFrameMounting ("CASSINI_SC_COORD","CASSINI_VIMS_V");

         SetFocalLength(143.0);
         if (iString((string)inst["SamplingMode"]).UpCase() == "NORMAL") {
         SetPixelPitch(3*.024);
         }
         else {
           SetPixelPitch(.024);
         }
       }
       else if (channel == "IR") {
         //LoadFrameMounting ("CASSINI_SC_COORD","CASSINI_VIMS_IR");

         SetFocalLength(426.0);
         SetPixelPitch(.2);
       }
      
      // Get the start time in et
      iString stime = (string) inst ["NativeStartTime"];
      string intTime = stime.Token(".");
      
      double etStart;
      scs2e_c(NaifSpkCode(),intTime.c_str(),&etStart);
      //  Add 2 seconds to either side of time range because the time are for IR
      // channel, the VIS may actually start integrating before NATIVE_START_TIME.
      //  This insures the cache is large enough.
      etStart += stime.ToDouble() / 15959.0 - 2.;

      // Get the end time in et
      iString etime = (string) inst ["NativeStopTime"];
      intTime = etime.Token(".");

      double etStop;
      scs2e_c(NaifSpkCode(),intTime.c_str(),&etStop);
      //  Add 2 seconds to either side of time range because the time are for IR
      // channel, the VIS may actually start integrating before NATIVE_START_TIME.
      //  This insures the cache is large enough.
      etStop += stime.ToDouble() / 15959.0 + 2.;

      //  Setup detector map
      new CameraDetectorMap(this);

      // Setup focal plane map
      new CameraFocalPlaneMap(this,NaifIkCode());

      // Setup distortion map
      new CameraDistortionMap(this);

      // Setup the ground and sky map
      new VimsGroundMap(this,lab);
      new VimsSkyMap(this,lab);

      ((VimsGroundMap*)GroundMap())->Init(lab);
      ((VimsSkyMap*)SkyMap())->Init(lab);

      double tol = PixelResolution();

      if (tol < 0.) {
        // Alternative calculation of .01*ground resolution of a pixel
        tol = PixelPitch()*SpacecraftAltitude()/FocalLength()/1000./100.;
        }

      if (channel == "VIS") CreateCache(etStart,etStop,64*64, tol);
      if (channel == "IR") CreateCache(etStart,etStop,64*64, tol);

      //  Call SetImage so that the et is reset to beginning of image w/o
      //   padding.
      IgnoreProjection(true);
      SetImage(1,1);
      IgnoreProjection(false);
    }


  }
}
    
// Plugin
extern "C" Isis::Camera *VimsCameraPlugin (Isis::Pvl &lab) {
  return new Isis::Cassini::VimsCamera(lab);
}

