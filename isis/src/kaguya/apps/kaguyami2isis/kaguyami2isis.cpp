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
  catch(iException &e) {
    string msg = "Unable to read [DATA_SET_ID] from input file [" +
                 inFile.Expanded() + "]";
    throw iException::Message(iException::Io, msg, _FILEINFO_);
  }

  p.SetPdsFile(inFile.Expanded(), "", label);
  Cube *outcube = p.SetOutputCube("TO");

  p.SetOrganization(Isis::ProcessImport::BSQ);

  double upleftlat = label.FindKeyword("UPPER_LEFT_LATITUDE");
  double loleftlat = label.FindKeyword("LOWER_LEFT_LATITUDE");

  p.StartProcess();

  // Get the directory where the Kaguya MI translation tables are.
  PvlGroup dataDir(Preference::Preferences().FindGroup("DataDirectory"));
  iString transDir = (string) dataDir["Kaguya"] + "/translations/";
  Pvl inputLabel(inFile.Expanded());
  Pvl *outputLabel = outcube->Label();
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

  if (upleftlat < loleftlat) {
    Filename outFile = ui.GetFilename("TO");
    string tmpName = "$TEMPORARY/" + inFile.Basename() + ".tmp.cub";
    Filename tmpFile(tmpName);
    string pars = "from=" + outFile.Expanded() + " to=" + tmpFile.Expanded();
    ProgramLauncher::RunIsisProgram("flip",pars);
    pars = "mv " + tmpFile.Expanded() + " " + outFile.Expanded();
    ProgramLauncher::RunSystemCommand(pars);
  }
}
