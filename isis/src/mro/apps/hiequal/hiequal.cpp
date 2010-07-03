#include "Isis.h"
#include "ProcessByLine.h"
#include "FileList.h"
#include "iString.h"
#include "Statistics.h"
#include "OverlapNormalization.h"
#include "CubeAttribute.h"

using namespace Isis;

void GatherStatistics(Buffer &in);
void Apply(Buffer &in, Buffer &out);
void ErrorCheck(FileList &imageList, FileList &outList);

OverlapNormalization *g_oNorm;
Statistics g_s, g_sl, g_sr;
std::vector<Statistics> g_allStats, g_leftStats, g_rightStats;
int g_imageNum;

void IsisMain() {

  // Get the list of cubes to process
  FileList imageList;
  UserInterface &ui = Application::GetUserInterface();
  imageList.Read(ui.GetFilename("FROMLIST"));

  // Read to list if one was entered
  FileList outList;
  if(ui.WasEntered("TOLIST")) {
    outList.Read(ui.GetFilename("TOLIST"));
  }

  // Check for user input errors and return the file list sorted by CCD numbers
  ErrorCheck(imageList, outList);

  // Adds statistics for whole and side regions of every cube
  for(int img = 0; img < (int)imageList.size(); img++) {
    g_s.Reset();
    g_sl.Reset();
    g_sr.Reset();

    iString maxCube((int)imageList.size());
    iString curCube(img + 1);
    ProcessByLine p;
    p.Progress()->SetText("Gathering Statistics for Cube " +
                          curCube + " of " + maxCube);
    CubeAttributeInput att;
    const std::string inp = imageList[img];
    p.SetInputCube(inp, att);
    p.StartProcess(GatherStatistics);
    p.EndProcess();

    g_allStats.push_back(g_s);
    g_leftStats.push_back(g_sl);
    g_rightStats.push_back(g_sr);
  }

  // Initialize the object that will calculate the gains and offsets
  g_oNorm = new OverlapNormalization(g_allStats);

  // Add the known overlaps between two cubes, and apply a weight to each
  // overlap equal the number of pixels in the overlapping area
  for(int i = 0; i < (int)imageList.size() - 1; i++) {
    int j = i + 1;
    g_oNorm->AddOverlap(g_rightStats[i], i, g_leftStats[j], j,
                        g_rightStats[i].ValidPixels());
  }

  // Read in and then set the holdlist
  FileList holdList;
  holdList.Read(ui.GetFilename("HOLD"));

  for(unsigned i = 0; i < holdList.size(); i++) {
    int index = -1;
    for(unsigned j = 0; j < imageList.size(); j++) {
      std::string curName = imageList.at(j);
      if(curName.compare(holdList[i]) == 0) {
        index = j;
        g_oNorm->AddHold(index);
      }
    }
  }

  // Attempt to solve the least squares equation
  g_oNorm->Solve(OverlapNormalization::Both);

  // Apply correction to the cubes if desired
  bool applyopt = ui.GetBoolean("APPLY");
  if(applyopt) {
    // Loop through correcting the gains and offsets by line for every cube
    for(int img = 0; img < (int)imageList.size(); img++) {
      g_imageNum = img;
      ProcessByLine p;
      iString max_cube((int)imageList.size());
      iString cur_cube(img + 1);
      p.Progress()->SetText("Equalizing Cube " + cur_cube + " of " + max_cube);
      CubeAttributeInput att;
      const std::string inp = imageList[img];
      Cube *icube = p.SetInputCube(inp, att);
      Filename file = imageList[img];

      // Establish the output file depending upon whether or not a to list
      // was entered
      std::string out;
      if(ui.WasEntered("TOLIST")) {
        out = outList[img];
      }
      else {
        Filename file = imageList[img];
        out = file.Path() + "/" + file.Basename() + ".equ." + file.Extension();
      }

      CubeAttributeOutput outAtt;
      p.SetOutputCube(out, outAtt, icube->Samples(), icube->Lines(), icube->Bands());
      p.StartProcess(Apply);
      p.EndProcess();
    }
  }

  // Setup the output text file if the user requested one
  if(ui.WasEntered("OUTSTATS")) {
    std::string out = Filename(ui.GetFilename("OUTSTATS")).Expanded();
    std::ofstream os;
    os.open(out.c_str(), std::ios::app);

    // Get statistics for each cube with PVL
    Pvl p;
    PvlObject equ("EqualizationInformation");
    for(int img = 0; img < (int)imageList.size(); img++) {
      std::string cur = imageList[img];
      PvlGroup a("Adjustment");
      a += PvlKeyword("FileName", cur);
      a += PvlKeyword("Average", g_oNorm->Average(img));
      a += PvlKeyword("Base", g_oNorm->Offset(img));
      a += PvlKeyword("Multiplier", g_oNorm->Gain(img));
      equ.AddGroup(a);
    }
    p.AddObject(equ);

    os << p << std::endl;
  }

  PvlGroup results("Results");
  for(int img = 0; img < (int)imageList.size(); img++) {
    results += PvlKeyword("FileName", imageList[img]);
    results += PvlKeyword("Average", g_oNorm->Average(img));
    results += PvlKeyword("Base", g_oNorm->Offset(img));
    results += PvlKeyword("Multiplier", g_oNorm->Gain(img));
  }
  Application::Log(results);

  // Clean-up for batch list runs
  delete g_oNorm;
  g_oNorm = NULL;

  g_allStats.clear();
  g_leftStats.clear();
  g_rightStats.clear();
}

// Gather statistics for the cube
void GatherStatistics(Buffer &in) {
  // Number of samples per line that intersect with the next and the
  // previous images
  unsigned int intersect;

  // Check if samples equal 682 or 683
  if(in.size() == 682 || in.size() == 683) {
    intersect = 18;
  }
  // If not the above case, then we perform an algorithm to account for binning
  else {
    // Number of intersecting samples is directly related to total
    // number of samples in the line, with 2048 being the maximum possible
    unsigned int div = 2048 / in.size();
    intersect = 48 / div;
  }

  g_s.AddData(&in[0], in.size());
  g_sl.AddData(&in[0], intersect);
  g_sr.AddData(&in[in.size()-intersect], intersect);
}

// Apply equalization to the cube
void Apply(Buffer &in, Buffer &out) {
  for(int i = 0; i < in.size(); i++) {
    out[i] = g_oNorm->Evaluate(in[i], g_imageNum);
  }
}

// Checks for user input errors where present and attempts to correct them
// where possible
void ErrorCheck(FileList &imageList, FileList &outList) {
  UserInterface &ui = Application::GetUserInterface();

  // Ensures number of images is within bound
  if(imageList.size() < 2) {
    std::string msg = "The input file [" + ui.GetFilename("FROMLIST") +
                      "] must contain at least 2 file names";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }
  if(imageList.size() > 10) {
    std::string msg = "The input file [" + ui.GetFilename("FROMLIST") +
                      "] cannot contain more than 10 file names";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  // Make sure the user enters a "OUTSTATS" file if the apply option is not selected
  if(!ui.GetBoolean("APPLY")) {
    if(!ui.WasEntered("OUTSTATS")) {
      std::string msg = "If the apply option is not selected, you must enter a";
      msg += " OUTSTATS file";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  // Reference for converting a CPMM number to a CCD number
  const int cpmm2ccd[] = {0, 1, 2, 3, 12, 4, 10, 11, 5, 13, 6, 7, 8, 9};

  // Place ccd in vector, try-catch opening cubes
  std::vector<int> ccds;
  for(int i = 0; i < (int)imageList.size(); i++) {
    try {
      Cube cube1;
      cube1.Open(imageList[i]);
      PvlGroup &from1Instrument = cube1.GetGroup("INSTRUMENT");
      int cpmmNumber = from1Instrument["CpmmNumber"];
      ccds.push_back(cpmm2ccd[cpmmNumber]);
    }
    // If any part of the above didn't work, we can safely assume the current
    // file is not a valid HiRise image
    catch(...) {
      std::string msg = "The [" + imageList[i] +
                        "] file is not a valid HiRise image";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  // Error check TO LIST if was entered
  if(ui.WasEntered("TOLIST")) {

    // Make sure each file in the tolist matches a file in the fromlist
    if(outList.size() != imageList.size()) {
      std::string msg = "Each input file in the FROM LIST must have a ";
      msg += "corresponding output file in the TO LIST.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // Make sure that all output files do not have the same names as their
    // corresponding input files
    for(unsigned i = 0; i < outList.size(); i++) {
      if(outList[i].compare(imageList[i]) == 0) {
        std::string msg = "The TO LIST file [" + outList[i] +
                          "] has the same name as its corresponding FROM LIST file.";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
  }

  // Insertion sorts a list of filenames by their CCD numbers
  for(int i = 1; i < (int)imageList.size(); i++) {
    int ccd1 = ccds[i];
    std::string temp = imageList[i];

    int j = i - 1;
    int ccd2 = ccds[j];

    while(j >= 0 && ccd2 > ccd1) {
      imageList[j+1] = imageList[j];
      ccds[j+1] = ccds[j];
      if(ui.WasEntered("TOLIST")) outList[j+1] = outList[j];

      j--;
      if(j >= 0) {
        ccd2 = ccds[j];
      }
    }
    imageList[j+1] = temp;
    ccds[j+1] = ccd1;
    if(ui.WasEntered("TOLIST")) outList[j+1] = outList[i];
  }

  // Ensures BG and IR only have two files
  if(ccds[0] == 10 || ccds[0] == 11) {
    if(imageList.size() != 2) {
      std::string msg = "A list of IR images must have exactly two ";
      msg += "file names";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }
  if(ccds[0] == 12 || ccds[0] == 13) {
    if(imageList.size() != 2) {
      std::string msg = "A list of BG images must have exactly two ";
      msg += "file names";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  // Error checking to ensure CCDID types match and the filenames follow a
  // strict sequence
  for(int i = 0; i < (int)imageList.size() - 1; i++) {
    int ccd1 = ccds[i];

    // CCDID type
    int id1;
    // RED
    if(ccd1 >= 0 && ccd1 <= 9) id1 = 0;
    // IR
    else if(ccd1 == 10 || ccd1 == 11) id1 = 1;
    // BG
    else id1 = 2;

    int ccd2 = ccds[i+1];

    int id2;
    if(ccd2 >= 0 && ccd2 <= 9) id2 = 0;
    else if(ccd2 == 10 || ccd2 == 11) id2 = 1;
    else id2 = 2;

    // CCDID types don't match
    if(id1 != id2) {
      std::string msg = "The list of input images must be all RED, all IR, or ";
      msg += "all BG";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // Doesn't follow strict sequence for RED CCDID
    if(ccd2 != ccd1 + 1) {
      std::string msg = "The list of input images do not numerically follow ";
      msg += "one another";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  // Make sure each file in the holdlist matches a file in the fromlist
  FileList holdList;
  holdList.Read(ui.GetFilename("HOLD"));
  if(holdList.size() > imageList.size()) {
    std::string msg = "The list of identifiers to be held must be less than or ";
    msg += "equal to the total number of identitifers.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  for(int i = 0; i < (int)holdList.size(); i++) {
    bool matched = false;
    for(int j = 0; j < (int)imageList.size(); j++) {
      if(holdList[i] == imageList[j]) {
        matched = true;
        break;
      }
    }
    if(!matched) {
      std::string msg = "The hold list file [" + holdList[i] +
                        "] does not match a file in the from list";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }
}

