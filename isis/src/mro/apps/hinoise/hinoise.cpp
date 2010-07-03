#include "Isis.h"
#include <string>
#include <stdio.h>
#include "Filename.h"
#include "iException.h"
#include "Histogram.h"
#include "ProcessByLine.h"
#include "Pixel.h"


using namespace std;
using namespace Isis;

// global Filenames                   // Perl script identifiers
string g_zapFilename;                 // tfile1
string g_lpfFilename;                 // tfile1a
string g_hpfFilename;                 // tfile1b
string g_addFilename;                 // tfile1c
string g_noisefilter1Filename;        // tfile2
string g_noisefilter2Filename;        // tfile2a
string g_noisefilter3Filename;        // tfile2b
string g_lpfz1Filename;               // tfile3
string g_lpfz2Filename;               // tfile3a
string g_isisnormFilenameSuffix = ""; // isisnorm

// global data
vector<int> g_validPixelCounts;
int g_maxValidPixelCount;
double g_nonValidFraction;
enum status { good, bad };
vector<status> g_columnStatus;

// function prototypes
void defineFilenames();
void getCounts(Buffer & in);
bool areaFailed(const pair<int, int> & someArea);
void zapAllAreas(const pair<int, int> areas[4]);
void applyColumnStatus(Buffer & in, Buffer & out);
void rmTempFiles();

void IsisMain() {
  UserInterface & ui = Application::GetUserInterface();
  defineFilenames();

  // *********** vertical destriping ***********

  // The idea here is that some columns in the input cube have too many invalid pixels
  // where "too many invalid pixels" is defined by the user with the parameter
  // NONVALIDFRACTION.  If the cube's binning is anything other than 1 then all
  // columns are checked.  If a column has too many invalid pixels then every dn in
  // that column is set to 0.  If the binning is 1 then only columns in known problem
  // areas (pause points) are checked.  If one of these checked columns fails then
  // every column in area has its valid dns set to NULL.  After all this the resulting
  // cube is run through lowpass and highpass with the results from each merged back
  // together 

  // First get valid pixel counts for all columns.
  // Next get the maximum of these counts and the NONVALIDFRACTION.
  // Then build a column status vector based on the cubes binning.
  //   - The column status vector has elements such that each element represents
  //   - a column of the cube and contains a value of either good if the column
  //   - is ok or bad if the column is out of tolerance.
  // Finally use the column status vector to destripe the cube
  // After all this run lowpass, highpass, and algebra for further destriping
 

  // get valid pixel counts for all columns of the cube
  ProcessByLine p;
  Cube * inCube = p.SetInputCube("FROM");
  g_validPixelCounts.assign(inCube->Samples(), 0);
  p.StartProcess(getCounts);
  
  // get the maximum count and the nonValidFraction
  g_maxValidPixelCount = g_validPixelCounts[0];
  for (unsigned int i = 1; i < g_validPixelCounts.size(); i++) {
    if (g_maxValidPixelCount < g_validPixelCounts[i]) {
      g_maxValidPixelCount = g_validPixelCounts[i];
    }
  }
  g_nonValidFraction = ui.GetDouble("NONVALIDFRACTION");
  
  // build column status vector
  g_columnStatus.assign(inCube->Samples(), good);
  int binning = inCube->GetGroup("Instrument").FindKeyword("Summing")[0];
  if (binning != 1) {
    for (unsigned int i = 0; i < g_columnStatus.size(); i++) {
      if ((double) g_validPixelCounts[i] / g_maxValidPixelCount < g_nonValidFraction) {
        g_columnStatus[i] = bad;
      }
    }
  }
  else {
    // First define the problem areas to be tested, which depend on what
    // channel this cube happens to be.  Each area is stored as a pair
    // where the starting column is first and the ending column is second.
    // The boundries defining an area are INCLUSIVE!
    pair<int, int> testAreas[4];
    int channel = inCube->GetGroup("Instrument").FindKeyword("ChannelNumber")[0];
    switch (channel) {
      case 1:
        testAreas[0].first  = 241;
        testAreas[0].second = 246;
        testAreas[1].first  = 505;
        testAreas[1].second = 509;
        testAreas[2].first  = 768;
        testAreas[2].second = 772;
        testAreas[3].first  = 1021;
        testAreas[3].second = 1023;
        break;
      case 0:
        testAreas[0].first  = 0;
        testAreas[0].second = 2;
        testAreas[1].first  = 251;
        testAreas[1].second = 255;
        testAreas[2].first  = 514;
        testAreas[2].second = 518;
        testAreas[3].first  = 777;
        testAreas[3].second = 781;
        break;
      default:
        // this should really never happen unless histitch was ran!
        string message = "Cube has invalid channel number!\n";
              message += "valid channels are 0 and 1\n\n";
              message += "If this cube is the result of merging then\n";
              message += "it is too late for vertical destriping.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
    }
    for (int k = 0; k < 4; k++) {
      if (areaFailed(testAreas[k])) {
        zapAllAreas(testAreas);
        break;
      }
    }
  }

  // Finally use the column status vector to zero the bad columns
  CubeAttributeOutput atts;
  p.SetOutputCube(g_zapFilename, atts, inCube->Samples(), inCube->Lines(), inCube->Bands());
  p.StartProcess(applyColumnStatus);
  p.EndProcess();

  // Now run lowpass, highpass, and algebra for further destriping
  Isis::iApp->Exec("lowpass", "FROM=" + g_zapFilename + " TO=" +
      g_lpfFilename + "+32bit NULL=FALSE HRS=FALSE HIS=FALSE LRS=FALSE LIS=FALSE LINE=" +
      ui.GetString("LPFLINE") + " SAMP=" + ui.GetString("LPFSAMP") +
      " MINOPT=PERCENT MINIMUM=" + ui.GetString("LPFMINPER") +
      " REPLACE=NULL");
  Isis::iApp->Exec("highpass", "FROM=" + g_zapFilename + " TO=" + g_hpfFilename +
      " LINE=" + ui.GetString("HPFLINE") + " SAMP=" + ui.GetString("HPFSAMP") +
      " MINOPT=PERCENT" + " MINIMUM=" + ui.GetString("HPFMINPER"));
  Isis::iApp->Exec("algebra", "FROM1=" + g_lpfFilename + " FROM2=" +
      g_hpfFilename + " TO=" + g_addFilename + g_isisnormFilenameSuffix + " OPERATOR=ADD");

  // ******** vertical destriping done *********

  
  // ************* noise filtering *************
  
  // noise filters need FLATTOL, LOW, HIGH, TOLMIN, and TOLMAX parameters
  // calculated first

  // get FLATTOL
  Cube cube;
  cube.Open(ui.GetFilename("FROM"));
  Histogram * hist = cube.Histogram();
  double flattol = hist->StandardDeviation() * ui.GetDouble("FLATTOL");

  // Get HIGH
  double lisPixels = hist->LisPixels();
  double HardFiltering = ui.GetDouble("HARDFILTERING");
  double high;
  double HardHighEnd = ui.GetDouble("HARDHIGHENDPERCENT");
  double HighEnd = ui.GetDouble("HIGHENDPERCENT");
  if (lisPixels > HardFiltering) {
    high = hist->Percent(HardHighEnd);
  }
  else {
    high = hist->Percent(HighEnd);
  }
  
  // don't need hist anymore so close cube,
  // which needs done now before noisefilter is called
  cube.Close();

  // get TOLMIN and TOLMAX
  double tolmin = ui.GetDouble("TOLMIN");
  double tolmax = ui.GetDouble("TOLMAX");
  if (lisPixels >= HardFiltering) {
    tolmin = ui.GetDouble("HARDTOLMIN");
    tolmax = ui.GetDouble("HARDTOLMAX");
  }

  // run first noise filter
  Isis::iApp->Exec("noisefilter", "FROM=" + g_addFilename + " TO=" + g_noisefilter1Filename +
      g_isisnormFilenameSuffix + " FLATTOL=" + iString(flattol) + " TOLDEF=STDDEV LOW=" +
      ui.GetAsString("LOW") + " HIGH=" + iString(high) + " TOLMIN=" + iString(tolmin) + " TOLMAX=" +
      iString(tolmax) + " REPLACE=NULL SAMPLE=" + ui.GetString("NOISESAMPLE") + " LINE=" +
      ui.GetString("NOISELINE") + " LISISNOISE=true LRSISNOISE=true");

  // run second noise filter
  Isis::iApp->Exec("noisefilter", "FROM=" + g_noisefilter1Filename + " TO=" +
      g_noisefilter2Filename + g_isisnormFilenameSuffix + " FLATTOL=" + iString(flattol) +
      " TOLDEF=STDDEV LOW=" + ui.GetAsString("LOW") + " HIGH=" + iString(high) + " TOLMIN=" +
      iString(tolmin) + " TOLMAX=" + iString(tolmax) + " REPLACE=NULL SAMPLE=" +
      ui.GetString("NOISESAMPLE") + " LINE=" + ui.GetString("NOISELINE") +
      " LISISNOISE=true LRSISNOISE=true");

  // run third noise filter
  Isis::iApp->Exec("noisefilter", "FROM=" + g_noisefilter2Filename + " TO=" +
      g_noisefilter3Filename + g_isisnormFilenameSuffix + " FLATTOL=" + iString(flattol) +
      " TOLDEF=STDDEV LOW=" + ui.GetAsString("LOW") + " HIGH=" + iString(high) + " TOLMIN=" +
      iString(tolmin) + " TOLMAX=" + iString(tolmax) + " REPLACE=NULL SAMPLE=" +
      ui.GetString("NOISESAMPLE") + " LINE=" + ui.GetString("NOISELINE") +
      " LISISNOISE=true LRSISNOISE=true");

  // ********** noise filtering done  **********


  // ************** fill in holes **************

  Isis::iApp->Exec("lowpass", "FROM=" + g_noisefilter3Filename + " TO=" + g_lpfz1Filename +
      g_isisnormFilenameSuffix + " sample=3 line=3 minopt=COUNT minimum=3 filter=OUTSIDE" +
      " null=TRUE hrs=FALSE his=TRUE lrs=TRUE lis=TRUE");

  // if binning is not 1 then stop after next lowpass
  string outFile = g_lpfz2Filename;
  if (binning != 1) {
    outFile = ui.GetFilename("TO");
  }

  // get minimum and use it for another lowpass
  int lpfzsamples =  ui.GetInteger("LPFZSAMPLES");
  int lpfzlines = ui.GetInteger("LPFZLINES");
  int minimum = (int) ((lpfzlines * lpfzsamples) / 3);
  Isis::iApp->Exec("lowpass", "FROM=" + g_lpfz1Filename + " TO=" + outFile +
      g_isisnormFilenameSuffix + " sample=" + iString(lpfzsamples) + " line=" + iString(lpfzlines) + " minopt=COUNT" +
      " minimum=" + iString(minimum) + " filter=OUTSIDE null=TRUE hrs=FALSE his=TRUE " +
      "lrs=TRUE lis=TRUE");

  // ************** holes filled ***************

  // if binning is 1 then need to check pause point areas again
  if (binning == 1) {
    CubeAttributeInput atts;
    ProcessByLine p2;
    p2.SetInputCube(g_lpfz2Filename, atts);
    p2.SetOutputCube("TO");
    p2.StartProcess(applyColumnStatus);
    p2.EndProcess();
  }

  rmTempFiles();

} // of IsisMain()

void defineFilenames() {
  UserInterface & ui = Application::GetUserInterface();
  Filename fn;
  fn.Temporary("hinoise-zap", "cub");          g_zapFilename = fn.Expanded();
  fn.Temporary("hinoise-lpf", "cub");          g_lpfFilename = fn.Expanded();
  fn.Temporary("hinoise-hpf", "cub");          g_hpfFilename = fn.Expanded();
  fn.Temporary("hinoise-add", "cub");          g_addFilename = fn.Expanded();
  fn.Temporary("hinoise-noisefilter1", "cub"); g_noisefilter1Filename = fn.Expanded();
  fn.Temporary("hinoise-noisefilter2", "cub"); g_noisefilter2Filename = fn.Expanded();
  fn.Temporary("hinoise-noisefilter3", "cub"); g_noisefilter3Filename = fn.Expanded();
  fn.Temporary("hinoise-lpfz1", "cub");         g_lpfz1Filename = fn.Expanded();
  fn.Temporary("hinoise-lpfz2", "cub");         g_lpfz2Filename = fn.Expanded();

  if (ui.GetString("MINNORMALIZATION") != "Null" && ui.GetString("MAXNORMALIZATION") != "Null") {
    g_isisnormFilenameSuffix = "+SignedWord+" + ui.GetString("MINNORMALIZATION") + ":" +
        ui.GetString("MAXNORMALIZATION");
  }
}

void getCounts(Buffer & in) {
  // get the counts
  for (unsigned int i = 0; i < g_validPixelCounts.size(); i++) {
    if (Pixel::IsValid(in[i])) {
      g_validPixelCounts[i]++;
    }
  }
}


bool areaFailed(const pair<int, int> & someArea) {
  for (int i = someArea.first; i <= someArea.second; i++) {
    if ((double) g_validPixelCounts[i] / g_maxValidPixelCount < g_nonValidFraction) {
      return true;
    }
  }
  return false;
}

void zapAllAreas(const pair<int, int> areas[4]) {
  for (int i = 0; i < 4; i++) {
    for (int j = areas[i].first; j <= areas[i].second; j++) {
      g_columnStatus[j] = bad;
    }
  }
}

void applyColumnStatus(Buffer & in, Buffer & out) {
  for (int i = 0; i < in.size(); i++) {
    if (g_columnStatus[i] == bad && Pixel::IsValid(in[i])) {
      out[i] = NULL8;
    }
    else {
      out[i] = in[i];
    }
  }
}

void rmTempFiles() {
  remove(g_zapFilename.c_str());
  remove(g_lpfFilename.c_str());
  remove(g_hpfFilename.c_str());
  remove(g_addFilename.c_str());
  remove(g_noisefilter1Filename.c_str());
  remove(g_noisefilter2Filename.c_str());
  remove(g_noisefilter3Filename.c_str());
  remove(g_lpfz1Filename.c_str());
  remove(g_lpfz2Filename.c_str());
}
