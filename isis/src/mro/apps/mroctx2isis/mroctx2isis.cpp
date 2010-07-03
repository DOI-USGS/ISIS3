#include "Isis.h"
#include "ProcessImportPds.h"
#include "ProcessByLine.h"
#include "UserInterface.h"
#include "Filename.h"
#include "iException.h"
#include "Preference.h"
#include "iString.h"
#include "Pvl.h"
#include "Table.h"
#include "Stretch.h"
#include "TextFile.h"
#include "PixelType.h"

using namespace std;
using namespace Isis;

// Global variables for processing functions
Stretch stretch;

void FixDns8 (Buffer &buf);
void TranslateMroCtxLabels (Filename &labelFile, Cube *ocube);
void SaveDarkData(ProcessImportPds &process, Cube *ocube, int startPix, int endPix);
vector<int> ConvertDarkPixels (int samples, Isis::PixelType pixelType,
                               unsigned char *data);
bool fillGap;

void IsisMain (){
  ProcessImportPds p;
  
  //Check that the file comes from the right camera
  UserInterface &ui = Application::GetUserInterface();
  Filename inFile = ui.GetFilename("FROM");
  fillGap = false;
  iString id,bitMode;
  int sumMode,editMode;
  bool projected;
  try {
    Pvl lab(inFile.Expanded());
    id = (string) lab.FindKeyword ("DATA_SET_ID");
    projected = lab.HasObject("IMAGE_MAP_PROJECTION");
    if (lab.HasKeyword("SPATIAL_SUMMING")) {
      sumMode = (int)lab.FindKeyword("SPATIAL_SUMMING");
    }
    else {
      sumMode = (int)lab.FindKeyword("SAMPLING_FACTOR");
    }

    bitMode = (string) lab.FindKeyword("SAMPLE_BIT_MODE_ID");
    if (lab.HasKeyword("EDIT_MODE_ID")) {
      editMode = (int)lab.FindKeyword("EDIT_MODE_ID");
    }
    else {
      editMode = (int)lab.FindKeyword("SAMPLE_FIRST_PIXEL");
    }

  }
  catch (iException &e) {
    string msg = "Unable to read [DATA_SET_ID] from input file [" +
                 inFile.Expanded() + "]";
    throw iException::Message(iException::Io,msg, _FILEINFO_);
  }

  //Checks if in file is rdr
  if( projected ) {
    string msg = "[" + inFile.Name() + "] appears to be an rdr file.";
    msg += " Use pds2isis.";
    throw iException::Message(iException::User,msg, _FILEINFO_);
  }

  id.ConvertWhiteSpace();
  id.Compress();
  id.Trim(" ");
  if (id != "MRO-M-CTX-2-EDR-L0-V1.0") {
    string msg = "Input file [" + inFile.Expanded() + "] does not appear to be " +
                 "in MRO-CTX EDR format. DATA_SET_ID is [" + id + "]";
    throw iException::Message(iException::Io,msg, _FILEINFO_);
  }

  // Check to make sure the SampleBitModeId is SQROOT 
  if (bitMode != "SQROOT") {
    string msg = "Can't handle Sample Bit Mode [" + bitMode + "]";
    throw iException::Message(iException::User,msg, _FILEINFO_);
  }

  //Process the file
  Pvl pdsLab;
  p.SetPdsFile(inFile.Expanded(), "", pdsLab);

  int startPix = 0;
  int endPix = 0;
  int suf = 0;

  // Set the data prefix and suffix values
  if (ui.WasEntered("PREFIX")) {
    endPix = ui.GetInteger("PREFIX");
  }
  else {
    if (sumMode == 1) {
      if (editMode == 0) {
        startPix = 14;
        endPix = 37;
      }
      else {
        startPix = 0;
        endPix = 15;
      }
    }
    else if (sumMode == 2) {
      if (editMode == 0) {
        startPix = 7;
        endPix = 18;
      }
      else {
        startPix = 0;
        endPix = 7;
      }
    }
  }
  if (ui.WasEntered("SUFFIX")) {
    suf = ui.GetInteger("SUFFIX");
  }
  else {
    if (sumMode == 1) {
      if (editMode == 0) {
        suf = 18;
      }
      else {
        suf = 0;
      }
    }
    else if (sumMode == 2) {
      if (editMode == 0) {
        suf = 9;
      }
      else {
        suf = 0;
      }
    }
  }
  p.SetDataPrefixBytes(endPix+1);
  p.SetDataSuffixBytes(suf);
  int samps = p.Samples() - endPix - suf - 1;
  p.SetDimensions(samps,p.Lines(),p.Bands());

  // Save off the dark pixel data
  p.SaveDataPrefix();
  
  // Set the output bit type to Real
  CubeAttributeOutput &outAtt = ui.GetOutputAttribute("TO");
  outAtt.PixelType (Isis::SignedWord);
  outAtt.Minimum((double)VALID_MIN2);
  outAtt.Maximum((double)VALID_MAX2);
  Cube *ocube = p.SetOutputCube(ui.GetFilename("TO"), outAtt);

  // Translate the labels
  p.StartProcess();
  TranslateMroCtxLabels(inFile, ocube);

  // Set up the strech for the 8 to 12 bit conversion from file
  Filename *temp = new Filename("$mro/calibration/ctxsqroot_???.lut");
  temp->HighestVersion();
  TextFile *stretchPairs = new TextFile(temp->Expanded());

  // Create the stretch pairs
  stretch.ClearPairs();
  for (int i=0; i<stretchPairs->LineCount(); i++) {
    iString line;
    stretchPairs->GetLine(line,true);
    int temp1 = line.Token(" ");
    int temp2 = line.Trim(" ");
    stretch.AddPair(temp1,temp2);
  }

  stretchPairs->Close();

  SaveDarkData(p, ocube, startPix, endPix);
  p.EndProcess();

  // Do 8 bit to 12 bit conversion
  fillGap = ui.GetBoolean("FILLGAP");
  ProcessByLine p2;
  string ioFile = ui.GetFilename("TO");
  CubeAttributeInput att;
  p2.SetInputCube(ioFile, att, ReadWrite);
  p2.Progress()->SetText("Converting 8 bit pixels to 16 bit");
  p2.StartProcess(FixDns8);
  p2.EndProcess();
}

// The input buffer has a raw 16 bit buffer but the values are still 0 to 255
void FixDns8 (Buffer &buf) {
  for (int i=0; i<buf.size(); i++) {
    if(!fillGap || buf[i] != 0) {
      buf[i] = stretch.Map(buf[i]);
    }
    else {
      buf[i] = Isis::Null;
    }
  }
}

//Function to translate the labels
void TranslateMroCtxLabels(Filename &labelFile, Cube *ocube){

  //Pvl to store the labels
  Pvl outLabel;
  //Set up the directory where the translations are
  PvlGroup dataDir (Preference::Preferences().FindGroup("DataDirectory"));
  iString transDir = (string) dataDir["Mro"] + "/translations/";
  Pvl labelPvl (labelFile.Expanded());

  //Translate the Instrument group
  Filename transFile (transDir + "mroctxInstrument.trn");
  PvlTranslationManager instrumentXlator (labelPvl, transFile.Expanded());
  instrumentXlator.Auto(outLabel);

  //Translate the Archive grooup
  transFile  = transDir + "mroctxArchive.trn";
  PvlTranslationManager archiveXlater (labelPvl, transFile.Expanded());  
  archiveXlater.Auto (outLabel);

  // Set up the BandBin groups
  PvlGroup bbin("BandBin");
  bbin += PvlKeyword("FilterName","BroadBand");
  bbin += PvlKeyword("Center",0.650,"micrometers");
  bbin += PvlKeyword("Width",0.150,"micrometers");

  //Set up the Kernels group
  PvlGroup kern("Kernels");
  kern += PvlKeyword("NaifFrameCode",-74021);

  Pvl lab(labelFile.Expanded());
  int sumMode, startSamp;
  if (lab.HasKeyword("SPATIAL_SUMMING")) {
    sumMode = (int)lab.FindKeyword("SPATIAL_SUMMING");
  }
  else {
    sumMode = (int)lab.FindKeyword("SAMPLING_FACTOR");
  }
  if (lab.HasKeyword("EDIT_MODE_ID")) {
    startSamp = (int)lab.FindKeyword("EDIT_MODE_ID");
  }
  else {
    startSamp = (int)lab.FindKeyword("SAMPLE_FIRST_PIXEL");
  }
  PvlGroup inst = outLabel.FindGroup("Instrument", Pvl::Traverse);
  inst += PvlKeyword("SpatialSumming",sumMode);
  inst += PvlKeyword("SampleFirstPixel",startSamp);

  //Add all groups to the output cube
  ocube->PutGroup (inst);
  ocube->PutGroup (outLabel.FindGroup("Archive", Pvl::Traverse));
  ocube->PutGroup (bbin);
  ocube->PutGroup (kern);
}

void SaveDarkData(ProcessImportPds &process, Cube *ocube, int startPix, int endPix) {
  int pixNum = endPix - startPix + 1;
  TableField dark("DarkPixels", TableField::Integer, pixNum);
  TableRecord darkRecord;
  darkRecord += dark;
  Table darkTable("Ctx Prefix Dark Pixels", darkRecord);
  vector<vector<char *> > pre = process.DataPrefix();
  vector<char *> prefix = pre.at(0);

  for (int l=0; l<(int)prefix.size(); l++) {
    unsigned char *linePrefix = (unsigned char *)(prefix[l]+startPix*SizeOf(process.PixelType()));
    darkRecord[0] = ConvertDarkPixels(pixNum, process.PixelType(),linePrefix);
    darkTable += darkRecord;
  }
  ocube->Write(darkTable);
}

vector<int> ConvertDarkPixels (int samples, Isis::PixelType pixelType,
                               unsigned char *data) {
  Isis::Buffer pixelBuf(samples, 1, 1, Isis::SignedWord);

  for (int b=0; b<samples; b++) {
    short int pixel = data[b];
    ((short int*)pixelBuf.RawBuffer())[b] = pixel;
    pixelBuf[b] = pixel;
  }
  FixDns8(pixelBuf);

  // Move the calibration pixels from the buffer to a vector
  vector<int> calibrationPixels;
  double pixel;
  for (int b=0; b<samples; b++) {
    pixel = pixelBuf[b];
    if (pixel == NULL8) calibrationPixels.push_back(NULL2);
    else if (pixel == LOW_REPR_SAT8) calibrationPixels.push_back(LOW_REPR_SAT2);
    else if (pixel == LOW_INSTR_SAT8) calibrationPixels.push_back(LOW_INSTR_SAT2);
    else if (pixel == HIGH_INSTR_SAT8) calibrationPixels.push_back(HIGH_INSTR_SAT2);
    else if (pixel == HIGH_REPR_SAT8) calibrationPixels.push_back(HIGH_REPR_SAT2);
    else calibrationPixels.push_back((int)(pixel+0.5));
  }

  return calibrationPixels;
}
