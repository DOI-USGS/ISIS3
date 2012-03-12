#include "Isis.h"

#include <cstdio>
#include <string>

#include "ProcessImportPds.h"
#include "ProgramLauncher.h"

#include "UserInterface.h"
#include "Filename.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  ProcessImportPds p;
  Pvl label;
  UserInterface &ui = Application::GetUserInterface();

  Filename inFile = ui.GetFilename("FROM");
  iString id;
  Pvl lab(inFile.Expanded());

  try {
    id = (string) lab.FindKeyword("DATA_SET_ID");
  }
  catch(IException &e) {
    string msg = "Unable to read [DATA_SET_ID] from input file [" +
                 inFile.Expanded() + "]";
    throw IException(e, IException::Unknown, msg, _FILEINFO_);
  }

  p.SetPdsFile(inFile.Expanded(), "", label);
  Cube *outcube = p.SetOutputCube("TO");

  p.SetOrganization(Isis::ProcessImport::BSQ);

  p.StartProcess();

  // Get the directory where the Kaguya MI translation tables are.
  PvlGroup dataDir(Preference::Preferences().FindGroup("DataDirectory"));
  iString transDir = (string) dataDir["Kaguya"] + "/translations/";
  Pvl inputLabel(inFile.Expanded());
  Pvl *outputLabel = outcube->getLabel();
  Filename transFile;

  // Translate the Archive group
  transFile = transDir + "kaguyamiArchive.trn";
  PvlTranslationManager archiveXlater(inputLabel, transFile.Expanded());
  archiveXlater.Auto(*(outputLabel));

  // Translate the Instrument group
  transFile = transDir + "kaguyamiInstrument.trn";
  PvlTranslationManager instrumentXlater(inputLabel, transFile.Expanded());
  instrumentXlater.Auto(*(outputLabel));

  // Translate the BandBin group
  transFile = transDir + "kaguyamiBandBin.trn";
  PvlTranslationManager bandBinXlater(inputLabel, transFile.Expanded());
  bandBinXlater.Auto(*(outputLabel));

  p.EndProcess();
}
