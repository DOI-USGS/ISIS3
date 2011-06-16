#include "Isis.h"

#include <string>

#include "ProcessImportPds.h"
#include "Pvl.h"
#include "UserInterface.h"
#include "TextFile.h"
#include "Stretch.h"
#include "Table.h"
#include "TableRecord.h"

using namespace std;
using namespace Isis;

void TranslateUoACassiniLabels(Pvl &labelPvl, Cube *ocube);
vector<int> ConvertLinePrefixPixels(Isis::PixelType pixelType,
                                    unsigned char *data);
void FixDns8(Buffer &buf);
Stretch stretch;
void CreateStretchPairs();

void IsisMain() {

  ProcessImportPds p;
  UserInterface &ui = Application::GetUserInterface();

  // Get the input
  Filename inFile = ui.GetFilename("FROM");
  Pvl pdsLabel;
  p.SetPdsFile(inFile.Expanded(), "", pdsLabel);

  CubeAttributeOutput &outAtt = ui.GetOutputAttribute("TO");
  Cube *ocube = p.SetOutputCube(ui.GetFilename("TO"), outAtt);

  // Process
  p.StartProcess();
  TranslateUoACassiniLabels(pdsLabel, ocube);

  // Fix the StartTime and StopTime keywords from having the 'Z' value at the end
  Pvl *outLabel = ocube->getLabel();
  PvlGroup &inst = outLabel->FindGroup("Instrument", Isis::PvlObject::Traverse);
  PvlKeyword &start = inst.FindKeyword("StartTime");
  PvlKeyword &stop  = inst.FindKeyword("StopTime");
  iString startValue = start[0];
  iString stopValue  = stop[0];
  start[0] = startValue.TrimTail("Z");
  stop[0]  = stopValue.TrimTail("Z");

  // All finished with the ImportPds object
  p.EndProcess();

}

vector<int> ConvertLinePrefixPixels(Isis::PixelType pixelType,
                                    unsigned char *data) {
  Isis::Buffer pixelBuf(1, 1, 1, Isis::SignedWord);

  vector<int> calibrationPixels;
  //***CHECK LABEL FOR ACTUAL ENDIAN VALUE RATHER THAN ASSUMING MSB***???
  EndianSwapper swapper("MSB");

  vector<short int> pixel;
  //12 is start byte for First Overclocked Pixel Sum in Binary Line Prefix, see SIS pg 83
  pixel.push_back(swapper.ShortInt(& (data[12])));
  //22 is start byte for Last Overclocked Pixel Sum in Binary Line Prefix, see SIS pg 83
  pixel.push_back(swapper.ShortInt(& (data[22])));
  for(int i = 0; i < (int)pixel.size(); i++) {
    pixelBuf[0] = pixel[i];
    FixDns8(pixelBuf);
    double pix = pixelBuf[0];
    if(pix == NULL8) calibrationPixels.push_back(NULL2);
    else if(pix == LOW_REPR_SAT8) calibrationPixels.push_back(LOW_REPR_SAT2);
    else if(pix == LOW_INSTR_SAT8) calibrationPixels.push_back(LOW_INSTR_SAT2);
    else if(pix == HIGH_INSTR_SAT8) calibrationPixels.push_back(HIGH_INSTR_SAT2);
    else if(pix == HIGH_REPR_SAT8) calibrationPixels.push_back(HIGH_REPR_SAT2);
    else calibrationPixels.push_back((int)(pix + 0.5));
  }

  return calibrationPixels;
}

void FixDns8(Buffer &buf) {
  for(int i = 0; i < buf.size(); i++) {
    if(buf[i] != 0) {
      buf[i] = stretch.Map(buf[i]);
    }
    else {
      buf[i] = Isis::Null;
    }
  }
}

void CreateStretchPairs() {
  // Set up the strech for the 8 to 12 bit conversion from file
  Filename *temp = new Filename("$cassini/calibration/cisslog_???.lut");
  temp->HighestVersion();
  TextFile *stretchPairs = new TextFile(temp->Expanded());

  // Create the stretch pairs
  stretch.ClearPairs();
  for(int i = 0; i < stretchPairs->LineCount(); i++) {
    iString line;
    stretchPairs->GetLine(line, true); //assigns value to line
    int temp1 = line.Token(" ");
    int temp2 = line.Trim(" ");
    stretch.AddPair(temp1, temp2);
  }
  stretchPairs->Close();

  // Clean up
  delete temp;
  delete stretchPairs;

}


void TranslateUoACassiniLabels(Pvl &labelPvl, Cube *ocube) {

  //Create a PVL to store the translated labels
  Pvl *outLabel = ocube->getLabel();

  // Get the directory where the CISS translation tables are.
  PvlGroup dataDir(Preference::Preferences().FindGroup("DataDirectory"));
  iString transDir = (string) dataDir["Cassini"] + "/translations/";

  // Translate
  Filename transFile(transDir + "cissua2isis.trn");
  PvlTranslationManager instrumentXlater(labelPvl, transFile.Expanded());
  instrumentXlater.Auto((*outLabel));

  PvlGroup &inst = outLabel->FindGroup("Instrument", Isis::PvlObject::Traverse);

  // Create the correct SpacecraftClockCount value
  PvlGroup &inInst = labelPvl.FindGroup("ISIS_INSTRUMENT", Pvl::Traverse);
  string scc = inInst.FindKeyword("SPACECRAFT_CLOCK_CNT_PARTITION");
  scc += "/" + (string) inInst.FindKeyword("ORIGINAL_SPACECRAFT_CLOCK_START_COUN");
  inst.AddKeyword(PvlKeyword("SpacecraftClockCount", scc));

  // dataConv is used later
  string dataConv = inInst.FindKeyword("DATA_CONVERSION_TYPE");
  inst.AddKeyword(PvlKeyword("DataConversionType", dataConv));

  //to add an array of values
  PvlKeyword opticsTemp(inInst.FindKeyword("OPTICS_TEMPERATURE"));
  opticsTemp.SetName("OpticsTemperature");
  inst.AddKeyword(opticsTemp);

  //two possible label names for same keyword
  if(labelPvl.HasKeyword("ENCODING_TYPE")) {
    string encodingType = labelPvl.FindKeyword("ENCODING_TYPE", Pvl::Traverse);
    inst.AddKeyword(PvlKeyword("CompressionType", encodingType));
  }
  else {
    string instCmprsType = labelPvl.FindKeyword("INST_CMPRS_TYPE", Pvl::Traverse);
    inst.AddKeyword(PvlKeyword("CompressionType", instCmprsType));
  }

  string flightSoftware = labelPvl.FindKeyword("FLIGHT_SOFTWARE_VERSION_ID", Pvl::Traverse);
  inst.AddKeyword(PvlKeyword("FlightSoftwareVersionId", flightSoftware));

  // Sets the needed Kernel FrameCode
  string instrumentID = inst.FindKeyword("InstrumentId");
  PvlGroup kerns("Kernels");
  if(instrumentID == "ISSNA") {
    kerns += PvlKeyword("NaifFrameCode", -82360);
  }
  else if(instrumentID == "ISSWA") {
    kerns += PvlKeyword("NaifFrameCode", -82361);
  }
  else {
    string msg = "CISS2ISIS only imports Cassini ISS narrow ";
    msg += "angle or wide angle images";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }
  outLabel->FindObject("IsisCube").AddGroup(kerns);

  // Create BandBin group
  iString filter = labelPvl.FindKeyword("BAND_BIN_FILTER_NAME", Isis::PvlObject::Traverse)[0];
  filter = filter.substr(0, 3) + "/" + filter.substr(4);
  string cameraAngleDefs;
  if(instrumentID.at(3) == 'N') {
    cameraAngleDefs = transDir + "narrowAngle.def";
  }
  else if(instrumentID.at(3) == 'W') {
    cameraAngleDefs = transDir + "wideAngle.def";
  }
  double center = 0;
  double width = 0;
  TextFile cameraAngle(cameraAngleDefs);
  int numLines = cameraAngle.LineCount();
  bool foundfilter = false;
  for(int i = 0; i < numLines; i++) {
    iString line;
    cameraAngle.GetLine(line, true);
    iString token = line.Token(" ");
    if(token == filter) {
      line = line.Trim(" ");
      center = line.Token(" ");
      line = line.Trim(" ");
      width = line.Token(" ");
      foundfilter = true;
      break;
    }
  }
  if(!foundfilter) {
    string msg = "Camera Angle Lookup Failed: ";
    msg += "Filter combination " + filter + " unknown.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }
  PvlGroup bandBin("BandBin");
  bandBin += PvlKeyword("FilterName", filter);
  bandBin += PvlKeyword("OriginalBand", 1);
  bandBin += PvlKeyword("Center", center);
  bandBin += PvlKeyword("Width", width);
  outLabel->FindObject("IsisCube").AddGroup(bandBin);

}
