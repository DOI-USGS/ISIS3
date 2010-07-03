#include "ProcessImportPds.h"

#include "TableRecord.h"
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

// Construct a BLOb to contain the Hirise main line suffix and prefix data
void SaveHiriseAncillaryData (ProcessImportPds &process, Cube *ocube) {

  vector<int> ConvertCalibrationPixels (int samples,
                                        Isis::PixelType pixelType,
                                        unsigned char *data);


  // Setup a Table to hold the main image prefix/suffix data
  TableField gap("GapFlag", TableField::Integer);
  TableField line("LineNumber", TableField::Integer);
  TableField buffer("BufferPixels", TableField::Integer, 12);
  TableField dark("DarkPixels", TableField::Integer, 16);

  TableRecord rec;
  rec += gap;
  rec += line;
  rec += buffer;
  rec += dark;

  Table table("HiRISE Ancillary", rec);
  table.SetAssociation (Table::Lines);

  // Loop through all the prefix and suffix data and construct the table records
  // In the case of HiRISE there is only one band so the outside vector
  // only contains one entry. The inside vector contains nl entries.
  vector<vector<char *> > pre = process.DataPrefix();
  vector<vector<char *> > suf = process.DataSuffix();
  vector<char *> prefix = pre.at(0);
  vector<char *> suffix = suf.at(0);

  Progress progress;
  progress.SetText("Saving ancillary data");
  progress.SetMaximumSteps(prefix.size());
  progress.CheckStatus();

  for (unsigned int l=0; l<prefix.size(); l++) {

    unsigned char *linePrefix = (unsigned char *)(prefix[l]);

    // Pull out the gap byte (in byte 0)
    rec[0] = (int)(linePrefix[0]);

    // Pull out the line number (bytes 3-5 3=MSB, 5=LSB)
    int lineCounter = 0;
    lineCounter += ((int)(linePrefix[3])) << 16;
    lineCounter += ((int)(linePrefix[4])) << 8;
    lineCounter += ((int)(linePrefix[5]));
    rec[1] = lineCounter;

    // Pull the 12 buffer pixels (same type as image data)
    // from the image prefix area
    linePrefix += 6;
    section = 3;
    rec[2] = ConvertCalibrationPixels (12, process.PixelType(),linePrefix);
    linePrefix += 12 * SizeOf(process.PixelType());

    // Pull the 16 dark pixels (same type as image data)
    // from the image suffix area
    unsigned char *lineSuffix = (unsigned char *)(suffix[l]);
    section = 5;
    rec[3] = ConvertCalibrationPixels (16, process.PixelType(),lineSuffix);
    lineSuffix += 16 * SizeOf(process.PixelType());

    // Add this record to the table
    table += rec;

    // Report the progress
    progress.CheckStatus();
  }

  // Add the table to the output cube
  ocube->Write(table);
}
