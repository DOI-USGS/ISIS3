/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "csminit.h"

#include <QList>
#include <QString>
#include <QStringList>

#include "csm/csm.h"
#include "csm/GeometricModel.h"
#include "csm/Isd.h"
#include "csm/Model.h"
#include "csm/NitfIsd.h"
#include "csm/Plugin.h"

#include "Blob.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "Cube.h"
#include "IException.h"
#include "Process.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "StringBlob.h"

using namespace std;

namespace Isis {

  /**
   * csminit a cube in an Application
   *
   * @param ui The Application UI
   * @param(out) log The Pvl that attempted models will be logged to
   */
  void csminit(UserInterface &ui, Pvl *log) {
    // We are not processing the image data, so this process object is just for
    // managing the Cube in memory and adding history
    Process p;
    // Get the cube here so that we check early if it doesn't exist
    Cube *cube = p.SetInputCube(ui.GetFileName("FROM"), ui.GetInputAttribute("FROM"), ReadWrite);

    // We have to call this to get the plugin list loaded right now
    try {
      Camera *cam = CameraFactory::Create(*cube);
      delete cam;
    }
    catch(...) {
      // Noop
    }

    QString isdFilePath = ui.GetFileName("ISD");

    QList<QStringList> possibleModels;
    for (const csm::Plugin * plugin : csm::Plugin::getList()) {
      QString pluginName = QString::fromStdString(plugin->getPluginName());
      if (ui.WasEntered("PLUGINNAME") && pluginName != ui.GetString("PLUGINNAME")) {
        continue;
      }

      for (size_t modelIndex = 0; modelIndex < plugin->getNumModels(); modelIndex++) {
        QString modelName = QString::fromStdString(plugin->getModelName(modelIndex));
        if (ui.WasEntered("MODELNAME") && modelName != ui.GetString("MODELNAME")) {
          continue;
        }

        csm::Isd fileIsd(isdFilePath.toStdString());
        if (plugin->canModelBeConstructedFromISD(fileIsd, modelName.toStdString())) {
          QStringList modelSpec = {pluginName, modelName, QString::fromStdString(fileIsd.format())};
          possibleModels.append(modelSpec);
          continue; // If the file ISD works, don't check the others
        }

        csm::Nitf21Isd nitf21Isd(isdFilePath.toStdString());
        if (plugin->canModelBeConstructedFromISD(nitf21Isd, modelName.toStdString())) {
          QStringList modelSpec = {pluginName, modelName, QString::fromStdString(nitf21Isd.format())};
          possibleModels.append(modelSpec);
          continue; // If the NITF 2.1 ISD works, don't check the others
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
        QString pluginName = QString::fromStdString(plugin->getPluginName());
        for (size_t modelIndex = 0; modelIndex < plugin->getNumModels(); modelIndex++) {
          QString modelName = QString::fromStdString(plugin->getModelName(modelIndex));
          message += "Plugin [" + pluginName + "], Model [" + modelName + "]\n";
        }
      }
      throw IException(IException::User, message, _FILEINFO_);
    }

    // If we are here, then we have exactly 1 model
    QStringList modelSpec = possibleModels.front();
    if (modelSpec.size() != 3) {
      QString message = "Model specification [" + modelSpec.join(" ") + "] has [" + modelSpec.size() + "] elements "
                        "when it should have 3 elements.";
      throw IException(IException::Programmer, message, _FILEINFO_);
    }
    const csm::Plugin *plugin = csm::Plugin::findPlugin(modelSpec[0].toStdString());
    csm::Model *model;
    csm::Isd fileIsd(isdFilePath.toStdString());
    csm::Nitf21Isd nitf21Isd(isdFilePath.toStdString());
    if (modelSpec[2] == QString::fromStdString(fileIsd.format())) {
      model = plugin->constructModelFromISD(fileIsd, modelSpec[1].toStdString());
    }
    else if (modelSpec[2] == QString::fromStdString(nitf21Isd.format())) {
      model = plugin->constructModelFromISD(nitf21Isd, modelSpec[1].toStdString());
    }
    else {
      QString message = "Invalid ISD format specifications [" + modelSpec[2] + "].";
      throw IException(IException::Programmer, message, _FILEINFO_);
    }

    string modelState = model->getModelState();

    // Add the TargetName to the instrument group, if specified:
    if (ui.WasEntered("TARGETNAME")) {
      if (!cube->hasGroup("Instrument")) {
        cube->putGroup(PvlGroup("Instrument"));
      }
      PvlGroup &instrumentGroup = cube->group("Instrument");
      instrumentGroup.addKeyword(PvlKeyword("TargetName", ui.GetString("TARGETNAME")), Pvl::Replace);
    }

    // Populate the CsmInfo group with useful information
    cube->deleteGroup("CsmInfo");
    PvlGroup infoGroup("CsmInfo");
    infoGroup += PvlKeyword("CSMPlatformID",
                            QString::fromStdString(model->getPlatformIdentifier()));
    infoGroup += PvlKeyword("CSMInstrumentId",
                            QString::fromStdString(model->getSensorIdentifier()));
    infoGroup += PvlKeyword("ReferenceTime",
                            QString::fromStdString(model->getReferenceDateAndTime()));
    csm::GeometricModel *modelWithParams = dynamic_cast<csm::GeometricModel*>(model);
    if (modelWithParams) {
      PvlKeyword paramNames("ModelParameterNames");
      PvlKeyword paramUnits("ModelParameterUnits");
      PvlKeyword paramTypes("ModelParameterTypes");
      for (const csm::GeometricModel::Parameter &param : modelWithParams->getParameters()) {
        paramNames += QString::fromStdString(param.name);
        paramUnits += QString::fromStdString(param.units);
        switch (param.type) {
          case csm::param::NONE:
            paramTypes += "NONE";
            break;

          case csm::param::FICTITIOUS:
            paramTypes += "FICTITIOUS";
            break;
        
          case csm::param::REAL:
            paramTypes += "REAL";
            break;
        
          case csm::param::FIXED:
            paramTypes += "FIXED";
            break;
        
          default:
            paramTypes += "UNKNOWN";
            break;
        }
      }

      infoGroup += paramNames;
      infoGroup += paramUnits;
      infoGroup += paramTypes;
    }
    cube->putGroup(infoGroup);

    // Update existing Kernels Group or create new one and add shapemodel if provided
    if (!cube->hasGroup("Kernels")) {
      cube->putGroup(PvlGroup("Kernels"));
    }
    PvlGroup &kernelsGroup = cube->group("Kernels");

    if (ui.WasEntered("SHAPEMODEL")) {
      // TODO validate the shapemodel
      kernelsGroup.addKeyword(PvlKeyword("ShapeModel", ui.GetString("SHAPEMODEL")), Pvl::Replace);
    }
    else {
      kernelsGroup.addKeyword(PvlKeyword("ShapeModel", "Ellipsoid"), Pvl::Replace);
    }

    // Get rid of keywords from spiceinit
    if (kernelsGroup.hasKeyword("LeapSecond")) {
      kernelsGroup.deleteKeyword("LeapSecond");
    }
    if (kernelsGroup.hasKeyword("TargetAttitudeShape")) {
      kernelsGroup.deleteKeyword("TargetAttitudeShape");
    }
    if (kernelsGroup.hasKeyword("TargetPosition")) {
      kernelsGroup.deleteKeyword("TargetPosition");
    }
    if (kernelsGroup.hasKeyword("InstrumentPointing")) {
      kernelsGroup.deleteKeyword("InstrumentPointing");
    }
    if (kernelsGroup.hasKeyword("InstrumentPointingQuality")) {
      kernelsGroup.deleteKeyword("InstrumentPointingQuality");
    }
    if (kernelsGroup.hasKeyword("Instrument")) {
      kernelsGroup.deleteKeyword("Instrument");
    }
    if (kernelsGroup.hasKeyword("SpacecraftClock")) {
      kernelsGroup.deleteKeyword("SpacecraftClock");
    }
    if (kernelsGroup.hasKeyword("InstrumentPositionQuality")) {
      kernelsGroup.deleteKeyword("InstrumentPositionQuality");
    }
    if (kernelsGroup.hasKeyword("InstrumentPosition")) {
      kernelsGroup.deleteKeyword("InstrumentPosition");
    }
    if (kernelsGroup.hasKeyword("InstrumentAddendum")) {
      kernelsGroup.deleteKeyword("InstrumentAddendum");
    }
    if (kernelsGroup.hasKeyword("EXTRA")) {
      kernelsGroup.deleteKeyword("EXTRA");
    }
    if (kernelsGroup.hasKeyword("Source")) {
      kernelsGroup.deleteKeyword("Source");
    }
    if (kernelsGroup.hasKeyword("SpacecraftPointing")) {
      kernelsGroup.deleteKeyword("SpacecraftPointing");
    }
    if (kernelsGroup.hasKeyword("SpacecraftPosition")) {
      kernelsGroup.deleteKeyword("SpacecraftPosition");
    }
    if (kernelsGroup.hasKeyword("CameraVersion")) {
      kernelsGroup.deleteKeyword("CameraVersion");
    }
    if (kernelsGroup.hasKeyword("ElevationModel")) {
      kernelsGroup.deleteKeyword("ElevationModel");
    }
    if (kernelsGroup.hasKeyword("Frame")) {
      kernelsGroup.deleteKeyword("Frame");
    }
    if (kernelsGroup.hasKeyword("StartPadding")) {
      kernelsGroup.deleteKeyword("StartPadding");
    }
    if (kernelsGroup.hasKeyword("EndPadding")) {
      kernelsGroup.deleteKeyword("EndPadding");
    }
    if (kernelsGroup.hasKeyword("RayTraceEngine")) {
      kernelsGroup.deleteKeyword("RayTraceEngine");
    }
    if (kernelsGroup.hasKeyword("OnError")) {
      kernelsGroup.deleteKeyword("OnError");
    }
    if (kernelsGroup.hasKeyword("Tolerance")) {
      kernelsGroup.deleteKeyword("Tolerance");
    }

    // Remove tables from spiceinit
    cube->deleteBlob("Table", "InstrumentPointing");
    cube->deleteBlob("Table", "InstrumentPosition");
    cube->deleteBlob("Table", "BodyRotation");
    cube->deleteBlob("Table", "SunPosition");
    cube->deleteBlob("Table", "CameraStatistics");
    cube->deleteBlob("Polygon", "Footprint");

    if (cube->label()->hasObject("NaifKeywords")) {
      cube->label()->deleteObject("NaifKeywords");
    }

    cube->deleteBlob("String", "CSMState");

    // Create our CSM State blob as a string
    // Add the CSM string to the Blob.
    StringBlob csmStateBlob(modelState, "CSMState");
    PvlObject &blobLabel = csmStateBlob.Label();
    blobLabel += PvlKeyword("ModelName", QString::fromStdString(model->getModelName()));
    blobLabel += PvlKeyword("PluginName", QString::fromStdString(plugin->getPluginName()));

    // Write CSM State blob to cube
    cube->write(csmStateBlob);

    // TODO attempt to get the CSM Model from the cube

    p.WriteHistory(*cube);
  }

}
