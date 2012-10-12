#include "Isis.h"
#include "ProcessImportPds.h"
#include "UserInterface.h"
#include "FileName.h"
#include "Pvl.h"
#include "IException.h"
#include "TextFile.h"
#include <string>

using namespace std;
using namespace Isis;

void TranslateLunarLabels(FileName &labelFile, Cube *ocube);

void IsisMain() {
  ProcessImportPds p;
  Pvl label;
  UserInterface &ui = Application::GetUserInterface();
  FileName in = ui.GetFileName("FROM");

  //Checks if in file is rdr
  label = in.expanded();
  if(label.HasObject("IMAGE_MAP_PROJECTION")) {
    string msg = "[" + in.name() + "] appears to be an rdr file.";
    msg += " Use pds2isis.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  p.SetPdsFile(in.expanded(), "", label);
  Cube *ocube = p.SetOutputCube("TO");
  p.StartProcess();
  TranslateLunarLabels(in, ocube);
  p.EndProcess();

  return;
}

void TranslateLunarLabels(FileName &labelFile, Cube *ocube) {

  // Get the directory where the Lunar translation tables are.
  PvlGroup &dataDir = Preference::Preferences().FindGroup("DataDirectory");

  // Transfer the instrument group to the output cube
  IString transDir = (string) dataDir["Lo"] + "/translations/";
  Pvl inputLabel(labelFile.expanded());
  FileName transFile;
  FileName bandBinTransFile;

  bool hasFiducial = false;
  // Check to see if file is PDS
  if(inputLabel.HasKeyword("PDS_VERSION_ID", Pvl::None)) {
    IString pdsVersion = inputLabel.FindKeyword("PDS_VERSION_ID", Pvl::None)[0];

    if(pdsVersion == "PDS3") {
      if(inputLabel.HasKeyword("LO:FIDUCIAL_ID", Pvl::Traverse)) {
        hasFiducial = true;
        bandBinTransFile = transDir + "LoPdsFiducialImport.trn";
      }
      else if(inputLabel.HasKeyword("LO:BORESIGHT_SAMPLE", Pvl::Traverse)) {
        bandBinTransFile = transDir + "LoPdsBoresightImport.trn";
      }
      else {
        string msg = "[" + labelFile.name() + "] does not contain boresight or fiducial information";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    else {
      string msg = "[" + labelFile.name() + "] contains unknown PDS version [" +
                   pdsVersion + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }
  // Else the input is an Isis2 cube
  else {
    if(inputLabel.HasKeyword("FIDUCIAL_ID", Pvl::Traverse)) {
      hasFiducial = true;
      bandBinTransFile = transDir + "LoIsis2FiducialImport.trn";
    }
    else if(inputLabel.HasKeyword("BORESIGHT_SAMPLE", Pvl::Traverse)) {
      bandBinTransFile = transDir + "LoIsis2BoresightImport.trn";
    }
    else {
      string msg = "[" + labelFile.name() + "] does not contain boresight or fiducial information";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }

  transFile = transDir + "LoGeneralImport.trn";
  // Get the translation manager ready
  PvlTranslationManager commonlabelXlater(inputLabel, transFile.expanded());
  // Pvl outputLabels;
  Pvl *outputLabel = ocube->getLabel();
  commonlabelXlater.Auto(*(outputLabel));

  PvlTranslationManager labelXlater(inputLabel, bandBinTransFile.expanded());
  labelXlater.Auto(*(outputLabel));

  PvlGroup &inst = outputLabel->FindGroup("Instrument", Pvl::Traverse);

  //Creates FiducialCoordinateMicron with the proper units
  if(!inputLabel.HasKeyword("LO:BORESIGHT_SAMPLE", Pvl::Traverse)) {
    IString fcm = (string) inst.FindKeyword("FiducialCoordinateMicron");
    IString fcmUnits = fcm;
    fcmUnits = IString::TrimHead("0123456789.", fcmUnits);
    fcm = IString::TrimTail("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", fcm);
    inst.FindKeyword("FiducialCoordinateMicron").SetValue((int)fcm, fcmUnits);
  }

  // High Resolution & Fiducial Medium Case
  if(hasFiducial) {
    //Add units to some keywords
    PvlKeyword fxc = inst.FindKeyword("FiducialXCoordinates");
    inst.FindKeyword("FiducialXCoordinates").Clear();
    for(int i = 0; i < fxc.Size(); i++) {
      inst.FindKeyword("FiducialXCoordinates").AddValue(fxc[i], "mm");
    }

    PvlKeyword fyc = inst.FindKeyword("FiducialYCoordinates");
    inst.FindKeyword("FiducialYCoordinates").Clear();
    for(int i = 0; i < fyc.Size(); i++) {
      inst.FindKeyword("FiducialYCoordinates").AddValue(fyc[i], "mm");
    }

    PvlKeyword fl = inst.FindKeyword("FiducialLines");
    inst.FindKeyword("FiducialLines").Clear();
    for(int i = 0; i < fl.Size(); i++) {
      inst.FindKeyword("FiducialLines").AddValue(fl[i], "pixels");
    }

    PvlKeyword fs = inst.FindKeyword("FiducialSamples");
    inst.FindKeyword("FiducialSamples").Clear();
    for(int i = 0; i < fs.Size(); i++) {
      inst.FindKeyword("FiducialSamples").AddValue(fs[i], "pixels");
    }
  }
  else if(!hasFiducial) {
    //What needs to be done if it contains Boresight info
  }

  string instrumentID = inst.FindKeyword("InstrumentId");
  string spacecraftName = inst.FindKeyword("SpacecraftName");

  //Determines the NaifFrameCode
  PvlGroup kerns("Kernels");
  IString frameCode;
  if(spacecraftName.compare("Lunar Orbiter 3") == 0) {
    frameCode = "-533";
  }
  else if(spacecraftName.compare("Lunar Orbiter 4") == 0) {
    frameCode = "-534";
  }
  else if(spacecraftName.compare("Lunar Orbiter 5") == 0) {
    frameCode = "-535";
  }

  if(instrumentID == "High Resolution Camera") {
    frameCode += "001";
  }
  else if(instrumentID == "Medium Resolution Camera") {
    frameCode += "002";
  }

  //Create subframe and frame keywords
  IString imgNumber = (string) inst.FindKeyword("ImageNumber");
  int subFrame = ((IString)imgNumber.substr(5)).ToInteger();

  inst.AddKeyword(PvlKeyword("SubFrame", subFrame));
  //ImageNumber is auto translated, and no longer needed
  inst.DeleteKeyword("ImageNumber");

  kerns += PvlKeyword("NaifFrameCode", frameCode);
  outputLabel->FindObject("IsisCube").AddGroup(kerns);

  return;
}
