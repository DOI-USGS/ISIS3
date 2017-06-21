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
  UserInterface &ui = Application::GetUserInterface();

  QString labelFile = ui.GetFileName("FROM");
  FileName inFile = ui.GetFileName("FROM");
  QString id;
  Pvl label(inFile.expanded());

  try {
    id = (QString) label.findKeyword("DATA_SET_ID");
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

  if (!label.hasKeyword("TARGET_NAME")) {
    label.addKeyword(PvlKeyword("TARGET_NAME", "MOON"), Pvl::Replace);
  }

  p.SetPdsFile(label, labelFile);

  Cube *outcube = p.SetOutputCube("TO");

  // Get user entered special pixel ranges
  if(ui.GetBoolean("SETNULLRANGE")) {
    p.SetNull(ui.GetDouble("NULLMIN"), ui.GetDouble("NULLMAX"));
  }
  if(ui.GetBoolean("SETHRSRANGE")) {
    p.SetHRS(ui.GetDouble("HRSMIN"), ui.GetDouble("HRSMAX"));
  }
  if(ui.GetBoolean("SETHISRANGE")) {
    p.SetHIS(ui.GetDouble("HISMIN"), ui.GetDouble("HISMAX"));
  }
  if(ui.GetBoolean("SETLRSRANGE")) {
    p.SetLRS(ui.GetDouble("LRSMIN"), ui.GetDouble("LRSMAX"));
  }
  if(ui.GetBoolean("SETLISRANGE")) {
    p.SetLIS(ui.GetDouble("LISMIN"), ui.GetDouble("LISMAX"));
  }

  p.SetOrganization(Isis::ProcessImport::BSQ);

  p.StartProcess();

  // Get the mapping labels
  Pvl otherLabels;
  p.TranslatePdsProjection(otherLabels);

  // Translate the remaining MI MAP labels
  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["Kaguya"] + "/translations/";
  
  FileName transFile(transDir + "tcmapBandBin.trn");
  PvlToPvlTranslationManager bandBinXlater(label, transFile.expanded());
  bandBinXlater.Auto(otherLabels);
  
  transFile = transDir + "tcmapInstrument.trn";
  PvlToPvlTranslationManager instXlater(label, transFile.expanded());
  instXlater.Auto(otherLabels);
  
  transFile = transDir + "tcmapArchive.trn";
  PvlToPvlTranslationManager archiveXlater(label, transFile.expanded());
  archiveXlater.Auto(otherLabels);
  
  if(otherLabels.hasGroup("Mapping") &&
    (otherLabels.findGroup("Mapping").keywords() > 0)) {
    outcube->putGroup(otherLabels.findGroup("Mapping"));
  }
  if(otherLabels.hasGroup("Instrument") &&
    (otherLabels.findGroup("Instrument").keywords() > 0)) {
    outcube->putGroup(otherLabels.findGroup("Instrument"));
  }
  if(otherLabels.hasGroup("BandBin") &&
    (otherLabels.findGroup("BandBin").keywords() > 0)) {
    outcube->putGroup(otherLabels.findGroup("BandBin"));
  }
  if(otherLabels.hasGroup("Archive") &&
    (otherLabels.findGroup("Archive").keywords() > 0)) {
    outcube->putGroup(otherLabels.findGroup("Archive"));
  }

  // Add the BandBin group
  PvlGroup bbin("BandBin");
  bbin += PvlKeyword("FilterName", "BroadBand");
  bbin += PvlKeyword("Center", "640nm");
  bbin += PvlKeyword("Width", "420nm");
  outcube->putGroup(bbin);

  p.EndProcess();
}
