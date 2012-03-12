#define GUIHELPERS

#include "Isis.h"

#include "AutoReg.h"
#include "AutoRegFactory.h"
#include "Chip.h"
#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Cube.h"
#include "ProgramLauncher.h"
#include "Progress.h"
#include "PvlGroup.h"
#include "SerialNumber.h"
#include "Statistics.h"
#include "UserInterface.h"
#include "IException.h"
#include "iString.h"
#include "iTime.h"

using namespace std;
using namespace Isis;

//helper button functins in the code
void helperButtonLog();

map <string, void *> GuiHelpers() {
  map <string, void *> helper;
  helper ["helperButtonLog"] = (void *) helperButtonLog;
  return helper;
}

void IsisMain() {
  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  // Make sure the correct parameters are entered
  if(ui.WasEntered("TO")) {
    if(ui.GetString("TRANSFORM") == "WARP") {
      if(!ui.WasEntered("ONET")) {
        string msg = "A Control Net file must be entered if the TO parameter is ";
        msg += "entered";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
  }

  // Open the first cube.  It will be matched to the second input cube.
  Cube trans;
  CubeAttributeInput &attTrans = ui.GetInputAttribute("FROM");
  std::vector<string> bandTrans = attTrans.Bands();
  trans.setVirtualBands(bandTrans);
  trans.open(ui.GetFilename("FROM"), "r");


  // Open the second cube, it is held in place.  We will be matching the
  // first to this one by attempting to compute a sample/line translation
  Cube match;
  CubeAttributeInput &attMatch = ui.GetInputAttribute("MATCH");
  std::vector<string> bandMatch = attMatch.Bands();
  match.setVirtualBands(bandMatch);
  match.open(ui.GetFilename("MATCH"), "r");

  // Input cube Lines and Samples must be the same and each must have only
  // one band
  if((trans.getLineCount() != match.getLineCount()) ||
      (trans.getSampleCount() != match.getSampleCount())) {
    string msg = "Input Cube Lines and Samples must be equal!";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if(trans.getBandCount() != 1 || match.getBandCount() != 1) {
    string msg = "Input Cubes must have only one band!";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Get serial number
  string serialTrans = SerialNumber::Compose(trans, true);
  string serialMatch = SerialNumber::Compose(match, true);

//  This still precludes band to band registrations.
  if(serialTrans == serialMatch) {
    string sTrans = Filename(trans.getFilename()).Name();
    string sMatch = Filename(match.getFilename()).Name();
    if(sTrans == sMatch) {
      string msg = "Cube Serial Numbers must be unique - FROM=" + serialTrans +
                   ", MATCH=" + serialMatch;
      throw IException(IException::User, msg, _FILEINFO_);
    }
    serialTrans = sTrans;
    serialMatch = sMatch;
  }


  // We need to get a user definition of how to auto correlate around each
  // of the control points.
  Pvl regdef;
  regdef.Read(ui.GetFilename("DEFFILE"));
  AutoReg *ar = AutoRegFactory::Create(regdef);

  // We want to create a grid of control points that is N rows by M columns.
  // Get row and column variables, if not entered, default to 1% of the input
  // image size
  int rows, cols;
  if(ui.WasEntered("ROWS")) {
    rows = ui.GetInteger("ROWS");
  }
  else {
    rows = (int)((trans.getLineCount() - 1) / ar->SearchChip()->Lines() + 1);
  }
  if(ui.WasEntered("COLUMNS")) {
    cols = ui.GetInteger("COLUMNS");
  }
  else {
    cols = (int)((trans.getSampleCount() - 1)
                 / ar->SearchChip()->Samples() + 1);
  }

  // Display the progress...10% 20% etc.
  Progress prog;
  prog.SetMaximumSteps(rows * cols);
  prog.CheckStatus();

  // Calculate spacing for the grid of points
  double lSpacing = (double)trans.getLineCount() / rows;
  double sSpacing = (double)trans.getSampleCount() / cols;

  // Initialize control point network and set target name (only required
  // field)
  ControlNet cn;
  if (match.hasGroup("Instrument")) {
    PvlGroup inst = match.getGroup("Instrument");
    PvlKeyword &targname = inst.FindKeyword("TargetName");
    string targetname = targname;
    cn.SetTarget(targetname);
  }

  // Loop through grid of points and get statistics to compute
  // translation values
  Statistics sStats, lStats;
  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      int line = (int)(lSpacing / 2.0 + lSpacing * r + 0.5);
      int samp = (int)(sSpacing / 2.0 + sSpacing * c + 0.5);
      ar->PatternChip()->TackCube(samp, line);
      ar->PatternChip()->Load(match);
      ar->SearchChip()->TackCube(samp, line);
      ar->SearchChip()->Load(trans);

      // Set up ControlMeasure for cube to translate
      ControlMeasure * cmTrans = new ControlMeasure;
      cmTrans->SetCubeSerialNumber(serialTrans);
      cmTrans->SetCoordinate(samp, line, ControlMeasure::Candidate);
      cmTrans->SetChooserName("coreg");

      // Set up ControlMeasure for the pattern/Match cube
      ControlMeasure * cmMatch = new ControlMeasure;
      cmMatch->SetCubeSerialNumber(serialMatch);
      cmMatch->SetCoordinate(samp, line, ControlMeasure::RegisteredPixel);
      cmMatch->SetChooserName("coreg");

      ar->Register();

      // Match found
      if(ar->Success()) {
        double sDiff = samp - ar->CubeSample();
        double lDiff = line - ar->CubeLine();
        sStats.AddData(&sDiff, (unsigned int)1);
        lStats.AddData(&lDiff, (unsigned int)1);
        cmTrans->SetCoordinate(ar->CubeSample(), ar->CubeLine(),
                              ControlMeasure::RegisteredPixel);
        cmTrans->SetResidual(sDiff, lDiff);
        cmTrans->SetLogData(ControlMeasureLogData(
              ControlMeasureLogData::GoodnessOfFit,
              ar->GoodnessOfFit()));
      }

      // Add the measures to a control point
      string str = "Row " + iString(r) + " Column " + iString(c);
      ControlPoint * cp = new ControlPoint(str);
      cp->SetType(ControlPoint::Free);
      cp->Add(cmTrans);
      cp->Add(cmMatch);
      cp->SetRefMeasure(cmMatch);
      if(!cmTrans->IsMeasured()) cp->SetIgnored(true);
      cn.AddPoint(cp);
      prog.CheckStatus();
    }
  }

  // Write translation to log
  PvlGroup results("Translation");
  double sMin = (int)(sStats.Minimum() * 100.0) / 100.0;
  double sTrans = (int)(sStats.Average() * 100.0) / 100.0;
  double sMax = (int)(sStats.Maximum() * 100.0) / 100.0;
  double sDev = (int)(sStats.StandardDeviation() * 100.0) / 100.0;
  double lMin = (int)(lStats.Minimum() * 100.0) / 100.0;
  double lTrans = (int)(lStats.Average() * 100.0) / 100.0;
  double lMax = (int)(lStats.Maximum() * 100.0) / 100.0;
  double lDev = (int)(lStats.StandardDeviation() * 100.0) / 100.0;

  results += PvlKeyword("SampleMinimum", sMin);
  results += PvlKeyword("SampleAverage", sTrans);
  results += PvlKeyword("SampleMaximum", sMax);
  results += PvlKeyword("SampleStandardDeviation", sDev);
  results += PvlKeyword("LineMinimum", lMin);
  results += PvlKeyword("LineAverage", lTrans);
  results += PvlKeyword("LineMaximum", lMax);
  results += PvlKeyword("LineStandardDeviation", lDev);
  Application::Log(results);

  Pvl arPvl = ar->RegistrationStatistics();

  for(int i = 0; i < arPvl.Groups(); i++) {
    Application::Log(arPvl.Group(i));
  }

  // add the auto registration information to print.prt
  PvlGroup autoRegTemplate = ar->RegTemplate();
  Application::Log(autoRegTemplate);

  // If none of the points registered, throw an error
  if(sStats.TotalPixels() < 1) {
    string msg = "Coreg was unable to register any points. Check your algorithm definition.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Don't need the cubes opened anymore
  trans.close();
  match.close();

  // If a cnet file was entered, write the ControlNet pvl to the file
  if(ui.WasEntered("ONET")) {
    cn.Write(ui.GetFilename("ONET"));
  }

  // If flatfile was entered, create the flatfile
  // The flatfile is comma seperated and can be imported into an excel
  // spreadsheet
  if(ui.WasEntered("FLATFILE")) {
    string fFile = Filename(ui.GetFilename("FLATFILE")).Expanded();
    ofstream os;
    os.open(fFile.c_str(), ios::out);
    os << "Sample,Line,TranslatedSample,TranslatedLine," <<
       "SampleDifference,LineDifference,GoodnessOfFit" << endl;
    for(int i = 0; i < cn.GetNumPoints(); i++) {
      const ControlPoint * cp = cn[i];
      if(cp->IsIgnored()) continue;
      const ControlMeasure * cmTrans = cp->GetMeasure(0);
      const ControlMeasure * cmMatch = cp->GetMeasure(1);

      double goodnessOfFit = cmTrans->GetLogData(
          ControlMeasureLogData::GoodnessOfFit).GetNumericalValue();

      os << cmTrans->GetSample() << "," << cmTrans->GetLine() << ","
         << cmMatch->GetSample() << "," << cmMatch->GetLine() << ","
         << cmTrans->GetSampleResidual() << "," << cmTrans->GetLineResidual()
         << "," << goodnessOfFit << endl;
    }
  }

  // If a TO parameter was specified, apply the average translation found to the
  // second input image
  if(ui.WasEntered("TO")) {
    if(ui.GetString("TRANSFORM") == "TRANSLATE") {
      string params = " from="   + ui.GetFilename("FROM") +
                      " to="     + ui.GetFilename("TO") +
                      " strans=" + iString(sTrans) +
                      " ltrans=" + iString(lTrans) +
                      " interp=" + ui.GetString("INTERP");
      ProgramLauncher::RunIsisProgram("translate", params);
    }
    else {
      string params = " from="    + ui.GetFilename("FROM") +
                      " to="     + ui.GetFilename("TO") +
                      " cube="   + ui.GetFilename("MATCH") +
                      " cnet="   + ui.GetFilename("ONET") +
                      " interp=" + ui.GetString("INTERP") +
                      " degree=" + iString(ui.GetInteger("DEGREE"));
      ProgramLauncher::RunIsisProgram("warp", params);
    }
  }
}

//Helper function to output the regdeft file to log.
void helperButtonLog() {
  UserInterface &ui = Application::GetUserInterface();
  string file(ui.GetFilename("DEFFILE"));
  Pvl p;
  p.Read(file);
  Application::GuiLog(p);
}
//...........end of helper function ........
