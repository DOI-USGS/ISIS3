#include "Isis.h"

#include "iTime.h"
#include "ProcessExportPds.h"
#include "PvlTranslationManager.h"
#include "PvlFormatPds.h"
#include "OriginalLabel.h"

using namespace std;
using namespace Isis;

enum Pixtype { NONE, NEG, BOTH };

void setRangeAndPixels(UserInterface &ui, ProcessExportPds &p,
                       double &min, double &max, Pixtype ptype);
void IsisMain() {
  // Set the processing object
  ProcessExportPds p;

  // Setup the input cube
  Cube *iCube = p.SetInputCube("FROM");

  UserInterface &ui = Application::GetUserInterface();

  double min = -DBL_MAX;
  double max = DBL_MAX;

  if(ui.GetString("BITTYPE") == "8BIT") {
    p.SetOutputType(Isis::UnsignedByte);
    min = 0.0;
    max = 255.0;
    setRangeAndPixels(ui, p, min, max, BOTH);
  }
  else if(ui.GetString("BITTYPE") == "S16BIT") {
    p.SetOutputType(Isis::SignedWord);
    min = -32768.0;
    max = 32767.0;
    setRangeAndPixels(ui, p, min, max, NEG);
  }
  else if(ui.GetString("BITTYPE") == "U16BIT") {
    p.SetOutputType(Isis::UnsignedWord);
    min = 0.0;
    max = 65535.0;
    setRangeAndPixels(ui, p, min, max, BOTH);
  }
  else {
    p.SetOutputType(Isis::Real);
    p.SetOutputNull(Isis::NULL4);
    p.SetOutputLrs(Isis::LOW_REPR_SAT4);
    p.SetOutputLis(Isis::LOW_INSTR_SAT4);
    p.SetOutputHrs(Isis::HIGH_REPR_SAT4);
    p.SetOutputHis(Isis::HIGH_INSTR_SAT4);
    setRangeAndPixels(ui, p, min, max, NONE);
  }

  if(ui.GetString("ENDIAN") == "MSB")
    p.SetOutputEndian(Isis::Msb);
  else if(ui.GetString("ENDIAN") == "LSB")
    p.SetOutputEndian(Isis::Lsb);

  p.SetExportType(ProcessExportPds::Fixed);

  // Get the PDS label from the process, omitting keywords unnecessary for LO
  p.ForceBands(false);
  p.ForceBandName(false);
  p.ForceCenterFilterWavelength(false);
  p.ForceBandStorageType(false);
  p.ForceOffset(false);
  p.ForceScalingFactor(false);
  Pvl &pdsLabel = p.StandardPdsLabel(ProcessExportPds::Image);

  // Add PRODUCT_ID keyword, the first part of the output filename
  FileName outFileNoExt(ui.GetFileName("TO"));
  outFileNoExt = outFileNoExt.removeExtension();
  iString productID(outFileNoExt.baseName());
  PvlKeyword productId("PRODUCT_ID", productID.UpCase());
  pdsLabel.AddKeyword(productId);

  // Translate the keywords from the original labels that go in this label
  OriginalLabel origBlob;
  iCube->read(origBlob);
  Pvl origLabel;
  PvlObject origLabelObj = origBlob.ReturnLabels();
  origLabelObj.SetName("OriginalLabelObject");
  origLabel.AddObject(origLabelObj);

  // Get the directory where the Lunar translation tables are.
  PvlGroup &dataDir = Preference::Preferences().FindGroup("DataDirectory");

  // Transfer the instrument group to the output cube
  iString transDir = (string) dataDir["Lo"] + "/translations/";

  // Isis 3 cubes being exported for the first time
  if(!origLabel.HasKeyword("PRODUCT_TYPE", Pvl::Traverse)) {

    PvlTranslationManager orig(origLabel, transDir + "LoOriginalExport.trn");
    orig.Auto(pdsLabel);

    // Add elements of SCAN_PARAMETER keyword to label
    PvlObject &qube = origLabelObj.FindObject("QUBE");
    PvlGroup &ingestion = qube.FindGroup("ISIS_INGESTION");

    // Strips and level1 products use the following two keywords in different ways
    PvlKeyword &scanResolution = ingestion.FindKeyword("SCAN_RESOLUTION");
    PvlKeyword &scanDensityRange = ingestion.FindKeyword("SCAN_DENSITY_RANGE");

    // Change the units of SCAN_RESOLUTION from "um" to "<micron>"
    string scanResolutionStr = scanResolution[0];
    unsigned umPos = scanResolutionStr.find("um");
    string resolution = scanResolutionStr.substr(0, umPos);
    scanResolution[0] = resolution;
    scanResolution.SetUnits("micron");
    scanResolution.SetName("LO:FILMSTRIP_SCAN_RESOLUTION");

    // Break keyword SCAN_DENSITY_RANGE into two different parts
    string scanDensityRangeStr = scanDensityRange[0];
    unsigned toPos = scanDensityRangeStr.find("_TO_");
    string range1 = scanDensityRangeStr.substr(0, toPos);
    string range2 = scanDensityRangeStr.substr(toPos + 4);

    // Label translation for strips
    if(qube.FindGroup("ISIS_INSTRUMENT").HasKeyword("STRIP_NUMBER")) {
      PvlTranslationManager orig(origLabel, transDir + "LoStripExport.trn");
      orig.Auto(pdsLabel);

      pdsLabel.AddKeyword(scanResolution, PvlContainer::Replace);
    }
    // Translation for level1 products
    else if(qube.FindGroup("ISIS_INSTRUMENT").HasKeyword("START_TIME")) {
      PvlTranslationManager orig(origLabel, transDir + "LoLevel1Export.trn");
      orig.Auto(pdsLabel);

      pdsLabel.AddKeyword(scanResolution, PvlContainer::Replace);

      PvlKeyword &outputMicron = qube.FindKeyword("OUTPUT_MICRON");

      // Change the units of OUTPUT_MICRON from "um" to "<micron>"
      string outputMicronStr = outputMicron[0];
      umPos = outputMicronStr.find("um");
      resolution = outputMicronStr.substr(0, umPos);
      outputMicron[0] = resolution;
      outputMicron.SetUnits("micron");
      outputMicron.SetName("LO:FILMSTRIP_SCAN_PROCESSING_RES");

      pdsLabel.AddKeyword(outputMicron, PvlContainer::Replace);

      // Calculate statistics on the cube to be processed and place
      // its MINIMUM and MAXIMUM into the output label
      p.CalculateStatistics();
      pdsLabel.FindObject("IMAGE").AddKeyword(PvlKeyword("MINIMUM", p.CubeStatistics(0)->Minimum()), Pvl::Replace);
      pdsLabel.FindObject("IMAGE").AddKeyword(PvlKeyword("MAXIMUM", p.CubeStatistics(0)->Maximum()), Pvl::Replace);
    }
    else {
      FileName inputFile(ui.GetFileName("FROM"));
      string msg = "[" + inputFile.expanded() + "] does not appear to be an LO file.  ";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }
  // Reexporting a product created by this program and reingested into Isis
  else {

    PvlTranslationManager orig(origLabel, transDir + "LoReimportExport.trn");
    orig.Auto(pdsLabel);

    // Reexporting strips
    if(origLabel.HasKeyword("STRIP_NUMBER", Pvl::Traverse)) {
      PvlTranslationManager strip(origLabel, transDir + "LoStripExport.trn");
      strip.Auto(pdsLabel);
    }
    // Reexporting level 1 products
    else {
      PvlTranslationManager lvl1(origLabel, transDir + "LoLevel1Export.trn");
      lvl1.Auto(pdsLabel);
    }
  }

  // Add to labels boresight or fiducial data
  string bandBinTransFile;
  if(iCube->getLabel()->HasKeyword("FiducialId", Pvl::Traverse)) {
    bandBinTransFile = transDir + "LoFiducialExport.trn";
    PvlTranslationManager bandLab(*(iCube->getLabel()), bandBinTransFile);
    bandLab.Auto(pdsLabel);

    // Change the units of FIDCUAIL_COORDINATE_MICRON from "um" to "<micron>"
    PvlKeyword &coordMicron = iCube->getLabel()->FindKeyword(
        "FiducialCoordinateMicron", Pvl::Traverse);
    string coordMicronStr = coordMicron[0];
    unsigned umPos = coordMicronStr.find("um");
    string coord = coordMicronStr.substr(0, umPos);
    coordMicron[0] = coord;
    coordMicron.SetUnits("micron");
    coordMicron.SetName("LO:FIDUCIAL_COORDINATE_MICRON");

    pdsLabel.AddKeyword(coordMicron, PvlContainer::Replace);
  }
  else if(iCube->getLabel()->HasKeyword("BoresightSample", Pvl::Traverse)) {
    bandBinTransFile = transDir + "LoBoresightExport.trn";
    PvlTranslationManager bandLab(*(iCube->getLabel()), bandBinTransFile);
    bandLab.Auto(pdsLabel);
  }
  else {
    FileName inputFile(ui.GetFileName("FROM"));
    string msg = "[" + inputFile.expanded() + "] does not contain boresight or fiducial information.  ";
    msg += "Try ingesting your data with lo2isis first.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Add PRODUCT_CREATION_TIME
  time_t startTime = time(NULL);
  struct tm *tmbuf = gmtime(&startTime);
  char timestr[80];
  strftime(timestr, 80, "%Y-%m-%dT%H:%M:%S", tmbuf);
  string dateTime = (string) timestr;
  iTime tmpDateTime(dateTime);
  pdsLabel += PvlKeyword("PRODUCT_CREATION_TIME", tmpDateTime.UTC());

  if(ui.WasEntered("NOTE")) {
    pdsLabel.AddKeyword(PvlKeyword("NOTE", ui.GetString("NOTE")), Pvl::Replace);
  }

  // Add a keyword type (i.e., string, bool, int...) file to the PDS label Pvl
  PvlFormatPds *formatter = new PvlFormatPds();
  formatter->SetCharLimit(128);
  formatter->Add(transDir + "LoExportFormatter.typ");
  pdsLabel.SetFormat(formatter);

  // Add an output format template (group, object, & keyword output order) to
  // the PDS PVL
  iString formatDir = (string) dataDir["Lo"] + "/templates/labels/";
  pdsLabel.SetFormatTemplate(formatDir + "LoExportTemplate.pft");

  // Write labels to output file
  FileName outFile(ui.GetFileName("TO", "img"));
  string outFileName(outFile.expanded());
  ofstream oCube(outFileName.c_str());
  p.OutputLabel(oCube);
  p.StartProcess(oCube);
  oCube.close();
  p.EndProcess();

  return;
}

//Sets up special pixels and valid pixel ranges
void setRangeAndPixels(UserInterface &ui, ProcessExportPds &p, double &min, double &max, Pixtype ptype) {
  if(ptype == NEG) {
    if(ui.GetBoolean("NULL")) {
      p.SetOutputNull(min++);
    }
    if(ui.GetBoolean("LRS")) {
      p.SetOutputLrs(min++);
    }
    if(ui.GetBoolean("LIS")) {
      p.SetOutputLis(min++);
    }
    if(ui.GetBoolean("HIS")) {
      p.SetOutputHis(min++);
    }
    if(ui.GetBoolean("HRS")) {
      p.SetOutputHrs(min++);
    }
  }
  else if(ptype == BOTH) {
    if(ui.GetBoolean("NULL")) {
      p.SetOutputNull(min++);
    }
    if(ui.GetBoolean("LRS")) {
      p.SetOutputLrs(min++);
    }
    if(ui.GetBoolean("LIS")) {
      p.SetOutputLis(min++);
    }
    if(ui.GetBoolean("HRS")) {
      p.SetOutputHrs(max--);
    }
    if(ui.GetBoolean("HIS")) {
      p.SetOutputHis(max--);
    }
  }
  p.SetOutputRange(min, max);
}
