#include "Isis.h"

#include <cstdio>
#include <string>

#include "pds.h"
#include "ProcessByLine.h"
#include "PvlTranslationManager.h"
#include "SpecialPixel.h"
#include "UserInterface.h"
#include "Filename.h"
#include "iException.h"
#include "iTime.h"
#include "Preference.h"
#include "iString.h"
#include "OriginalLabel.h"

using namespace std; 
using namespace Isis;

void WriteLine (Buffer &b);
void TranslateLabels (Filename in, Cube *ocube);
PDSINFO *pdsi;

void IsisMain () {
  // Grab the file to import
  UserInterface &ui = Application::GetUserInterface();
  Filename in = ui.GetFilename("FROM");
  Filename out = ui.GetFilename("TO");
 
  // Make sure it is a Clementine EDR
  bool projected;
  try {
    Pvl lab(in.Expanded());
    projected = lab.HasObject("IMAGE_MAP_PROJECTION");
    iString id;
    id = (string)lab["DATA_SET_ID"];
    id.ConvertWhiteSpace();
    id.Compress();
    id.Trim(" ");
    if (id.find("CLEM") == string::npos) {
      string msg = "Invalid DATA_SET_ID [" + id + "]";
      throw iException::Message(iException::Pvl,msg,_FILEINFO_);
    }
  }
  catch (iException &e) {
    string msg = "Input file [" + in.Expanded() + 
                 "] does not appear to be " +
                 "in Clementine EDR format";
    throw iException::Message(iException::Io,msg, _FILEINFO_);
  }

  //Checks if in file is rdr
  if( projected ) {
    string msg = "[" + in.Name() + "] appears to be an rdr file.";
    msg += " Use pds2isis.";
    throw iException::Message(iException::User,msg, _FILEINFO_);
  }

  //Decompress the file
  long int lines = 0;
  long int samps = 0;
  iString filename = in.Expanded();
  pdsi = PDSR((char *)filename.c_str(),&lines,&samps);

  ProcessByLine p;
  CubeAttributeOutput cubeAtt("+unsignedByte+1.0:254.0");
  Cube *ocube = p.SetOutputCube(ui.GetFilename("TO"), cubeAtt, pdsi->image_ncols, pdsi->image_nrows);
  p.StartProcess (WriteLine);
  TranslateLabels(in, ocube);
  p.EndProcess ();
}

//Function to move uncompressed data to a cube
void WriteLine (Buffer &b){
  for (int i=0; i<pdsi->image_ncols; i++) {
    double d = pdsi->image[((b.Line()-1)*pdsi->image_ncols) + i];
    if(d<=0.0){
      b[i] = Isis::Lis;
    }
    else if(d >= 255.0){
      b[i] = Isis::His;
    }
    else{
      b[i] = d;
    }
  }
}

/**
 *  Function to propagate the labels.
 *  
 *  @internal
 *  @history  2009-02-17  Tracie Sucharski - Added BandBin keywords Center and
 *            Width to the translation table, Clementine.trn.  Do not alter this
 *            keywords for filter F,simply translate.
 *  
 */

void TranslateLabels (Filename in, Cube *ocube) {
  // Get the directory where the Clementine translation tables are.
  PvlGroup &dataDir = Preference::Preferences().FindGroup("DataDirectory");

  // Transfer the instrument group to the output cube
  iString transDir = (string) dataDir["clementine1"];
  Filename transFile (transDir + "/translations/clementine.trn");

  Pvl pdsLab(in.Expanded());
  PvlTranslationManager labelXlater (pdsLab, transFile.Expanded());

  // Pvl outputLabels;
  Pvl *outputLabel = ocube->Label();
  labelXlater.Auto(*(outputLabel));

  //Instrument group
  PvlGroup inst = outputLabel->FindGroup ("Instrument",Pvl::Traverse);

  PvlKeyword &startTime = inst.FindKeyword("StartTime");
  startTime.SetValue( startTime[0].substr(0, startTime[0].size()-1));

  // Old PDS labels used keyword INSTRUMENT_COMPRESSION_TYPE & PDS Labels now use ENCODING_TYPE
  if(pdsLab.FindObject("Image").HasKeyword("InstrumentCompressionType")){
    inst += PvlKeyword("EncodingFormat",(string) pdsLab.FindObject("Image")["InstrumentCompressionType"]);
  }
  else {
    inst += PvlKeyword("EncodingFormat",(string) pdsLab.FindObject("Image")["EncodingType"]);
  }

  if (((string)inst["InstrumentId"]) == "HIRES") {
    inst += PvlKeyword("MCPGainModeID", (string)pdsLab["MCP_Gain_Mode_ID"], "");
  }

  ocube->PutGroup(inst);

  PvlGroup bBin = outputLabel->FindGroup ("BandBin", Pvl::Traverse);
  std::string filter = pdsLab["FilterName"];
  if (filter != "F") {
    //Band Bin group
    double center = pdsLab["CenterFilterWavelength"];
    center /= 1000.0;
    bBin.FindKeyword("Center").SetValue(center,"micrometers");
  }
  double width = pdsLab["Bandwidth"];
  width /= 1000.0;
  bBin.FindKeyword("Width").SetValue(width,"micrometers");
  ocube->PutGroup(bBin);

  //Kernel group
  PvlGroup kern("Kernels");
  if (((string)inst["InstrumentId"]) == "HIRES") {
    kern += PvlKeyword("NaifFrameCode","-40001");
  }
  if (((string)inst["InstrumentId"]) == "UVVIS") {
    kern += PvlKeyword("NaifFrameCode","-40002");
  }
  if (((string)inst["InstrumentId"]) == "NIR") {
    kern += PvlKeyword("NaifFrameCode","-40003");
  }
  if (((string)inst["InstrumentId"]) == "LWIR") {
    kern += PvlKeyword("NaifFrameCode","-40004");
  }
  ocube->PutGroup(kern);

  OriginalLabel org(pdsLab);
  ocube->Write(org);
}
