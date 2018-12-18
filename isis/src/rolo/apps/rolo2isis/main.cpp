#include "Isis.h"
#include "ProcessImportPds.h"
#include "UserInterface.h"
#include "FileName.h"
#include "Pvl.h"
#include "PvlToPvlTranslationManager.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  QString labelFile = ui.GetFileName("FROM");

  ProcessImportPds p;
  Pvl label;
  p.SetPdsFile(labelFile, "", label);
  Cube *ocube = p.SetOutputCube("TO");
  p.StartProcess();

  PvlGroup &dataDir = Preference::Preferences().findGroup("DataDirectory");
  QString transDir = (QString) dataDir["Rolo"];

  FileName transFile;
  Pvl inputLabel(labelFile);
  Pvl outputLabel;
  PvlToPvlTranslationManager *translator;

  // translate Mapping group
  transFile = transDir + "/" + "translations/roloMapping.trn";
  translator = new PvlToPvlTranslationManager(inputLabel, transFile.expanded());
  translator->Auto(outputLabel);
  delete translator;
  translator = NULL;

  // translate Instrument group
  transFile = transDir + "/" + "translations/roloInstrument.trn";
  translator = new PvlToPvlTranslationManager(inputLabel, transFile.expanded());
  translator->Auto(outputLabel);
  delete translator;
  translator = NULL;

  // translate BandBin group
  transFile = transDir + "/" + "translations/roloBandBin.trn";
  translator = new PvlToPvlTranslationManager(inputLabel, transFile.expanded());
  translator->Auto(outputLabel);
  outputLabel.findGroup("BandBin").findKeyword("OriginalBand").setUnits(
    translator->Translate("BandBinUnit"));
  outputLabel.findGroup("BandBin").findKeyword("Center").setUnits(translator->
      Translate("BandBinUnit"));
  outputLabel.findGroup("BandBin").findKeyword("Width").setUnits(translator->
      Translate("BandBinUnit"));
  outputLabel.findGroup("BandBin").findKeyword("Exposure").setUnits(translator->
      Translate("ExposureUnit"));
  delete translator;
  translator = NULL;

  // translate Archive group
  transFile = transDir + "/" + "translations/roloArchive.trn";
  translator = new PvlToPvlTranslationManager(inputLabel, transFile.expanded());
  translator->Auto(outputLabel);
  delete translator;
  translator = NULL;

  // add outputLabel to cube
  if(outputLabel.hasGroup("Mapping") &&
      (outputLabel.findGroup("Mapping").keywords() > 0)) {
    ocube->putGroup(outputLabel.findGroup("Mapping"));
  }
  if(outputLabel.hasGroup("Instrument") &&
      (outputLabel.findGroup("Instrument").keywords() > 0)) {
    ocube->putGroup(outputLabel.findGroup("Instrument"));
  }
  if(outputLabel.hasGroup("BandBin") &&
      (outputLabel.findGroup("BandBin").keywords() > 0)) {
    ocube->putGroup(outputLabel.findGroup("BandBin"));
  }
  if(outputLabel.hasGroup("Archive") &&
      (outputLabel.findGroup("Archive").keywords() > 0)) {
    ocube->putGroup(outputLabel.findGroup("Archive"));
  }

  p.EndProcess();

  return;
}
