/**                                                                       
 * @file                                                                  
 * $Revision: 1.0 $ 
 * $Date: 2009/05/27 12:08:01 $ 
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

#include "VoyagerCamera.h"

#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "Filename.h"
#include "iString.h"
#include "naif/SpiceUsr.h"
#include "ReseauDistortionMap.h"
#include "Spice.h"


using namespace std;
namespace Isis {
  VoyagerCamera::VoyagerCamera (Pvl &lab) : FramingCamera(lab) {

    // Set the pixel pitch
    SetPixelPitch();
    SetFocalLength();
    // Find out what camera is being used, and set the focal length, altinstcode,
    // and camera
    PvlGroup inst = lab.FindGroup ("Instrument",Pvl::Traverse);
    iString spacecraft = (string)inst["SpacecraftName"];
    iString instId = (string)inst["InstrumentId"];
    
    iString reseauFilename = "";
    int spacecraftCode = 0;
    int instCode = 0;

    // These set up which kernel and other files to access, 
    if (spacecraft == "VOYAGER_1") {
      reseauFilename += "1/reseaus/vg1";
      spacecraftCode = -31;

      if (instId == "NARROW_ANGLE_CAMERA") {
        reseauFilename += "na";
        instCode = -31101;
      }
      else if (instId == "WIDE_ANGLE_CAMERA") {
        reseauFilename += "wa";
        instCode = -31102;
      }
      else {
        string msg = "File does not appear to be a voyager image";
        throw iException::Message(iException::User,msg, _FILEINFO_);
      }
    }
    else if (spacecraft == "VOYAGER_2") {
      reseauFilename += "2/reseaus/vg2";
      spacecraftCode = -32;

      if (instId == "NARROW_ANGLE_CAMERA") {
        reseauFilename += "na";
        instCode = -32101;
      }
      else if (instId == "WIDE_ANGLE_CAMERA") {
        reseauFilename += "wa";
        instCode = -32102;
      }
      else {
        string msg = "File does not appear to be a voyager image";
        throw iException::Message(iException::User,msg, _FILEINFO_);
      }
    }
    else {
      string msg = "File does not appear to be a voyager image";
      throw iException::Message(iException::User,msg, _FILEINFO_);
    }

    new CameraDetectorMap(this);

    // Setup focal plane map, and detector origin
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this,NaifIkCode());
    focalMap->SetDetectorOrigin(500.0, 500.0);

    // Master reseau location file
    reseauFilename = "$voyager" + reseauFilename + "MasterReseaus.pvl";
    Filename masterReseaus(reseauFilename);
    try {
      new ReseauDistortionMap(this, lab, masterReseaus.Expanded()); 
    } catch (iException &e) {
      e.Report();
    }

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);
   
    // StartTime is the most accurate time available because in voy2isis the
    // StartTime is modified to be highly accurate.  
    string startTime = inst["StartTime"];
    double eTime = 0.0;
    utc2et_c(startTime.c_str(), &eTime);
                                         
    double expoDur = inst["ExposureDuration"];        

    // So, StartTime was actually a time two seconds after the end of the
    // exposure. For that reason, we will take of two seconds, and then
    // take off half the exposure duration to get the center if the image
    eTime -= 2.0;
    eTime -= 0.5 * expoDur;

    SetEphemerisTime(eTime);

    LoadCache();
  }
}//End Isis namespace

extern "C" Isis::Camera *VoyagerCameraPlugin(Isis::Pvl &lab) {
  return new Isis::VoyagerCamera(lab);
}
