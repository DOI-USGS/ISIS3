#include "Isis.h"

#include "ProcessImportPds.h"


/**
 * @author 2006-01-23 Brian Lipkowitz
 * @author 2006-01-23 Robert Wallace
 */

using namespace std; 
using namespace Isis;

void IsisMain() {
  void TranslateMerEdrLabels (Filename &labelFile, Cube *ocube);

  UserInterface &ui = Application::GetUserInterface();

  Filename input = Filename (ui.GetFilename ("FROM"));

  //Checks if in file is rdr
  Pvl lab(input.Expanded());
  if( lab.HasObject("IMAGE_MAP_PROJECTION") ) {
    string msg = "[" + input.Name() + "] has already been projected.";
    msg += " Use pds2isis.";
    throw iException::Message(iException::User,msg, _FILEINFO_);
  }

  iString output;
  if ( ui.WasEntered("TO")) {
    output = ui.GetFilename("TO");
  }
  else {
    output = input.Path() + input.Basename() + ".cub";
  }

  Pvl inputFile;

  iString paramaters = "FROM=" + input.Expanded();
  paramaters += " TO=" + output;

  ProcessImportPds p;
  p.SetPdsFile(input.Expanded(),"",inputFile);

  Cube *ocube = p.SetOutputCube("TO");
  p.StartProcess ();
  TranslateMerEdrLabels(input,ocube);
  p.EndProcess();
}

void TranslateMerEdrLabels (Filename &labelFile, Cube *ocube) {
  //Create a PVL to store the translated labels
  Pvl outLabel;

  // Get the directory where the MER translation tables are.
  PvlGroup dataDir (Preference::Preferences().FindGroup("DataDirectory"));
  iString transDir = (string) dataDir["Mer"];
  transDir = transDir + "/" + "translations/";

  // Get a filename for the MESSENGER EDR label
  Pvl labelPvl (labelFile.Expanded());
  Filename transFile;

  // Translate the Archive group
  transFile = transDir + "merStructure.trn";
  PvlTranslationManager structXlater (labelPvl, transFile.Expanded());
  structXlater.Auto (outLabel);
  ocube->PutGroup (outLabel.FindGroup("ARCHIVE", Pvl::Traverse));

  // Translate the Instrument group
  transFile = transDir + "merInstrument.trn";
  PvlTranslationManager instrumentXlater (labelPvl, transFile.Expanded());
  instrumentXlater.Auto (outLabel);
  ocube->PutGroup (outLabel.FindGroup("INSTRUMENT", Pvl::Traverse));

  // Pull out MiCCD and MiElectronic from the TemperatureName in the instrument group
  void MiFixLab(PvlGroup & instGroup);
  PvlGroup instGroup = ocube->GetGroup("INSTRUMENT");
  MiFixLab(instGroup);
  ocube->PutGroup(instGroup);

  // Translate the Image_Request group
  transFile = transDir + "merImageRequest.trn";
  PvlTranslationManager imageReqXlater (labelPvl, transFile.Expanded());
  imageReqXlater.Auto (outLabel);
  ocube->PutGroup (outLabel.FindGroup("MER_IMAGE_REQUEST_PARMS", Pvl::Traverse));
  
  // Translate the Subframe group
  transFile = transDir + "merSubframe.trn";
  PvlTranslationManager subframeXlater (labelPvl, transFile.Expanded());
  subframeXlater.Auto (outLabel);
  ocube->PutGroup (outLabel.FindGroup("MER_SUBFRAME_REQUEST_PARMS", Pvl::Traverse));
}

void MiFixLab( PvlGroup & instGroup ) {
  // code to get instrment and electronics temperatures.
  PvlKeyword temp = instGroup.FindKeyword("InstrumentTemperature");
  PvlKeyword tempName = instGroup.FindKeyword("InstrumentTemperatureName");

  PvlKeyword miCCD;
  miCCD.SetName( "TemperatureMiCCD");
  miCCD.SetValue(temp[6]);
  instGroup.AddKeyword(miCCD);

  PvlKeyword miElectornic;
  miElectornic.SetName( "TemperatureMiElectronics");
  miElectornic.SetValue(temp[7]);
  instGroup.AddKeyword(miElectornic);

  //Code to remove "Z" from the StartTime and StopTime keywords 
  //StartTime code
  iString Newstarttime = (string)instGroup.FindKeyword("StartTime");
  Newstarttime.Remove("Z");
  instGroup.FindKeyword("StartTime").SetValue(Newstarttime);
  //StopTime code
  iString Newstoptime = (string)instGroup.FindKeyword("StopTime");
  Newstoptime.Remove("Z");
  instGroup.FindKeyword("StopTime").SetValue(Newstoptime);
}
