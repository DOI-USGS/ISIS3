#include "Isis.h"

#include <cstdio>
#include <string>

#include "ProcessImportPds.h"

#include "UserInterface.h"
#include "FileName.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  ProcessImportPds p;
  Pvl label;
  UserInterface &ui = Application::GetUserInterface();

  QString labelFile = ui.GetFileName("FROM");
  FileName inFile = ui.GetFileName("FROM");
  QString id;
  Pvl lab(inFile.expanded());

  try {
    id = (QString) lab.findKeyword("DATA_SET_ID");
  }
  catch(IException &e) {
    QString msg = "Unable to read [DATA_SET_ID] from input file [" +
                 inFile.expanded() + "]";
    throw IException(e, IException::Unknown, msg, _FILEINFO_);
  }

  id = id.simplified().trimmed();
  if(id != "TC_MAP" && id != "TCO_MAP") {
    QString msg = "Input file [" + inFile.expanded() + "] does not appear to be " +
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
  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["base"] + "/translations/";

  // Translate the Archive group
  FileName transFile(transDir + "pdsImageArchive.trn");
  PvlTranslationManager archiveXlater(label, transFile.expanded());
  archiveXlater.Auto(otherLabels);

  // Write the Archive and Mapping groups to the output cube label
  outcube->putGroup(otherLabels.findGroup("Mapping"));
  outcube->putGroup(otherLabels.findGroup("Archive"));

  // Add the BandBin group
  PvlGroup bbin("BandBin");
  bbin += PvlKeyword("FilterName", "BroadBand");
  bbin += PvlKeyword("Center", "640nm");
  bbin += PvlKeyword("Width", "420nm");
  outcube->putGroup(bbin);

  p.EndProcess();
}
