#include <iostream>

#include "ProcessImportPds.h"

#include "Buffer.h"
#include "Table.h"

using namespace std; 
using namespace Isis;

extern int gapCount[6];
extern int suspectGapCount[6];
extern int invalidCount[6];
extern int lisCount[6];
extern int hisCount[6];
extern int validCount[6];
extern int section;


// Construct two BLOBs one to hold the Hirise calibration line prefix/suffix data
// and the other to hold the calibration image line
void SaveHiriseCalibrationData (ProcessImportPds &process, Cube *ocube,
                                Pvl &pdsLabel) {

  vector<int> ConvertCalibrationPixels (int samples,
                                        Isis::PixelType pixelType,
                                        unsigned char *data);

  // Create the Table to hold the prefix/suffix data
  TableField gap("GapFlag", TableField::Integer);
  TableField line("LineNumber", TableField::Integer);
  TableField buffer("BufferPixels", TableField::Integer, 12);
  TableField dark("DarkPixels", TableField::Integer, 16);

  TableRecord calAncillaryRecord;
  calAncillaryRecord += gap;
  calAncillaryRecord += line;
  calAncillaryRecord += buffer;
  calAncillaryRecord += dark;

  Table calAncillaryTable("HiRISE Calibration Ancillary", calAncillaryRecord);

  // Create the Table to hold the calibration lines
  TableField image("Calibration", TableField::Integer, ocube->Samples());

  TableRecord calImageRecord;
  calImageRecord += image;

  Table calImageTable("HiRISE Calibration Image", calImageRecord);


  // Find the beginning of the calibration data
  unsigned char *header = (unsigned char *)process.FileHeader();
  header += (int)pdsLabel["^CALIBRATION_LINE_PREFIX_TABLE"] - 1;

  // Get the number of lines in the calibration area. This includes the
  // calibration, mask and ramp lines
  int calsize = (int)(pdsLabel.FindObject("CALIBRATION_IMAGE")["LINES"]);

  // Loop through the calibration lines and extract the info needed for the
  // tables.
  for (unsigned int l=0; l<(unsigned int)calsize; l++) {

    // Pull out the gap byte (in byte 0)
    calAncillaryRecord[0] = (int)(*header++);

    // Skip the sync patterns and channel number (bytes 1 and 2)
    header++;
    header++;

    // Pull out the line number (bytes 3-5 3=MSB, 5=LSB)
    int lineCounter = 0;
    lineCounter += (int)(*header++) << 16;
    lineCounter += (int)(*header++) << 8;
    lineCounter += (int)(*header++);
    calAncillaryRecord[1] = lineCounter;

    // Pull the 12 buffer pixels (same type as image data)
    section = 0;
    calAncillaryRecord[2] = ConvertCalibrationPixels (12, process.PixelType(),
                                                      header);
    header += 12 * SizeOf(process.PixelType());

    // Don't add this record to the table yet. It still needs the dark pixels.

    // Pull the calibration pixels out (same type as image data)
    section = 1;
    calImageRecord[0] = ConvertCalibrationPixels (ocube->Samples(),
                                                  process.PixelType(), header);
    header += ocube->Samples() * SizeOf(process.PixelType());
    calImageTable += calImageRecord;


    // Pull the 16 dark pixels (same type as image data)
    section = 2;
    calAncillaryRecord[3] = ConvertCalibrationPixels (16, process.PixelType(),
                                                      header);
    header += 16 * SizeOf(process.PixelType());

    // Add this record to the table
    calAncillaryTable += calAncillaryRecord;
  }

  // Add the tables to the output cube
  ocube->Write(calAncillaryTable);
  ocube->Write(calImageTable);
}



vector<int> ConvertCalibrationPixels (int samples,
                                      Isis::PixelType pixelType,
                                      unsigned char *data) {
  void FixDns8(Buffer &buf);
  void FixDns16(Buffer &buf);


  // Pull the calibration pixels out (same type as image data)
  // The buffer is always 16-bit because FixDn8/16 both expect a 16-bit
  // buffer, but are looking for original data ranges and specific values
  Isis::Buffer pixelBuf(samples, 1, 1, Isis::SignedWord);

  for (int b=0; b<samples; b++) {
    short int pixel = 0;
    int shift = 8 * (SizeOf(pixelType) - 1);
    for (int byte=0; byte<SizeOf(pixelType); byte++) {
      pixel += ((unsigned int)(*data++)) << shift;
      shift -= 8;
    }

    ((short int*)pixelBuf.RawBuffer())[b] = pixel;

    pixelBuf[b] = pixel;
  }

  // Convert gaps, HiRISE special pixels... to 16 bit
  if (pixelType == Isis::UnsignedByte) {
    FixDns8 (pixelBuf);
  }
  else {
    FixDns16 (pixelBuf);
  }

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

