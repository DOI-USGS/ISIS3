/**                                                                       
 * @file                                                                  
 * $Revision: 1.6 $                                                             
 * $Date: 2009/12/28 21:37:00 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.                                                           
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website,              
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

#include "VikingCamera.h"
#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "ReseauDistortionMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "iString.h"
#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"
#include "Filename.h"


using namespace std;
namespace Isis {
 /**
  * Creates a Viking Camera Model
  * 
  * @param lab Pvl Label from the image
  *
  * @throws Isis::iException::User - The file does not appear to be a viking image
  */
  VikingCamera::VikingCamera (Pvl &lab) : FramingCamera(lab) {
    // Set the pixel pitch
    SetPixelPitch (1.0 / 85.0);

    // Find out what camera is being used, and set the focal length, altinstcode,
    // raster orientation, cone, crosscone, and camera
    PvlGroup inst = lab.FindGroup ("Instrument",Pvl::Traverse);
    iString spacecraft = (string)inst["SPACECRAFTNAME"];
    iString instId = (string)inst["INSTRUMENTID"];
    string cam;
    int spn;
    double raster, cone, crosscone;
    int altinstcode = 0;
    if (spacecraft == "VIKING_ORBITER_1") {
      spn=1;
      altinstcode = -27999;
      if (instId == "VISUAL_IMAGING_SUBSYSTEM_CAMERA_A") {
        cam = "1a";
        SetFocalLength(474.398);
        crosscone = -0.707350;
        cone = -0.007580;
        raster = 89.735690;
      }
      else if (instId == "VISUAL_IMAGING_SUBSYSTEM_CAMERA_B") {
        cam = "1b";
        SetFocalLength(474.448);
        crosscone = 0.681000;
        cone = -0.032000;
        raster = 90.022800;
      }
      else {
        string msg = "File does not appear to be a viking image";
        throw iException::Message(iException::User,msg, _FILEINFO_);
      }
    }
    else if (spacecraft == "VIKING_ORBITER_2") {
      spn=2;
      altinstcode = -30999;
      if (instId == "VISUAL_IMAGING_SUBSYSTEM_CAMERA_A") {
        cam = "2a";
        SetFocalLength(474.610);
        crosscone = -0.679330;
        cone = -0.023270;
        raster = 89.880691; 
      }
      else if (instId == "VISUAL_IMAGING_SUBSYSTEM_CAMERA_B") {
        cam = "2b";
        SetFocalLength(474.101);
        crosscone = 0.663000;
        cone = -0.044000;
        raster = 89.663790;
      }
      else {
        string msg = "File does not appear to be a viking image";
        throw iException::Message(iException::User,msg, _FILEINFO_);
      }
    }
    else {
      string msg = "File does not appear to be a viking image";
      throw iException::Message(iException::User,msg, _FILEINFO_);
    }
    
    // Get clock count and convert it to a time
    string spacecraftClock = inst["SpacecraftClockCount"];
    double et, offset1; 
    scs2e_c(altinstcode, spacecraftClock.c_str(), &et);

    // Calculate and load the euler angles
    SpiceDouble	CP[3][3];
    eul2m_c ((SpiceDouble)raster*rpd_c(), (SpiceDouble)cone*rpd_c(), 
             (SpiceDouble)-crosscone*rpd_c(), 3, 2, 1, CP);

//    LoadEulerMounting(CP);

    double exposure_duration = inst["ExposureDuration"];
    if (exposure_duration <= .420) {
      offset1 = 7.0 / 8.0 * 4.48;    //4.48 seconds
    }
    else offset1 = 3.0 / 8.0 * 4.48;
    double offset2 = 1.0 / 64.0 * 4.48;
                                        
    et += offset1 + offset2 + exposure_duration / 2.0;

    char timepds[25];
    et2utc_c(et,"ISOC",3,25,timepds);
    utc2et_c(timepds,&et);

    // Setup detector map
    new CameraDetectorMap(this);

    // Setup focal plane map, and detector origin
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this,NaifIkCode());
    focalMap->SetDetectorOrigin(602.0,528.0);

    // Setup distortion map
    const string fname = Filename("$viking" + iString(spn) + "/reseaus/vik" + cam 
                                  + "MasterReseaus.pvl").Expanded(); 
    new ReseauDistortionMap(this, lab, fname);

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);
  
    SetEphemerisTime(et);
    LoadCache();
  }
} // end namespace isis

extern "C" Isis::Camera *VikingCameraPlugin(Isis::Pvl &lab) {
  return new Isis::VikingCamera(lab);
}
