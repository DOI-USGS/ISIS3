#include "Isis.h"

#include "Cube.h"
#include "IException.h"
#include "IString.h"
#include "OriginalLabel.h"
#include "ProcessImport.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "UserInterface.h"

#include <fstream>
#include <iostream>
#include <QString>

using namespace std;
using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();

  QString from = ui.GetFileName("FROM");

  // Setup to read headers/labels
  ifstream input;
  input.open(from.toAscii().data(), ios::in | ios::binary);

  // Check stream open status
  if(!input.is_open()) {
    QString msg = "Cannot open input file [" + from + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  char reading[81];
  IString line = "";
  unsigned int place = 0;
  PvlGroup labels("OriginalLabels");

  // Load first line
  input.seekg(0);
  input.read(reading, 80);
  reading[80] = '\0';
  line = reading;
  place += 80;

  // Read in and place in PvlKeywords and a PvlGroup
  while(line.substr(0, 3) != "END") {
    // Check for blank lines
    if(line.substr(0, 1) != " " && line.substr(0, 1) != "/") {
      // Name of keyword
      PvlKeyword label(line.Token(" =").ToQt());
      // Remove up to beginning of data
      line.TrimHead(" ='");
      line.TrimTail(" ");
      if(label.name() == "COMMENT" || label.name() == "HISTORY") {
        label += line.ToQt();
      }
      else {
        // Access the data without the comment if there is one
        IString value = line.Token("/");
        // Clear to end of data, including single quotes
        value.TrimTail(" '");
        label += value.ToQt();
        line.TrimHead(" ");
        // If the remaining line QString has anything, it is comments.
        if(line.size() > 0) {
          label.addComment(line.ToQt());
          // A possible format for units, other possiblites exist.
          if(line != line.Token("[")) {
            label.setUnits(line.Token("[").Token("]").ToQt());
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
  if(labels["SIMPLE"][0] == "F") {
    QString msg = "The file [" + ui.GetFileName("FROM") + "] does not conform to the FITS standards";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  ProcessImport pfits;

  pfits.SetInputFile(ui.GetFileName("FROM"));

  // Header size will be a multiple of 2880
  int multiple = (int)((place + 2881) / 2880);
  pfits.SetFileHeaderBytes(multiple * 2880);
  pfits.SaveFileHeader();

  // Find pixel type, there are several unsupported possiblites
  Isis::PixelType type;
  QString msg = "";
  switch(toInt(labels["BITPIX"][0])) {
    case 8:
      type = Isis::UnsignedByte;
      break;
    case 16:
      type = Isis::SignedWord;
      break;
    case 32:
      msg = "Signed 32 bit integer (int) pixel type is not supported at this time";
      throw IException(IException::User, msg, _FILEINFO_);
      break;
    case 64:
      msg = "Signed 64 bit integer (long) pixel type is not supported at this time";
      throw IException(IException::User, msg, _FILEINFO_);
      break;
    case -32:
      type = Isis::Real;
      break;
    case -64:
      msg = "64 bit floating point (double) pixel type is not supported at this time";
      throw IException(IException::User, msg, _FILEINFO_);
      break;
    default:
      msg = "Unknown pixel type [" + labels["BITPIX"][0] + "] cannot be imported";
      throw IException(IException::User, msg, _FILEINFO_);
      break;
  }

  pfits.SetPixelType(type);

  // It is possible to have a NAXIS value of 0 meaning no data, the file could include
  // xtensions with data, however, those aren't supported as of Oct '09
  if(toInt(labels["NAXIS"][0]) == 2) {
    pfits.SetDimensions(toInt(labels["NAXIS1"][0]), toInt(labels["NAXIS2"][0]), 1);
  }
  else if(toInt(labels["NAXIS"][0]) == 3) {
    pfits.SetDimensions(toInt(labels["NAXIS1"][0]), toInt(labels["NAXIS2"][0]),
                        toInt(labels["NAXIS3"][0]));
  }
  else {
    QString msg = "NAXIS count of [" + labels["NAXIS"][0] + "] is not supported at this time";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Base and multiplier
  if(labels.hasKeyword("BZERO")) {
    pfits.SetBase(toDouble(labels["BZERO"][0]));
  }
  if(labels.hasKeyword("BSCALE")) {
    pfits.SetMultiplier(toDouble(labels["BSCALE"][0]));
  }

  // Byte order
  pfits.SetByteOrder(Isis::Msb);

  // Limited section of standardized keywords that could exist
  bool instGrp = false;
  PvlGroup inst("Instrument");
  if(labels.hasKeyword("DATE-OBS")) {
    instGrp = true;
    inst += PvlKeyword("StartTime", labels["DATE-OBS"][0]);
  }
  if(labels.hasKeyword("OBJECT")) {
    instGrp = true;
    inst += PvlKeyword("Target", labels["OBJECT"][0]);
  }
  if(labels.hasKeyword("INSTRUME")) {
    instGrp = true;
    inst += PvlKeyword("InstrumentId", labels["INSTRUME"][0]);
  }
  if(labels.hasKeyword("OBSERVER")) {
    instGrp = true;
    inst += PvlKeyword("SpacecraftName", labels["OBSERVER"][0]);
  }

  Cube *output = pfits.SetOutputCube("TO");

  // Add instrument group if any relevant data exists
  Pvl *lbls = output->label();
  if(instGrp) {
    lbls->findObject("IsisCube") += inst;
  }

  // Save original labels
  Pvl pvl;
  pvl += labels;
  OriginalLabel originals(pvl);
  output->write(originals);

  // Process...
  pfits.StartProcess();
  pfits.EndProcess();
}
