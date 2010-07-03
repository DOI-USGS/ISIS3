//************************************************************************
// See Full documentation in isis2fits.xml
//************************************************************************
#include "Isis.h"

#include <iostream>
#include <sstream>

#include "iException.h"
#include "iString.h"
#include "ProcessExport.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlSequence.h"
#include "UserInterface.h"

using namespace std; 
using namespace Isis;

// Global variables
int headerBytes = 0;

string FitsKeyword(string keyword, bool isVal, Isis::iString value, Isis::iString unit = "");

string WritePvl(string fitsKey, string group, iString key, Cube *icube, bool isString);

// Main program
void IsisMain(){
  
  // Create an object for exporting Isis data
  ProcessExport p;
  // Open the input cube
  Cube *icube = p.SetInputCube("FROM");
 
  // Conform to the Big-Endian format for FITS
  if(IsLsb()) p.SetOutputEndian(Isis::Msb);

  // Generate the name of the fits file and open it
  UserInterface &ui = Application::GetUserInterface();
    
  // specify the bits per pixel
  string bitpix;
  if (ui.GetString ("BITTYPE") == "8BIT") bitpix = "8";
  else if (ui.GetString ("BITTYPE") == "16BIT") bitpix = "16";
  else if (ui.GetString ("BITTYPE") == "32BIT") bitpix = "-32";
  else {
    string msg = "Pixel type of [" + ui.GetString("BITTYPE") + "] is unsupported"; 
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  //  Determine bit size and calculate number of bytes to write
  //  for each line.
  if (bitpix == "8") p.SetOutputType(Isis::UnsignedByte);
  if (bitpix == "16") p.SetOutputType(Isis::SignedWord);
  if (bitpix == "-32") p.SetOutputType(Isis::Real);
  
  // determine core base and multiplier, set up the stretch
  PvlGroup pix = icube->Label()->FindObject("IsisCube").FindObject("Core").FindGroup("Pixels");
  double scale = pix["Multiplier"][0].ToDouble();
  double base = pix["Base"][0].ToDouble();

  if (ui.GetString("STRETCH") != "NONE" && bitpix != "-32") {
    if (ui.GetString("STRETCH") == "LINEAR") {
      p.SetInputRange();
    }
    else if (ui.GetString("STRETCH") == "MANUAL") {
       p.SetInputRange(ui.GetDouble("MINIMUM"), ui.GetDouble("MAXIMUM"));
    }
    
    // create a proper scale so pixels look like 32bit data.
    scale = ((p.GetInputMaximum() - p.GetInputMinimum()) *
            (p.GetOutputMaximum() - p.GetOutputMinimum()));

    // round off after 14 decimals to avoid system architecture differences
    scale = ((floor(scale * 1e14)) / 1e14);

    // create a proper zero point so pixels look like 32bit data.
    base = -1.0 * (scale * p.GetOutputMinimum()) + p.GetInputMinimum();
    // round off after 14 decimals to avoid system architecture differences
    base = ((floor(base * 1e14)) / 1e14);
  }

  
  //////////////////////////////////////////
  // Write the minimal fits header	  //
  //////////////////////////////////////////
  string header;
  
  // specify that this file conforms to simple fits standard
  header += FitsKeyword("SIMPLE", true, "T");  
  
  
  // specify the bits per pixel
  header += FitsKeyword("BITPIX", true, bitpix);
  
  // specify the number of data axes (2: samples by lines)
  int axes = 2;
  if (icube->Bands() > 1) {
    axes = 3;
  }
  
  header += FitsKeyword("NAXIS", true, iString(axes));
  
  // specify the limit on data axis 1 (number of samples)
  header += FitsKeyword("NAXIS1", true, iString(icube->Samples()));

  // specify the limit on data axis 2 (number of lines)
  header += FitsKeyword("NAXIS2", true, iString(icube->Lines()));
 
  if (axes == 3){
    header += FitsKeyword("NAXIS3", true, iString(icube->Bands()));
  }

  header += FitsKeyword("BZERO", true,  base);

  header += FitsKeyword("BSCALE", true, scale);
  
  // Sky and All cases  
  if (ui.GetString("INFO") == "SKY" || ui.GetString("INFO") == "ALL") {  
    iString msg = "cube has not been skymapped";
    PvlGroup map;

    if (icube->HasGroup("mapping")) {
      map = icube->GetGroup("mapping");   
      msg = (string)map["targetname"];
    }
    // If we have sky we want it
    if (msg == "Sky") {
      double midRa = 0, midDec = 0;
  
      midRa = ((double)map["MaximumLongitude"] +
               (double)map["MinimumLongitude"])/2;
  
      midDec = ((double)map["MaximumLatitude"] +
                (double)map["MinimumLatitude"])/2;
  
      header += FitsKeyword("OBJCTRA", true, iString(midRa));
  
      // Specify the Declination
      header += FitsKeyword("OBJCTDEC", true, iString(midDec));
  
    }

    if (ui.GetString("INFO") == "ALL") {
      header += WritePvl("INSTRUME","Instrument","InstrumentId", icube, true);  
      header += WritePvl("OBSERVER","Instrument","SpacecraftName", icube, true);
      header += WritePvl("OBJECT  ","Instrument","TargetName", icube, true);
      // StartTime is sometimes middle of the exposure and somtimes beginning, 
      // so StopTime can't be calculated off of exposure reliably.
      header += WritePvl("DATE-OBS","Instrument","StartTime", icube, true);
      // Some cameras don't have StopTime
      if (icube->HasGroup("Instrument")) {
        PvlGroup inst = icube->GetGroup("Instrument");
        if (inst.HasKeyword("StopTime")) {
          header += WritePvl("TIME_END","Instrument","StopTime", icube, true);
        }
        if (inst.HasKeyword("ExposureDuration")) {
          header += WritePvl("EXPTIME","Instrument","ExposureDuration", icube, false);
        }
      }
    }  
    // If we were set on SKY and Sky doesn't exist
    else if (msg != "Sky") {  
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
  }
  
  // signal the end of the header
  header += FitsKeyword("END", false, "");

  // fill the rest of the fits header with space so to conform with the fits header
  // size of 2880 bytes
  for (int i = header.length() % 2880 ; i < 2880 ; i++) header += " ";

  // open the cube for writing
  string to = ui.GetFilename("TO","fits");
  ofstream fout;  
  fout.open (to.c_str (), ios::out|ios::binary);
  if (!fout.is_open ()) {
    string msg = "Cannot open fits output file";
    throw iException::Message(iException::Programmer,msg,_FILEINFO_);
  }
 
  fout.seekp(0);
  fout.write(header.c_str(),header.length());
  // write the raw cube data
  p.StartProcess (fout);

  // Finish off data area to a number n % 2880 == 0 is true
  // 2880 is the size of the data blocks
  int count = 2880 - (fout.tellp() % 2880);
  for (int i = 0; i < count; i++) {
    // Write nul characters as needed. ascii 0, hex 00...
    fout.write("\0", 1);  
  }
  fout.close();  
  p.EndProcess();
}

string FitsKeyword(string key, bool isValue, Isis::iString value, Isis::iString unit) {
  // pad the keyword with space
  for (int i = key.length() ; i < 8 ; i++) 
    key += " ";
  
  // add the value indicator (or lack thereof)
  if (isValue)
    key += "= ";
  else 
    key += "  ";
  
  // right-justify the value
  if (value.length() < 70){
    // pad the left part of the value with space
    for (int i =  value.length() ; i < 20 ; i++)
      key += " ";
    
    // add the actual value
    key += value;
    if (isValue) {
      key += " / ";
      if (unit != "") {
        key += "[" + unit + "]";
      }
    }

    // finish the line by padding the rest of the space
    for (int i =  key.length() ; i < 80 ; i++)
      key += " ";
  }
  
  // record the total length of the header
  headerBytes += key.length();
  return key;
}

string WritePvl(string fitsKey, string group, iString key, Cube *icube, bool isString) {
  if(icube->HasGroup(group)){
    PvlGroup theGroup = icube->GetGroup(group);   
    iString name = (string)theGroup[key];
    if (isString) { 
      name = "'" + name + "'";
    }
    iString unit = theGroup[key].Unit();
    return FitsKeyword(fitsKey, true, name, unit); 
  }
  return NULL;
}
