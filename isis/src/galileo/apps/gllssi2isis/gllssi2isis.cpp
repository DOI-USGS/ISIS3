#include "Isis.h"

#include <cstdio>
#include <string>

#include "ProcessImportPds.h"

#include "UserInterface.h"
#include "CubeAttribute.h"
#include "Filename.h"
#include "iException.h"
#include "iTime.h"

using namespace std; 
using namespace Isis;
bool summed;

Cube *summedOutput;

void TranslateData(Buffer &inData);
void TranslateLabels (Pvl &pdsLabel, Cube *ocube);

void IsisMain (){

  //initialize globals
  summed = false; 
  summedOutput = NULL;
  // Grab the file to import
  ProcessImportPds p;
  UserInterface &ui = Application::GetUserInterface();
  Filename inFile = ui.GetFilename("FROM");
  Filename out = ui.GetFilename("TO");

  // Make sure it is a Galileo SSI image
  Pvl lab(inFile.Expanded());

  //Checks if in file is rdr
  if( lab.HasObject("IMAGE_MAP_PROJECTION") ) {
    string msg = "[" + inFile.Name() + "] appears to be an rdr file.";
    msg += " Use pds2isis.";
    throw iException::Message(iException::Io,msg, _FILEINFO_);
  }

  // data set id value must contain "SSI-2-REDR-V1.0"(valid SSI image) 
  // or "SSI-4-REDR-V1.0"(reconstructed from garbled SSI image)
  string dataSetId;
  dataSetId = (string)lab["DATA_SET_ID"];
  try {
    if (dataSetId.find("SSI-2-REDR-V1.0") == string::npos
        && dataSetId.find("SSI-4-REDR-V1.0") == string::npos) {
      string msg = "Invalid DATA_SET_ID [" + dataSetId + "]";
      throw iException::Message(iException::Pvl,msg,_FILEINFO_);
    }
  }
  catch (iException &e) {
    string msg = "Unable to read [DATA_SET_ID] from input file [" +
                 inFile.Expanded() + "]";
    throw iException::Message(iException::Io,msg,_FILEINFO_);
  }

  // set summing mode 
  if(ui.GetString("FRAMEMODE") == "AUTO") {
    double frameDuration = lab["FRAME_DURATION"];
    // reconstructed images are 800x800 (i.e. not summed)
    // even though they have frame duration of 2.333 
    // (which ordinarily indicates a summed image)
    if (dataSetId.find("SSI-4-REDR-V1.0") != string::npos) {
      summed = false; 
    }
    else if (frameDuration > 2.0 && frameDuration < 3.0) {
      summed = true;
    }
    // seti documentation implies valid frame duration values are 2.333, 8.667, 30.333, 60.667 
    // however some images have value 15.166 (see example 3700R.LBL)
    else if (frameDuration > 15.0 && frameDuration < 16.0) {
      summed = true;
    }
  }
  else if(ui.GetString("FRAMEMODE") == "SUMMED") {
    summed = true;
  }
  else {
    summed = false;
  }

  Progress prog;
  Pvl pdsLabel;
  p.SetPdsFile(inFile.Expanded(),"",pdsLabel);

  //Set up the output file
  Cube *ocube;

  if(!summed) {
    ocube = p.SetOutputCube("TO");
    p.StartProcess();
  }
  else {
    summedOutput = new Cube();
    summedOutput->SetDimensions(p.Samples()/2, p.Lines()/2, p.Bands());
    summedOutput->SetPixelType(p.PixelType());
    summedOutput->Create(ui.GetFilename("TO"));
    p.StartProcess(TranslateData);
    ocube = summedOutput;
  }

  TranslateLabels(pdsLabel, ocube);
  p.EndProcess ();

  if(summed) {
    summedOutput->Close();
    delete summedOutput;
  }

  return;
}

void TranslateData(Buffer &inData) {
  summedOutput->Write(inData);
}

void TranslateLabels(Pvl &pdsLabel, Cube *ocube) {
  // Get the directory where the MOC translation tables are.
  PvlGroup &dataDir = Preference::Preferences().FindGroup("DataDirectory");

  // Transfer the instrument group to the output cube
  iString transDir = (string) dataDir["Galileo"];
  Filename transFile (transDir + "/translations/galileoSsi.trn");

  // Get the translation manager ready
  PvlTranslationManager labelXlater (pdsLabel, transFile.Expanded());
  // Pvl outputLabels;
  Pvl *outputLabel = ocube->Label();
  labelXlater.Auto(*(outputLabel));

  //Add to the Archive Group
  PvlGroup &arch = outputLabel->FindGroup("Archive",Pvl::Traverse);
  PvlGroup &inst = outputLabel->FindGroup("Instrument",Pvl::Traverse);
  arch.AddKeyword(PvlKeyword("DataType","RADIANCE"));
  string CTC = (string) arch.FindKeyword("ObservationId");
  string CTCout = CTC.substr(0,2);
  arch.AddKeyword(PvlKeyword("CalTargetCode",CTCout));

  // Add to the Instrument Group
  iString itest =(string) inst.FindKeyword("StartTime");
  itest.Remove("Z");
  inst.FindKeyword("StartTime").SetValue(itest);
  //change exposure duration to seconds
  double expDur = inst.FindKeyword("exposureDuration");
  double expDurOut = expDur / 1000.0;
  inst.FindKeyword("exposureDuration").SetValue(expDurOut,"seconds");
  inst.AddKeyword(PvlKeyword("FrameDuration",
                     (string) pdsLabel["frameDuration"],"seconds"));

  //Calculate the Frame_Rate_Id keyword
  string frameModeId = "FULL";
  int summingMode = 1;

  if(summed) {
    frameModeId = "SUMMATION";
    summingMode = 2;
  }

  inst.AddKeyword(PvlKeyword("Summing",summingMode));
  inst.AddKeyword(PvlKeyword("FrameModeId",frameModeId));

  // Create the Band bin Group
  PvlGroup &bandBin = outputLabel->FindGroup("BandBin",Pvl::Traverse);
  string filterName = pdsLabel["FILTER_NAME"];
  string waveLength = "";
  string width = "";
  if (filterName == "CLEAR") {
    waveLength = "0.611";
    width = ".44";
  }
  if (filterName == "VIOLET") {
    waveLength = "0.404";
    width = ".05";
  }
  if (filterName == "GREEN") {
    waveLength = "0.559";
    width = ".06";
  }
  if (filterName == "RED") {
    waveLength = "0.671";
    width = ".06";
  }
  if (filterName == "IR-7270") {
    waveLength = "0.734";
    width = ".01";
  }
  if (filterName == "IR-7560") {
    waveLength = "0.756";
    width = ".018";
  }
  if (filterName == "IR-8890") {
    waveLength = "0.887";
    width = ".116";
  }
  if (filterName == "INFRARED") {
     waveLength = "0.986";
     width = ".04";
  }
  bandBin.AddKeyword(PvlKeyword("Center",waveLength, "micrometers"));
  bandBin.AddKeyword(PvlKeyword("Width",width,"micrometers"));

  //create the kernel group
  PvlGroup kern("Kernels");
  kern += PvlKeyword("NaifFrameCode",-77001);
  ocube->PutGroup(kern);
}
