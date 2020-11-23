#include "csmpt.h"

#include <QList>
#include <QString>
#include <QStringList>
#include <QLibrary>

#include "csm/Model.h"
#include "csm/RasterGM.h"
#include "csm/Plugin.h"

#include "Blob.h"
#include "Cube.h"
#include "IException.h"
#include "Process.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "StringBlob.h"

using namespace std;

namespace Isis {

  /**
   * csmpt
   *
   * @param ui The Application UI
   * @param(out) log The Pvl that attempted models will be logged to
   */
  void csmpt(UserInterface &ui, Pvl *log) {
    //TESTING LOAD USGSCSM
    QLibrary usgscsm("/Users/jmapel/miniconda3/envs/isis4_build/lib/libusgscsm.dylib");
    if (usgscsm.load()) {
      std::cout << "Successfully loaded usgscsm" << std::endl;
    }
    else {
      std::cout << "Failed to load usgscsm" << std::endl;
    }
    //END TESTING
    // We are not processing the image data, so this process object is just for
    // managing the Cube in memory and adding history
    Process p;
    // Get the cube here so that we check early if it doesn't exist
    Cube cube(ui.GetFileName("FROM"));

    StringBlob stateBlob("String", "CSMState");

    try {
      cube.read(stateBlob);
    }
    catch(IException &e) {
      QString message = "Could not read CSM state string from input cube [" +
                        ui.GetFileName("FROM") + "]. Check that csminit "
                        "has been successfully run on it.";
      throw IException(e, IException::User, message, _FILEINFO_);
    }

    PvlObject stateLabel = stateBlob.Label();
    if (!stateLabel.hasKeyword("PluginName") || !stateLabel.hasKeyword("ModelName")) {
      QString message = "Label for CSM State BLOB is malformed.";
      throw IException(IException::Unknown, message, _FILEINFO_);
    }

    QString pluginName = stateLabel.findKeyword("PluginName");
    QString modelName = stateLabel.findKeyword("ModelName");

    const csm::Plugin *plugin = csm::Plugin::findPlugin(pluginName.toStdString());
    if (!plugin) {
      QString message = "Could not find plugin [" + pluginName + "] to instantiate "
                        "model from. Loaded plugins:\n";
      for (const csm::Plugin * plugin : csm::Plugin::getList()) {
        message += QString::fromStdString(plugin->getPluginName()) + "\n";
      }
      throw IException(IException::User, message, _FILEINFO_);
    }
    if (!plugin->canModelBeConstructedFromState(modelName.toStdString(), stateBlob.string())) {
      QString message = "Plugin [" + pluginName + "] cannot construct model [" +
                         modelName + "] from state string [" +
                         QString::fromStdString(stateBlob.string()) + "].";
      throw IException(IException::Unknown, message, _FILEINFO_);
    }

    csm::Model *model = plugin->constructModelFromState(stateBlob.string());
    csm::RasterGM *rasterModel = dynamic_cast<csm::RasterGM*>(model);

    csm::ImageVector imageSize = rasterModel->getImageSize();
    csm::ImageCoord imagePt(imageSize.line / 2.0, imageSize.samp / 2.0);
    if (ui.WasEntered("SAMPLE")) {
      imagePt.samp = ui.GetDouble("SAMPLE");
    }
    if (ui.WasEntered("LINE")) {
      imagePt.line = ui.GetDouble("LINE");
    }
    double height = 0.0;
    if (ui.WasEntered("HEIGHT")) {
      height = ui.GetDouble("HEIGHT");
    }
    csm::EcefCoord groundPt = rasterModel->imageToGround(imagePt, height);

    std::cout << "Image point: (" << imagePt.samp << ", " << imagePt.line << ", " << height << ")" << std::endl;
    std::cout << "Ground point: (" << groundPt.x << ", " << groundPt.y << ", " << groundPt.z << ")" << std::endl;
  }

}
