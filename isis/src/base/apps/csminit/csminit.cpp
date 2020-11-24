#include "csminit.h"

#include <QList>
#include <QString>
#include <QStringList>
#include <QLibrary>

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

    // TESTING
    // We have to call this to get the plugin list loaded right now
    try {
      Camera *cam = CameraFactory::Create(*cube);
      delete cam;
    }
    catch(...) {
      // Noop
    }
    // END TESTING

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
      // std::cout << "No model found" << std::endl;
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
    // string modelState = "TestModel\nThis is the test model state.";

    // TODO Just do spiceinit clean-up routine instead
    try {
      cube->camera();
      QString message = "Input cube [" + ui.GetFileName("FROM") + "]. "
                        "Already has an ISIS camera model associated with it. CSM "
                        "models cannot be added to cubes with an ISIS camera model.";
      throw IException(IException::Programmer, message, _FILEINFO_);
    }
    catch(IException &e) {
      // no operation, continue
    }

    // Add the TargetName to the instrument group, if specified:
    if (ui.WasEntered("TARGETNAME")) {
      if (!cube->hasGroup("Instrument")) {
        cube->putGroup(PvlGroup("Instrument"));
      }
      PvlGroup &instrumentGroup = cube->group("Instrument");
      if (instrumentGroup.hasKeyword("TargetName")) {
        instrumentGroup.deleteKeyword("TargetName");
      }
      instrumentGroup += PvlKeyword("TargetName", ui.GetString("TARGETNAME"));
    }

    // Popualte the Archive group with useful information
    if (!cube->hasGroup("Archive")) {
      cube->putGroup(PvlGroup("Archive"));
    }
    PvlGroup &archiveGroup = cube->group("Archive");
    if (archiveGroup.hasKeyword("CSMPlatformID")) {
      archiveGroup.deleteKeyword("CSMPlatformID");
    }
    archiveGroup += PvlKeyword("CSMPlatformID",
                              //  "TestPlatformID");
                               QString::fromStdString(model->getPlatformIdentifier()));
    if (archiveGroup.hasKeyword("CSMInstrumentId")) {
      archiveGroup.deleteKeyword("CSMInstrumentId");
    }
    archiveGroup += PvlKeyword("CSMInstrumentId",
                              //  "TestInstrumentID");
                               QString::fromStdString(model->getSensorIdentifier()));
    if (archiveGroup.hasKeyword("ReferenceTime")) {
      archiveGroup.deleteKeyword("ReferenceTime");
    }
    archiveGroup += PvlKeyword("ReferenceTime",
                              //  "TestReferenceTime");
                               QString::fromStdString(model->getReferenceDateAndTime()));

    // Update existing Kernels Group or create new one and add shapemodel if provided
    if (!cube->hasGroup("Kernels")) {
      cube->putGroup(PvlGroup("Kernels"));
    }
    PvlGroup &kernelsGroup = cube->group("Kernels");
    if (kernelsGroup.hasKeyword("ShapeModel")) {
      kernelsGroup.deleteKeyword("ShapeModel");
    }
    if (ui.WasEntered("SHAPEMODEL")) {
      // TODO validate the shapemodel
      kernelsGroup += PvlKeyword("ShapeModel", ui.GetString("SHAPEMODEL"));
    }
    else {
      kernelsGroup += PvlKeyword("ShapeModel", "Ellipsoid");
    }

    cube->deleteBlob("String", "CSMState");

    // Create our CSM State blob as a string
    // Add the CSM string to the Blob.
    StringBlob csmStateBlob(modelState, "CSMState");
    PvlObject &blobLabel = csmStateBlob.Label();
    // blobLabel += PvlKeyword("ModelName", "TestModelName");
    blobLabel += PvlKeyword("ModelName", QString::fromStdString(model->getModelName()));
    // blobLabel += PvlKeyword("PluginName", "TestPluginName");
    blobLabel += PvlKeyword("PluginName", QString::fromStdString(plugin->getPluginName()));

    // Write CSM State blob to cube
    cube->write(csmStateBlob);

    // TODO attempt to get the CSM Model from the cube

    p.WriteHistory(*cube);
  }

}
