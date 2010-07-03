#include "Isis.h"

#include <cstdio>
#include <string>

#include "ProcessImportPds.h"
#include "ProcessByLine.h"

#include "UserInterface.h"
#include "Filename.h"
#include "iException.h"
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


void IsisMain ()
{
  stretch.ClearPairs();

  for (int i=0; i<6; i++) {
    gapCount[i] = 0;
    suspectGapCount[i] = 0;
    invalidCount[i] = 0;
    lisCount[i] = 0;
    hisCount[i] = 0;
    validCount[i] = 0;
  }

  void TranslateHiriseEdrLabels (Filename &labelFile, Cube *);
  void SaveHiriseCalibrationData (ProcessImportPds &process, Cube *,
                                  Pvl &pdsLabel);
  void SaveHiriseAncillaryData (ProcessImportPds &process, Cube *);
  void FixDns8 (Buffer &buf);
  void FixDns16 (Buffer &buf);

  ProcessImportPds p;
  Pvl pdsLabel;
  UserInterface &ui = Application::GetUserInterface();

  // Get the input filename and make sure it is a HiRISE EDR
  Filename inFile = ui.GetFilename("FROM");
  iString id;
  bool projected;
  try {
    Pvl lab(inFile.Expanded());
    id = (string) lab.FindKeyword ("DATA_SET_ID");
    projected = lab.HasObject("IMAGE_MAP_PROJECTION");
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
  if (id != "MRO-M-HIRISE-2-EDR-V1.0") {
    string msg = "Input file [" + inFile.Expanded() + "] does not appear to be " +
                 "in HiRISE EDR format. DATA_SET_ID is [" + id + "]";
    throw iException::Message(iException::Io,msg, _FILEINFO_);
  }

  p.SetPdsFile (inFile.Expanded(), "", pdsLabel);

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
  outAtt.PixelType (Isis::SignedWord);
  outAtt.Minimum((double)VALID_MIN2);
  outAtt.Maximum((double)VALID_MAX2);
  Cube *ocube = p.SetOutputCube(ui.GetFilename("TO"), outAtt);
  p.StartProcess ();
  TranslateHiriseEdrLabels (inFile, ocube);

  // Pull out the lookup table so we can apply it in the second pass
  // and remove it from the labels.
  // Add the UNLUTTED keyword to the instrument group so we know
  // if the lut has been used to convert back to 14 bit data
  PvlGroup &instgrp = ocube->GetGroup("Instrument");
  PvlKeyword lutKey = instgrp["LookupTable"];
  PvlSequence lutSeq;
  lutSeq = lutKey;

  // Set up the Stretch object with the info from the lookup table
  // If the first entry is (0,0) then no lut was applied.
  if ((lutKey.IsNull()) ||
      (lutSeq.Size()==1 && lutSeq[0][0]=="0" && lutSeq[0][1]=="0")) {
    stretch.AddPair(0.0, 0.0);
    stretch.AddPair(65536.0, 65536.0);
    instgrp.AddKeyword(PvlKeyword("Unlutted","TRUE"));
    instgrp.DeleteKeyword ("LookupTable");
  }
  // The user wants it unlutted
  else if (ui.GetBoolean("UNLUT")) {
    for (int i=0; i<lutSeq.Size(); i++) {
      stretch.AddPair(i, (((double)lutSeq[i][0] + (double)lutSeq[i][1]) / 2.0));
    }
    instgrp.AddKeyword(PvlKeyword("Unlutted","TRUE"));
    instgrp.DeleteKeyword ("LookupTable");
  }
  // The user does not want the data unlutted
  else {
    stretch.AddPair(0.0, 0.0);
    stretch.AddPair(65536.0, 65536.0);
    instgrp.AddKeyword(PvlKeyword("Unlutted","FALSE"));
  }
  ocube->PutGroup(instgrp);

  // Save the calibration and ancillary data as BLOBs. Both get run thru the
  // lookup table just like the image data.
  SaveHiriseCalibrationData (p, ocube, pdsLabel);
  SaveHiriseAncillaryData (p, ocube);
  
  // Save off the input bit type so we know how to process it on the
  // second pass below.
  Isis::PixelType inType = p.PixelType();

  // All finished with the ImportPds object
  p.EndProcess ();


  // Make another pass thru the data using the output file in read/write mode
  // This allows us to correct gaps, remap special pixels and accumulate some
  // counts
  lsbGap = ui.GetBoolean("LSBGAP");
  ProcessByLine p2;
  string ioFile = ui.GetFilename("TO");
  CubeAttributeInput att;
  p2.SetInputCube(ioFile, att, ReadWrite);
  p2.Progress()->SetText("Converting special pixels");
  section = 4;
  p2.StartProcess((inType == Isis::UnsignedByte) ? FixDns8 : FixDns16);
  p2.EndProcess();


  // Log the results of the image conversion
  PvlGroup results("Results");
  results += PvlKeyword ("From", inFile.Expanded());

  results += PvlKeyword ("CalibrationBufferGaps", gapCount[0]);
  results += PvlKeyword ("CalibrationBufferLIS", lisCount[0]);
  results += PvlKeyword ("CalibrationBufferHIS", hisCount[0]);
  results += PvlKeyword ("CalibrationBufferPossibleGaps", suspectGapCount[0]);
  results += PvlKeyword ("CalibrationBufferInvalid", invalidCount[0]);
  results += PvlKeyword ("CalibrationBufferValid", validCount[0]);

  results += PvlKeyword ("CalibrationImageGaps", gapCount[1]);
  results += PvlKeyword ("CalibrationImageLIS", lisCount[1]);
  results += PvlKeyword ("CalibrationImageHIS", hisCount[1]);
  results += PvlKeyword ("CalibrationImagePossibleGaps", suspectGapCount[1]);
  results += PvlKeyword ("CalibrationImageInvalid", invalidCount[1]);
  results += PvlKeyword ("CalibrationImageValid", validCount[1]);

  results += PvlKeyword ("CalibrationDarkGaps", gapCount[2]);
  results += PvlKeyword ("CalibrationDarkLIS", lisCount[2]);
  results += PvlKeyword ("CalibrationDarkHIS", hisCount[2]);
  results += PvlKeyword ("CalibrationDarkPossibleGaps", suspectGapCount[2]);
  results += PvlKeyword ("CalibrationDarkInvalid", invalidCount[2]);
  results += PvlKeyword ("CalibrationDarkValid", validCount[2]);

  results += PvlKeyword ("ObservationBufferGaps", gapCount[3]);
  results += PvlKeyword ("ObservationBufferLIS", lisCount[3]);
  results += PvlKeyword ("ObservationBufferHIS", hisCount[3]);
  results += PvlKeyword ("ObservationBufferPossibleGaps", suspectGapCount[3]);
  results += PvlKeyword ("ObservationBufferInvalid", invalidCount[3]);
  results += PvlKeyword ("ObservationBufferValid", validCount[3]);

  results += PvlKeyword ("ObservationImageGaps", gapCount[4]);
  results += PvlKeyword ("ObservationImageLIS", lisCount[4]);
  results += PvlKeyword ("ObservationImageHIS", hisCount[4]);
  results += PvlKeyword ("ObservationImagePossibleGaps", suspectGapCount[4]);
  results += PvlKeyword ("ObservationImageInvalid", invalidCount[4]);
  results += PvlKeyword ("ObservationImageValid", validCount[4]);

  results += PvlKeyword ("ObservationDarkGaps", gapCount[5]);
  results += PvlKeyword ("ObservationDarkLIS", lisCount[5]);
  results += PvlKeyword ("ObservationDarkHIS", hisCount[5]);
  results += PvlKeyword ("ObservationDarkPossibleGaps", suspectGapCount[5]);
  results += PvlKeyword ("ObservationDarkInvalid", invalidCount[5]);
  results += PvlKeyword ("ObservationDarkValid", validCount[5]);

  // Write the results to the log
  Application::Log(results);

  return;
}


void TranslateHiriseEdrLabels (Filename &labelFile, Cube *ocube) {

  //Create a PVL to store the translated labels
  Pvl outLabel;

  // Get the directory where the MRO HiRISE translation tables are.
  PvlGroup dataDir (Preference::Preferences().FindGroup("DataDirectory"));
  iString transDir = (string) dataDir["Mro"] + "/translations/";

  // Get a filename for the HiRISE EDR label
  Pvl labelPvl (labelFile.Expanded());

  // Translate the Instrument group
  Filename transFile (transDir + "hiriseInstrument.trn");
  PvlTranslationManager instrumentXlater (labelPvl, transFile.Expanded());
  instrumentXlater.Auto (outLabel);

  // Translate the BandBin group
  transFile  = transDir + "hiriseBandBin.trn";
  PvlTranslationManager bandBinXlater (labelPvl, transFile.Expanded());
  bandBinXlater.Auto (outLabel);

  // Translate the Archive group
  transFile  = transDir + "hiriseArchive.trn";
  PvlTranslationManager archiveXlater (labelPvl, transFile.Expanded());  
  archiveXlater.Auto (outLabel);

  // Create the Instrument group keyword CcdId from the ProductId
  // SCS 28-03-06 Do it in the instrument translation table instead of here
//  PvlGroup &archiveGroup(outLabel.FindGroup("Archive", Pvl::Traverse));
//  iString productId = (string)archiveGroup.FindKeyword("ProductId");
//  productId.Token("_");
//  productId.Token("_");
//  productId = productId.Token("_");
//  outLabel.FindGroup("Instrument", Pvl::Traverse) +=
//      PvlKeyword ("CcdId", productId);

  // Create the Kernel Group
  PvlGroup kerns("Kernels");
  kerns += PvlKeyword("NaifIkCode", "-74699");

  // Write the Instrument, BandBin, Archive, and Kernels groups to the output
  // cube label
  ocube->PutGroup (outLabel.FindGroup("Instrument", Pvl::Traverse));
  ocube->PutGroup (outLabel.FindGroup("BandBin", Pvl::Traverse));
  ocube->PutGroup (outLabel.FindGroup("Archive", Pvl::Traverse));
  ocube->PutGroup (kerns);
}


// The input buffer has a raw 16 bit buffer but the values are still 0 to 255
void FixDns8 (Buffer &buf) {

  // Convert all 8bit image values of =255 (xFF) to 16bit NULL, count as gap
  // Convert all 8bit image values of =254 (xFE) to 16bit HIS, count as HIS
  // Convert all 8bit image values of =0 (x00) to 16bit LIS, count as NULL
  // Convert 8bit image data to 16bit by applying the LUT

  short int *raw = (short int*)(buf.RawBuffer());

  for (int i=0; i<buf.size(); i++) {

    if (raw[i] == (short int)255) {
      buf[i] = Isis::NULL8;
      gapCount[section]++;
    }
    else if (raw[i] == (short int)254) {
      buf[i] = Isis::HIGH_INSTR_SAT8;
      hisCount[section]++;
    }
    else if (raw[i] == (short int)0) {
      buf[i] = Isis::LOW_INSTR_SAT8;
      lisCount[section]++;
    }
    else { // It's valid so just run it thru the lookup table to get a 16 bit dn
      buf[i] = stretch.Map(buf[i]);
      validCount[section]++;
    }
  }
}


void FixDns16 (Buffer &buf) {

  // Convert 16-bit image values of =65535 (xFFFF) to NULL, count as gap
  // Convert 16-bit image values of >16383 (x3FFF) to NULL, count as gap
  // Convert 16-bit image values of =16383 (x3FFF) to HIS, count as HIS
  // Convert 16-bit image values of =0 (x00) to LIS, count as LIS

  short int *raw = (short int*)(buf.RawBuffer());

  for (int i=0; i<buf.size(); i++) {

    // This pixel is a gap
    if (raw[i] == (short int)0xffff) { // 0xffff = -1 = 65535 = gap
      buf[i] = Isis::NULL8;
      gapCount[section]++;
    }
    // The low order byte of this pixel is the beginning of a gap
    else if ((i+1<buf.size()) && // is there at least on more pixel to look at
             (raw[i+1] == (short int)0xffff) && // Is the next pixel a gap
             ((short int)(raw[i] & 0x00ff) == (short int)0x00ff)) { // Is the low byte 0xff
      suspectGapCount[section]++;
      if (lsbGap) {
        buf[i] = Isis::NULL8;
      }
    }
    // This pixel is not within the legal 14 bit range (0) to (16383=0x3fff)
    // It might be the end of a gap (0xff??) but it's still illegal
    else if ((raw[i] > 16383) || (raw[i] < 0)) {
      buf[i] = Isis::NULL8;
      invalidCount[section]++;
    }
    // This pixel is saturated on the bright end
    else if (raw[i] == 16383) { // Max value for instrument
      buf[i] = Isis::HIGH_INSTR_SAT8;
      hisCount[section]++;
    }
    // This pixel is saturated on the dark end
    // Shouldn't happen because dark currents are above zero (0)
    else if (raw[i] == 0) {
      buf[i] = Isis::LOW_INSTR_SAT8;
      lisCount[section]++;
    }
    // Pixel value is ok, so just leave it alone
    else {
      validCount[section]++;
    }
  }
}

