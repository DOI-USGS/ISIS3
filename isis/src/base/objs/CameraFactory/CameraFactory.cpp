/**                                                                       
 * @file                                                                  
 * $Revision: 1.5 $                                                             
 * $Date: 2009/05/12 20:07:31 $                                                                 
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

#include "CameraFactory.h"
#include "Camera.h"
#include "Plugin.h"
#include "iException.h"
#include "Filename.h"

using namespace std;
namespace Isis {
 /**
  * Creates a Camera object using Pvl Specifications
  * 
  * @param lab Pvl label containing specifications for the Camera object
  * 
  * @return Camera* The Camera object created
  * 
  * @throws Isis::iException::System - Unsupported camera model, unable to find
  *                                    the plugin
  * @throws Isis::iException::Camera - Unable to initialize camera model
  */
  Camera *CameraFactory::Create(Isis::Pvl &lab) {
    // Try to load a plugin file in the current working directory and then
    // load the system file
    Plugin p;
    Filename localFile("Camera.plugin");
    if (localFile.Exists()) p.Read(localFile.Expanded());
    Filename systemFile("$ISISROOT/lib/Camera.plugin");
    if (systemFile.Exists()) p.Read(systemFile.Expanded());
  
    try {
      // First get the spacecraft and instrument and combine them
      PvlGroup &inst = lab.FindGroup("Instrument",Isis::Pvl::Traverse);
      iString spacecraft = (string) inst["SpacecraftName"];
      iString name = (string) inst["InstrumentId"];
      spacecraft.UpCase();
      name.UpCase();
      iString group = spacecraft + "/" + name;
      group.Remove(" ");

      PvlGroup &kerns = lab.FindGroup("Kernels",Isis::Pvl::Traverse);
      // Default version 1 for backwards compatibility (spiceinit'd cubes before camera model versioning)
      if(!kerns.HasKeyword("CameraVersion")) {
        kerns.AddKeyword(PvlKeyword("CameraVersion", "1"));
      }

      int cameraOriginalVersion = (int)kerns["CameraVersion"];
      int cameraNewestVersion = CameraVersion(lab);

      if(cameraOriginalVersion != cameraNewestVersion) {
        string msg = "The camera model used to create a camera for this cube is out of date, " \
                     "please re-run spiceinit on the file or process with an old Isis version " \
                     "that has the correct camera model.";
        throw Isis::iException::Message(Isis::iException::System,msg,_FILEINFO_);
      }
  
      // See if we have a camera model plugin
      void *ptr;
      try {
        ptr = p.GetPlugin(group);
      }
      catch (Isis::iException &e) {
        string msg = "Unsupported camera model, unable to find plugin for ";
        msg += "SpacecraftName [" + spacecraft + "] with InstrumentId [";
        msg += name + "]";
        throw Isis::iException::Message(Isis::iException::System,msg,_FILEINFO_);
      }
  
      // Now cast that pointer in the proper way
      Camera * (*plugin) (Isis::Pvl &lab);
      plugin = (Camera * (*)(Isis::Pvl &lab)) ptr;
      
      // Create the projection as requested
      return (*plugin)(lab);
    }
    catch (Isis::iException &e) {
      string message = "Unable to initialize camera model from group [Instrument]";
      throw Isis::iException::Message(Isis::iException::Camera,message,_FILEINFO_);
    }
  }

  /**
   * This looks up the current camera model version from the cube labels.
   * 
   * @param lab Input Cube Labels
   * 
   * @return int Latest Camera Version
   */
  int CameraFactory::CameraVersion(Pvl &lab) {
    // Try to load a plugin file in the current working directory and then
    // load the system file
    Pvl cameraPluginGrp;
    Filename localFile("Camera.plugin");
    if (localFile.Exists()) cameraPluginGrp.Read(localFile.Expanded());
    Filename systemFile("$ISISROOT/lib/Camera.plugin");
    if (systemFile.Exists()) cameraPluginGrp.Read(systemFile.Expanded());
  
    try {
      // First get the spacecraft and instrument and combine them
      PvlGroup &inst = lab.FindGroup("Instrument", Isis::Pvl::Traverse);
      iString spacecraft = (string) inst["SpacecraftName"];
      iString name = (string) inst["InstrumentId"];
      spacecraft.UpCase();
      name.UpCase();
      iString group = spacecraft + "/" + name;
      group.Remove(" ");

      PvlGroup plugin;
      try {
        plugin = cameraPluginGrp.FindGroup(group);
      }
      catch (iException &e) {
        string msg = "Unsupported camera model, unable to find plugin for ";
        msg += "SpacecraftName [" + spacecraft + "] with InstrumentId [";
        msg += name + "]";
        throw Isis::iException::Message(Isis::iException::Camera,msg,_FILEINFO_);
      }
      
      if(!plugin.HasKeyword("Version")) {
        string msg = "Camera model identified by [" + group + "] does not have a version number";
        throw Isis::iException::Message(Isis::iException::Camera,msg,_FILEINFO_);
      }

      return (int)plugin["Version"];
    }
    catch (Isis::iException &e) {
      string msg = "Unable to locate latest camera model version number from group [Instrument]";
      throw Isis::iException::Message(Isis::iException::Camera,msg,_FILEINFO_);
    }
  }
} // end namespace isis


