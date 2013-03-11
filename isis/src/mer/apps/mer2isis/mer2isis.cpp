#include "Isis.h"

#include "ProcessImportPds.h"


/**
 * @author 2006-01-23 Brian Lipkowitz
 * @author 2006-01-23 Robert Wallace
 */

using namespace std;
using namespace Isis;

void IsisMain() {
  void TranslateMerEdrLabels(FileName & labelFile, Cube * ocube);

  UserInterface &ui = Application::GetUserInterface();

  FileName input = FileName(ui.GetFileName("FROM"));

  //Checks if in file is rdr
  Pvl lab(input.expanded());
  if(lab.hasObject("IMAGE_MAP_PROJECTION")) {
    QString msg = "[" + input.name() + "] has already been projected.";
    msg += " Use pds2isis.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  QString output;
  if(ui.WasEntered("TO")) {
    output = ui.GetFileName("TO");
  }
  else {
    output = input.path() + input.baseName() + ".cub";
  }

  Pvl inputFile;

  QString paramaters = "FROM=" + input.expanded();
  paramaters += " TO=" + output;

  ProcessImportPds p;
  p.SetPdsFile(input.expanded(), "", inputFile);

  Cube *ocube = p.SetOutputCube("TO");
  p.StartProcess();
  TranslateMerEdrLabels(input, ocube);
  p.EndProcess();
}

void TranslateMerEdrLabels(FileName &labelFile, Cube *ocube) {
  //Create a PVL to store the translated labels
  Pvl outLabel;

  // Get the directory where the MER translation tables are.
  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["Mer"];
  transDir = transDir + "/" + "translations/";

  // Get a filename for the MESSENGER EDR label
  Pvl labelPvl(labelFile.expanded());
  FileName transFile;

  // Translate the Archive group
  transFile = transDir + "merStructure.trn";
  PvlTranslationManager structXlater(labelPvl, transFile.expanded());
  structXlater.Auto(outLabel);
  ocube->putGroup(outLabel.findGroup("ARCHIVE", Pvl::Traverse));

  // Translate the Instrument group
  transFile = transDir + "merInstrument.trn";
  PvlTranslationManager instrumentXlater(labelPvl, transFile.expanded());
  instrumentXlater.Auto(outLabel);
  ocube->putGroup(outLabel.findGroup("INSTRUMENT", Pvl::Traverse));

  // Pull out MiCCD and MiElectronic from the TemperatureName in the instrument group
  void MiFixLab(PvlGroup & instGroup);
  PvlGroup instGroup = ocube->group("INSTRUMENT");
  MiFixLab(instGroup);
  ocube->putGroup(instGroup);

  // Translate the Image_Request group
  transFile = transDir + "merImageRequest.trn";
  PvlTranslationManager imageReqXlater(labelPvl, transFile.expanded());
  imageReqXlater.Auto(outLabel);
  ocube->putGroup(outLabel.findGroup("MER_IMAGE_REQUEST_PARMS", Pvl::Traverse));

  // Translate the Subframe group
  transFile = transDir + "merSubframe.trn";
  PvlTranslationManager subframeXlater(labelPvl, transFile.expanded());
  subframeXlater.Auto(outLabel);
  ocube->putGroup(outLabel.findGroup("MER_SUBFRAME_REQUEST_PARMS", Pvl::Traverse));
}

void MiFixLab(PvlGroup &instGroup) {
  // code to get instrment and electronics temperatures.
  PvlKeyword temp = instGroup.findKeyword("InstrumentTemperature");
  PvlKeyword tempName = instGroup.findKeyword("InstrumentTemperatureName");

  PvlKeyword miCCD;
  miCCD.setName("TemperatureMiCCD");
  miCCD.setValue(temp[6]);
  instGroup.addKeyword(miCCD);

  PvlKeyword miElectornic;
  miElectornic.setName("TemperatureMiElectronics");
  miElectornic.setValue(temp[7]);
  instGroup.addKeyword(miElectornic);

  //Code to remove "Z" from the StartTime and StopTime keywords
  //StartTime code
  QString Newstarttime = (QString)instGroup.findKeyword("StartTime");
  Newstarttime.remove("Z");
  instGroup.findKeyword("StartTime").setValue(Newstarttime);
  //StopTime code
  QString Newstoptime = (QString)instGroup.findKeyword("StopTime");
  Newstoptime.remove("Z");
  instGroup.findKeyword("StopTime").setValue(Newstoptime);
}
