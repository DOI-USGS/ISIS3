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
   * @param cube The original cube with the current version camera model
   *
   * @return Camera* The Camera object created
   *
   * @throws Isis::iException::System - Unsupported camera model, unable to find
   *                                    the plugin
   * @throws Isis::iException::Camera - Unable to initialize camera model
   */
  Camera *CameraFactory::Create(Cube &cube) {
    // Try to load a plugin file in the current working directory and then
    // load the system file
    initPlugin();

    try {
      // First get the spacecraft and instrument and combine them
      Pvl &lab = *cube.label();
      PvlGroup &inst = lab.findGroup("Instrument", Isis::Pvl::Traverse);
      QString spacecraft = (QString) inst["SpacecraftName"];
      QString name = (QString) inst["InstrumentId"];
      spacecraft = spacecraft.toUpper();
      name = name.toUpper();
      QString group = spacecraft + "/" + name;
      group = group.remove(" ");

      PvlGroup &kerns = lab.findGroup("Kernels", Isis::Pvl::Traverse);
      // Default version 1 for backwards compatibility (spiceinit'd cubes before camera model versioning)
      if (!kerns.hasKeyword("CameraVersion")) {
        kerns.addKeyword(PvlKeyword("CameraVersion", "1"));
      }

      int cameraOriginalVersion = (int)kerns["CameraVersion"];
      int cameraNewestVersion = CameraVersion(cube);

      if (cameraOriginalVersion != cameraNewestVersion) {
        string msg = "The camera model used to create a camera for this cube is out of date, " \
                     "please re-run spiceinit on the file or process with an old Isis version " \
                     "that has the correct camera model.";
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }

      // See if we have a camera model plugin
      QFunctionPointer ptr;
      try {
        ptr = m_cameraPlugin.GetPlugin(group);
      }
      catch(IException &e) {
        QString msg = "Unsupported camera model, unable to find plugin for ";
        msg += "SpacecraftName [" + spacecraft + "] with InstrumentId [";
        msg += name + "]";
        throw IException(e, IException::Unknown, msg, _FILEINFO_);
      }

      // Now cast that pointer in the proper way
      Camera * (*plugin)(Isis::Cube &cube);
      plugin = (Camera * ( *)(Isis::Cube &cube)) ptr;

      // Create the projection as requested
      return (*plugin)(cube);
    }
    catch(IException &e) {
      string message = "Unable to initialize camera model from group [Instrument]";
      throw IException(e, IException::Unknown, message, _FILEINFO_);
    }
  }


  /**
   * Reads the appropriate plugin file for the camera
   */
  void CameraFactory::initPlugin() {
    if (m_cameraPlugin.fileName() == "") {
      FileName localFile("Camera.plugin");
      if (localFile.fileExists())
        m_cameraPlugin.read(localFile.expanded());

      FileName systemFile("$ISISROOT/lib/Camera.plugin");
      if (systemFile.fileExists())
        m_cameraPlugin.read(systemFile.expanded());
    }
  }


  /**
   * This looks up the current camera model version from the cube.
   *
   * @param cube Input cube
   *
   * @return int Latest Camera Version
   */
  int CameraFactory::CameraVersion(Cube &cube) {
    return CameraVersion(*cube.label());
  }


  /**
   * Looks up the current camera model version in the pvl labels.
   *
   * @param lab The pvl labels
   *
   * @returns The current camera model version
   */
  int CameraFactory::CameraVersion(Pvl &lab) {
    // Try to load a plugin file in the current working directory and then
    // load the system file
    initPlugin();

    try {
      // First get the spacecraft and instrument and combine them
      PvlGroup &inst = lab.findGroup("Instrument", Isis::Pvl::Traverse);
      QString spacecraft = (QString) inst["SpacecraftName"];
      QString name = (QString) inst["InstrumentId"];
      spacecraft = spacecraft.toUpper();
      name = name.toUpper();
      QString group = spacecraft + "/" + name;
      group = group.remove(" ");

      PvlGroup plugin;
      try {
        bool found = false;
        // Find the most recent (last) version of the camera model
        for (int i = m_cameraPlugin.groups() - 1; i >= 0; i--) {
          if (m_cameraPlugin.group(i) == group) {
            plugin = m_cameraPlugin.group(i);
            found = true;
            break;
          }
        }
        if (!found) {
          QString msg = "Unable to find PVL group [" + group + "].";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }
      }
      catch(IException &e) {
        QString msg = "Unsupported camera model, unable to find plugin for ";
        msg += "SpacecraftName [" + spacecraft + "] with InstrumentId [";
        msg += name + "]";
        throw IException(e, IException::Unknown, msg, _FILEINFO_);
      }

      if (!plugin.hasKeyword("Version")) {
        QString msg = "Camera model identified by [" + group + "] does not have a version number";
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
