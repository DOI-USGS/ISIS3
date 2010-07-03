#include "Isis.h"

#include "Cube.h"
#include "iException.h"
#include "iString.h"
#include "OriginalLabel.h"
#include "ProcessImport.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "UserInterface.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();

  string from = ui.GetFilename("FROM");

  // Setup to read headers/labels
  ifstream input;
  input.open(from.c_str(), ios::in | ios::binary);

  // Check stream open status
  if (!input.is_open()) {
    string msg = "Cannot open input file [" + from + "]";
    throw Isis::iException::Message(Isis::iException::Io, msg, _FILEINFO_);
  }

  char reading[81];
  iString line = "";
  unsigned int place = 0;
  PvlGroup labels("OriginalLabels");

  // Load first line
  input.seekg(0);
  input.read(reading, 80);
  reading[80] = '\0';
  line = reading;
  place += 80;

  // Read in and place in PvlKeywords and a PvlGroup
  while (line.substr(0,3) != "END") {
    // Check for blank lines
    if (line.substr(0,1) != " " && line.substr(0,1) != "/") {
      // Name of keyword
      PvlKeyword label(line.Token(" ="));
      // Remove up to beginning of data
      line.TrimHead(" ='");
      line.TrimTail(" ");
      if (label.Name() == "COMMENT" || label.Name() == "HISTORY") {
        label += line;
      }
      else {
        // Access the data without the comment if there is one
        iString value = line.Token("/");
        // Clear to end of data, including single quotes
        value.TrimTail(" '");        
        label += value;
        line.TrimHead(" ");
        // If the remaining line string has anything, it is comments.
        if (line.size() > 0) {
          label.AddComment(line);
          // A possible format for units, other possiblites exist.
          if (line != line.Token("[")) {
            label.SetUnits(line.Token("[").Token("]"));
          }
        }
      }
      labels += label;
    }
    // Load next line
    input.seekg(place);
    input.read(reading, 80);
    reading[80] = '\0';
    place += 80;
    line = reading;
  }

  // Done with stream
  input.close();

  // Its possible they could have this instead of T, in which case we won't even try
  if (labels["SIMPLE"][0] == "F") {
    string msg = "The file [" + ui.GetFilename("FROM") + "] does not conform to the FITS standards";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  ProcessImport pfits;

  pfits.SetInputFile(ui.GetFilename("FROM"));

  // Header size will be a multiple of 2880
  int multiple = (int)((place + 2881)/2880);
  pfits.SetFileHeaderBytes(multiple * 2880);
  pfits.SaveFileHeader();

  // Find pixel type, there are several unsupported possiblites
  Isis::PixelType type;
  string msg = "";
  switch (labels["BITPIX"][0].ToInteger()) {
    case 8: 
      type = Isis::UnsignedByte;
      break;
    case 16: 
      type = Isis::SignedWord;
      break;
    case 32: 
      msg = "Signed 32 bit integer (int) pixel type is not supported at this time";
      throw iException::Message(iException::User, msg, _FILEINFO_);
      break;
    case 64:
      msg = "Signed 64 bit integer (long) pixel type is not supported at this time";
      throw iException::Message(iException::User, msg, _FILEINFO_);
      break;
    case -32: 
      type = Isis::Real;
      break;
    case -64: 
      msg = "64 bit floating point (double) pixel type is not supported at this time";
      throw iException::Message(iException::User, msg, _FILEINFO_);
      break;
    default:
      msg = "Unknown pixel type [" + labels["BITPIX"][0] + "] cannot be imported";
      throw iException::Message(iException::User, msg, _FILEINFO_);
      break; 
  }

  pfits.SetPixelType(type);

  // It is possible to have a NAXIS value of 0 meaning no data, the file could include 
  // xtensions with data, however, those aren't supported as of Oct '09  
  if (labels["NAXIS"][0].ToInteger() == 2) {
    pfits.SetDimensions(labels["NAXIS1"][0], labels["NAXIS2"][0], 1);
  }
  else if (labels["NAXIS"][0].ToInteger() == 3) {
    pfits.SetDimensions(labels["NAXIS1"][0], labels["NAXIS2"][0], labels["NAXIS3"][0]);
  }
  else {
    string msg = "NAXIS count of [" + labels["NAXIS"][0] + "] is not supported at this time";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  // Base and multiplier
  if (labels.HasKeyword("BZERO")) {
    pfits.SetBase(labels["BZERO"][0]);
  }
  if (labels.HasKeyword("BSCALE")) {
    pfits.SetMultiplier(labels["BSCALE"][0]);
  }

  // Byte order
  pfits.SetByteOrder(Isis::Msb);  

  // Limited section of standardized keywords that could exist
  bool instGrp = false;
  PvlGroup inst("Instrument");
  if (labels.HasKeyword("DATE-OBS")) {
    instGrp = true;
    inst += PvlKeyword("StartTime", labels["DATE-OBS"][0]);
  }
  if (labels.HasKeyword("OBJECT")) {
    instGrp = true;
    inst += PvlKeyword("Target", labels["OBJECT"][0]);
  }
  if (labels.HasKeyword("INSTRUME")) {
    instGrp = true;
    inst += PvlKeyword("InstrumentId", labels["INSTRUME"][0]);
  }
  if (labels.HasKeyword("OBSERVER")) {
    instGrp = true;
    inst += PvlKeyword("SpacecraftName", labels["OBSERVER"][0]);
  }

  Cube * output = pfits.SetOutputCube("TO");

  // Add instrument group if any relevant data exists
  Pvl * lbls = output->Label();
  if (instGrp) {
    lbls->FindObject("IsisCube") += inst;
  }

  // Save original labels
  Pvl pvl;
  pvl += labels;
  OriginalLabel originals(pvl);
  output->Write(originals);

  // Process...
  pfits.StartProcess();
  pfits.EndProcess();
}
