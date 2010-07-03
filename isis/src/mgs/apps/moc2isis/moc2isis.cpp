#include "Isis.h"

#include <cstdio>
#include <string>

#include "ProcessImportPds.h"

#include "UserInterface.h"
#include "Filename.h"
#include "iException.h"
#include "iException.h"
#include "iTime.h"
#include "Preference.h"

using namespace std; 
using namespace Isis;

void IsisMain ()
{
  void TranslateMocEdrLabels (Filename &labelFile, Cube *ocube);

  ProcessImportPds p;
  Pvl pdsLabel;
  UserInterface &ui = Application::GetUserInterface();

  // Get the input filename and make sure it is a MOC EDR
  Filename in = ui.GetFilename("FROM");
  iString id;
  bool compressed = false;
  bool projected;
  try {
    Pvl lab(in.Expanded());
    id = (string) lab["DATA_SET_ID"];
    if (lab.FindObject("IMAGE").HasKeyword("ENCODING_TYPE")) compressed = true;
    projected = lab.HasObject("IMAGE_MAP_PROJECTION");
  }
  catch (iException &e) {
    string msg = "Unable to read [DATA_SET_ID] from input file [" +
                 in.Expanded() + "]";
    throw iException::Message(iException::Io,msg, _FILEINFO_);
  }

  //Checks if in file is rdr
  if( projected ) {
    string msg = "[" + in.Name() + "] appears to be an rdr file.";
    msg += " Use pds2isis.";
    throw iException::Message(iException::User,msg, _FILEINFO_);
  }

  id.ConvertWhiteSpace();
  id.Compress();
  id.Trim(" ");
  if ((id != "MGS-M-MOC-NA/WA-2-DSDP-L0-V1.0") &&
      (id != "MGS-M-MOC-NA/WA-2-SDP-L0-V1.0")) {
    string msg = "Input file [" + in.Expanded() + "] does not appear to be " +
                 "in MOC EDR format. DATA_SET_ID [" + id + "]";
    throw iException::Message(iException::Io,msg, _FILEINFO_);
  }


  // Get a temporary file for the uncompressed version incase we need it
  Filename uncompressed(in.Basename(), "img");

  // Set up conditional transfer of PDS labels to output cube
  Filename &translbl(in);

  // If the input file is compressed, use "mocuncompress" to uncompress it
  if (compressed) {
    iString command = "mocuncompress " + in.Expanded() + " " + 
                       uncompressed.Expanded();
    if (system (command.c_str()) == 1) {
      string msg = "Unable to execute [mocuncompress]";
      throw iException::Message(iException::Programmer,msg, _FILEINFO_);
    }
    p.SetPdsFile (uncompressed.Expanded(), "", pdsLabel);
    translbl = uncompressed;
  
  }
  else {
    p.SetPdsFile (in.Expanded(), "", pdsLabel);
  }

  Cube *ocube = p.SetOutputCube("TO");
  p.StartProcess ();
  TranslateMocEdrLabels(translbl, ocube);
  p.EndProcess ();

  if (compressed) {
    string uncompressedName(uncompressed.Expanded());
    remove (uncompressedName.c_str());
  }

  return;
}

void TranslateMocEdrLabels (Filename &labelFile, Cube *ocube) {

  string startTime, productId, clockCount;

  // Get the directory where the MOC translation tables are.
  PvlGroup &dataDir = Preference::Preferences().FindGroup("DataDirectory");

  // Transfer the instrument group to the output cube
  iString transDir = (string) dataDir["Mgs"];
  Filename transFile (transDir + "/" + "translations/mocInstrument.trn");

  // Get the translation manager ready
  Pvl labelPvl (labelFile.Expanded());
  PvlTranslationManager instrumentXlater (labelPvl, transFile.Expanded());

  PvlGroup inst("Instrument");

  if (instrumentXlater.InputHasKeyword("SpacecraftName")) {
    string str = instrumentXlater.Translate ("SpacecraftName");
    inst += PvlKeyword ("SpacecraftName", str);
  }

  if (instrumentXlater.InputHasKeyword("InstrumentId")) {
    string str = instrumentXlater.Translate ("InstrumentId");
    inst += PvlKeyword ("InstrumentId", str);
  }

  if (instrumentXlater.InputHasKeyword("TargetName")) {
    string str = instrumentXlater.Translate ("TargetName");
    inst += PvlKeyword ("TargetName", str);
  }

  if (instrumentXlater.InputHasKeyword("StartTime")) {
    string str = instrumentXlater.Translate ("StartTime");
    inst += PvlKeyword ("StartTime", str);
    // isisLab.AddKeyword ("StartTime", str+"Z");
    startTime = str;
  }

  if (instrumentXlater.InputHasKeyword("StopTime")) {
    string str = instrumentXlater.Translate ("StopTime");
    inst += PvlKeyword ("StopTime", str);
    // isisLab.AddKeyword ("StopTime", str+"Z");
  }

  if (instrumentXlater.InputHasKeyword("CrosstrackSumming")) {
    string str = instrumentXlater.Translate ("CrosstrackSumming");
    inst += PvlKeyword ("CrosstrackSumming", str);
  }

  if (instrumentXlater.InputHasKeyword("DowntrackSumming")) {
    string str = instrumentXlater.Translate ("DowntrackSumming");
    inst += PvlKeyword ("DowntrackSumming", str);
  }

  if (instrumentXlater.InputHasKeyword("FocalPlaneTemperature")) {
    string str = instrumentXlater.Translate ("FocalPlaneTemperature");
    inst += PvlKeyword ("FocalPlaneTemperature", str);
  }

  if (instrumentXlater.InputHasKeyword("GainModeId")) {
    string str = instrumentXlater.Translate ("GainModeId");
    inst += PvlKeyword ("GainModeId", str);
  }

  if (instrumentXlater.InputHasKeyword("LineExposureDuration")) {
    string str = instrumentXlater.Translate ("LineExposureDuration");
    inst += PvlKeyword ("LineExposureDuration", str, "milliseconds");
  }

  if (instrumentXlater.InputHasKeyword("MissionPhaseName")) {
    string str = instrumentXlater.Translate ("MissionPhaseName");
    inst += PvlKeyword ("MissionPhaseName", str);
  }

  if (instrumentXlater.InputHasKeyword("OffsetModeId")) {
    string str = instrumentXlater.Translate ("OffsetModeId");
    inst += PvlKeyword ("OffsetModeId", str);
  }

  if (instrumentXlater.InputHasKeyword("SpacecraftClockCount")) {
    string str = instrumentXlater.Translate ("SpacecraftClockCount");
    inst += PvlKeyword ("SpacecraftClockCount", str);
    clockCount = str;
  }

  if (instrumentXlater.InputHasKeyword("RationaleDesc")) {
    string str = instrumentXlater.Translate ("RationaleDesc");
    inst += PvlKeyword ("RationaleDesc", str);
  }

  if (instrumentXlater.InputHasKeyword("OrbitNumber")) {
    string str = instrumentXlater.Translate ("OrbitNumber");
    inst += PvlKeyword ("OrbitNumber", str);
  }

  if (instrumentXlater.InputHasKeyword("FirstLineSample")) {
    iString str = instrumentXlater.Translate ("FirstLineSample");
    int num = str.ToInteger();
    num++;
    inst += PvlKeyword ("FirstLineSample", num);
  }

  // Add the instrument specific info to the output file
  ocube->PutGroup(inst);


  // Transfer the archive group to the output cube
  transDir = (string) dataDir["Mgs"];
  Filename transFileArchive (transDir + "/" + "translations/mocArchive.trn");

  // Get the translation manager ready for the archive group
  PvlTranslationManager archiveXlater (labelPvl, transFileArchive.Expanded());

  PvlGroup arch("Archive");

  if (archiveXlater.InputHasKeyword("DataSetId")) {
    string str = archiveXlater.Translate ("DataSetId");
    arch += PvlKeyword ("DataSetId", str);
  }

  if (archiveXlater.InputHasKeyword("ProductId")) {
    string str = archiveXlater.Translate ("ProductId");
    arch += PvlKeyword ("ProductId", str);
    productId = str;
  }

  if (archiveXlater.InputHasKeyword("ProducerId")) {
    string str = archiveXlater.Translate ("ProducerId");
    arch += PvlKeyword ("ProducerId", str);
  }

  if (archiveXlater.InputHasKeyword("ProductCreationTime")) {
    string str = archiveXlater.Translate ("ProductCreationTime");
    arch += PvlKeyword ("ProductCreationTime", str);
  }

  if (archiveXlater.InputHasKeyword("SoftwareName")) {
    string str = archiveXlater.Translate ("SoftwareName");
    arch += PvlKeyword ("SoftwareName", str);
  }

  if (archiveXlater.InputHasKeyword("UploadId")) {
    string str = archiveXlater.Translate ("UploadId");
    arch += PvlKeyword ("UploadId", str);
  }

  if (archiveXlater.InputHasKeyword("DataQualityDesc")) {
    string str = archiveXlater.Translate ("DataQualityDesc");
    arch += PvlKeyword ("DataQualityDesc", str);
  }


  // New labels (not in the PDS file)

  // The ImageNumber is made up of pieces of the StartTime
  // The first piece is the
  //   Last digit of the year (eg, 1997 => 7), followed by the
  //   Day of the year (Julian day) followed by the
  //   Last five digits of the ProductId
  if (startTime.length() > 0 && productId.length() > 0) {
    iTime tim(startTime);
    string imageNumber = tim.YearString();
    imageNumber = imageNumber.substr(3, 1);
    imageNumber += tim.DayOfYearString();

    imageNumber += productId.substr(4);

    arch += PvlKeyword ("ImageNumber", imageNumber);
  }


  // The ImageKeyId is made up of the:
  //   First five digits of the SpacecraftClockCount followed by the
  //   Last five dights of the ProductId
  if (clockCount.length() > 0 && productId.length() > 0) {
    arch += PvlKeyword ("ImageKeyId", clockCount.substr(0,5) +
                            productId.substr(4));
  }

  // Add the archive info to the output file
  ocube->PutGroup(arch);


  // Create the BandBin Group and populate it
  transDir = (string) dataDir["Mgs"];
  Filename transFileBandBin (transDir + "/" + "translations/mocBandBin.trn");

  // Get the translation manager ready for the BandBin group
  PvlTranslationManager bandBinXlater (labelPvl, transFileBandBin.Expanded());





  PvlGroup bandBin("BandBin");
  string frameCode;

  if (bandBinXlater.InputHasKeyword("FilterName")) {
    iString str = bandBinXlater.Translate ("FilterName");
    str.UpCase();

    if (str == "BLUE") {
      bandBin += PvlKeyword ("FilterName", str);
      bandBin += PvlKeyword ("OriginalBand", 1);
      bandBin += PvlKeyword ("Center", 0.4346, "micrometers");
      bandBin += PvlKeyword ("Width", 0.050, "micrometers");
      frameCode = "-94033";
    }
    else if (str == "RED") {
      bandBin += PvlKeyword ("FilterName", str);
      bandBin += PvlKeyword ("OriginalBand", 1);
      bandBin += PvlKeyword ("Center", 0.6134, "micrometers");
      bandBin += PvlKeyword ("Width", 0.050, "micrometers");
      frameCode = "-94032";
    }
    else {
      string msg = "Invalid value for filter name [" + str + "]";
    }
  }
  else {
    bandBin += PvlKeyword ("FilterName", "BROAD_BAND");
    bandBin += PvlKeyword ("OriginalBand", 1);
    bandBin += PvlKeyword ("Center", 0.700, "micrometers");
    bandBin += PvlKeyword ("Width", 0.400, "micrometers");
    frameCode = "-94031";
  }

  // Add the bandbin info to the output file
  ocube->PutGroup(bandBin);


  // Create the Kernel Group
  PvlGroup kerns("Kernels");
  kerns += PvlKeyword("NaifFrameCode", frameCode);
  ocube->PutGroup(kerns);
}

