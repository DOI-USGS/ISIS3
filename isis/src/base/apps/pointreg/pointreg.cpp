#define GUIHELPERS

#include "Isis.h"

#include "UserInterface.h"
#include "AutoReg.h"
#include "AutoRegFactory.h"
#include "Camera.h"
#include "Chip.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "Cube.h"
#include "CubeManager.h"
#include "iException.h"
#include "iTime.h"
#include "Progress.h"
#include "SerialNumberList.h"

using namespace std;
using namespace Isis;

void PrintTemp ();

map <string,void*> GuiHelpers(){
  map <string,void*> helper;
  helper ["PrintTemp"] = (void*) PrintTemp;
  return helper;
}

void IsisMain() {

  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  //Determine which points/measures to register
  string registerPoints = ui.GetString("POINTS");
  string registerMeasures = ui.GetString("MEASURES");

  // Open the files list in a SerialNumberList for
  // reference by SerialNumber
  SerialNumberList files(ui.GetFilename("FILES"));

  // Create a ControlNet from the input file
  ControlNet inNet(ui.GetFilename("CNET"));

  if (inNet.Size() <= 0) {
    std::string msg = "Input control network [" + ui.GetFilename("CNET") + "] ";
    msg += "contains no points";
    throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
  }

  // Create an AutoReg from the template file
  Pvl pvl(ui.GetFilename("TEMPLATE"));
  AutoReg *ar = AutoRegFactory::Create(pvl);

  // Create the output ControlNet
  ControlNet outNet;
  outNet.SetType(inNet.Type());
  outNet.SetUserName(Application::UserName());
  outNet.SetDescription(inNet.Description());
  outNet.SetCreatedDate(iTime::CurrentLocalTime());
  outNet.SetTarget(inNet.Target());
  outNet.SetNetworkId(inNet.NetworkId());

  Progress progress;
  progress.SetText("Registering Points");
  progress.SetMaximumSteps(inNet.Size());
  progress.CheckStatus();

  int ignored=0, unmeasured=0, registered=0, unregistered=0, notintersected=0, validated=0;

  CubeManager cubeMgr;
  cubeMgr.SetNumOpenCubes(50);

  // Register the points and create a new
  // ControlNet containing the refined measurements
  for (int i=0; i<inNet.Size(); i++) {
    progress.CheckStatus();

    ControlPoint &inPoint = inNet[i];
    ControlPoint outPoint;
    outPoint.SetType(inPoint.Type());
    outPoint.SetId(inPoint.Id());
    outPoint.SetUniversalGround(inPoint.UniversalLatitude(), inPoint.UniversalLongitude(), inPoint.Radius());
    outPoint.SetHeld(inPoint.Held());
    outPoint.SetIgnore(inPoint.Ignore());

    // CHECK TO SEE IF THE CONTROL POINT SHOULD BE REGISTERED

    // "Ignore" point and we are not registering ignored
    if (inPoint.Ignore() && registerPoints == "NONIGNORED"){
      ignored++;
      // add "Ignored" to network only if indicated
      if (ui.GetBoolean("OUTPUTIGNORED")) {
        // only include appropriate control measures
        for (int j = 0; j < inPoint.Size(); j++) {
          if (inPoint[j].IsMeasured()){
            outPoint.Add(inPoint[j]); 
          } 
          else{
            unmeasured++;
            if (ui.GetBoolean("OUTPUTUNMEASURED")){
              outPoint.Add(inPoint[j]); 
            } 
          }
        }
        // only add this point if OUTPUTIGNORED
        outNet.Add(outPoint);
      }
      // go to next control point
      continue;
    }
    // Not "Ignore" point (i.e. "valid") and we are only registering "Ignored"
    else if (!inPoint.Ignore() && registerPoints == "IGNORED") {
      // add all "valid" points to network
      // only include appropriate control measures
      for (int j = 0; j < inPoint.Size(); j++) {
        if (inPoint[j].IsMeasured()){
          outPoint.Add(inPoint[j]); 
        } 
        else{
          unmeasured++;
          if (ui.GetBoolean("OUTPUTUNMEASURED")) {
            outPoint.Add(inPoint[j]);
          } 
        }
      }
      // add this point since it is not ignored
      outNet.Add(outPoint);
      // go to next control point
      continue;
    }
    // "Ignore" point or "valid" point to be registered
    else { 
      
      if (inPoint.Ignore()) { outPoint.SetIgnore(false); }
      
      ControlMeasure &patternCM = inPoint[inPoint.ReferenceIndex()];
      Cube &patternCube = *cubeMgr.OpenCube(files.Filename(patternCM.CubeSerialNumber()));
      
      ar->PatternChip()->TackCube(patternCM.Sample(), patternCM.Line());
      ar->PatternChip()->Load(patternCube);
      
      if (patternCM.IsValidated()) validated++;
      if (!patternCM.IsMeasured()) continue;
      if(!patternCM.IsReference()) {
        patternCM.SetReference(true);
        patternCM.SetChooserName("Application pointreg");
        patternCM.SetDateTime();
      }
      // add reference measure
      outPoint.Add(patternCM);
      
      // reset goodMeasureCount for this point before looping measures
      int goodMeasureCount = 0; 
      // Register all the unvalidated measurements
      for (int j = 0; j < inPoint.Size(); j++) {
        // don't register the reference, go to next measure
        if (j == inPoint.ReferenceIndex()){
          if (!inPoint[j].Ignore()) goodMeasureCount++;
          continue;
        }
        // if the measurement is valid, keep it as is and go to next measure
        if (inPoint[j].IsValidated()) {
          validated++;
          outPoint.Add(inPoint[j]);
          if (!inPoint[j].Ignore()) goodMeasureCount++;
          continue;
        }
        // if the point is unmeasured, add to output only if necessary and go to next measure
        if (!inPoint[j].IsMeasured()) {
          unmeasured++;
          if (ui.GetBoolean("OUTPUTUNMEASURED")) {
            outPoint.Add(inPoint[j]);
          }
          continue;
        }
        // if user chose not to reprocess successful measures, keep registered measure as is and go to next measure
        if (registerMeasures == "ESTIMATED" && inPoint[j].Type() != ControlMeasure::Estimated) {
          outPoint.Add(inPoint[j]);
          if (!inPoint[j].Ignore()) goodMeasureCount++;
          continue;
        }
      
        ControlMeasure searchCM = inPoint[j];
      
        // refresh pattern cube pointer to ensure it stays valid
        Cube &patternCube = *cubeMgr.OpenCube(files.Filename(patternCM.CubeSerialNumber()));
        Cube &searchCube = *cubeMgr.OpenCube(files.Filename(searchCM.CubeSerialNumber()));

        ar->SearchChip()->TackCube(searchCM.Sample(), searchCM.Line());
      
        try {
          ar->SearchChip()->Load(searchCube,*(ar->PatternChip()),patternCube);
      
          // If the measurements were correctly registered
          // Write them to the new ControlNet
          AutoReg::RegisterStatus res = ar->Register();
      
          double score1, score2;
          ar->ZScores(score1, score2);
          searchCM.SetZScores(score1, score2);

          if (res == AutoReg::Success) {
            // Check to make sure the newly calculated measure position is on the
            // surface of the planet
            Camera* cam = searchCube.Camera();
            bool foundLatLon = cam->SetImage(ar->CubeSample(), ar->CubeLine());

            if (foundLatLon) {
              registered++;
              searchCM.SetType(ControlMeasure::Automatic);
              searchCM.SetError(searchCM.Sample() - ar->CubeSample(), searchCM.Line() - ar->CubeLine());
              searchCM.SetCoordinate(ar->CubeSample(),ar->CubeLine());
              searchCM.SetGoodnessOfFit(ar->GoodnessOfFit());
              searchCM.SetChooserName("Application pointreg");
              searchCM.SetDateTime();
              searchCM.SetIgnore(false);
              outPoint.Add(searchCM);
              goodMeasureCount++;
            }
            else {
              notintersected++;
              searchCM.SetType(ControlMeasure::Estimated);
              searchCM.SetChooserName("Application pointreg");
              searchCM.SetDateTime();
              searchCM.SetIgnore(true);
              outPoint.Add(searchCM);
            }
          }
          // Else use the original marked as "Estimated"
          else {
            unregistered++;
            searchCM.SetType(ControlMeasure::Estimated);
      
            if(res == AutoReg::FitChipToleranceNotMet) {
              searchCM.SetError(inPoint[j].Sample() - ar->CubeSample(), inPoint[j].Line() - ar->CubeLine());
              searchCM.SetGoodnessOfFit(ar->GoodnessOfFit());
            }
            searchCM.SetChooserName("Application pointreg");
            searchCM.SetDateTime();
            searchCM.SetIgnore(true);
            outPoint.Add(searchCM);
          }
        } 
        catch (iException &e) {
          e.Clear();
          unregistered++;
          searchCM.SetType(ControlMeasure::Estimated);
          searchCM.SetChooserName("Application pointreg");
          searchCM.SetDateTime();
          searchCM.SetIgnore(true);
          outPoint.Add(searchCM);
        }
      }

      // Jeff Anderson put in this test (Dec 2, 2008) to allow for control 
      // points to be good so long as at least two measure could be 
      // registered. When a measure can't be registered to the reference then
      // that measure is set to be ignored where in the past the whole point
      // was ignored
      if (goodMeasureCount < 2) {
        if (!outPoint.Held() && outPoint.Type() != ControlPoint::Ground) {
          outPoint.SetIgnore(true);
        }
      }
      // Otherwise, ignore=false. This is already set at the beginning of the registration process

      // Check to see if the control point has now been assigned
      // to "ignore".  If not, add it to the network. If so, only 
      // add it to the output if the OUTPUTIGNORED parameter is selected
      // 2008-11-14 Jeannie Walldren
      if (!outPoint.Ignore()) {                             
        outNet.Add(outPoint);
      }
      else{                                              
        ignored++;                                   
        if (ui.GetBoolean("OUTPUTIGNORED")) outNet.Add(outPoint);
      }
    }
  }

  // If flatfile was entered, create the flatfile
  // The flatfile is comma seperated and can be imported into an excel
  // spreadsheet
  if (ui.WasEntered("FLATFILE")) {
    string fFile = Filename(ui.GetFilename("FLATFILE")).Expanded();
    ofstream os;
    os.open(fFile.c_str(),ios::out);
    os << "PointId,OriginalMeasurementSample,OriginalMeasurementLine," <<
      "RegisteredMeasurementSample,RegisteredMeasurementLine,SampleDifference," <<
      "LineDifference,ZScoreMin,ZScoreMax,GoodnessOfFit" << endl;
    os << NULL8 << endl;
    for (int i=0; i<outNet.Size(); i++) {
      // get point from output control net and its
      // corresponding point from input control net
      ControlPoint outPoint = outNet[i];
      ControlPoint *inPoint = inNet.Find(outPoint.Id());
      if (outPoint.Ignore()) continue;
      for (int i = 0; i<outPoint.Size();i++) {
        // get measure and find its corresponding measure from input net
        ControlMeasure cmTrans = outPoint[i];
        ControlMeasure cmOrig = (*inPoint)[cmTrans.CubeSerialNumber()];
        double inSamp = cmOrig.Sample();
        double inLine = cmOrig.Line();
        double outSamp = cmTrans.Sample();
        double outLine = cmTrans.Line();
        double sampErr = cmTrans.SampleError();
        double lineErr = cmTrans.LineError();
        double zScoreMin = cmTrans.GetZScoreMin();
        if (fabs(zScoreMin) <= DBL_EPSILON || zScoreMin == NULL8) zScoreMin = 0;
        double zScoreMax = cmTrans.GetZScoreMax();
        if (fabs(zScoreMax) <= DBL_EPSILON || zScoreMax == NULL8) zScoreMax = 0;
        double goodnessOfFit = cmTrans.GoodnessOfFit();
        if (fabs(goodnessOfFit) <= DBL_EPSILON || goodnessOfFit == NULL8) goodnessOfFit = 0;
        string pointId = outPoint.Id();
        
        os << pointId << "," << inSamp << ","
           << inLine << "," << outSamp << ","
           << outLine << "," << sampErr << "," 
           << lineErr << "," << zScoreMin << ","
           << zScoreMax << "," << goodnessOfFit << endl;
      }
    }
  }

  PvlGroup pLog("Points");
  pLog+=PvlKeyword("Ignored", ignored);
  Application::Log(pLog);

  PvlGroup mLog("Measures");
  mLog+=PvlKeyword("Validated", validated);
  mLog+=PvlKeyword("Registered", registered);
  mLog+=PvlKeyword("NotIntersected", notintersected);
  mLog+=PvlKeyword("Unregistered", unregistered);
  mLog+=PvlKeyword("Unmeasured", unmeasured);
  Application::Log(mLog);

  // Log Registration Statistics
  Pvl arPvl = ar->RegistrationStatistics();

  for(int i = 0; i < arPvl.Groups(); i++) {
    Application::Log(arPvl.Group(i));
  }

  // add the auto registration information to print.prt
  PvlGroup autoRegTemplate = ar->RegTemplate(); 
  Application::Log(autoRegTemplate); 

  outNet.Write(ui.GetFilename("TO"));

  delete ar;
}

// Helper function to print out template to session log
void PrintTemp() {
  UserInterface &ui = Application::GetUserInterface();

  // Get template pvl
  Pvl userTemp;
  userTemp.Read(ui.GetFilename("TEMPLATE"));

  //Write template file out to the log
  Isis::Application::GuiLog(userTemp);
}
