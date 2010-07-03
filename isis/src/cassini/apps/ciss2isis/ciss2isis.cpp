//Source:  Cassini ISS Tour VICAR Image Data File and Detatched PDS Label SIS, Tour Version 1.1 December 1, 2004

#include "Isis.h"

#include <string>
#include <vector>

#include "CisscalFile.h"
#include "Cube.h"
#include "Filename.h"
#include "Preference.h"
#include "ProcessImportPds.h"
#include "ProcessByLine.h"
#include "Pvl.h"
#include "SpecialPixel.h"
#include "Stretch.h"
#include "Table.h"
#include "UserInterface.h"
#include "TextFile.h"
#include "iException.h"
#include "iString.h"


using namespace std;
using namespace Isis;

// Function prototypes
double ComputeOverclockAvg(vector <double> pixel);
vector<double> ConvertLinePrefixPixels (unsigned char *data);
Table CreateLinePrefixTable(vector<char *> prefixData);
void CreateStretchPairs();
void FixDns (Buffer &buf);
void TranslateCassIssLabels (Filename &labelFile, Cube *ocube);
//Global variables
string compressionType;
string dataConversionType;
double flightSoftware;
Stretch stretch;
int sumMode;
int validMax;

void IsisMain ()
{
  //PROCESS 1: saves off label, header, and line prefix data ==========================================//
  ProcessImportPds p;
  Pvl label;
  UserInterface &ui = Application::GetUserInterface();
  Filename in = ui.GetFilename("FROM");

  p.SetPdsFile (in.Expanded(), "", label);

  //Checks if in file is rdr
  if( label.HasObject("IMAGE_MAP_PROJECTION") ) {
    string msg = "[" + in.Name() + "] appears to be an rdr file.";
    msg += " Use pds2isis.";
    throw iException::Message(iException::User,msg, _FILEINFO_);
  }

  // Set the output bit type to SignedWord
  CubeAttributeOutput &outAtt = ui.GetOutputAttribute("TO");
  outAtt.PixelType (SignedWord);
  outAtt.Minimum((double)VALID_MIN2);
  outAtt.Maximum((double)VALID_MAX2);
  Cube *ocube = p.SetOutputCube(ui.GetFilename("TO"), outAtt);

  TranslateCassIssLabels(in, ocube);

  //Save off header (includes vicar labels and binary telemetry header)
  // No need to SetFileHeaderBytes() this is already done by ProcessImportPds automatically
  int vicarLabelBytes = label.FindObject("IMAGE_HEADER").FindKeyword("BYTES");
  p.SaveFileHeader();

  //Save off line prefix data, always 24 bytes of binary prefix per line,see SIS version 1.1 pg 103
  int linePrefixBytes = label.FindObject("IMAGE").FindKeyword("LINE_PREFIX_BYTES");
  p.SetDataPrefixBytes(linePrefixBytes);
  p.SaveDataPrefix();

  //SET PROGRESS TEXT, VALID MAXIMUM PIXEL VALUE, AND CREATE STRETCH IF NEEDED
  if (dataConversionType != "Table") {  //Conversion Type is 12Bit or 8LSB, only save off overclocked pixels
    validMax = 255;
    if (dataConversionType == "12Bit") {  
      p.Progress()->SetText("Image was 12 bit. No conversion needed. \nSaving line prefix data...");
    }
    else { //if (dataConversionType == "8LSB") {
      p.Progress()->SetText("Image was truncated to 8 least significant bits. No conversion needed. \nSaving line prefix data...");
    }
  }
  else {  //if ConversionType == Table, Use LUT to create stretch pairs for conversion
    validMax = 4095;
    CreateStretchPairs();
    // Pvl outputLabels;
    Pvl *outputLabel = ocube->Label();
    //Adjust Table-encoded values from 8 bit back to 12 bit.
    PvlGroup &inst = outputLabel->FindGroup("Instrument",Pvl::Traverse);
    double biasStripMean = inst.FindKeyword("BiasStripMean");
    inst.FindKeyword("BiasStripMean").SetValue(stretch.Map(biasStripMean));
    inst.FindKeyword("BiasStripMean").AddComment("BiasStripMean value converted back to 12 bit.");
    p.Progress()->SetText("Image was converted using 12-to-8 bit table. \nConverting prefix pixels back to 12 bit and saving line prefix data...");
  }

  p.StartProcess ();  

  // Write line prefix data to table in output cube
  vector<vector<char *> > dataPrefix = p.DataPrefix();
  vector<char *> prefixBand0 = dataPrefix.at(0); //There is only one band so the outside vector only contains 
                                                 // one entry and the inside vector only contains nl entries
  Table linePrefixTable = CreateLinePrefixTable(prefixBand0);
  ocube->Write (linePrefixTable);
  // Compute readout order (roo) and save to output cube's instrument group
  unsigned char *header =  (unsigned char *) p.FileHeader();
  int roo = *(header+50+vicarLabelBytes)/32 % 2;//**** THIS MAY NEED TO BE CHANGED, 
                                                // SEE BOTTOM OF THIS FILE FOR IN DEPTH COMMENTS ON READOUTORDER
  PvlGroup &inst = ocube->Label()->FindGroup("Instrument",Pvl::Traverse);
  inst.AddKeyword(PvlKeyword("ReadoutOrder",roo));
  p.EndProcess();              

  // PROCESS 2 : Do 8 bit to 12 bit conversion for image ==============================================//
  ProcessByLine p2;
  string ioFile = ui.GetFilename("TO");
  CubeAttributeInput att;
  p2.SetInputCube(ioFile, att, ReadWrite);
  //if ConversionType == 12Bit or 8LSB, only save off overclocked pixels
  if (dataConversionType == "12Bit") {  
    p2.Progress()->SetText("Setting special pixels and saving as 16bit...");
  }
  else if (dataConversionType == "8LSB") {
    p2.Progress()->SetText("Setting special pixels and saving as 16bit...");
  }
  //if ConversionType == Table, Use LUT to create stretch pairs for conversion
  else {  
    p2.Progress()->SetText("Converting image pixels back to 12-bit and saving as 16bit...");
  }
  p2.StartProcess(FixDns);
  p2.EndProcess();
  return;
}

//call this method after stretch is closed in IsisMain()
// write data into table to save in output cube 
// author Jeannie Walldren 2008-08-21
Table CreateLinePrefixTable(vector<char *> prefixData) {
  TableField overclockPixels("OverclockPixels", TableField::Double, 3);
  //3 columns, first two are overclocked pixels and the third is their average
  TableRecord linePrefixRecord;
  linePrefixRecord += overclockPixels;
  Table linePrefixTable("ISS Prefix Pixels", linePrefixRecord);
  linePrefixTable.SetAssociation(Table::Lines);
  for (int l=0; l<(int)prefixData.size(); l++) {
    unsigned char *linePrefix = (unsigned char *)(prefixData[l]);
    linePrefixRecord[0] = ConvertLinePrefixPixels(linePrefix);
    linePrefixTable += linePrefixRecord;
  }
  return linePrefixTable;
}

//used by CreateLinePrefixTable() to convert prefix data 
// author Jeannie Walldren 2008-08-21
vector<double> ConvertLinePrefixPixels (unsigned char *data) {
  Buffer pixelBuf(1, 1, 1, SignedWord);

  vector<double> calibrationPixels;
  //Pixel data is MSB, see SIS version 1.1 page 17
  EndianSwapper swapper ("MSB");

  vector<double> pixel;
  //12 is start byte for First Overclocked Pixel Sum in Binary Line Prefix, SIS version 1.1 page 94
  pixel.push_back(swapper.ShortInt(& (data[12])));
  //22 is start byte for Last Overclocked Pixel Sum in Binary Line Prefix, see SIS version 1.1 page 94
  pixel.push_back(swapper.ShortInt(& (data[22])));
  pixel.push_back(ComputeOverclockAvg(pixel));
  for (int i = 0; i < (int)pixel.size(); i++) {
    pixelBuf[0] = pixel[i];
    //  Do 8 bit to 12 bit conversion for prefix data
    FixDns(pixelBuf);
    double pix = pixelBuf[0];
    if (pix == NULL8) {
      calibrationPixels.push_back(NULL2);
    }
    else if (pix == LOW_REPR_SAT8) {
      calibrationPixels.push_back(LOW_REPR_SAT2);
    }
    else if (pix == LOW_INSTR_SAT8) {
      calibrationPixels.push_back(LOW_INSTR_SAT2);
    }
    else if (pix == HIGH_INSTR_SAT8) {
      calibrationPixels.push_back(HIGH_INSTR_SAT2);
    }
    else if (pix == HIGH_REPR_SAT8) {
      calibrationPixels.push_back(HIGH_REPR_SAT2);
    }
    else {
      calibrationPixels.push_back(pix);
    }
  }
  return calibrationPixels;
}

// Called in IsisMain() if DataConversionType == Table
// Creates stretch pairs for mapping in Fix Dns
// author Jeannie Walldren 2008-08-21
void CreateStretchPairs() {
  // Set up the strech for the 8 to 12 bit conversion from file
  PvlGroup &dataDir = Preference::Preferences().FindGroup("DataDirectory");
  iString missionDir = (string) dataDir["Cassini"];
  Filename *lutFile = new Filename(missionDir + "/calibration/lut/lut.tab");
  CisscalFile *stretchPairs = new CisscalFile(lutFile->Expanded());
  // Create the stretch pairs
  double temp1=0, temp2=0;
  stretch.ClearPairs();
  for (int i=0; i<stretchPairs->LineCount(); i++) {
    iString line; 
    stretchPairs->GetLine(line);  //assigns value to line
    line = line.TrimTail(", \t\n\r");
    while (line.size() > 0) {
      line = line.TrimHead(", \t");
      temp2 = line.Token(", \t\n\r").ToDouble();
      stretch.AddPair(temp1,temp2);
      temp1++;
    }
  }
  stretchPairs->Close();
  return;
}

// The input buffer has a raw 16 bit buffer but the values are still 0 to 255.
// We know that 255 (stretched to 4095 if Table converted) is saturated.
// Sky pixels could have valid DN of 0, but missing pixels are also saved as 0, 
// so it is impossible to distinguish between them.
// This method is used by ConvertLinePrefixPixels() and IsisMain() for ProcessByLine p2.
// author Jeannie Walldren 2008-08-21
void FixDns (Buffer &buf) {
  for (int i=0; i<buf.size(); i++) {
    // zeros and negatives are valid DN values, according to scientists,
    // but likelyhood of a zero in 16 bit is rare, 
    // so assume these are missing pixels and set them to null
    if (buf[i] == 0) {
      buf[i] = Null;
    }
    else if (dataConversionType == "Table") {
      buf[i] = stretch.Map((int)buf[i]);
    }
    // save max values (4095 for table-converted images and 255 for others) as HRS
    if (buf[i] == validMax) {
      buf[i] = Hrs;
    }
  }
}


//This method uses the translation table to read labels and adds any
// other needed keywords to Instrument, BandBin, and Kernels groups
// Called in IsisMain()
// modified by Jeannie Walldren 2008-08-21
void TranslateCassIssLabels (Filename &labelFile, Cube *ocube) {
  // Get the directory where the CISS translation tables are.
  PvlGroup &dataDir = Preference::Preferences().FindGroup("DataDirectory");
  iString missionDir = (string) dataDir["Cassini"];
  Filename transFile (missionDir + "/translations/cassiniIss.trn");

  // Get the translation manager ready
  Pvl inputLabel (labelFile.Expanded());
  PvlTranslationManager labelXlater (inputLabel, transFile.Expanded());

  // Pvl outputLabels;
  Pvl *outputLabel = ocube->Label();
  labelXlater.Auto(*(outputLabel));

  //Add needed keywords that are not in translation table to cube's instrument group
  PvlGroup &inst = outputLabel->FindGroup("Instrument",Pvl::Traverse);
  string scc = inputLabel.FindKeyword("SPACECRAFT_CLOCK_CNT_PARTITION");
  scc += "/" + (string) inputLabel.FindKeyword("SPACECRAFT_CLOCK_START_COUNT");
  inst.AddKeyword(PvlKeyword("SpacecraftClockCount",scc));

  //Add units of measurement to keywords from translation table
  double exposureDuration = inst.FindKeyword("ExposureDuration");
  inst.FindKeyword("ExposureDuration").SetValue(exposureDuration, "Milliseconds");

  int gainModeId = inst.FindKeyword("GainModeId");
  inst.FindKeyword("GainModeId").SetValue(gainModeId, "ElectronsPerDN");

  PvlKeyword opticsTemp = inst.FindKeyword("OpticsTemperature");
  inst.FindKeyword("OpticsTemperature").SetValue(opticsTemp[0]);
  inst.FindKeyword("OpticsTemperature").AddValue(opticsTemp[1], "DegreesCelcius");

  double instDataRate = inst.FindKeyword("InstrumentDataRate");
  inst.FindKeyword("InstrumentDataRate").SetValue(instDataRate, "KilobitsPerSecond");

  //  initialize global variables
  dataConversionType = (string) inst.FindKeyword("DataConversionType");
  sumMode = inst.FindKeyword("SummingMode");
  compressionType = (string) inst.FindKeyword("CompressionType");
  iString fsw((string) inst.FindKeyword("FlightSoftwareVersionId"));
  if (fsw == "Unknown") {
    flightSoftware = 0.0;
  }
  else{
    flightSoftware = fsw.ToDouble();
  }

  // create BandBin group
  iString filter = inputLabel.FindKeyword("FilterName")[0] + "/" + 
                    inputLabel.FindKeyword("FilterName")[1];

  string instrumentID = inst.FindKeyword("InstrumentId");
  string cameraAngleDefs;
  if (instrumentID.at(3) == 'N') {
    cameraAngleDefs = missionDir + "/translations/narrowAngle.def";
  }
  else if (instrumentID.at(3) == 'W') {
    cameraAngleDefs = missionDir + "/translations/wideAngle.def";
  }

  double center = 0;
  double width = 0;

  TextFile cameraAngle(cameraAngleDefs);
  int numLines = cameraAngle.LineCount();
  bool foundfilter = false;
  for (int i=0; i<numLines; i++) {
    iString line;
    cameraAngle.GetLine(line, true);
    iString token = line.Token(" ");
    if (token==filter) {
      line = line.Trim(" ");
      center = line.Token(" ");
      line = line.Trim(" ");
      width = line.Token(" ");
      foundfilter = true;
      break;
    }
  }
  PvlGroup bandBin ("BandBin");
  bandBin += PvlKeyword("FilterName", filter);
  bandBin += PvlKeyword("OriginalBand",1);

  if (foundfilter) {
    bandBin += PvlKeyword("Center",center);
    bandBin += PvlKeyword("Width",width);
  }
  else{
    PvlGroup msgGrp("Warnings");
    msgGrp += PvlKeyword("CameraAngleLookup", "Failed! No Camera information for filter combination: " + filter);
    Application::Log(msgGrp);
    bandBin += PvlKeyword("Center","None found for filter combination.");
    bandBin += PvlKeyword("Width","None found for filter combination.");
  }
  ocube->PutGroup(bandBin);

  PvlGroup kerns("Kernels");

  if (instrumentID == "ISSNA") {
    kerns += PvlKeyword("NaifFrameCode",-82360);
  }
  else if (instrumentID == "ISSWA") {
    kerns += PvlKeyword("NaifFrameCode",-82361);
  }
  else {
    string msg = "CISS2ISIS only imports Cassini ISS narrow ";
    msg += "angle or wide angle images";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }
  ocube->PutGroup(kerns);

  return;
}



// This method is called in ConvertLinePrefixPixels() and is
// modelled after IDL CISSCAL's OverclockAvg() in cassimg_define.pro
// author Jeannie Walldren 2008-08-21
double ComputeOverclockAvg(vector <double> pixel){
  // overclocks array is corrupt for lossy images (see cassimg_readvic.pro)

  if (compressionType != "Lossy" && flightSoftware < 1.3) { //numberOfOverclocks == 1
    // if Bltype CASSINI-ISS or CAS-ISS2, i.e. flight software version < 1.3
    // then there is only one column of valid overclocks in prefix pixels table, 
    // the first column contains nulls, so use column 2 as average
      return pixel[1];
  }
  else{//numberOfOverclocks == 2
    // number of columns of valid overclocks in prefix pixels table is 2
    // for CAS-ISS3 or CAS-ISS4, i.e. flight software version 1.3 or 1.4
    // calculate appropriate average (as in cassimg_define.pro, CassImg::OverclockAvg())
    if (sumMode == 1){
      return ((((double) pixel[0])/2 + ((double) pixel[1])/6)/2);
    }
    if (sumMode == 2){
      return ((((double)pixel[0]) + ((double) pixel[1])/3)/2);
    }
    if (sumMode == 4){
      return ((((double) pixel[0]) + ((double) pixel[1]))/2);
    }
    else return 0;
  }
}


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // File header and readout order comments...
  // OUR FILE HEADER INCLUDES TWO SECTIONS:
  //        -The first is the VICAR label (SIS page 52).  The number of bytes included here is calculated in the IsisMain()
  //        -The second is the Binary Label Header, or Binary Telemetry Header(SIS page 52).  This contains 60 bytes (SIS page 84) of significant data.
  // The READOUT ORDER of an image is the order in which the cameras were read.  This is needed for radiometric calibration (CISSCAL).
  // The possible values are :
  //        0 : Narrow-angle camera was read out first
  //        1 : Wide-angle camera was read out first
  // IDL CISSCAL FILE CASSIMG_SUBTRACTDARK.PRO LINE 333: 
  //        roo = bh[50]/32 MOD 2 ;Readout order is the 2nd bit of the 51st byte
  // According to SIS page 92 (Field=Software, Valid Values), the readout order is index 2 (the THIRD bit) of the byte.
  // Normally, we would assume that this was the third bit from the right, but there is some confusion on this matter.
  // SIS page 17 says bits and bytes are both "big endian" for pixel data, but doesn't mention whether this includes the binary telemetry table data, 
  // Reading the first 3 bytes of the binary header and comparing with bit values described in SIS Table 7.3.2,
  // if the bytes are read as most significant bit first (left-to-right), each value matches up except summation mode.
  // In this case, SIS says they shoud be sum1:01, sum2:10, sum4:11.  Actual values are sum1:00, sum2:01, sum4:10.
  // The IDL code also appears to be written as though bits are read in this manner, accessing the third bit from the left (32 ~ 00100000).
  // Since we haven't found a difinitive answer to this, we are mimicking the IDL code to determine the read out order.
  // We have not found an image with roo = 1 as of yet to test this.
  // If it is found to be the case that bits are read from left to right in this header, it may be more clear in the 
  // future to rewrite the line using a logical bitwise &-operator: roo = *(header+50+vicarLabelBytes) & (00100000);  
  // SOURCES : 
  //        Cassini ISS Tour VICAR Image Data File and Detatched PDS Label SIS, Tour Version 1.1 December 1, 2004
  //        IDL cisscal application files: cassimg_subtractdark.pro and linetime.pro
  // -Jeannie Walldren 08/06/2008
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

