// $Id: mdis2isis.cpp,v 1.18 2009/12/29 21:45:54 slambright Exp $
#include "Isis.h"

#include <cfloat>
#include <cstdio>
#include <string>

#include "ProcessImportPds.h"
#include "ProcessByLine.h"

#include "UserInterface.h"
#include "Filename.h"
#include "iException.h"
#include "iTime.h"
#include "Buffer.h"
#include "TextFile.h"
#include "CSVReader.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlSequence.h"
#include "LineManager.h"
#include "OriginalLabel.h"
#include "SpecialPixel.h"
#include "tnt_array1d.h"
#include "tnt_array1d_utils.h"

using namespace std; 
using namespace Isis;

typedef TNT::Array1D<double> LutTable;

//  Establish MDIS DN maximums
static const double WACValidMaximum = 3600.0;
static const double NACValidMaximum = 3400.0;

LutTable lut;
Pvl TranslateMdisEdrLabels (Filename &labelFile, const std::string &target = "");
int CreateFilterSpecs(const std::string &instId, int filter_code, 
                      PvlGroup &bandbin, std::string &naifId); 
void UnlutData(Buffer &data);
LutTable LoadLut(Pvl &label, std::string &tableused, std::string &lutid); 
Cube *outCube = NULL;
double validMaxDn = WACValidMaximum;  //  Assumes the WAC


/** 
 * @brief Helper function to convert values to doubles
 * 
 * @param T Type of value to convert
 * @param value Value to convert
 * 
 * @return double Converted value
 */
template <typename T> double ToDouble(const T &value) {
    return (iString(value).Trim(" \r\t\n").ToDouble());
}

template <typename T> int ToInteger(const T &value) {
    return (iString(value).Trim(" \r\t\n").ToInteger());
}


void IsisMain ()
{
  ProcessImportPds p;
  Pvl pdsLabel;
  UserInterface &ui = Application::GetUserInterface();
  bool needsUnlut = false;

  // Get the input filename and make sure it is a MESSENGER/MDIS EDR
  Filename inFile = ui.GetFilename("FROM");
  iString id;
  bool projected;
  Pvl lab(inFile.Expanded());

  try {
    needsUnlut = (int) lab.FindKeyword("MESS:COMP12_8");
    // Check for NAC imager
    if ((int) lab.FindKeyword("MESS:IMAGER") == 1) validMaxDn = NACValidMaximum;
    id = (string) lab.FindKeyword ("MISSION_NAME");
    projected = lab.HasObject("IMAGE_MAP_PROJECTION");
  }
  catch (iException &e) {
    string msg = "Unable to read [MISSION] from input file [" +
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
  if (id != "MESSENGER") {
    string msg = "Input file [" + inFile.Expanded() + "] does not appear to be " +
                 "in MESSENGER EDR format. MISSION_NAME is [" + id + "]";
    throw iException::Message(iException::Io,msg, _FILEINFO_);
  }

  std::string target;
  if (ui.WasEntered("TARGET")) {
    target = ui.GetString("TARGET");
  }

  p.SetPdsFile (inFile.Expanded(), "", pdsLabel);
  Pvl outLabel = TranslateMdisEdrLabels (inFile, target);
  PvlKeyword sourceId("SourceProductId", '"'+inFile.Basename()+'"');

  if(ui.GetBoolean("UNLUT") == false || !needsUnlut) {
    // We're not going to unlut the data, so just set output cube
	//   and let ProcessImportPds do the writing for us.
    CubeAttributeOutput &outAtt = ui.GetOutputAttribute("TO");
    outCube = p.SetOutputCube(ui.GetFilename("TO"), outAtt);

    // Write the Instrument, BandBin, Archive, and Kernels groups to the output
    // cube label
    PvlGroup &group =  outLabel.FindGroup("Instrument", Pvl::Traverse);
	  group.AddKeyword(PvlKeyword("Unlutted", !needsUnlut));
    outCube->PutGroup (group);
    outCube->PutGroup (outLabel.FindGroup("BandBin", Pvl::Traverse));

    group = outLabel.FindGroup("Archive", Pvl::Traverse);
    group.AddKeyword(sourceId, Pvl::Replace);
    outCube->PutGroup (group);

    outCube->PutGroup (outLabel.FindGroup("Kernels", Pvl::Traverse));

    outCube = NULL;

    //  Set valid ranges
    p.SetNull(DBL_MIN, 0.0);
    p.SetHIS(validMaxDn, DBL_MAX);

    p.StartProcess ();
  }
  else {
	// Unlut is indicated, so we need to handle the conversion and the cube
  // writing.   Also will enforce DN limits.
  std::string lutfile, lutid;
  lut = LoadLut(lab, lutfile, lutid);

	outCube = new Cube();
	outCube->SetDimensions(p.Samples(), p.Lines(), p.Bands());
	outCube->Create(ui.GetFilename("TO"));

  PvlGroup &group =  outLabel.FindGroup("Instrument", Pvl::Traverse);
	group.AddKeyword(PvlKeyword("Unlutted", true));
  group.AddKeyword(PvlKeyword("LutInversionTable", lutfile));
	outCube->Label()->FindObject("IsisCube").AddGroup(group);

  group = outLabel.FindGroup("Archive", Pvl::Traverse);
  sourceId.AddValue('"'+lutid+'"');
  group.AddKeyword(sourceId);
	outCube->Label()->FindObject("IsisCube").AddGroup(group);

	outCube->Label()->FindObject("IsisCube").AddGroup(outLabel.FindGroup("BandBin", Pvl::Traverse));
	outCube->Label()->FindObject("IsisCube").AddGroup(outLabel.FindGroup("Kernels", Pvl::Traverse));

	p.StartProcess (UnlutData);

	OriginalLabel ol(Pvl(inFile.Expanded()));
	outCube->Write(ol);
	outCube->Close();
	delete outCube;
  }

  // All finished with the ImportPds object
  p.EndProcess ();
}

Pvl TranslateMdisEdrLabels (Filename &labelFile, const std::string &target) {
  //Create a PVL to store the translated labels
  Pvl outLabel;

  // Get the directory where the MRO HiRISE translation tables are.
  PvlGroup dataDir (Preference::Preferences().FindGroup("DataDirectory"));
  iString transDir = (string) dataDir["Messenger"] + "/translations/";

  // Get a filename for the MESSENGER EDR label
  Pvl labelPvl (labelFile.Expanded());

  // Translate the Instrument group
  Filename transFile (transDir + "mdisInstrument.trn");
  PvlTranslationManager instrumentXlater (labelPvl, transFile.Expanded());
  instrumentXlater.Auto (outLabel);

  // Translate the BandBin group
  transFile  = transDir + "mdisBandBin.trn";
  PvlTranslationManager bandBinXlater (labelPvl, transFile.Expanded());
  bandBinXlater.Auto (outLabel);

  // Translate the Archive group
  transFile  = transDir + "mdisArchive.trn";
  PvlTranslationManager archiveXlater (labelPvl, transFile.Expanded());  
  archiveXlater.Auto (outLabel);

  // Create the Kernel Group
  PvlGroup kerns("Kernels");
  PvlGroup &bandbin(outLabel.FindGroup("BandBin", Pvl::Traverse));
  PvlGroup &instGrp(outLabel.FindGroup("Instrument", Pvl::Traverse));
  std::string instId = instGrp["InstrumentId"];
  std::string naifCode;

  // Establish Filter specific keywords
  CreateFilterSpecs(instId, (int) instGrp["FilterWheelPosition"], bandbin, 
                    naifCode); 
  kerns += PvlKeyword("NaifIkCode", naifCode);
  outLabel.AddGroup(kerns);

//  If the user specifed the target explicitly or it doesn't exist, create
//  something so the camera will always work
  if (instGrp.FindKeyword("TargetName").IsNull() || (!target.empty())) {
    if (!target.empty()) {
	  instGrp["TargetName"] = iString(target);
	}
    else {
	  instGrp["TargetName"] = iString("Sky");
	}
  }

//  Compute the gimble pivot angle and write to the label
  double pivotCounter = (double) instGrp["PivotPosition"];
  double pivotAngle   = pivotCounter / ((double) (2 << 15)) * 180.0;
  instGrp += PvlKeyword("PivotAngle", pivotAngle, "Degrees");

  return outLabel;
}

/** 
 * @brief Determine filter code from filter wheel position code 
 *  
 * This routine will determine the true filter wheel from the 
 * MESS:FW_POS in and image EDR keyword.  This routine will 
 * open the file 
 * $Messenger/calibration/mdisCalibration????.trn" 
 *  
 * @param filter_code The value of the MESS:FW_POS keyword in 
 *                    the EDR label.
 * @return int Valid filter number between 1 and 12, otherwise 
 *             returns 0. 
 */
int CreateFilterSpecs(const std::string &instId, int filter_code, 
                      PvlGroup  &bandbin, string &naifCode) {

    //  WAC Filter table
  struct { int filter;
           const char *code;
           const char *name; 
           const char *center; 
           const char *width; 
         } WACfilters[] = {
               {  1, "A",   "700 BW 5",  "698.8",   "5.3" },
               {  2, "B", "700 BW 600",  "700.0", "600.0" },
               {  3, "C",  "480 BW 10",  "479.9",  "10.1" },
               {  4, "D",   "560 BW 5",  "558.9",   "5.8" },
               {  5, "E",   "630 BW 5",  "628.8",   "5.5" },
               {  6, "F",  "430 BW 40",  "433.2",  "18.1" },
               {  7, "G",   "750 BW 5",  "748.7",   "5.1" },
               {  8, "H",   "950 BW 7",  "947.0",   "6.2" },
               {  9, "I", "1000 BW 15",  "996.2",  "14.3" },
               { 10, "J",   "900 BW 5",  "898.8",   "5.1" },
               { 11, "K", "1020 BW 40", "1012.6",  "33.3" },
               { 12, "L",   "830 BW 5",  "828.4",   "5.2" }
         };
    

 
  naifCode = "NULL";
  int filter(0);
  string name, center, width;

  if (instId == "MDIS-NAC") {
    naifCode =  "-236820";
    filter = 2;
    name = "748 BP 53";
    center = "747.7";
    width = "52.6";
  }
  else if (instId == "MDIS-WAC") {
   //  Set up WAC calibration file
    Filename calibFile("$messenger/calibration/mdisCalibration????.trn");
    calibFile.HighestVersion();
    Pvl config(calibFile.Expanded());

    PvlGroup &confgrp = config.FindGroup("FilterWheel"); 
    int tolerance = confgrp["EncoderTolerance"];

    naifCode =  "-236800";
    for(int filterTry = 1; filterTry <= 12 ; filterTry++) {
      int idealPosition = confgrp[iString("EncoderPosition") + iString(filterTry)];
      if ((filter_code <= (idealPosition + tolerance)) &&
          (filter_code >= (idealPosition - tolerance))) {
          int fno = filterTry - 1;
          filter = WACfilters[fno].filter;
          name   = WACfilters[fno].name;
          center = WACfilters[fno].center;
          width  = WACfilters[fno].width;
          break;
      }
    }
  }
  else {
    //  Not the expected instrument 
   string msg = "Unknown InstrumentId [" + instId + "], image does not " +
                "appear to be from the MESSENGER/MDIS Camera"; 
   throw iException::Message(iException::Io,msg, _FILEINFO_); 
  }

  if (!name.empty()) {
    bandbin.AddKeyword(PvlKeyword("Number", filter), PvlContainer::Replace);  
    bandbin.AddKeyword(PvlKeyword("Name", name),PvlContainer::Replace);  
    bandbin.AddKeyword(PvlKeyword("Center", center,"NM"),PvlContainer::Replace);  
    bandbin.AddKeyword(PvlKeyword("Width", width, "NM"),PvlContainer::Replace);
  }
  else {
    //  If we reach here, we cannot validate the number - set it to unknown
    bandbin.AddKeyword(PvlKeyword("Number", "Unknown"), PvlContainer::Replace);
  }

  return (0);
}

void UnlutData(Buffer &data) {
  LineManager out(*outCube);
  out.SetLine(data.Line(), data.Band());

  for(int i = 0; i < data.size(); i++) {
	int dnvalue = (int)(data[i] + 0.5);
	if(dnvalue < 0 || dnvalue > 255) {
	  throw iException::Message(iException::User, 
								"In the input file a value of [" + 
								iString(data[i]) + 
								"] was found. Unlutted images should only contain values 0 to 255.", 
								_FILEINFO_);
	}

	out[i] = lut[dnvalue];
  }

  outCube->Write(out);
}

LutTable LoadLut(Pvl &label, std::string &tableused, std::string &froot) {
  int tableToUse = label.FindKeyword("MESS:COMP_ALG");

  Filename tableFile("$messenger/calibration/LUT_INVERT/MDISLUTINV_?.TAB");
  tableFile.HighestVersion();
  tableused = tableFile.OriginalPath() + "/" + tableFile.Name();
  froot = tableFile.Basename();

  CSVReader csv(tableFile.Expanded());

  int nRows = csv.rows();
  if (nRows != 256) {
    std::ostringstream mess;
    mess << "MDIS LUT Inversion table, " << tableFile.Expanded() 
         << ", should contain 256 rows but has " << nRows;
    throw iException::Message(iException::User, mess.str(), _FILEINFO_);
  }

  int nCols = csv.columns();
  if (nCols != 9) {
    std::ostringstream mess;
    mess << "MDIS LUT Inversion table, " << tableFile.Expanded() 
         << ", should contain 9 columns but has " << nCols;
    throw iException::Message(iException::User, mess.str(), _FILEINFO_);
  }

  LutTable mlut(nRows, Null); // 8 bit => 12 bit, 2^8 = 256 conversion values

  // reset the lookup values to a known bad value
  int tableRow = tableToUse + 1;
  for(int i = 0; i < nRows; i++) {
    CSVReader::CSVAxis row = csv.getRow(i);
    int dn8 = ToInteger(row[0]);

    // Gut check!
    if ((dn8 < 0) || (dn8 >= nRows)) {
      std::ostringstream mess;
      mess << "Index (" << dn8 << ") at line " << i+1 
           << " is invalid inMDIS LUT Inversion table "
           << tableFile.Expanded() << " - valid range is 0 <= index < 256!"; 
      throw iException::Message(iException::User, mess.str(), _FILEINFO_); 
    }

    double dn16 = ToDouble(row[tableRow]);
    if (dn16 > validMaxDn) dn16 = His;
    mlut[dn8] = dn16;
  }

  //  Ensure the 0th pixel is NULL
  mlut[0] = Null;

  // cout << "Lut Table " << mlut << endl;
  return (mlut);
}

