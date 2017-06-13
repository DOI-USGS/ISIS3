#include "Isis.h"

#include <cstdio>
#include <QString>

#include "ProcessImportPds.h"
#include "ProcessByLine.h"

#include "UserInterface.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "Preference.h"
#include "Buffer.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlSequence.h"
#include "Stretch.h"

using namespace std;
using namespace Isis;

// Global variables for processing functions
Stretch stretch;

// The input raw EDR contains 6 sections. The following counts keep track
// of the types of pixels found in each section. The order of sections
// is the order they arae encountered within the raw EDR. (i.e., calibration
// buffer, calibration image, calibration dark/reference, image buffer,
// image, image dark/reference)
int gapCount[6];
int suspectGapCount[6];
int invalidCount[6];
int lisCount[6];
int hisCount[6];
int validCount[6];
int section;

bool lsbGap;


void IsisMain() {
  stretch.ClearPairs();

  for(int i = 0; i < 6; i++) {
    gapCount[i] = 0;
    suspectGapCount[i] = 0;
    invalidCount[i] = 0;
    lisCount[i] = 0;
    hisCount[i] = 0;
    validCount[i] = 0;
  }

  void TranslateHiriseEdrLabels(FileName & labelFile, Cube *);
  void SaveHiriseCalibrationData(ProcessImportPds & process, Cube *,
                                 Pvl & pdsLabel);
  void SaveHiriseAncillaryData(ProcessImportPds & process, Cube *);
  void FixDns8(Buffer & buf);
  void FixDns16(Buffer & buf);

  ProcessImportPds p;
  Pvl pdsLabel;
  UserInterface &ui = Application::GetUserInterface();

  // Get the input filename and make sure it is a HiRISE EDR
  FileName inFile = ui.GetFileName("FROM");
  QString id;
  bool projected;
  try {
    Pvl lab(inFile.expanded());
    id = (QString) lab.findKeyword("DATA_SET_ID");
    projected = lab.hasObject("IMAGE_MAP_PROJECTION");
  }
  catch(IException &e) {
    QString msg = "Unable to read [DATA_SET_ID] from input file [" +
                 inFile.expanded() + "]";
    throw IException(e, IException::Io, msg, _FILEINFO_);
  }

  //Checks if in file is rdr
  if(projected) {
    QString msg = "[" + inFile.name() + "] appears to be an rdr file.";
    msg += " Use pds2isis.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  id = id.simplified().trimmed();
  if(id != "MRO-M-HIRISE-2-EDR-V1.0") {
    QString msg = "Input file [" + inFile.expanded() + "] does not appear to be " +
                 "in HiRISE EDR format. DATA_SET_ID is [" + id + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  p.SetPdsFile(inFile.expanded(), "", pdsLabel);

  // Make sure the data we need for the BLOBs is saved by the Process
  p.SaveFileHeader();
  p.SaveDataPrefix();
  p.SaveDataSuffix();

  // Let the Process create the output file but override any commandline
  // output bit type and min/max. It has to be 16bit for the rest of hi2isis
  // to run.
  // Setting the min/max to the 16 bit min/max keeps all the dns (including
  // the 8 bit special pixels from changing their value when they are mapped
  // to the 16 bit output.
  CubeAttributeOutput &outAtt = ui.GetOutputAttribute("TO");
  outAtt.setPixelType(Isis::SignedWord);
  outAtt.setMinimum((double)VALID_MIN2);
  outAtt.setMaximum((double)VALID_MAX2);
  Cube *ocube = p.SetOutputCube(ui.GetFileName("TO"), outAtt);
  p.StartProcess();
  TranslateHiriseEdrLabels(inFile, ocube);

  // Pull out the lookup table so we can apply it in the second pass
  // and remove it from the labels.
  // Add the UNLUTTED keyword to the instrument group so we know
  // if the lut has been used to convert back to 14 bit data
  PvlGroup &instgrp = ocube->group("Instrument");
  PvlKeyword lutKey = instgrp["LookupTable"];
  PvlSequence lutSeq;
  lutSeq = lutKey;

  // Set up the Stretch object with the info from the lookup table
  // If the first entry is (0,0) then no lut was applied.
  if((lutKey.isNull()) ||
      (lutSeq.Size() == 1 && lutSeq[0][0] == "0" && lutSeq[0][1] == "0")) {
    stretch.AddPair(0.0, 0.0);
    stretch.AddPair(65536.0, 65536.0);
    instgrp.addKeyword(PvlKeyword("Unlutted", "TRUE"));
    instgrp.deleteKeyword("LookupTable");
  }
  // The user wants it unlutted
  else if(ui.GetBoolean("UNLUT")) {
    for(int i = 0; i < lutSeq.Size(); i++) {
      stretch.AddPair(i, ((toDouble(lutSeq[i][0]) + toDouble(lutSeq[i][1])) / 2.0));
    }
    instgrp.addKeyword(PvlKeyword("Unlutted", "TRUE"));
    instgrp.deleteKeyword("LookupTable");
  }
  // The user does not want the data unlutted
  else {
    stretch.AddPair(0.0, 0.0);
    stretch.AddPair(65536.0, 65536.0);
    instgrp.addKeyword(PvlKeyword("Unlutted", "FALSE"));
  }

  // Save the calibration and ancillary data as BLOBs. Both get run thru the
  // lookup table just like the image data.
  SaveHiriseCalibrationData(p, ocube, pdsLabel);
  SaveHiriseAncillaryData(p, ocube);

  // Save off the input bit type so we know how to process it on the
  // second pass below.
  Isis::PixelType inType = p.PixelType();

  // All finished with the ImportPds object
  p.EndProcess();


  // Make another pass thru the data using the output file in read/write mode
  // This allows us to correct gaps, remap special pixels and accumulate some
  // counts
  lsbGap = ui.GetBoolean("LSBGAP");
  ProcessByLine p2;
  QString ioFile = ui.GetFileName("TO");
  CubeAttributeInput att;
  p2.SetInputCube(ioFile, att, ReadWrite);
  p2.Progress()->SetText("Converting special pixels");
  section = 4;
  p2.StartProcess((inType == Isis::UnsignedByte) ? FixDns8 : FixDns16);
  p2.EndProcess();


  // Log the results of the image conversion
  PvlGroup results("Results");
  results += PvlKeyword("From", inFile.expanded());

  results += PvlKeyword("CalibrationBufferGaps", toString(gapCount[0]));
  results += PvlKeyword("CalibrationBufferLIS", toString(lisCount[0]));
  results += PvlKeyword("CalibrationBufferHIS", toString(hisCount[0]));
  results += PvlKeyword("CalibrationBufferPossibleGaps", toString(suspectGapCount[0]));
  results += PvlKeyword("CalibrationBufferInvalid", toString(invalidCount[0]));
  results += PvlKeyword("CalibrationBufferValid", toString(validCount[0]));

  results += PvlKeyword("CalibrationImageGaps", toString(gapCount[1]));
  results += PvlKeyword("CalibrationImageLIS", toString(lisCount[1]));
  results += PvlKeyword("CalibrationImageHIS", toString(hisCount[1]));
  results += PvlKeyword("CalibrationImagePossibleGaps", toString(suspectGapCount[1]));
  results += PvlKeyword("CalibrationImageInvalid", toString(invalidCount[1]));
  results += PvlKeyword("CalibrationImageValid", toString(validCount[1]));

  results += PvlKeyword("CalibrationDarkGaps", toString(gapCount[2]));
  results += PvlKeyword("CalibrationDarkLIS", toString(lisCount[2]));
  results += PvlKeyword("CalibrationDarkHIS", toString(hisCount[2]));
  results += PvlKeyword("CalibrationDarkPossibleGaps", toString(suspectGapCount[2]));
  results += PvlKeyword("CalibrationDarkInvalid", toString(invalidCount[2]));
  results += PvlKeyword("CalibrationDarkValid", toString(validCount[2]));

  results += PvlKeyword("ObservationBufferGaps", toString(gapCount[3]));
  results += PvlKeyword("ObservationBufferLIS", toString(lisCount[3]));
  results += PvlKeyword("ObservationBufferHIS", toString(hisCount[3]));
  results += PvlKeyword("ObservationBufferPossibleGaps", toString(suspectGapCount[3]));
  results += PvlKeyword("ObservationBufferInvalid", toString(invalidCount[3]));
  results += PvlKeyword("ObservationBufferValid", toString(validCount[3]));

  results += PvlKeyword("ObservationImageGaps", toString(gapCount[4]));
  results += PvlKeyword("ObservationImageLIS", toString(lisCount[4]));
  results += PvlKeyword("ObservationImageHIS", toString(hisCount[4]));
  results += PvlKeyword("ObservationImagePossibleGaps", toString(suspectGapCount[4]));
  results += PvlKeyword("ObservationImageInvalid", toString(invalidCount[4]));
  results += PvlKeyword("ObservationImageValid", toString(validCount[4]));

  results += PvlKeyword("ObservationDarkGaps", toString(gapCount[5]));
  results += PvlKeyword("ObservationDarkLIS", toString(lisCount[5]));
  results += PvlKeyword("ObservationDarkHIS", toString(hisCount[5]));
  results += PvlKeyword("ObservationDarkPossibleGaps", toString(suspectGapCount[5]));
  results += PvlKeyword("ObservationDarkInvalid", toString(invalidCount[5]));
  results += PvlKeyword("ObservationDarkValid", toString(validCount[5]));

  // Write the results to the log
  Application::Log(results);

  return;
}


void TranslateHiriseEdrLabels(FileName &labelFile, Cube *ocube) {

  //Create a PVL to store the translated labels
  Pvl outLabel;

  // Get the directory where the MRO HiRISE translation tables are.
  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["Mro"] + "/translations/";

  // Get a filename for the HiRISE EDR label
  Pvl labelPvl(labelFile.expanded());

  // Translate the Instrument group
  FileName transFile(transDir + "hiriseInstrument.trn");
  PvlToPvlTranslationManager instrumentXlater(labelPvl, transFile.expanded());
  instrumentXlater.Auto(outLabel);

  // Translate the BandBin group
  transFile  = transDir + "hiriseBandBin.trn";
  PvlToPvlTranslationManager bandBinXlater(labelPvl, transFile.expanded());
  bandBinXlater.Auto(outLabel);

  // Translate the Archive group
  transFile  = transDir + "hiriseArchive.trn";
  PvlToPvlTranslationManager archiveXlater(labelPvl, transFile.expanded());
  archiveXlater.Auto(outLabel);

  // Create the Instrument group keyword CcdId from the ProductId
  // SCS 28-03-06 Do it in the instrument translation table instead of here
//  PvlGroup &archiveGroup(outLabel.findGroup("Archive", Pvl::Traverse));
//  QString productId = (QString)archiveGroup.findKeyword("ProductId");
//  productId.Token("_");
//  productId.Token("_");
//  productId = productId.Token("_");
//  outLabel.findGroup("Instrument", Pvl::Traverse) +=
//      PvlKeyword ("CcdId", productId);

  // Create the Kernel Group
  PvlGroup kerns("Kernels");
  kerns += PvlKeyword("NaifIkCode", "-74699");

  // Write the Instrument, BandBin, Archive, and Kernels groups to the output
  // cube label
  ocube->putGroup(outLabel.findGroup("Instrument", Pvl::Traverse));
  ocube->putGroup(outLabel.findGroup("BandBin", Pvl::Traverse));
  ocube->putGroup(outLabel.findGroup("Archive", Pvl::Traverse));
  ocube->putGroup(kerns);
}


// The input buffer has a raw 16 bit buffer but the values are still 0 to 255
void FixDns8(Buffer &buf) {

  // Convert all 8bit image values of =255 (xFF) to 16bit NULL, count as gap
  // Convert all 8bit image values of =254 (xFE) to 16bit HIS, count as HIS
  // Convert all 8bit image values of =0 (x00) to 16bit LIS, count as NULL
  // Convert 8bit image data to 16bit by applying the LUT

  short int *raw = (short int *)(buf.RawBuffer());

  for(int i = 0; i < buf.size(); i++) {

    if(raw[i] == (short int)255) {
      buf[i] = Isis::NULL8;
      gapCount[section]++;
    }
    else if(raw[i] == (short int)254) {
      buf[i] = Isis::HIGH_INSTR_SAT8;
      hisCount[section]++;
    }
    else if(raw[i] == (short int)0) {
      buf[i] = Isis::LOW_INSTR_SAT8;
      lisCount[section]++;
    }
    else { // It's valid so just run it thru the lookup table to get a 16 bit dn
      buf[i] = stretch.Map(buf[i]);
      validCount[section]++;
    }
  }
}


void FixDns16(Buffer &buf) {

  // Convert 16-bit image values of =65535 (xFFFF) to NULL, count as gap
  // Convert 16-bit image values of >16383 (x3FFF) to NULL, count as gap
  // Convert 16-bit image values of =16383 (x3FFF) to HIS, count as HIS
  // Convert 16-bit image values of =0 (x00) to LIS, count as LIS

  short int *raw = (short int *)(buf.RawBuffer());

  for(int i = 0; i < buf.size(); i++) {

    // This pixel is a gap
    if(raw[i] == (short int)0xffff) {  // 0xffff = -1 = 65535 = gap
      buf[i] = Isis::NULL8;
      gapCount[section]++;
    }
    // The low order byte of this pixel is the beginning of a gap
    else if((i + 1 < buf.size()) && // is there at least on more pixel to look at
            (raw[i+1] == (short int)0xffff) && // Is the next pixel a gap
            ((short int)(raw[i] & 0x00ff) == (short int)0x00ff)) { // Is the low byte 0xff
      suspectGapCount[section]++;
      if(lsbGap) {
        buf[i] = Isis::NULL8;
      }
    }
    // This pixel is not within the legal 14 bit range (0) to (16383=0x3fff)
    // It might be the end of a gap (0xff??) but it's still illegal
    else if((raw[i] > 16383) || (raw[i] < 0)) {
      buf[i] = Isis::NULL8;
      invalidCount[section]++;
    }
    // This pixel is saturated on the bright end
    else if(raw[i] == 16383) {  // Max value for instrument
      buf[i] = Isis::HIGH_INSTR_SAT8;
      hisCount[section]++;
    }
    // This pixel is saturated on the dark end
    // Shouldn't happen because dark currents are above zero (0)
    else if(raw[i] == 0) {
      buf[i] = Isis::LOW_INSTR_SAT8;
      lisCount[section]++;
    }
    // Pixel value is ok, so just leave it alone
    else {
      validCount[section]++;
    }
  }
}
