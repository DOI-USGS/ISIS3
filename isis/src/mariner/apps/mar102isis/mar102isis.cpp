#include "Isis.h"

#include "Cube.h"
#include "Filename.h"
#include "iString.h"
#include "iTime.h"
#include "PixelType.h"
#include "ProcessImport.h"
#include "ProcessImportPds.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlFormat.h"
#include "PvlFormatPds.h"

#include <sstream>
#include <string>

using namespace std;
using namespace Isis;

void UpdateLabels (Cube *cube, const string &labels);
void TranslateIsis2Labels (Filename &labelFile, Cube *oCube);
string EbcdicToAscii (unsigned char *header);
string DaysToDate (int days);

void IsisMain () {
  UserInterface &ui = Application::GetUserInterface();

  // Determine whether input is a raw Mariner 10 image or an Isis 2 cube
  bool isRaw = false;
  Filename inputFile = ui.GetFilename("FROM");
  Pvl label (inputFile.Expanded());

  // If the PVL created from the input labels is empty, then input is raw
  if (label.Groups() + label.Objects() + label.Keywords() == 0) {
    isRaw = true;
  }

  // Import for RAW files
  if (isRaw) {
    ProcessImport p;
    
    // All mariner images from both cameras share this size
    p.SetDimensions(832,700,1);
    p.SetFileHeaderBytes(968);
    p.SaveFileHeader();
    p.SetPixelType(UnsignedByte);
    p.SetByteOrder(Lsb);
    p.SetDataSuffixBytes(136);

    p.SetInputFile (ui.GetFilename("FROM"));
    Cube *oCube = p.SetOutputCube("TO");
  
    p.StartProcess ();
    unsigned char *header = (unsigned char *) p.FileHeader();
    string labels = EbcdicToAscii(header);
    UpdateLabels(oCube, labels);
    p.EndProcess ();
  }
  // Import for Isis 2 cubes
  else {
    ProcessImportPds p;
  
    // All mariner images from both cameras share this size
    p.SetDimensions(832,700,1);
    p.SetPixelType(UnsignedByte);
    p.SetByteOrder(Lsb);
    p.SetDataSuffixBytes(136);
  
    p.SetPdsFile (inputFile.Expanded(), "", label);
    Cube *oCube = p.SetOutputCube("TO");
  
    TranslateIsis2Labels(inputFile, oCube);
    p.StartProcess ();
    p.EndProcess ();
  }
}

// Converts labels into standard pvl format and adds necessary 
// information not included in original labels
void UpdateLabels (Cube *cube, const string &labels) {
  // First, we parse out as much valid information as possible from the 
  // original labels
  string key;
  int keyPosition;
  int consumeChars;

  // Image number
  key = "FDS=";
  keyPosition = labels.find(key);
  consumeChars = labels.find("IM-1") - keyPosition - key.length();
  iString fds (labels.substr(keyPosition + key.length(), consumeChars));
  fds.Trim(" ");

  // Year the image was taken
  key = "YR=";
  keyPosition = labels.find(key);
  consumeChars = labels.find("DAY") - keyPosition - key.length();
  iString yr (labels.substr(keyPosition + key.length(), consumeChars));
  yr.Trim(" ");

  // Day the image was taken
  key = "DAY=";
  keyPosition = labels.find(key);
  consumeChars = labels.find("GMT") - keyPosition - key.length();
  iString day (labels.substr(keyPosition + key.length(), consumeChars));
  day.Trim(" ");

  // Greenwich Mean Time
  key = "GMT=";
  keyPosition = labels.find(key);
  consumeChars = labels.find("CCAMERA") - keyPosition - key.length();
  iString gmt (labels.substr(keyPosition + key.length(), consumeChars));
  gmt.Trim(" ");

  // Which of the two cameras took the image
  key = "CCAMERA=";
  keyPosition = labels.find(key);
  consumeChars = labels.find("FILTER") - keyPosition - key.length();
  iString ccamera (labels.substr(keyPosition + key.length(), consumeChars));
  ccamera.Trim(" ");

  // Filter number
  key = "FILTER=";
  keyPosition = labels.find(key);
  consumeChars = labels.find("(") - keyPosition - key.length();
  iString filterNum (labels.substr(keyPosition + key.length(), consumeChars));
  filterNum.Trim(" ");

  // Filter name
  consumeChars = labels.find(")") - keyPosition - key.length() - consumeChars;
  iString filterName (labels.substr(labels.find("(") + 1, consumeChars));
  filterName.Trim(" ");

  // Center wavelength
  int fnum = filterNum.ToInteger();
  double filterCenter = 0.;
  switch (fnum) {
    case 0:
      filterCenter = .575;
      break;
    case 2:
      filterCenter = .475;
      break;
    case 3:
      filterCenter = .360;
      break;
    case 4:
      filterCenter = .511;
      break;
    case 5:
      filterCenter = .487;
      break;
    case 6:
      filterCenter = .355;
      break;
    default:
      break;
  }
  
  // Exposure duration
  key = "EXPOSURE=";
  keyPosition = labels.find(key);
  consumeChars = labels.find("MSEC") - keyPosition - key.length();
  iString exposure (labels.substr(keyPosition + key.length(), consumeChars));
  exposure.Trim(" ");

  // Create the instrument group
  PvlGroup inst("Instrument");
  inst += PvlKeyword("SpacecraftName","Mariner_10");
  inst += PvlKeyword("InstrumentId","M10_VIDICON_" + ccamera);

  // Get the date
  int days = day.ToInteger();
  iString date = DaysToDate(days);

  // Get the time
  iString time = gmt;
  time = time.Replace("/",":");

  // Construct the Start Time in yyyy-mm-ddThh:mm:ss format
  string fullTime = date + "T" + time + ".000";
  iTime startTime(fullTime);

  // Create the archive group
  PvlGroup archive ("Archive");

  int year = yr.ToInteger();
  year += 1900;
  string fullGMT = iString(year) + ":" + day + ":" + time;
  archive += PvlKeyword ("GMT", fullGMT);
  archive += PvlKeyword ("ImageNumber", fds);

  // Create the band bin group
  PvlGroup bandBin("BandBin");
  iString filter = filterName;
  filter = filter.Replace(")","");
  bandBin += PvlKeyword("FilterName", filter);
  iString number = filterNum;
  bandBin += PvlKeyword("FilterNumber", number);
  bandBin += PvlKeyword("OriginalBand", "1");
  iString center = filterCenter;
  bandBin += PvlKeyword("Center", center);
  bandBin.FindKeyword("Center").SetUnits("micrometers");

  // Dates taken from ASU Mariner website - http://ser.ses.asu.edu/M10/TXT/encounters.html and
  // from Nasa website - http://www.jpl.nasa.gov/missions/missiondetails.cfm?mission_Mariner10
  // under fast facts. Mariner encountered the Moon, Venus, and Mercury three times.
  // Date used for all is two days before date of first encounter on website. Information
  // is important for nominal reseaus used and for Keyword encounter  
  string target = "";
  if (startTime < iTime("1974-2-3T12:00:00")) { 
    target = "$mariner10/reseaus/mar10MoonNominal.pvl";
    inst += PvlKeyword("TargetName", "Moon");
    archive += PvlKeyword ("Encounter", "Moon");
  } 
  //Disagreement on the below date between ASU website and NASA website, used earlier of the two - ASU
  else if (startTime < iTime("1974-3-22T12:00:00")) { 
    target = "$mariner10/reseaus/mar10VenusNominal.pvl";
    inst += PvlKeyword("TargetName", "Venus");
    archive += PvlKeyword ("Encounter", "Venus");
  }
  else if (startTime < iTime("1974-9-19T12:00:00")) {
    target = "$mariner10/reseaus/mar10Merc1Nominal.pvl";
    inst += PvlKeyword("TargetName", "Mercury");
    archive += PvlKeyword ("Encounter", "Mercury_1");
  }
  // No specific date on ASU website, used NASA date
  else if (startTime < iTime("1975-3-14T12:00:00")) {
    target = "$mariner10/reseaus/mar10Merc2Nominal.pvl";
    inst += PvlKeyword("TargetName", "Mercury");
    archive += PvlKeyword ("Encounter", "Mercury_2");
  }
  else {
    target = "$mariner10/reseaus/mar10Merc3Nominal.pvl";
    inst += PvlKeyword("TargetName", "Mercury");
    archive += PvlKeyword ("Encounter", "Mercury_3");
  }
  
  // Place start time and exposure duration in intrument group
  inst += PvlKeyword("StartTime", fullTime);
  inst += PvlKeyword("ExposureDuration", exposure, "milliseconds");

  // Open nominal positions pvl named by string encounter
  Pvl nomRx (target);

  // Allocate all keywords within reseaus groups well as the group its self
  PvlGroup rx("Reseaus");
  PvlKeyword line("Line");
  PvlKeyword sample("Sample");
  PvlKeyword type("Type");
  PvlKeyword valid("Valid");
  PvlKeyword templ("Template");
  PvlKeyword status("Status");  

  // All cubes will stay this way until findrx is run on them
  status = "Nominal";

  // Kernels group
  PvlGroup kernels("Kernels");
  PvlKeyword naif("NaifFrameCode");
  
  // Camera dependent information
  string camera = "";
  if (iString("M10_VIDICON_A") == inst["InstrumentId"][0]) {
    templ = "$mariner10/reseaus/mar10a.template.cub";
    naif += "-76110"; 
    camera = "M10_VIDICON_A_RESEAUS";
  } 
  else {
    templ = "$mariner10/reseaus/mar10b.template.cub";
    naif += "-76120";
    camera = "M10_VIDICON_B_RESEAUS";
  }

  // Add naif frame code
  kernels += naif;
  
  // Find the correct PvlKeyword corresponding to the camera for nominal positions
  PvlKeyword resnom = nomRx[camera];
  
  // This loop goes through the PvlKeyword resnom which contains data 
  // in the format: line, sample, type, on each line. There are 111 reseaus for 
  // both cameras. To put data away correctly, it must go through a total 333 items, 
  // all in one PvlKeyword.  
  int i = 0;
  while (i < 333) {
    line += resnom[i];
    i++;
    sample += resnom[i];
    i++;
    type += resnom[i];
    i++;
    valid += 0;
  }

  // Add all the PvlKeywords to the PvlGroup Reseaus
  rx += line;
  rx += sample;
  rx += type;
  rx += valid;
  rx += templ;
  rx += status;

  // Get the labels and add the updated labels to them
  Pvl *cubeLabels = cube->Label();
  cubeLabels->FindObject("IsisCube").AddGroup(inst);
  cubeLabels->FindObject("IsisCube").AddGroup(archive);
  cubeLabels->FindObject("IsisCube").AddGroup(bandBin);
  cubeLabels->FindObject("IsisCube").AddGroup(kernels);
  cubeLabels->FindObject("IsisCube").AddGroup(rx);

  PvlObject original ("OriginalLabel");
  original += PvlKeyword ("Label", labels);
  cubeLabels->AddObject(original);
}

// Translate Isis 2 labels into Isis 3 labels.
void TranslateIsis2Labels (Filename &labelFile, Cube *oCube) {
  // Get the directory where the Mariner translation tables are.
  PvlGroup &dataDir = Preference::Preferences().FindGroup("DataDirectory");

  // Transfer the instrument group to the output cube
  iString transDir = (string) dataDir["Mariner10"] + "/translations/";
  Pvl inputLabel (labelFile.Expanded());
  Filename transFile;

  transFile = transDir + "mariner10isis2.trn";

  // Get the translation manager ready
  PvlTranslationManager translation (inputLabel, transFile.Expanded());
  Pvl *outputLabel = oCube->Label();
  translation.Auto(*(outputLabel));

  //Instrument group
  PvlGroup &inst = outputLabel->FindGroup("Instrument", Pvl::Traverse);

  PvlKeyword &instrumentId = inst.FindKeyword("InstrumentId");
  instrumentId.SetValue("M10_VIDICON_" + instrumentId[0]);

  PvlKeyword &targetName = inst.FindKeyword("TargetName");
  iString targetTail (targetName[0].substr(1));
  targetTail = targetTail.DownCase();
  targetName.SetValue( targetName[0].at(0) + targetTail );

  PvlKeyword &startTime = inst.FindKeyword("StartTime");
  startTime.SetValue( startTime[0].substr(0, startTime[0].size()-1));

  PvlGroup &archive = outputLabel->FindGroup("Archive", Pvl::Traverse);
  PvlKeyword &imgNo = archive.FindKeyword("ImageNumber");
  iString ino = imgNo[0];
  ino.Trim(" ");
  imgNo.SetValue(ino);

  iTime time (startTime[0]);
  if (time < iTime("1974-2-3T12:00:00")) { 
    archive += PvlKeyword ("Encounter", "Moon");
  } 
  else if (time < iTime("1974-3-22T12:00:00")) { 
    archive += PvlKeyword ("Encounter", "Venus");
  }
  else if (time < iTime("1974-9-19T12:00:00")) {
    archive += PvlKeyword ("Encounter", "Mercury_1");
  }
  else if (time < iTime("1975-3-14T12:00:00")) {
    archive += PvlKeyword ("Encounter", "Mercury_2");
  }
  else {
    archive += PvlKeyword ("Encounter", "Mercury_3");
  }

  inst.FindKeyword("ExposureDuration").SetUnits("milliseconds");

  PvlGroup &bBin = outputLabel->FindGroup ("BandBin", Pvl::Traverse);
  std::string filter = inputLabel.FindObject("QUBE")["FILTER_NAME"];
  if (filter != "F") {
    //Band Bin group
    bBin.FindKeyword("Center").SetUnits("micrometers");
  }

  // Kernels group
  PvlGroup &kernels = outputLabel->FindGroup ("Kernels", Pvl::Traverse);
  PvlGroup &reseaus = outputLabel->FindGroup ("Reseaus", Pvl::Traverse);
  PvlKeyword &templ = reseaus.FindKeyword("Template");
  PvlKeyword &valid = reseaus.FindKeyword("Valid");

  for (int i = 0; i < valid.Size(); i++) {
    valid[i] = valid[i].substr(0, 1);
  }

  // Camera dependent information
  string camera = "";
  if (iString("M10_VIDICON_A") == inst["InstrumentId"][0]) {
    templ = "$mariner10/reseaus/mar10a.template.cub";
    kernels.FindKeyword("NaifFrameCode").SetValue("-76110");
    camera = "M10_VIDICON_A_RESEAUS";
  } 
  else {
    templ = "$mariner10/reseaus/mar10b.template.cub";
    kernels.FindKeyword("NaifFrameCode").SetValue("-76120");
    camera = "M10_VIDICON_B_RESEAUS";
  }
}

// FYI, mariner10 original labels are stored in ebcdic, a competitor with ascii,
// a conversion table is necessary then to get the characters over to ascii. For
// more info: http://en.wikipedia.org/wiki/Extended_Binary_Coded_Decimal_Interchange_Code
//! Converts ebsidic Mariner10 labels to ascii
string EbcdicToAscii (unsigned char *header) {
  // Table to convert ebcdic to ascii
  unsigned char xlate[] = {
    0x00,0x01,0x02,0x03,0x9C,0x09,0x86,0x7F,0x97,0x8D,0x8E,0x0B,0x0C,0x0D,0x0E,0x0F,
    0x10,0x11,0x12,0x13,0x9D,0x85,0x08,0x87,0x18,0x19,0x92,0x8F,0x1C,0x1D,0x1E,0x1F,
    0x80,0x81,0x82,0x83,0x84,0x0A,0x17,0x1B,0x88,0x89,0x8A,0x8B,0x8C,0x05,0x06,0x07,
    0x90,0x91,0x16,0x93,0x94,0x95,0x96,0x04,0x98,0x99,0x9A,0x9B,0x14,0x15,0x9E,0x1A,
    0x20,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xD5,0x2E,0x3C,0x28,0x2B,0x7C,
    0x26,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0x21,0x24,0x2A,0x29,0x3B,0x5E,
    0x2D,0x2F,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xE5,0x2C,0x25,0x5F,0x3E,0x3F,
    0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,0xC2,0x60,0x3A,0x23,0x40,0x27,0x3D,0x22,
    0xC3,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,
    0xCA,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,
    0xD1,0x7E,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0xD2,0xD3,0xD4,0x5B,0xD6,0xD7,
    0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,0xE0,0xE1,0xE2,0xE3,0xE4,0x5D,0xE6,0xE7,
    0x7B,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,
    0x7D,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,
    0x5C,0x9F,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
  };

  // Mariner 10 has 360 bytes of header information
  for (int i=0; i<360; i++) {
    header[i] = xlate[header[i]];
  }

  // Put in a end of string mark and return
  header[215] = 0;
  printf("Header: %s\n", header);
  return string((const char *)header);
}

// Mariner 10 labels provide the number of days since the beginning of the year
// 1974 in the GMT keyword, but not always a start time.  In order to derive an
// estimated start time, with an actual date attached, a conversion must be
// performed.
string DaysToDate (int days) {
  int currentMonth = 12;
  int currentDay = 31;
  int currentYear = 1973;
  while (days > 0) {
    // The Mariner 10 mission took place in the years 1973 through 1975, 
    // none of which were Leap Years, thus February always had 28 days
    if (currentDay == 28 && currentMonth == 2) {
      currentMonth = 3;
      currentDay = 1;
    }
    else if (currentDay == 30 &&
             (currentMonth == 4 || currentMonth == 6 ||
              currentMonth == 9 || currentMonth == 11)) {
      currentMonth++;
      currentDay = 1;
    }
    else if (currentDay == 31 && currentMonth == 12) {
      currentMonth = 1;
      currentDay = 1; 
      currentYear++;
    }
    else if (currentDay == 31) {
      currentMonth++;
      currentDay = 1;
    }
    else {
      currentDay++;
    }
    days--;
  }
  iString year = currentYear;
  iString month = (currentMonth < 10) ? "0" + iString(currentMonth) : iString(currentMonth);
  iString day = (currentDay < 10) ? "0" + iString(currentDay) : iString(currentDay);
  return year + "-" + month + "-" + day;
}
