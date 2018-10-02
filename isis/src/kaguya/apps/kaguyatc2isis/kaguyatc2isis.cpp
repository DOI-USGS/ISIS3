#include "Isis.h"

#include <cstdio>
#include <string>

#include "FileName.h"
#include "ProcessImportPds.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  ProcessImportPds importPds;
  UserInterface &ui = Application::GetUserInterface();

  FileName inFile = ui.GetFileName("FROM");
  QString labelFile = inFile.expanded();
  Pvl label(labelFile);

  QString dataFile = "";
  if ( inFile.extension().toLower() == "lbl" ) {
    dataFile = inFile.path() + (QString) label.findKeyword("FILE_NAME");
  }
  else {
    dataFile = labelFile;
  }

  QString id = "";
  try {
    id = (QString) label.findKeyword("DATA_SET_ID");
  }
  catch(IException &e) {
    QString msg = "Unable to read [DATA_SET_ID] from label file [" 
                  + labelFile + "]";
    throw IException(e, IException::Unknown, msg, _FILEINFO_);
  }

  id = id.simplified().trimmed();
  if (id != "TC_MAP" 
      && id != "TCO_MAP"
      && id != "TC1_Level2B") {
    QString msg = "Input file [" + labelFile + "] does not appear to be " +
                  "a supported Kaguya Terrain Camera format. " +
                  "DATA_SET_ID is [" + id + "]" +
                  "Valid formats include [TC_MAP, TCO_MAP, TC1_Level2B]";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  if (!label.hasKeyword("TARGET_NAME")) {
    label.addKeyword(PvlKeyword("TARGET_NAME", "MOON"), Pvl::Replace);
  }

  importPds.SetPdsFile(label, dataFile);

  Cube *outcube = importPds.SetOutputCube("TO");

  // Get user entered special pixel ranges
  if (ui.GetBoolean("SETNULLRANGE")) {
    importPds.SetNull(ui.GetDouble("NULLMIN"), ui.GetDouble("NULLMAX"));
  }
  if (ui.GetBoolean("SETHRSRANGE")) {
    importPds.SetHRS(ui.GetDouble("HRSMIN"), ui.GetDouble("HRSMAX"));
  }
  if (ui.GetBoolean("SETHISRANGE")) {
    importPds.SetHIS(ui.GetDouble("HISMIN"), ui.GetDouble("HISMAX"));
  }
  if (ui.GetBoolean("SETLRSRANGE")) {
    importPds.SetLRS(ui.GetDouble("LRSMIN"), ui.GetDouble("LRSMAX"));
  }
  if (ui.GetBoolean("SETLISRANGE")) {
    importPds.SetLIS(ui.GetDouble("LISMIN"), ui.GetDouble("LISMAX"));
  }

  importPds.SetOrganization(Isis::ProcessImport::BSQ);

  importPds.StartProcess();

  // Get the mapping labels
  Pvl otherLabels;
  importPds.TranslatePdsProjection(otherLabels);

  // Translate the remaining MI MAP labels
  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["Kaguya"] + "/translations/";
  
  FileName transFile(transDir + "kaguyaTcBandBin.trn");
  PvlToPvlTranslationManager bandBinXlater(label, transFile.expanded());
  bandBinXlater.Auto(otherLabels);
  
  transFile = transDir + "kaguyaTcInstrument.trn";
  PvlToPvlTranslationManager instXlater(label, transFile.expanded());
  instXlater.Auto(otherLabels);
  
  transFile = transDir + "kaguyaTcArchive.trn";
  PvlToPvlTranslationManager archiveXlater(label, transFile.expanded());
  archiveXlater.Auto(otherLabels);
  
  if ( otherLabels.hasGroup("Mapping") 
       && otherLabels.findGroup("Mapping").keywords() > 0 ) {
    outcube->putGroup(otherLabels.findGroup("Mapping"));
  }
  if ( otherLabels.hasGroup("Instrument") 
       && otherLabels.findGroup("Instrument").keywords() > 0 ) {
    outcube->putGroup(otherLabels.findGroup("Instrument"));
  }
  if ( otherLabels.hasGroup("BandBin") 
       && otherLabels.findGroup("BandBin").keywords() > 0 ) {

    PvlGroup &bandBinGroup = otherLabels.findGroup("BandBin");
    if (!bandBinGroup.hasKeyword("FilterName")) {
      bandBinGroup += PvlKeyword("FilterName", "BroadBand");
    }
    if (!bandBinGroup.hasKeyword("Center")) {
      bandBinGroup += PvlKeyword("Center", "640", "nanometers");
    }
    if (!bandBinGroup.hasKeyword("Width")) {
      bandBinGroup += PvlKeyword("Width", "420", "nanometers");
    }
    outcube->putGroup(bandBinGroup);
  }
  else {
    // Add the BandBin group
    PvlGroup bandBinGroup("BandBin");
    bandBinGroup += PvlKeyword("FilterName", "BroadBand");
    bandBinGroup += PvlKeyword("Center", "640nm");
    bandBinGroup += PvlKeyword("Width", "420nm");
    outcube->putGroup(bandBinGroup);
  }

  if ( otherLabels.hasGroup("Archive") 
       && otherLabels.findGroup("Archive").keywords() > 0 ) {
    outcube->putGroup(otherLabels.findGroup("Archive"));
  }

  importPds.EndProcess();
}
