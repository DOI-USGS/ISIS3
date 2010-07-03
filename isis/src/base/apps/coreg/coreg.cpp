#define GUIHELPERS

#include "Isis.h"
#include "PvlGroup.h"
#include "UserInterface.h"
#include "Cube.h"
#include "Chip.h"
#include "Progress.h"
#include "iException.h"
#include "AutoReg.h"
#include "AutoRegFactory.h"
#include "Statistics.h"
#include "ControlNet.h"
#include "SerialNumber.h"
#include "ControlMeasure.h"
#include "iTime.h"
                           
using namespace std;
using namespace Isis;

//helper button functins in the code
void helperButtonLog();

map <string,void*> GuiHelpers(){
  map <string,void*> helper;
  helper ["helperButtonLog"] = (void*) helperButtonLog;
  return helper;
}

void IsisMain() {
  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  // Make sure the correct parameters are entered
  if (ui.WasEntered("TO")) {
    if (ui.GetString("TRANSFORM") == "WARP") {
      if (!ui.WasEntered("CNETFILE")) {
        string msg = "A Control Net file must be entered if the TO parameter is ";
        msg += "entered";
        throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
      }
    }
  }

  // Open the first cube.  It will be matched to the second input cube.
  Cube trans;
  CubeAttributeInput &attTrans = ui.GetInputAttribute("FROM");
  vector<string> bandTrans = attTrans.Bands();
  trans.SetVirtualBands(bandTrans);
  trans.Open(ui.GetFilename("FROM"),"r");


  // Open the second cube, it is held in place.  We will be matching the
  // first to this one by attempting to compute a sample/line translation
  Cube match;
  CubeAttributeInput &attMatch = ui.GetInputAttribute("MATCH");
  vector<string> bandMatch = attMatch.Bands();
  match.SetVirtualBands(bandMatch);
  match.Open(ui.GetFilename("MATCH"),"r");

  // Input cube Lines and Samples must be the same and each must have only
  // one band
  if ((trans.Lines() != match.Lines()) ||
      (trans.Samples() != match.Samples())) {
    string msg = "Input Cube Lines and Samples must be equal!";
    throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }

  if (trans.Bands() != 1 || match.Bands() != 1) {
    string msg = "Input Cubes must have only one band!";
    throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }

  // Get serial number
  string serialTrans = SerialNumber::Compose(trans, true);
  string serialMatch = SerialNumber::Compose(match, true);

//  This still precludes band to band registrations.
  if (serialTrans == serialMatch) {
    string sTrans = Filename(trans.Filename()).Name();
    string sMatch = Filename(match.Filename()).Name();
    if (sTrans == sMatch) {
      string msg = "Cube Serial Numbers must be unique - FROM=" + serialTrans +
                   ", MATCH=" + serialMatch;
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
    serialTrans = sTrans;
    serialMatch = sMatch;
  } 


  // We need to get a user definition of how to auto correlate around each
  // of the control points.
  Pvl regdef;
  regdef.Read(ui.GetFilename("REGDEF"));
  AutoReg *ar = AutoRegFactory::Create(regdef);

  // We want to create a grid of control points that is N rows by M columns.
  // Get row and column variables, if not entered, default to 1% of the input
  // image size
  int rows, cols;
  if (ui.WasEntered("ROWS")) {
    rows = ui.GetInteger("ROWS");
  }
  else {
    rows = (int)((trans.Lines() - 1) / ar->SearchChip()->Lines() + 1);
  }
  if (ui.WasEntered("COLUMNS")) {
    cols = ui.GetInteger("COLUMNS");
  }
  else {
    cols = (int)((trans.Samples() - 1) / ar->SearchChip()->Samples() + 1);
  }

  // Display the progress...10% 20% etc.
  Progress prog;
  prog.SetMaximumSteps(rows * cols);
  prog.CheckStatus();

  // Calculate spacing for the grid of points
  double lSpacing = (double)trans.Lines() / rows;
  double sSpacing = (double)trans.Samples() / cols;

  // Initialize control point network
  ControlNet cn;
  cn.SetType(ControlNet::ImageToImage);
  cn.SetUserName(Application::UserName());
  cn.SetCreatedDate(iTime::CurrentLocalTime());

  // Loop through grid of points and get statistics to compute
  // translation values
  Statistics sStats, lStats;
  for (int r=0; r<rows; r++) {
    for (int c=0; c<cols; c++) {
      int line = (int)(lSpacing / 2.0 + lSpacing * r + 0.5);
      int samp = (int)(sSpacing / 2.0 + sSpacing * c + 0.5);
      ar->PatternChip()->TackCube(samp, line);
      ar->PatternChip()->Load(match);
      ar->SearchChip()->TackCube(samp, line);
      ar->SearchChip()->Load(trans);

      // Set up ControlMeasure for cube to translate
      ControlMeasure cmTrans;
      cmTrans.SetCubeSerialNumber(serialTrans);
      cmTrans.SetCoordinate(samp, line, ControlMeasure::Unmeasured);
      cmTrans.SetChooserName("coreg");       
      cmTrans.SetReference(false);

      // Set up ControlMeasure for the pattern/Match cube 
      ControlMeasure cmMatch;
      cmMatch.SetCubeSerialNumber(serialMatch);
      cmMatch.SetCoordinate(samp, line, ControlMeasure::Automatic);
      cmMatch.SetChooserName("coreg");
      cmMatch.SetReference(true);

      // Match found
      if (ar->Register()==AutoReg::Success) {
        double sDiff = samp - ar->CubeSample();
        double lDiff = line - ar->CubeLine();
        sStats.AddData(&sDiff,(unsigned int)1);
        lStats.AddData(&lDiff,(unsigned int)1);
        cmTrans.SetCoordinate(ar->CubeSample(), ar->CubeLine(),
                          ControlMeasure::Automatic);
        cmTrans.SetError(sDiff, lDiff);
        cmTrans.SetGoodnessOfFit(ar->GoodnessOfFit());
      }

      // Add the measures to a control point
      string str = "Row " + iString(r) + " Column " + iString(c);
      ControlPoint cp(str);
      cp.SetType(ControlPoint::Tie);
      cp.Add(cmTrans);
      cp.Add(cmMatch);
      if (!cmTrans.IsMeasured()) cp.SetIgnore(true);
      cn.Add(cp);
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

  results += PvlKeyword ("SampleMinimum", sMin);
  results += PvlKeyword ("SampleAverage", sTrans);
  results += PvlKeyword ("SampleMaximum", sMax);
  results += PvlKeyword ("SampleStandardDeviation", sDev);
  results += PvlKeyword ("LineMinimum", lMin);
  results += PvlKeyword ("LineAverage", lTrans);
  results += PvlKeyword ("LineMaximum", lMax);
  results += PvlKeyword ("LineStandardDeviation", lDev);
  Application::Log(results);

  Pvl arPvl = ar->RegistrationStatistics();

  for(int i = 0; i < arPvl.Groups(); i++) {
    Application::Log(arPvl.Group(i));
  }

  // add the auto registration information to print.prt
  PvlGroup autoRegTemplate = ar->RegTemplate(); 
  Application::Log(autoRegTemplate);

  // If none of the points registered, throw an error
  if (sStats.TotalPixels() < 1) {
    string msg = "Coreg was unable to register any points. Check your algorithm definition.";
    throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }

  // Don't need the cubes opened anymore
  trans.Close();
  match.Close();

  // If a cnet file was entered, write the ControlNet pvl to the file
  if (ui.WasEntered("CNETFILE")) {
    cn.Write(ui.GetFilename("CNETFILE"));
  }

  // If flatfile was entered, create the flatfile
  // The flatfile is comma seperated and can be imported into an excel
  // spreadsheet
  if (ui.WasEntered("FLATFILE")) {
    string fFile = Filename(ui.GetFilename("FLATFILE")).Expanded();
    ofstream os;
    os.open(fFile.c_str(),ios::out);
    os << "Sample,Line,TranslatedSample,TranslatedLine," <<
      "SampleDifference,LineDifference,GoodnessOfFit" << endl;
    for (int i=0; i<cn.Size(); i++) {
      ControlPoint cp = cn[i];
      if (cp.Ignore()) continue;
      ControlMeasure cmTrans = cp[0];
      ControlMeasure cmMatch = cp[1];
      os << cmTrans.Sample() << "," << cmTrans.Line() << ","
         << cmMatch.Sample() << "," << cmMatch.Line() << ","
         << cmTrans.SampleError() << "," << cmTrans.LineError() << ","
         << cmTrans.GoodnessOfFit() << endl;
    }
  }

  // If a TO parameter was specified, apply the average translation found to the
  // second input image
  if (ui.WasEntered("TO")) {
    if (ui.GetString("TRANSFORM") == "TRANSLATE") {
      string params = "from=" + ui.GetFilename("FROM") + " to=" +
        ui.GetFilename("TO") + " strans=" + iString(sTrans) + " ltrans="
        + iString(lTrans) + " interp=" + ui.GetString("INTERP");
      iApp->Exec("translate",params);
    }
    else {
      string params = "from=" + ui.GetFilename("FROM") + " to=" +
        ui.GetFilename("TO") + " cube=" + ui.GetFilename("MATCH") + " control=" + 
        ui.GetFilename("CNETFILE") + " interp=" + ui.GetString("INTERP") + 
        " degree=" + iString(ui.GetInteger("DEGREE"));
      iApp->Exec("warp",params);

    }
  }
}

//Helper function to output the regdeft file to log.
void helperButtonLog () {
  UserInterface &ui = Application::GetUserInterface();
  string file(ui.GetFilename("REGDEF"));
  Pvl p;
  p.Read(file);
  Application::GuiLog(p);
}
//...........end of helper function ........
