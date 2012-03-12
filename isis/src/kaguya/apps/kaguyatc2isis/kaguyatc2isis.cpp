#include "Isis.h"

#include <cstdio>
#include <string>

#include "ProcessImportPds.h"

#include "UserInterface.h"
#include "Filename.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  ProcessImportPds p;
  Pvl label;
  UserInterface &ui = Application::GetUserInterface();

  string labelFile = ui.GetFilename("FROM");
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

  id.ConvertWhiteSpace();
  id.Compress();
  id.Trim(" ");
  if(id != "TC_MAP") {
    string msg = "Input file [" + inFile.Expanded() + "] does not appear to be " +
                 "in Kaguya Terrain Camera level 2 format. " +
                 "DATA_SET_ID is [" + id + "]";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  p.SetPdsFile(labelFile, "", label);
  Cube *outcube = p.SetOutputCube("TO");

  p.SetOrganization(Isis::ProcessImport::BSQ);

  p.StartProcess();

  // Get the mapping labels
  Pvl otherLabels;
  p.TranslatePdsProjection(otherLabels);

  // Get the directory where the generic pds2isis level 2 translation tables are.
  PvlGroup dataDir(Preference::Preferences().FindGroup("DataDirectory"));
  iString transDir = (string) dataDir["base"] + "/translations/";

  // Translate the Archive group
  Filename transFile(transDir + "pdsImageArchive.trn");
  PvlTranslationManager archiveXlater(label, transFile.Expanded());
  archiveXlater.Auto(otherLabels);

  // Write the Archive and Mapping groups to the output cube label
  outcube->putGroup(otherLabels.FindGroup("Mapping"));
  outcube->putGroup(otherLabels.FindGroup("Archive"));

  // Add the BandBin group
  PvlGroup bbin("BandBin");
  bbin += PvlKeyword("FilterName", "BroadBand");
  bbin += PvlKeyword("Center", "640nm");
  bbin += PvlKeyword("Width", "420nm");
  outcube->putGroup(bbin);

  p.EndProcess();
}
