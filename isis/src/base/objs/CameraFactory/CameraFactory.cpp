/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <csm/Plugin.h>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QLibrary>

#include "CameraFactory.h"

#include "Camera.h"
#include "CSMCamera.h"
#include "FileName.h"
#include "IException.h"
#include "Plugin.h"
#include "Preference.h"

#include "csm/csm.h"
#include "csm/GeometricModel.h"
#include "csm/Isd.h"
#include "csm/Model.h"
#include "csm/NitfIsd.h"
#include "csm/Plugin.h"

using namespace csm;
using namespace std;

namespace Isis {
  Plugin CameraFactory::m_cameraPlugin;
  bool CameraFactory::m_initialized = false;

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
      // Is there a CSM blob on the cube?
      if (cube.hasBlob("CSMState", "String")) {
        // Create ISIS CSM Camera Model
        try {
          return new CSMCamera(cube);
        }
        catch (IException &e) {
          QString msg = "Unable to create CSM camera using CSMState Cube blob.";
          throw IException(e, IException::Unknown, msg, _FILEINFO_);
        }
      }
      else {
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

        // Create the camera as requested
        return (*plugin)(cube);
      }
    }
    catch(IException &e) {
      string message = "Unable to initialize camera model in Camera Factory.";
      throw IException(e, IException::Unknown, message, _FILEINFO_);
    }
  }


  /**
   * Reads the appropriate plugin file for the ISIS cameras, and scans the
   * directories specified in IsisPreferences for CSM cameras.
   */
  void CameraFactory::initPlugin() {
    if (!m_initialized) {
      // Handle the ISIS camera plugins
      if (m_cameraPlugin.fileName() == "") {
        FileName localFile("Camera.plugin");
        if (localFile.fileExists())
          m_cameraPlugin.read(localFile.expanded());

        FileName systemFile("$ISISROOT/lib/Camera.plugin");
        if (systemFile.fileExists())
          m_cameraPlugin.read(systemFile.expanded());
      }

      // Find the CSM plugins by searching the directories identified in the Preferences.
      // Load the found libraries. This causes the static instance(s) to be constructed,
      // and thus registering the model with the csm Plugin class.
      Preference &p = Preference::Preferences();
      PvlGroup &grp = p.findGroup("Plugins", Isis::Pvl::Traverse);
      for (int i = 0; i<grp["CSMDirectory"].size(); i++) {
        FileName csmDir = grp["CSMDirectory"][i];

        QDirIterator csmLib(csmDir.expanded(), {"*.so", "*.dylib"}, QDir::Files);
        while (csmLib.hasNext()) {
          QString csmLibName = csmLib.next();
          QLibrary csmDynamicLib(csmLibName);
          csmDynamicLib.load();
        }
      }
    }
    m_initialized = true;
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

  /**
   * Create a CSMCamera from an external ISD file and attach it to a cube.
   *
   * @param isdFile Path to the external ISD file.
   * @param cube The cube to attach the camera to.
   *
   * @return Camera* The CSMCamera object created.
   */
  Camera *CameraFactory::CreateFromIsd(const QString &isdFile, Cube &cube) {
    // This raw pointer is not managed. Pre-existing issue.
    csm::Model *model = constructModelFromIsdOrState(isdFile);

    updateLabelForCsm(cube, model);

    // Cast to RasterGM
    csm::RasterGM *rasterModel = dynamic_cast<csm::RasterGM*>(model);
    if (!rasterModel) {
      delete model;
      QString msg = "CSM model from [" + isdFile + "] is not a RasterGM.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    return new CSMCamera(cube, rasterModel);
  }


  /**
   * Update the cube label in memory to support CSM cameras.
   * Follows the same logic as csminit: sets ShapeModel=Null in the
   * Kernels group and injects a CsmInfo group with platform/instrument
   * identifiers derived from the CSM model. This allows serial number
   * derivation and camera creation to work as if csminit had been run,
   * but without modifying the cube on disk.
   *
   * @param cube  The cube whose label will be modified.
   * @param model The CSM sensor model.
   */
  void CameraFactory::updateLabelForCsm(Cube &cube, csm::Model *model) {
    if (!model) return;
    PvlObject &cubeObj = cube.label()->findObject("IsisCube");

    // Ensure Kernels group exists and has ShapeModel=Null.
    // This matches csminit behavior.
    if (!cubeObj.hasGroup("Kernels")) {
      cubeObj.addGroup(PvlGroup("Kernels"));
    }
    PvlGroup &kernels = cubeObj.findGroup("Kernels");
    kernels.addKeyword(PvlKeyword("ShapeModel", "Null"), Pvl::Replace);

    // Add Target group if it doesn't exist. Needed for Target resolution.
    // Get TargetName from the Instrument group (same approach as csminit).
    if (!cubeObj.hasGroup("Target")) {
      QString targetName = "Unknown";
      if (cubeObj.hasGroup("Instrument") &&
          cubeObj.findGroup("Instrument").hasKeyword("TargetName"))
        targetName = cubeObj.findGroup("Instrument")["TargetName"][0];
      PvlGroup target("Target");
      target += PvlKeyword("TargetName", targetName);
      cubeObj.addGroup(target);
    }

    // Add CsmInfo group
    if (cubeObj.hasGroup("CsmInfo"))
      cubeObj.deleteGroup("CsmInfo");

    PvlGroup infoGroup("CsmInfo");
    infoGroup += PvlKeyword("TargetName", cubeObj.findGroup("Target")["TargetName"][0]);
    infoGroup += PvlKeyword("CSMPlatformID", QString::fromStdString(model->getPlatformIdentifier()));
    infoGroup += PvlKeyword("CSMInstrumentId", QString::fromStdString(model->getSensorIdentifier()));
    infoGroup += PvlKeyword("ReferenceTime", QString::fromStdString(model->getReferenceDateAndTime()));
    cubeObj.addGroup(infoGroup);
  }


  /**
   * Write adjusted CSM model state to a JSON file. The output file is
   * named <prefix><cube_basename>.adjusted_state.json, following the
   * same convention as ASP bundle_adjust.
   *
   * @param cubeFile Path to the input cube (basename used for output name).
   * @param state    The adjusted CSM model state string.
   * @param prefix   Optional file prefix (e.g., "run/run_").
   */
  void CameraFactory::writeAdjustedCsmState(const QString &cubeFile,
                                            const QString &state,
                                            const QString &prefix) {
    QFileInfo fi(cubeFile);
    QString outPath = prefix + fi.completeBaseName() + ".adjusted_state.json";

    std::ofstream outFile(outPath.toStdString().c_str());
    if (!outFile.is_open()) {
      QString msg = "Error creating adjusted CSM state file [" + outPath + "].";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    outFile << state.toStdString() << "\n";
    outFile.close();
  }


  /**
   * Construct a CSM model from either an ISD or a model state file.
   * Tries the ISD path first (canModelBeConstructedFromISD), then
   * falls back to the state path (canModelBeConstructedFromState).
   * Follows the same plugin iteration logic as csminit.
   *
   * @param filePath Path to the ISD or model state file.
   *
   * @return csm::Model* The constructed CSM model (caller owns).
   */
  csm::Model *CameraFactory::constructModelFromIsdOrState(
      const QString &filePath) {
    initPlugin();

    // Try ISD path first
    csm::Isd isd(filePath.toStdString());
    for (const csm::Plugin *plugin : csm::Plugin::getList())
      for (size_t j = 0; j < plugin->getNumModels(); j++) {
        std::string modelName = plugin->getModelName(j);
        if (plugin->canModelBeConstructedFromISD(isd, modelName))
          return plugin->constructModelFromISD(isd, modelName);
      }

    // ISD path failed. Try interpreting the file as a model state string.
    std::ifstream stateFile(filePath.toStdString());
    if (!stateFile.is_open()) {
      QString msg = "Unable to open CSM file [" + filePath + "].";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    std::stringstream buffer;
    buffer << stateFile.rdbuf();
    std::string content = buffer.str();

    for (const csm::Plugin *plugin : csm::Plugin::getList())
      for (size_t j = 0; j < plugin->getNumModels(); j++) {
        std::string modelName = plugin->getModelName(j);
        if (plugin->canModelBeConstructedFromState(modelName, content))
          return plugin->constructModelFromState(content);
      }

    QString msg = "No loaded CSM plugin could construct a model from [" +
      filePath + "].";
    throw IException(IException::User, msg, _FILEINFO_);
  }


   /**
   * Constructs CSM Model from ISD.
   *
   * @param isdFilePath Path to ISD file
   * @param pluginName Plugin name
   * @param modelName Model name
   * @param isdFormat ISD format type (e.g., NITF)
   *
   */
  csm::Model *CameraFactory::constructModelFromIsd(QString isdFilePath, QString pluginName, QString modelName, QString isdFormat) {

    // Get model spec if any of these params are not passed in
    if (pluginName.isEmpty() || modelName.isEmpty() || isdFormat.isEmpty()) {
      QStringList modelSpec = getModelSpecFromIsd(isdFilePath);
      pluginName = modelSpec[0];
      modelName = modelSpec[1];
      isdFormat = modelSpec[2];
    }

    csm::Model *model = nullptr;

    const csm::Plugin *plugin = csm::Plugin::findPlugin(pluginName.toStdString());
    if (plugin == NULL) {
      QString message = "Cannot find requested Plugin: [" + pluginName + "].";
      throw IException(IException::User, message, _FILEINFO_);
    }

    csm::Isd fileIsd(isdFilePath.toStdString());
    csm::Nitf21Isd nitf21Isd(isdFilePath.toStdString());
    if (isdFormat == QString::fromStdString(fileIsd.format())) {
      model = plugin->constructModelFromISD(fileIsd, modelName.toStdString());
    }
    else if (isdFormat == QString::fromStdString(nitf21Isd.format())) {
      model = plugin->constructModelFromISD(nitf21Isd, modelName.toStdString());
    }
    else {
      QString message = "Invalid ISD format specifications [" + isdFormat + "].";
      throw IException(IException::Programmer, message, _FILEINFO_);
    }

    return model;
  }

  /**
   * Generate CSM Model specs from ISD. 
   * 
   * @param isdFilePath Path to ISD file
   * @param pluginName Plugin name
   * @param modelName Model name
   */
  QStringList CameraFactory::getModelSpecFromIsd(QString isdFilePath, QString pluginName, QString modelName) {
    QList<QStringList> possibleModels;
    for (const csm::Plugin * plugin : csm::Plugin::getList()) {
      QString currentPluginName = QString::fromStdString(plugin->getPluginName());

      if (!pluginName.isEmpty() && pluginName != pluginName) {
        continue;
      }

      for (size_t modelIndex = 0; modelIndex < plugin->getNumModels(); modelIndex++) {
        QString currentModelName = QString::fromStdString(plugin->getModelName(modelIndex));

        if (!modelName.isEmpty() && currentModelName != modelName) {
          continue;
        }

        csm::Isd fileIsd(isdFilePath.toStdString());
        if (plugin->canModelBeConstructedFromISD(fileIsd, currentModelName.toStdString())) {
          QStringList modelSpec = {
              currentPluginName,
              currentModelName,
              QString::fromStdString(fileIsd.format())};
          possibleModels.append(modelSpec);
          continue; // If the file ISD works, don't check the other ISD formats
        }

        csm::Nitf21Isd nitf21Isd(isdFilePath.toStdString());
        if (plugin->canModelBeConstructedFromISD(nitf21Isd, currentModelName.toStdString())) {
          QStringList modelSpec = {
              currentPluginName,
              currentModelName,
              QString::fromStdString(nitf21Isd.format())};
          possibleModels.append(modelSpec);
          continue; // If the NITF 2.1 ISD works, don't check the other ISD formats
        }
      }
    }

    if (possibleModels.size() > 1) {
      QString message = "Multiple models can be created from the ISD [" + isdFilePath + "]. "
                        "Re-run with the PLUGINNAME and MODELNAME parameters. "
                        "Possible plugin & model names:\n";
      for (const QStringList &modelSpec : possibleModels) {
        message += "Plugin [" + modelSpec[0] + "], Model [" + modelSpec[1] + "]\n";
      }
      throw IException(IException::User, message, _FILEINFO_);
    }

    if (possibleModels.empty()) {
      QString message = "No loaded model could be created from the ISD [" + isdFilePath + "]."
                        "Loaded plugin & model names:\n";
      for (const csm::Plugin * plugin : csm::Plugin::getList()) {
        QString currentPluginName = QString::fromStdString(plugin->getPluginName());
        for (size_t modelIndex = 0; modelIndex < plugin->getNumModels(); modelIndex++) {
          QString modelName = QString::fromStdString(plugin->getModelName(modelIndex));
          message += "Plugin [" + currentPluginName + "], Model [" + modelName + "]\n";
        }
      }
      throw IException(IException::User, message, _FILEINFO_);
    }

    // If we are here, then we have exactly 1 model
    QStringList modelSpec = possibleModels.front();

    if (modelSpec.size() != 3) {
      QString message = "Model specification [" + modelSpec.join(" ") + "] has [" + QString::number(modelSpec.size()) + "] elements "
        "when it should have 3 elements.";
      throw IException(IException::Programmer, message, _FILEINFO_);
    }

    return modelSpec;
  }
} // end namespace isis
