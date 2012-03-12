#include "Isis.h"
#include "Application.h"
#include "FileList.h"
#include "IException.h"
#include "UserInterface.h"
#include "iString.h"
#include "Preference.h"
#include "ProgramLauncher.h"

using namespace std;
using namespace Isis;

void IsisMain(){

  UserInterface &ui = Application::GetUserInterface();

  // Determine which listing files will be created
  bool reportHiInc = ui.WasEntered("HIGHINCLIST");
  bool reportNoFile = ui.WasEntered("NOFILELIST");
  bool reportNadirSpc = ui.WasEntered("NADIRSPCLIST");
  bool reportImageGap = ui.WasEntered("IMAGEGAPLIST");

  // Get all user parameters before proceeding with
  // processing

  // Input file options
  FileList cubes;
  Pvl &pref = Preference::Preferences();
  string pathName = (string)pref.FindGroup("DataDirectory")["Temporary"] + "/";
  if (ui.WasEntered("TOPATH")) {
    pathName = (string)ui.GetString("TOPATH") + "/";
  }
  string outFile;
  if (ui.WasEntered("FROMLIST")) {
    cubes.Read(ui.GetFilename("FROMLIST"));
  }
  else {
    string msg = "Error: FROMLIST file must be specified";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Processing Options
  bool processNight  = ui.GetBoolean("NIGHT");
  bool ignoreAtmCorr = ui.GetBoolean("ATMOSCORR");

  // Output Information for GIS Software Package
  bool logCamStats = ui.GetBoolean("INFO");
  bool footPrintInit = false;
  string logFileName;
  if (logCamStats) {
    footPrintInit = ui.GetBoolean("FOOTPRINT");
    if (!ui.WasEntered("TOSTAT")) {
      logCamStats = false;
    } else {
      logFileName = ui.GetAsString("TOSTAT");
    }
  }

  // do stuff to build the file that will go into the spreadsheet
  // LOG_STATS is true will create a flat file of statistics.
  ofstream logFileos;
  if (logCamStats) {
    Filename logFile(logFileName);
    if (logFile.exists()) {
      logFileos.open(logFileName.c_str(),ios::app);
    }
    else {
      logFileos.open(logFileName.c_str(),ios::out);
      logFileos << "Filename," << "Duration," << "Summing,"<< "IncidenceAverage," <<
              "ResolutionAverage,"<< "IncidenceMinimum," << "IncidenceMaximum," <<  "Gaps,"<<endl;
    }
  }

  // Get Clean up options
  bool pdsSanFile;
  bool rmInput      = ui.GetBoolean("RMINPUT");
  bool rmHiIncInput = ui.GetBoolean("RMHIGHINC");

  // Create high incidence report file if requested
  Filename hiIncList;
  ofstream hiIncListos;
  if (rmHiIncInput && reportHiInc) {
    hiIncList = ui.GetFilename("HIGHINCLIST");
    iString hiIncListStr = hiIncList.Expanded();
    if (hiIncList.exists()) {
      hiIncListos.open(hiIncListStr.c_str(),ios::app);
    }
    else {
      hiIncListos.open(hiIncListStr.c_str(),ios::out);
      hiIncListos << "List of filenames with high incidence angles" << endl;
    }
  }

  // Create missing RDR file report if requested
  Filename noFileList;
  ofstream noFileListos;
  if (reportNoFile) {
    noFileList = ui.GetFilename("NOFILELIST");
    iString noFileListStr = noFileList.Expanded();
    if (noFileList.exists()) {
      noFileListos.open(noFileListStr.c_str(),ios::app);
    }
    else {
      noFileListos.open(noFileListStr.c_str(),ios::out);
      noFileListos << "List of files not processed and reason for not processing" << endl;
    }
  }

  // Create Nadir pointing file report if requested
  Filename nadirSpcList;
  ofstream nadirSpcListos;
  if (reportNadirSpc) {
    nadirSpcList = ui.GetFilename("NADIRSPCLIST");
    iString nadirSpcListStr = nadirSpcList.Expanded();
    if (nadirSpcList.exists()) {
      nadirSpcListos.open(nadirSpcListStr.c_str(),ios::app);
    }
    else {
      nadirSpcListos.open(nadirSpcListStr.c_str(),ios::out);
      nadirSpcListos << "List of Nadir pointing files" << endl;
    }
  }

  // Create image gap file report if requested
  Filename imageGapList;
  ofstream imageGapListos;
  if (reportImageGap) {
    imageGapList = ui.GetFilename("IMAGEGAPLIST");
    iString imageGapListStr = imageGapList.Expanded();
    if (imageGapList.exists()) {
      imageGapListos.open(imageGapListStr.c_str(),ios::app);
    }
    else {
      imageGapListos.open(imageGapListStr.c_str(),ios::out);
      imageGapListos << "List of files with gaps" << endl;
    }
  }

  // Main processing loop
  for (unsigned i = 0; i < cubes.size(); i++) {

  try {
    pdsSanFile = false;

    // Open the input file
    Filename infile = Filename(cubes[i]);
    iString inFileStr = infile.Expanded();

    if (!infile.Exists()) {
      if (reportNoFile) {
        noFileListos << infile.Basename() << " not processed because RDR file is missing" << endl;
      }
      continue;
    }

    Pvl lab(inFileStr);

    // Exit if not Themis image
    iString instrumentID = iString(lab["INSTRUMENT_ID"][0]);
    if (instrumentID.UpCase() != "THEMIS") {
      if (reportNoFile) {
        noFileListos << infile.Basename() << " not processed because is not a THEMIS image" << endl;
      }
      string msg = "Error: Not a Themis Image";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Verify for "IR" Detector ID
    iString detectorID = iString(lab["DETECTOR_ID"][0]);
    if (detectorID.UpCase() != "IR") {
      if (reportNoFile) {
        noFileListos << infile.Basename() << " not processed because is not an IR THEMIS image" << endl;
      }
      string msg = "Error: Not an IR Themis Image";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if (inFileStr.find("pds_san") != string::npos) {
      pdsSanFile = true;
    }

    string duration =(string)lab.FindObject("SPECTRAL_QUBE",Isis::PvlObject::Traverse)["IMAGE_DURATION"];

    // Make sure we have THEMIS IR at wavelength 12.57.
    // 12.57um wavelength has the best signal to noise ratio, and 14.88um wavelenght is atmospheric band
    PvlKeyword bandcenter = lab.FindGroup("BAND_BIN",Pvl::Traverse)["BAND_BIN_CENTER"];
    bool band12_57 = false;
    bool band14_88 = false;
    int procBand = 0;
    int atmosBand = 0;
    for (int index=0; index<bandcenter.Size(); index++) {
      if (bandcenter[index] == "12.57") {
        procBand = index + 1;
        band12_57 = true;
      }
      if (bandcenter[index] == "14.88") {
        atmosBand = index + 1;
        band14_88 = true;
      }
    }

    if (!band12_57) {
      if (reportNoFile) {
        noFileListos << infile.Basename() << " not processed because is missing filter 12.57" << endl;
      }
      string msg = "Filter 12.57 not found in input file [" + inFileStr + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    // Run thm2isis
    string output = (infile.Basename()) + ".cub";
    string parameters = "FROM=" + cubes[i] + " TO=" + output;
    ProgramLauncher::RunIsisProgram("thm2isis", parameters);

    //Run spiceinit
    string input = output;
    parameters = "FROM=" + input + " CKRECON=yes CKPREDICTED=yes CKNADIR=yes";
    ProgramLauncher::RunIsisProgram("spiceinit", parameters);
    if (reportNadirSpc) {
      Pvl spclab(input);
      PvlGroup kernels = spclab.FindGroup("Kernels", Pvl::Traverse);
      bool isNadir = false;
      for (int j=0; j < kernels["InstrumentPointing"].Size(); j++) {
        if (kernels["InstrumentPointing"][j].UpCase() == "NADIR") {
          isNadir = true;
        }
      }
      if (isNadir) {
        nadirSpcListos << infile.Basename() << endl;
      }
    }

    // Create a temporary pvl and fill with camstats used to test incedence
    Filename tstat1;
    string tmpcamstats1 = infile.Basename() + "_tmpcamstats1";
    tstat1.Temporary(tmpcamstats1,"pvl");
    string tempstat1 = tstat1.Expanded();

    parameters = "FROM=" + input + " TO=" + tempstat1 + " linc = 100 sinc = 100";
    ProgramLauncher::RunIsisProgram("camstats", parameters);
    Pvl p1;
    p1.Read(tempstat1);
    double incAngle =p1.FindGroup("IncidenceAngle",Pvl::Traverse)["IncidenceMinimum"];

    // if do not process night and incidence angle > 90, then exit
    if (!processNight && incAngle >= 90) {
      remove (tstat1.Expanded().c_str());
      if (rmHiIncInput) {
        if (reportHiInc) {
          hiIncListos << infile.Basename() << endl;
        }
        if (!pdsSanFile) {
          remove(inFileStr.c_str());
          remove(input.c_str());
        }
      }
      if (reportNoFile) {
        noFileListos << infile.Basename() << " not processed because average incidence angle >= 90 and DAY images requested" << endl;
        remove(output.c_str());
      }
      string msg = "The average incidence angle of [" + cubes[i] + "] is over 90";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // if process night and incidence angle < 90, then exit
    if (processNight && incAngle < 90) {
      remove (tstat1.Expanded().c_str());
      if (rmHiIncInput) {
        if (reportHiInc) {
          hiIncListos << infile.Basename() << endl;
        }
        if (!pdsSanFile) {
          remove(inFileStr.c_str());
          remove(input.c_str());
        }
      }
      if (reportNoFile) {
        noFileListos << infile.Basename() << " not processed because average incidence angle < 90 and NIGHT images requested" << endl;
        remove(output.c_str());
      }
      string msg = "The average incidence angle of [" + cubes[i] + "] is over 90";
      throw IException(IException::User,msg,_FILEINFO_);
    }

    // Run thmdriftcor
    // Perform atmospheric correction using filter 10 or 14.88um wavelength.
    // Note we use wavelength 12.57um in geologic mosaics.
    if (band14_88 && incAngle < 90 && !ignoreAtmCorr)  {
      output = (infile.Basename()) + "_driftcorr.cub";
      parameters = "FROM=" + input + "+" + iString(procBand) +
                   " ATM=" + input + "+" + iString(atmosBand) + " TO=" + output;
      ProgramLauncher::RunIsisProgram("thmdriftcor", parameters);
      remove (input.c_str());
    }
    else {
      output = (infile.Basename()) + "_no_driftcorr.cub";
      parameters = "FROM=" + input + "+" + iString(procBand) + " TO=" + output;
      ProgramLauncher::RunIsisProgram("stretch", parameters);
      remove (input.c_str());
    }

    // Run cosi, for incidence < 90 (day images)
    // if night is true then skip cosi
    if (incAngle < 90) {
      input = output;
      output = (infile.Basename()) + "_cosi.cub";
      parameters = "FROM=" + input + " TO=" + output;
      ProgramLauncher::RunIsisProgram("cosi", parameters);
      remove (input.c_str());
    }

    // Run cubenorm
    input = output;
    output = (infile.Basename()) + "cubenorm.cub";
    parameters = "FROM=" + input + " TO=" + output;
    ProgramLauncher::RunIsisProgram("cubenorm", parameters);
    remove (input.c_str());

    //Run lineeq
    input=output;
    outFile = pathName + infile.Basename() + ".lev1.cub";
    parameters ="FROM=" + input + " TO=" + outFile;
    ProgramLauncher::RunIsisProgram("lineeq", parameters);
    remove (input.c_str());

    remove (tstat1.Expanded().c_str());

    //************************************
    // Run findgaps
    // Create temporary pvl for gaps
    string sgap = "no";
    Filename tgaps;
    string tmpstats1 = infile.Basename() + "_tmpstats1";
    tgaps.Temporary(tmpstats1,"pvl");
    string tempgaps = tgaps.Expanded();

    //parameters = "FROM=" + iString(ui.GetFilename("TO")) +
    //             " TO=" + tempgaps + " CORTOL=.3";
    //ProgramLauncher::RunIsisProgram("findgaps", parameters);
    parameters = "FROM=" + outFile +
                 " TO=" + tempgaps + " APPEND=no";
    ProgramLauncher::RunIsisProgram("stats", parameters);
    Pvl tg;
    tg.Read(tempgaps);
    string totalpixels = tg.FindGroup("Results",Pvl::Traverse)["TotalPixels"];
    string validpixels = tg.FindGroup("Results",Pvl::Traverse)["ValidPixels"];
    //if (tg.HasGroup("Gap")) {
    if (iString(totalpixels).ToInteger() != iString(validpixels).ToInteger()) {
      cout << tg << endl;
      sgap = "yes";
      if (reportImageGap) {
        imageGapListos << infile.Basename() << endl;
      }
    }
    remove (tgaps.Expanded().c_str());

    // Create temporary pvl and fill with camstats
    Filename tstat2;
    string tmpcamstats2 = infile.Basename() + "_tmpcamstats2";
    tstat2.Temporary(tmpcamstats2,"pvl");
    string tempstat2 = tstat2.Expanded();

    parameters = "FROM=" + outFile +
                 " TO=" + tempstat2 + " linc = 100 sinc = 100";
    ProgramLauncher::RunIsisProgram("camstats", parameters);

    Pvl p2;
    p2.Read(tempstat2);
    string incavg = p2.FindGroup("IncidenceAngle",Pvl::Traverse)["IncidenceAverage"];
    string  resavg = p2.FindGroup("Resolution",Pvl::Traverse)["ResolutionAverage"];
    string incmin = p2.FindGroup("IncidenceAngle",Pvl::Traverse)["IncidenceMinimum"];
    string incmax = p2.FindGroup("IncidenceAngle",Pvl::Traverse)["IncidenceMaximum"];
    remove (tstat2.Expanded().c_str());

    double summing = 1;
    Filename tosum = Filename(outFile);
    Pvl sumlab(tosum.Expanded());
    PvlGroup InstGrp = sumlab.FindGroup("Instrument",Pvl::Traverse);
    if (InstGrp.HasKeyword("SpatialSumming")) {
      summing = InstGrp["SpatialSumming"];
    }

    // do stuff to build the file that will go into the spreadsheet
    // Add statistics to flat file.
    if (logCamStats) {
      logFileos << infile.Basename() << "," << duration << "," << summing << "," <<
        incavg << "," << resavg << "," << incmin << "," << incmax << "," << sgap << "," <<endl;
    }

    // Run footprint stuff if requested for GIS input
    if (footPrintInit) {
      parameters = "FROM=" + outFile +
                   " TO=" + (iString)(infile.baseName()) + ".gml" +
                   " LABEL=" + (iString)(infile.baseName());
      ProgramLauncher::RunIsisProgram("isis2gml", parameters);
    }

    if (rmInput && !pdsSanFile) {
      remove(inFileStr.c_str());
    }
  }
  catch(IException &e) {
  }
  }
}
