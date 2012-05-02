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
#include "IException.h"
#include "FileName.h"

using namespace std;

namespace Isis {
  Plugin CameraFactory::m_cameraPlugin;

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
  Camera *CameraFactory::Create(Pvl &lab) {
    // Try to load a plugin file in the current working directory and then
    // load the system file
    initPlugin();

    try {
      // First get the spacecraft and instrument and combine them
      PvlGroup &inst = lab.FindGroup("Instrument", Isis::Pvl::Traverse);
      iString spacecraft = (string) inst["SpacecraftName"];
      iString name = (string) inst["InstrumentId"];
      spacecraft.UpCase();
      name.UpCase();
      iString group = spacecraft + "/" + name;
      group.Remove(" ");

      PvlGroup &kerns = lab.FindGroup("Kernels", Isis::Pvl::Traverse);
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
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }

      // See if we have a camera model plugin
      void *ptr;
      try {
        ptr = m_cameraPlugin.GetPlugin(group);
      }
      catch(IException &e) {
        string msg = "Unsupported camera model, unable to find plugin for ";
        msg += "SpacecraftName [" + spacecraft + "] with InstrumentId [";
        msg += name + "]";
        throw IException(e, IException::Unknown, msg, _FILEINFO_);
      }

      // Now cast that pointer in the proper way
      Camera * (*plugin)(Isis::Pvl & lab);
      plugin = (Camera * ( *)(Isis::Pvl & lab)) ptr;

      // Create the projection as requested
      return (*plugin)(lab);
    }
    catch(IException &e) {
      string message = "Unable to initialize camera model from group [Instrument]";
      throw IException(e, IException::Unknown, message, _FILEINFO_);
    }
  }


  void CameraFactory::initPlugin() {
    if(m_cameraPlugin.FileName() == "") {
      FileName localFile("Camera.plugin");
      if(localFile.fileExists())
        m_cameraPlugin.Read(localFile.expanded());

      FileName systemFile("$ISISROOT/lib/Camera.plugin");
      if(systemFile.fileExists())
        m_cameraPlugin.Read(systemFile.expanded());
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
    initPlugin();

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
        plugin = m_cameraPlugin.FindGroup(group);
      }
      catch(IException &e) {
        string msg = "Unsupported camera model, unable to find plugin for ";
        msg += "SpacecraftName [" + spacecraft + "] with InstrumentId [";
        msg += name + "]";
        throw IException(e, IException::Unknown, msg, _FILEINFO_);
      }

      if(!plugin.HasKeyword("Version")) {
        string msg = "Camera model identified by [" + group + "] does not have a version number";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      return (int)plugin["Version"];
    }
    catch(IException &e) {
      string msg = "Unable to locate latest camera model version number from group [Instrument]";
      throw IException(e, IException::Unknown, msg, _FILEINFO_);
    }
  }
} // end namespace isis


