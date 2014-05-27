#include "Isis.h"

#include <QDebug>
#include <QString>

#include "Cube.h"
#include "CubeAttribute.h"
#include "FileName.h"
#include "IException.h"
#include "ImportPdsTable.h"
#include "iTime.h"
#include "OriginalLabel.h"
#include "PixelType.h"
#include "ProcessByLine.h"
#include "ProcessBySample.h"
#include "ProcessImportPds.h"
#include "Pvl.h"
#include "Table.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void importImage(QString outputParamName, ProcessImportPds::PdsFileType fileType);
void translateChandrayaan1M3Labels(Pvl &pdsLabel, Cube *ocube, Table &utcTable,
                                   ProcessImportPds::PdsFileType fileType);
void flip(Buffer &in);
void flipUtcTable(Table &utcTable);


void IsisMain() {
  importImage("TO", (ProcessImportPds::PdsFileType)(ProcessImportPds::Rdn|ProcessImportPds::L0));
  importImage("LOC", ProcessImportPds::Loc);
  importImage("OBS", ProcessImportPds::Obs);
}


void importImage(QString outputParamName, ProcessImportPds::PdsFileType fileType) {
  // We should be processing a PDS file
  UserInterface &ui = Application::GetUserInterface();
  if (!ui.WasEntered(outputParamName)) {
    return;
  }

  ProcessImportPds importPds;
  FileName in = ui.GetFileName("FROM");

  Pvl pdsLabel(in.expanded());
  if (fileType == (ProcessImportPds::L0 | ProcessImportPds::Rdn)) {
    //  Is this a L0 or L1B product?
    if ((QString) pdsLabel["PRODUCT_TYPE"] == "RAW_IMAGE") {
      fileType = ProcessImportPds::L0;
    }
    else {
      fileType = ProcessImportPds::Rdn;
    }
  }
  // Convert the pds file to a cube
  try {
    importPds.SetPdsFile(in.expanded(), "", pdsLabel, fileType);
  }
  catch(IException &e) {
    QString msg = "Input file [" + in.expanded() +
                 "] does not appear to be a Chandrayaan 1 M3 detached PDS label";
    throw IException(e, IException::User, msg, _FILEINFO_);
  }

  bool samplesNeedFlipped = false;
  bool linesNeedFlipped = false;
  if (fileType != ProcessImportPds::L0) {
    // M3 PDS L1B images may be flipped/mirrored in sample and/or line to visually appear with
    // north nearly up. The ISIS camera model does not take this into account, so this post
    // acquisition processing needs to be removed. There are four possible flip/mirror mode 
    // combinations.
    // 1.  Descending yaw / Forward orbit limb - No changes in sample or line
    // 2.  Descending yaw / Reverse orbit limb - Samples are reversed, first sample on west side of image
    // 3.  Ascending yaw / Forward orbit limb - Lines/times are reversed so northernmost image line first,
    //                                          Samples are reversed, first sample on west side of image
    // 4.  Ascending yaw / Reverse orbit limb - Lines/times are reversed so northernmost image line first,
    QString yawDirection = (QString) pdsLabel["CH1:SPACECRAFT_YAW_DIRECTION"];
    QString limbDirection = (QString) pdsLabel["CH1:ORBIT_LIMB_DIRECTION"];
    samplesNeedFlipped = ( ((yawDirection == "REVERSE") && (limbDirection == "DESCENDING")) ||
                           ((yawDirection == "FORWARD") && (limbDirection == "ASCENDING")) );
    linesNeedFlipped = (limbDirection == "ASCENDING");
  }

  // The following 2 lines are for testing purposes, No flipping will be done with these lines 
  // uncommented i.e. north is always up, lons always pos east to the right.
  //  samplesNeedFlipped = false;
  //  linesNeedFlipped = false;

  {
    Cube *importCube = importPds.SetOutputCube(outputParamName);

    Table *utcTable = NULL;
    if (fileType != ProcessImportPds::L0) {
      utcTable = &(importPds.ImportTable("UTC_FILE"));
      //  If lines are flipped, need to flip times also
      if (linesNeedFlipped) {
        flipUtcTable(*utcTable);
      }
    }

    importPds.StartProcess();

    translateChandrayaan1M3Labels(pdsLabel, importCube, *utcTable, fileType);
    if (fileType != ProcessImportPds::L0) importCube->write(*utcTable);
    importPds.Finalize();
  }

  CubeAttributeInput inAttribute;
  if (linesNeedFlipped) {
    ProcessBySample flipLines;
    Cube *cube = flipLines.SetInputCube(ui.GetFileName(outputParamName), inAttribute);
    cube->reopen("rw");
    flipLines.ProcessCubeInPlace(flip, false);
  }

  if (samplesNeedFlipped) {
    ProcessByLine flipSamples;
    Cube *cube = flipSamples.SetInputCube(ui.GetFileName(outputParamName), inAttribute);
    cube->reopen("rw");
    flipSamples.ProcessCubeInPlace(flip, false);
  }
}


void translateChandrayaan1M3Labels(Pvl& pdsLabel, Cube *ocube, Table& utcTable,
                                   ProcessImportPds::PdsFileType fileType) {
  Pvl outLabel;
  // Directory containing translation tables
  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["Chandrayaan1"] + "/translations/";

  // Translate the archive group
  FileName transFile(transDir + "m3Archive.trn");
  PvlTranslationManager archiveXlator(pdsLabel, transFile.expanded());
  archiveXlator.Auto(outLabel);
  ocube->putGroup(outLabel.findGroup("Archive", Pvl::Traverse));

  // Translate the instrument group
  transFile = transDir + "m3Instrument.trn";
  PvlTranslationManager instrumentXlator(pdsLabel, transFile.expanded());
  instrumentXlator.Auto(outLabel);

  PvlGroup &inst = outLabel.findGroup("Instrument", Pvl::Traverse);
  ocube->putGroup(inst);

  QString instMode = inst["InstrumentModeId"];
  // Initialize to GLOBAL value
  double expectedLineRate = .10176;
  if (instMode == "TARGET") {
    expectedLineRate = .05088;
  }

  //  Insure line rate is expected value and constant through the cube
  if (fileType == ProcessImportPds::Rdn) {
    if (utcTable.Records() > 1) {
      bool hasExpectedLineRate = true;
      bool lineRateConstant = true;
      double row0Ddoy = toDouble((QString)utcTable[0]["Ddoy"]);
      double row1Ddoy = toDouble((QString)utcTable[1]["Ddoy"]);
      double firstDdoyDiff = abs(row1Ddoy - row0Ddoy);
      double firstLineRate = firstDdoyDiff * 24 * 3600;
      if ((expectedLineRate - firstLineRate) > 1e-5) {
        hasExpectedLineRate = false;
      }
      double minDdoy = row0Ddoy;
      for (int i = 1; i < utcTable.Records(); i++) {
        minDdoy = qMin(minDdoy, toDouble((QString)utcTable[i]["Ddoy"]));
        double newDiff = abs(toDouble((QString)utcTable[i]["Ddoy"]) -
                         toDouble((QString)utcTable[i - 1]["Ddoy"]));

        if ((firstDdoyDiff - newDiff) > 1e-11) {
          lineRateConstant = false;
        }
      }

      if (!hasExpectedLineRate) {
        PvlGroup msg("Messages");
        msg += PvlKeyword("Warning", "Line scan rate of " + toString(firstLineRate) +
                          " not expected value of " + toString(expectedLineRate));
        Application::Log(msg);
      }
      if (!lineRateConstant) {
        PvlGroup msg("Messages");
        msg += PvlKeyword("Warning", "Line scan rate not constant.");
        Application::Log(msg);
      }
    }
  }

  if (fileType == ProcessImportPds::L0 || fileType == ProcessImportPds::Rdn) {
    // Setup the band bin group
    QString bandFile = "$chandrayaan1/bandBin/bandBin.pvl";
    Pvl bandBinTemplate(bandFile);
    PvlObject modeObject = bandBinTemplate.findObject(pdsLabel["INSTRUMENT_MODE_ID"]);
    PvlGroup bandGroup = modeObject.findGroup("BandBin");
    //  Add OriginalBand
    int numBands;
    if ((QString)pdsLabel["INSTRUMENT_MODE_ID"] == "TARGET") {
      numBands = 256;
    }
    else {
      numBands = 85;
    }
    PvlKeyword originalBand("OriginalBand");
    for (int i = 1; i <= numBands; i++) {
      originalBand.addValue(toString(i));
    }
    bandGroup += originalBand;
    ocube->putGroup(bandGroup);

    if (fileType == ProcessImportPds::Rdn) {      
      // Setup the radiometric calibration group for the image cube
      PvlGroup calib("RadiometricCalibration");
      PvlKeyword solar = pdsLabel["SOLAR_DISTANCE"];
      calib += PvlKeyword("Units", "W/m2/um/sr");
      calib += PvlKeyword("SolarDistance", toString((double)solar), solar.unit());
      calib += PvlKeyword("DetectorTemperature", toString((double)pdsLabel["DETECTOR_TEMPERATURE"]));
      calib += PvlKeyword("SpectralCalibrationFileName",
                          (QString)pdsLabel["CH1:SPECTRAL_CALIBRATION_FILE_NAME"]);
      calib += PvlKeyword("RadGainFactorFileName",
                          (QString)pdsLabel["CH1:RAD_GAIN_FACTOR_FILE_NAME"]);
      calib += PvlKeyword("GlobalBandpassFileName",
                          (QString)pdsLabel["CH1:SPECTRAL_CALIBRATION_FILE_NAME"]);
      ocube->putGroup(calib);
    }
  }

  // Setup the band bin group
  else if (fileType == ProcessImportPds::Loc) {
    PvlGroup bandBin("BandBin");
    PvlKeyword bandNames = pdsLabel.findObject("LOC_IMAGE", PvlObject::Traverse)["BAND_NAME"];
    bandNames.setName("Name");
    bandBin += bandNames;
    PvlKeyword bandUnits("Units", "(Degrees, Degrees, Meters)");
    bandBin += bandUnits;
    ocube->putGroup(bandBin);
  }
  else if (fileType == ProcessImportPds::Obs) {
    PvlGroup bandBin("BandBin");
    PvlKeyword bandNames = pdsLabel.findObject("OBS_IMAGE", PvlObject::Traverse)["BAND_NAME"];
    bandNames.setName("Name");
    bandBin += bandNames;
    ocube->putGroup(bandBin);
  }

  // Setup the kernel group
  PvlGroup kern("Kernels");
  kern += PvlKeyword("NaifFrameCode", "-86520");
  ocube->putGroup(kern);

  OriginalLabel origLabel(pdsLabel);
  ocube->write(origLabel);
}


void flip(Buffer &in) {

  for(int i = 0; i < in.size() / 2; i++) {
    swap(in[i], in[in.size() - i - 1]);
  }
}


void flipUtcTable(Table &utcTable) {
  int nrecs = utcTable.Records();
  for (int i = 0; i < nrecs / 2; i++) {
    TableRecord rec1 = utcTable[i];
    TableRecord rec2 = utcTable[nrecs - i - 1];
    utcTable.Update(rec1, nrecs - i - 1);
    utcTable.Update(rec2, i);
  }
}


