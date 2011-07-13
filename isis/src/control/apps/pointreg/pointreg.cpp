#define GUIHELPERS

#include "Isis.h"

#include "AutoReg.h"
#include "AutoRegFactory.h"
#include "Camera.h"
#include "Chip.h"
#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Cube.h"
#include "CubeManager.h"
#include "Pixel.h"
#include "Progress.h"
#include "SerialNumberList.h"
#include "UserInterface.h"
#include "iException.h"
#include "iTime.h"

using namespace std;
using namespace Isis;

void verifyCube(Cube & cube);
bool outputValue(ofstream &os, double value);
void printTemp();

map<string, void *> GuiHelpers() {
  map<string, void *> helper;
  helper["PrintTemp"] = (void *) printTemp;
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
  SerialNumberList files(ui.GetFilename("FROMLIST"));

  // Create the output ControlNet from the input file
  ControlNet outNet(ui.GetFilename("CNET"));

  if (outNet.GetNumPoints() <= 0) {
    std::string msg = "Control network [" + ui.GetFilename("CNET") + "] ";
    msg += "contains no points";
    throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
  }

  outNet.SetUserName(Application::UserName());

  // Create an AutoReg from the template file
  Pvl pvl(ui.GetFilename("DEFFILE"));
  AutoReg *ar = AutoRegFactory::Create(pvl);

  Progress progress;
  progress.SetText("Registering Points");
  progress.SetMaximumSteps(outNet.GetNumPoints());
  progress.CheckStatus();

  int ignored = 0;
  int locked = 0;
  int registered = 0;
  int notintersected = 0;
  int unregistered = 0;

  CubeManager cubeMgr;
  cubeMgr.SetNumOpenCubes(500);

  // Register the points and create a new
  // ControlNet containing the refined measurements
  int i = 0;
  while (i < outNet.GetNumPoints()) {
    progress.CheckStatus();

    ControlPoint * outPoint = outNet.GetPoint(i);

    // Establish whether or not we want to attempt to register this point.
    bool wantToRegister = true;
    if (outPoint->IsIgnored()) {
      if (registerPoints == "NONIGNORED") wantToRegister = false;
    }
    else {
      if (registerPoints == "IGNORED") wantToRegister = false;
    }

    // Check if this is a point we wish to disregard.
    if (!wantToRegister) {
      // Keep track of how many ignored points we didn't register.
      if (outPoint->IsIgnored()) {
        ignored++;

        // If the point is ignored and the user doesn't want them, delete it
        if (!ui.GetBoolean("OUTPUTIGNORED")) {
          outNet.DeletePoint(i);
          continue;
        }
      }
    }
    else {  // "Ignore" or "valid" point to be registered
      if (outPoint->IsIgnored()) {
        outPoint->SetIgnored(false);
      }

      ControlMeasure * patternCM = outPoint->GetRefMeasure();
      Cube &patternCube = *cubeMgr.OpenCube(
          files.Filename(patternCM->GetCubeSerialNumber()));

      ar->PatternChip()->TackCube(patternCM->GetSample(), patternCM->GetLine());
      ar->PatternChip()->Load(patternCube);

      if (patternCM->IsEditLocked()) {
        locked++;
      }

      if (outPoint->GetRefMeasure() != patternCM) {
        outPoint->SetRefMeasure(patternCM);
      }

      // reset goodMeasureCount for this point before looping measures
      int goodMeasureCount = 0;

      // Register all the unlocked measurements
      int j = 0;
      while (j < outPoint->GetNumMeasures()) {
        ControlMeasure * measure = outPoint->GetMeasure(j);

        if (j == outPoint->IndexOfRefMeasure()) {
          // don't register the reference, go to next measure
          if (!measure->IsIgnored()) goodMeasureCount++;
        }
        else if (measure->IsEditLocked()) {
          // if the measurement is locked, keep it as is and go to next measure
          locked++;
          if (!measure->IsIgnored()) goodMeasureCount++;
        }
        else if (registerMeasures == "CANDIDATES" && measure->IsMeasured()) {
          // if user chose not to reprocess successful measures, keep registered
          // measure as is and go to next measure
          if (!measure->IsIgnored()) goodMeasureCount++;
        }
        else {

          // refresh pattern cube pointer to ensure it stays valid
          Cube &patternCube = *cubeMgr.OpenCube(files.Filename(
                patternCM->GetCubeSerialNumber()));
          Cube &searchCube = *cubeMgr.OpenCube(files.Filename(
                measure->GetCubeSerialNumber()));

          ar->SearchChip()->TackCube(measure->GetSample(), measure->GetLine());
          
          verifyCube(patternCube);
          verifyCube(searchCube);

          try {

            ar->SearchChip()->Load(searchCube, *(ar->PatternChip()), patternCube);

            // If the measurements were correctly registered
            // Write them to the new ControlNet
            AutoReg::RegisterStatus res = ar->Register();
            searchCube.clearIoCache();
            patternCube.clearIoCache();

            double score1, score2;
            ar->ZScores(score1, score2);

            // Set the minimum and maximum z-score values for the measure
            measure->SetLogData(ControlMeasureLogData(
                  ControlMeasureLogData::MinimumPixelZScore, score1));
            measure->SetLogData(ControlMeasureLogData(
                  ControlMeasureLogData::MaximumPixelZScore, score2));

            if (ar->Success()) {
              // Check to make sure the newly calculated measure position is on
              // the surface of the planet
              Camera *cam = searchCube.getCamera();
              bool foundLatLon = cam->SetImage(ar->CubeSample(), ar->CubeLine());

              if (foundLatLon) {
                registered++;

                if (res == AutoReg::SuccessSubPixel)
                  measure->SetType(ControlMeasure::RegisteredSubPixel);
                else
                  measure->SetType(ControlMeasure::RegisteredPixel);

                double sampleShift = measure->GetSample() - ar->CubeSample();
                double lineShift = measure->GetLine() - ar->CubeLine();

                measure->SetLogData(ControlMeasureLogData(
                      ControlMeasureLogData::SampleShift, sampleShift));
                measure->SetLogData(ControlMeasureLogData(
                      ControlMeasureLogData::LineShift, lineShift));

                measure->SetLogData(ControlMeasureLogData(
                      ControlMeasureLogData::GoodnessOfFit,
                      ar->GoodnessOfFit()));

                measure->SetCoordinate(ar->CubeSample(), ar->CubeLine());
                measure->SetIgnored(false);
                goodMeasureCount++;
              }
              else {
                notintersected++;

                if (ui.GetBoolean("OUTPUTFAILED")) {
                  measure->SetType(ControlMeasure::Candidate);
                  measure->SetIgnored(true);
                }
                else {
                  outPoint->Delete(j);
                  continue;
                }
              }
            }
            // Else use the original marked as "Candidate"
            else {
              unregistered++;

              if (ui.GetBoolean("OUTPUTFAILED")) {
                measure->SetType(ControlMeasure::Candidate);

                if (res == AutoReg::FitChipToleranceNotMet) {
                  double sampleShift = measure->GetSample() - ar->CubeSample();
                  double lineShift = measure->GetLine() - ar->CubeLine();

                  measure->SetLogData(ControlMeasureLogData(
                        ControlMeasureLogData::SampleShift, sampleShift));
                  measure->SetLogData(ControlMeasureLogData(
                        ControlMeasureLogData::LineShift, lineShift));
                  measure->SetLogData(ControlMeasureLogData(
                        ControlMeasureLogData::GoodnessOfFit,
                        ar->GoodnessOfFit()));
                }
                measure->SetIgnored(true);
              }
              else {
                outPoint->Delete(j);
                continue;
              }
            }
          }
          catch (iException &e) {
            e.Clear();
            unregistered++;

            if (ui.GetBoolean("OUTPUTFAILED")) {
              measure->SetType(ControlMeasure::Candidate);
              measure->SetIgnored(true);
            }
            else {
              outPoint->Delete(j);
              continue;
            }
          }
        }

        // If we made it here (without continuing on to the next measure),
        // then the measure wasn't deleted and we should therefore increment
        // the index of the measure we're looking at.
        j++;
      }

      // Jeff Anderson put in this test (Dec 2, 2008) to allow for control
      // points to be good so long as at least two measure could be
      // registered. When a measure can't be registered to the reference then
      // that measure is set to be ignored where in the past the whole point
      // was ignored
      if (goodMeasureCount < 2 && outPoint->GetType() != ControlPoint::Fixed) {
        outPoint->SetIgnored(true);
      }

      // Otherwise, ignore=false. This is already set at the beginning of the
      // registration process

      // Check to see if the control point has now been assigned
      // to "ignore".  If not, add it to the network. If so, only
      // add it to the output if the OUTPUTIGNORED parameter is selected
      // 2008-11-14 Jeannie Walldren
      if (outPoint->IsIgnored()) {
        ignored++;
        if (!ui.GetBoolean("OUTPUTIGNORED")) {
          outNet.DeletePoint(i);
          continue;
        }
      }
    }
    // The point wasn't deleted, so the network size is the same and we should
    // increment our index.
    i++;
  }

  // If flatfile was entered, create the flatfile
  // The flatfile is comma seperated and can be imported into an excel
  // spreadsheet
  if (ui.WasEntered("FLATFILE")) {
    string fFile = Filename(ui.GetFilename("FLATFILE")).Expanded();

    ofstream os;
    os.open(fFile.c_str(), ios::out);
    os <<
       "PointId,MeasureType,Reference,EditLock,Ignore,Registered," <<
       "OriginalMeasurementSample,OriginalMeasurementLine," <<
       "RegisteredMeasurementSample,RegisteredMeasurementLine,SampleShift," <<
       "LineShift,PixelShift,ZScoreMin,ZScoreMax,GoodnessOfFit" << endl;
    os << Null << endl;

    // Create a ControlNet from the original input file
    ControlNet inNet(ui.GetFilename("CNET"));

    for (int i = 0; i < outNet.GetNumPoints(); i++) {

      // get point from output control net and its
      // corresponding point from input control net
      const ControlPoint * outPoint = outNet.GetPoint(i);
      QString outPointId = outPoint->GetId();
      const ControlPoint * inPoint = inNet.GetPoint(outPointId);

      if (!outPoint->IsIgnored()) {
        for (int i = 0; i < outPoint->GetNumMeasures(); i++) {

          // Get measure and find its corresponding measure from input net
          const ControlMeasure * cmTrans = outPoint->GetMeasure(i);
          const ControlMeasure * cmOrig =
              inPoint->GetMeasure((QString) cmTrans->GetCubeSerialNumber());

          string pointId = outPoint->GetId();

          string measureType = cmTrans->MeasureTypeToString(
              cmTrans->GetType());
          string reference = outPoint->GetRefMeasure() == cmTrans ?
              "true" : "false";
          string editLock = cmTrans->IsEditLocked() ? "true" : "false";
          string ignore = cmTrans->IsIgnored() ? "true" : "false";
          string registered =
              !cmOrig->IsRegistered() && cmTrans->IsRegistered() ?
              "true" : "false";

          double inSamp = cmOrig->GetSample();
          double inLine = cmOrig->GetLine();

          double outSamp = cmTrans->GetSample();
          double outLine = cmTrans->GetLine();

          os <<
              pointId << "," <<
              measureType << "," <<
              reference << "," <<
              editLock << "," <<
              ignore << "," <<
              registered << "," <<
              inSamp << "," <<
              inLine << "," <<
              outSamp << "," <<
              outLine;

          double sampleShift = cmTrans->GetLogData(
              ControlMeasureLogData::SampleShift).GetNumericalValue();
          double lineShift = cmTrans->GetLogData(
              ControlMeasureLogData::LineShift).GetNumericalValue();

          bool hasSampleShift = outputValue(os, sampleShift);
          bool hasLineShift = outputValue(os, lineShift);

          if (hasSampleShift && hasLineShift)
            outputValue(os, sqrt(pow(sampleShift, 2) + pow(lineShift, 2)));
          else
            os << ",";

          double zScoreMin = cmTrans->GetLogData(
              ControlMeasureLogData::MinimumPixelZScore).GetNumericalValue();
          double zScoreMax = cmTrans->GetLogData(
              ControlMeasureLogData::MaximumPixelZScore).GetNumericalValue();
          double goodnessOfFit = cmTrans->GetLogData(
              ControlMeasureLogData::GoodnessOfFit).GetNumericalValue();

          outputValue(os, zScoreMin);
          outputValue(os, zScoreMax);
          outputValue(os, goodnessOfFit);

          os << endl;
        }
      }
    }
  }

  PvlGroup pLog("Points");
  pLog += PvlKeyword("Total", outNet.GetNumPoints());
  pLog += PvlKeyword("Ignored", ignored);
  // TODO output the z-scores, goodness of fit, eccentricity for each point
  Application::Log(pLog);

  PvlGroup mLog("Measures");
  mLog += PvlKeyword("Locked", locked);
  mLog += PvlKeyword("Registered", registered);
  mLog += PvlKeyword("NotIntersected", notintersected);
  mLog += PvlKeyword("Unregistered", unregistered);
  Application::Log(mLog);

  // Log Registration Statistics
  Pvl arPvl = ar->RegistrationStatistics();

  for (int i = 0; i < arPvl.Groups(); i++) {
    Application::Log(arPvl.Group(i));
  }

  // add the auto registration information to print.prt
  PvlGroup autoRegTemplate = ar->RegTemplate();
  Application::Log(autoRegTemplate);

  outNet.Write(ui.GetFilename("ONET"));

  delete ar;
}

// Verify a cube has either a Camera or a Projection, throw an exception if not
void verifyCube(Cube & cube) {
  try {
    cube.getCamera();
  }
  catch (iException &e) {
    cube.getProjection();
    e.Clear();
  }
}


bool outputValue(ofstream &os, double value) {
  os << ",";

  if (fabs(value) > DBL_EPSILON && value != Null) {
    os << value;
    return true;
  }

  return false;
}


// Helper function to print out template to session log
void printTemp() {
  UserInterface &ui = Application::GetUserInterface();

  // Get template pvl
  Pvl userTemp;
  userTemp.Read(ui.GetFilename("DEFFILE"));

  //Write template file out to the log
  Isis::Application::GuiLog(userTemp);
}

