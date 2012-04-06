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
#include "IException.h"
#include "iTime.h"

using namespace std;
using namespace Isis;


AutoReg *ar;
AutoReg *validator;
CubeManager *cubeMgr;
SerialNumberList *files;
QList<QString> *falsePositives;

int ignored;
int locked;
int registered;
int notintersected;
int unregistered;

bool logFalsePositives;
bool revertFalsePositives;
int expansion;


class Validation {
  public:
    Validation(string test, ControlMeasure *held, ControlMeasure *registered,
        double tolerance) {

      m_test = test;
      m_pointId = registered->Parent()->GetId();
      m_heldId = Filename(
          files->Filename(held->GetCubeSerialNumber())).Basename();
      m_registeredId = Filename(
          files->Filename(registered->GetCubeSerialNumber())).Basename();

      m_aprioriSample = registered->GetSample();
      m_aprioriLine = registered->GetLine();
      m_shiftSample = 0.0;
      m_shiftLine = 0.0;

      m_shift = 0.0;
      m_shiftTolerance = tolerance;
      m_valid = false;
    }

    ~Validation() {}

    bool validated() {
      return m_valid;
    }

    void compare(double shiftSample, double shiftLine) {
      m_shiftSample = shiftSample;
      m_shiftLine = shiftLine;

      double sampleShift = shiftSample - m_aprioriSample;
      double lineShift = shiftLine - m_aprioriLine;

      m_shift = sqrt(pow(sampleShift, 2) + pow(lineShift, 2));
      m_valid = m_shift <= m_shiftTolerance;
    }

    static string getHeader() {
      return "Test,PointID,HeldID,RegisteredID,"
          "Sample,Line,ShiftedSample,ShiftedLine,"
          "Shift,ShiftTolerance";
    }

    string toString() {
      stringstream stream;
      stream <<
        m_test << "," << m_pointId << "," <<
        m_heldId << "," << m_registeredId << "," <<
        m_aprioriSample << "," << m_aprioriLine << "," <<
        m_shiftSample << "," << m_shiftLine << "," <<
        m_shift << "," << m_shiftTolerance;
      return stream.str();
    }

  private:
    string m_test;
    string m_pointId;
    string m_heldId;
    string m_registeredId;

    double m_aprioriSample;
    double m_aprioriLine;
    double m_shiftSample;
    double m_shiftLine;

    double m_shift;
    double m_shiftTolerance;

    bool m_valid;
};


void registerPoint(ControlPoint *outPoint, ControlMeasure *patternCM,
    string registerMeasures, bool outputFailed);
void validatePoint(ControlPoint *point, ControlMeasure *reference,
    double shiftTolerance);
Validation backRegister(ControlMeasure *measure, ControlMeasure *reference,
    double shiftTolerance);

double getResolution(Cube &cube, ControlMeasure &measure);
void verifyCube(Cube & cube);
bool outputValue(ofstream &os, double value);
int calcGoodMeasureCount(const ControlPoint *point);
void printTemp();

map<string, void *> GuiHelpers() {
  map<string, void *> helper;
  helper["PrintTemp"] = (void *) printTemp;
  return helper;
}


void IsisMain() {
  // Initialize variables
  ar = NULL;
  validator = NULL;
  cubeMgr = NULL;
  files = NULL;
  falsePositives = NULL;

  ignored = 0;
  locked = 0;
  registered = 0;
  notintersected = 0;
  unregistered = 0;

  logFalsePositives = false;
  revertFalsePositives = false;

  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  if (ui.WasEntered("FALSEPOSITIVES")) {
    logFalsePositives = true;
    falsePositives = new QList<QString>;
  }

  // Determine which points/measures to register
  string registerPoints = ui.GetString("POINTS");
  string registerMeasures = ui.GetString("MEASURES");

  bool outputFailed = ui.GetBoolean("OUTPUTFAILED");
  bool outputIgnored = ui.GetBoolean("OUTPUTIGNORED");

  // Open the files list in a SerialNumberList for
  // reference by SerialNumber
  files = new SerialNumberList(ui.GetFilename("FROMLIST"));

  // Create the output ControlNet from the input file
  ControlNet outNet(ui.GetFilename("CNET"));

  if (outNet.GetNumPoints() <= 0) {
    std::string msg = "Control network [" + ui.GetFilename("CNET") + "] ";
    msg += "contains no points";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  outNet.SetUserName(Application::UserName());

  // Create an AutoReg from the template file
  Pvl pvl(ui.GetFilename("DEFFILE"));
  ar = AutoRegFactory::Create(pvl);

  Progress progress;
  progress.SetText("Registering Points");
  progress.SetMaximumSteps(outNet.GetNumPoints());
  progress.CheckStatus();

  cubeMgr = new CubeManager;
  cubeMgr->SetNumOpenCubes(500);

  string validate = ui.GetString("VALIDATE");
  if (validate != "SKIP") {
    validator = AutoRegFactory::Create(pvl);

    validator->SetTolerance(validator->MostLenientTolerance());
    validator->SetPatternZScoreMinimum(DBL_MIN);
    validator->SetPatternValidPercent(DBL_MIN);
    validator->SetSubsearchValidPercent(DBL_MIN);

    validator->SetSurfaceModelDistanceTolerance(validator->WindowSize());
    validator->SetEccentricityTesting(false);
    validator->SetResidualTesting(false);

    expansion = ui.WasEntered("SEARCH") ?
      ui.GetInteger("SEARCH") : validator->WindowSize();
    expansion *= 2;

    int patternSamples = validator->PatternChip()->Samples();
    int patternLines = validator->PatternChip()->Lines();
    validator->SearchChip()->SetSize(
        patternSamples + expansion, patternLines + expansion);

    revertFalsePositives = ui.GetBoolean("REVERT");
  }

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
        if (!outputIgnored) {
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

      // In case this is an implicit reference, make it explicit since we'll be
      // registering measures to it
      outPoint->SetRefMeasure(patternCM);

      if (validate != "ONLY") {
        registerPoint(outPoint, patternCM, registerMeasures, outputFailed);
      }
      if (validate != "SKIP") {
        validatePoint(outPoint, patternCM, ui.GetDouble("SHIFT"));
      }

      // Check to see if the control point has now been assigned
      // to "ignore".  If not, add it to the network. If so, only
      // add it to the output if the OUTPUTIGNORED parameter is selected
      // 2008-11-14 Jeannie Walldren
      if (outPoint->IsIgnored()) {
        ignored++;
        if (!outputIgnored) {
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
      "PointId,Filename,MeasureType,Reference,EditLock,Ignore,Registered," <<
      "OriginalMeasurementSample,OriginalMeasurementLine," <<
      "RegisteredMeasurementSample,RegisteredMeasurementLine,SampleShift," <<
      "LineShift,PixelShift,ZScoreMin,ZScoreMax,WholePixelCorrelation," <<
      "SubPixelCorrelation,EccentricityRatio,AverageResidual" << endl;
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

          string fullName = files->Filename(cmTrans->GetCubeSerialNumber());
          string filename = Filename(fullName).Basename();

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
            filename << "," <<
            measureType << "," <<
            reference << "," <<
            editLock << "," <<
            ignore << "," <<
            registered << "," <<
            inSamp << "," <<
            inLine << "," <<
            outSamp << "," <<
            outLine;

          double sampleShift = cmTrans->GetSampleShift();
          double lineShift = cmTrans->GetLineShift();
          double pixelShift = cmTrans->GetPixelShift();

          outputValue(os, sampleShift);
          outputValue(os, lineShift);
          outputValue(os, pixelShift);

          double zScoreMin = cmTrans->GetLogData(
              ControlMeasureLogData::MinimumPixelZScore).GetNumericalValue();
          double zScoreMax = cmTrans->GetLogData(
              ControlMeasureLogData::MaximumPixelZScore).GetNumericalValue();
          double wholePixelCorrelation = cmTrans->GetLogData(
              ControlMeasureLogData::WholePixelCorrelation).GetNumericalValue();
          double subPixelCorrelation = cmTrans->GetLogData(
              ControlMeasureLogData::SubPixelCorrelation).GetNumericalValue();
          double eccentricity = cmTrans->GetLogData(
              ControlMeasureLogData::Eccentricity).GetNumericalValue();
          double averageResidual = cmTrans->GetLogData(
              ControlMeasureLogData::AverageResidual).GetNumericalValue();

          double eccentricityRatio = sqrt(
              1.0 / (1.0 - eccentricity * eccentricity));

          outputValue(os, zScoreMin);
          outputValue(os, zScoreMax);
          outputValue(os, wholePixelCorrelation);
          outputValue(os, subPixelCorrelation);
          outputValue(os, eccentricityRatio);
          outputValue(os, averageResidual);

          os << endl;
        }
      }
    }
  }

  if (logFalsePositives) {
    string filename = Filename(ui.GetFilename("FALSEPOSITIVES")).Expanded();
    ofstream os;
    os.open(filename.c_str(), ios::out);

    os << Validation::getHeader() << endl;
    for (int i = 0; i < falsePositives->size(); i++) {
      os << (*falsePositives)[i].toStdString() << endl;
    }

    os.close();
  }

  PvlGroup pLog("Points");
  pLog += PvlKeyword("Total", outNet.GetNumPoints());
  pLog += PvlKeyword("Ignored", ignored);
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

  if (validator) {
    PvlGroup validationGroup("ValidationStatistics");

    Pvl validationPvl = validator->RegistrationStatistics();
    for (int g = 0; g < validationPvl.Groups(); g++) {
      PvlGroup &group = validationPvl.Group(g);
      if (group.Keywords() > 0) {
        group[0].AddComment(group.Name());
        for (int k = 0; k < group.Keywords(); k++)
          validationGroup.AddKeyword(group[k]);
      }
    }

    Application::Log(validationGroup);

    PvlGroup validationTemplate = validator->UpdatedTemplate();
    validationTemplate.SetName("ValidationTemplate");
    Application::Log(validationTemplate);
  }

  outNet.Write(ui.GetFilename("ONET"));

  delete ar;
  ar = NULL;

  delete validator;
  validator = NULL;

  delete cubeMgr;
  cubeMgr = NULL;

  delete files;
  files = NULL;

  delete falsePositives;
  falsePositives = NULL;
}


void registerPoint(ControlPoint *outPoint, ControlMeasure *patternCM,
    string registerMeasures, bool outputFailed) {

  Cube &patternCube = *cubeMgr->OpenCube(
      files->Filename(patternCM->GetCubeSerialNumber()));

  ar->PatternChip()->TackCube(patternCM->GetSample(), patternCM->GetLine());
  ar->PatternChip()->Load(patternCube);

  if (patternCM->IsEditLocked()) {
    locked++;
  }

  if (outPoint->GetRefMeasure() != patternCM) {
    outPoint->SetRefMeasure(patternCM);
  }

  // Register all the unlocked measurements
  int j = 0;
  while (j < outPoint->GetNumMeasures()) {
    if (j != outPoint->IndexOfRefMeasure()) {

      ControlMeasure * measure = outPoint->GetMeasure(j);
      if (measure->IsEditLocked()) {
        // If the measurement is locked, keep it as is and go to next measure
        locked++;
      }
      else if (!measure->IsMeasured() || registerMeasures != "CANDIDATES") {

        // refresh pattern cube pointer to ensure it stays valid
        Cube &patternCube = *cubeMgr->OpenCube(files->Filename(
              patternCM->GetCubeSerialNumber()));
        Cube &searchCube = *cubeMgr->OpenCube(files->Filename(
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

              if (res == AutoReg::SuccessSubPixel) {
                measure->SetType(ControlMeasure::RegisteredSubPixel);

                measure->SetLogData(ControlMeasureLogData(
                      ControlMeasureLogData::SubPixelCorrelation,
                      ar->SubPixelCorrelation()));
                if (ar->Eccentricity() != Null)
                  measure->SetLogData(ControlMeasureLogData(
                        ControlMeasureLogData::Eccentricity,
                        ar->Eccentricity()));
                if (ar->AverageResidual() != Null)
                  measure->SetLogData(ControlMeasureLogData(
                        ControlMeasureLogData::AverageResidual,
                        ar->AverageResidual()));
              }
              else {
                measure->SetType(ControlMeasure::RegisteredPixel);
              }

              measure->SetLogData(ControlMeasureLogData(
                    ControlMeasureLogData::GoodnessOfFit,
                    ar->GoodnessOfFit()));
              measure->SetLogData(ControlMeasureLogData(
                    ControlMeasureLogData::WholePixelCorrelation,
                    ar->WholePixelCorrelation()));

              measure->SetAprioriSample(measure->GetSample());
              measure->SetAprioriLine(measure->GetLine());
              measure->SetCoordinate(ar->CubeSample(), ar->CubeLine());
              measure->SetIgnored(false);

              // We successfully registered the current measure to the
              // reference, and since we set the current measure to be
              // unignored, it follows that its reference should also be made
              // unignored.
              patternCM->SetIgnored(false);
            }
            else {
              notintersected++;

              if (outputFailed) {
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

            if (outputFailed) {
              measure->SetType(ControlMeasure::Candidate);

              if (res == AutoReg::FitChipToleranceNotMet) {
                measure->SetLogData(ControlMeasureLogData(
                      ControlMeasureLogData::GoodnessOfFit,
                      ar->GoodnessOfFit()));
                measure->SetLogData(ControlMeasureLogData(
                      ControlMeasureLogData::WholePixelCorrelation,
                      ar->WholePixelCorrelation()));
              }
              measure->SetIgnored(true);
            }
            else {
              outPoint->Delete(j);
              continue;
            }
          }
        }
        catch (IException &e) {
          unregistered++;

          if (outputFailed) {
            measure->SetType(ControlMeasure::Candidate);
            measure->SetIgnored(true);
          }
          else {
            outPoint->Delete(j);
            continue;
          }
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
  if (calcGoodMeasureCount(outPoint) < 2 &&
      outPoint->GetType() != ControlPoint::Fixed) {
    outPoint->SetIgnored(true);
  }

  // Otherwise, ignore=false. This is already set at the beginning of the
  // registration process
}


void validatePoint(ControlPoint *point, ControlMeasure *reference,
    double shiftTolerance) {

  for (int i = 0; i < point->GetNumMeasures(); i++) {
    if (i != point->IndexOfRefMeasure()) {
      ControlMeasure *measure = point->GetMeasure(i);
      if (measure->IsMeasured() && !measure->IsEditLocked()) {
        Validation validation = backRegister(
            reference, measure, shiftTolerance);

        if (!validation.validated()) {
          if (revertFalsePositives) {
            measure->SetType(ControlMeasure::Candidate);
            measure->SetCoordinate(
                measure->GetAprioriSample(), measure->GetAprioriLine());
            measure->SetIgnored(true);
            // TODO remove log data here
          }

          if (logFalsePositives) {
            falsePositives->append(
                QString::fromStdString(validation.toString()));
          }
        }
      }
    }
  }
}


Validation backRegister(ControlMeasure *reference, ControlMeasure *measure,
    double shiftTolerance) {

  Cube &patternCube = *cubeMgr->OpenCube(files->Filename(
        measure->GetCubeSerialNumber()));
  Cube &searchCube = *cubeMgr->OpenCube(files->Filename(
        reference->GetCubeSerialNumber()));

  /* TODO remove
  int patternSamples = validator->PatternChip()->Samples();
  int patternLines = validator->PatternChip()->Lines();

  double patternResolution = getResolution(patternCube, *measure);
  double searchResolution = getResolution(searchCube, *reference);

  double tolerance = shiftTolerance;
  if (patternResolution > searchResolution) {
    double ratio = patternResolution / searchResolution;
    tolerance *= ratio;

    int adjustedSamples = (int) patternSamples * ratio;
    int adjustedLines = (int) patternLines * ratio;

    if (adjustedSamples % 2 == 0) adjustedSamples++;
    if (adjustedLines % 2 == 0) adjustedLines++;

    validator->SearchChip()->SetSize(
        adjustedSamples + expansion, adjustedLines + expansion);
    validator->PatternChip()->SetSize(adjustedSamples, adjustedLines);
  }
  */

  validator->SearchChip()->TackCube(
      reference->GetSample(), reference->GetLine());
  validator->PatternChip()->TackCube(measure->GetSample(), measure->GetLine());
  validator->PatternChip()->Load(patternCube);

  /* TODO remove
   double validPercent = validator->PatternValidPercent();
   Statistics *stats = validator->PatternChip()->Statistics();
   double validRatio = (double) stats->ValidPixels() / stats->TotalPixels();
   if (validRatio * 100.0 > validPercent) {
     double newValidPercent = validPercent * validRatio;
     validator->SetPatternValidPercent(newValidPercent);
   }
   */

  verifyCube(patternCube);
  verifyCube(searchCube);

  Validation validation(
      "Back-Registration", measure, reference, shiftTolerance);
  try {
    validator->SearchChip()->Load(
        searchCube, *(validator->PatternChip()), patternCube);

    // If the measurements were correctly registered
    // Write them to the new ControlNet
    validator->Register();
    searchCube.clearIoCache();
    patternCube.clearIoCache();

    if (validator->Success()) {
      // Check to make sure the newly calculated measure position is on
      // the surface of the planet
      Camera *cam = searchCube.getCamera();
      bool foundLatLon = cam->SetImage(
          validator->CubeSample(), validator->CubeLine());

      if (foundLatLon) {
        validation.compare(
            validator->CubeSample(), validator->CubeLine());
      }
    }
  }
  catch (IException &e) {
  }

  /* TODO remove
  validator->PatternChip()->SetSize(patternSamples, patternLines);
  validator->SearchChip()->SetSize(
      patternSamples + expansion, patternLines + expansion);
  validator->SetPatternValidPercent(validPercent);
  */

  return validation;
}


double getResolution(Cube &cube, ControlMeasure &measure) {
  // TODO retrieve for projection
  Camera *camera = cube.getCamera();
  camera->SetImage(measure.GetSample(), measure.GetLine());
  return camera->PixelResolution();
}


// Verify a cube has either a Camera or a Projection, throw an exception if not
void verifyCube(Cube & cube) {
  try {
    cube.getCamera();
  }
  catch (IException &camError) {
    try {
      cube.getProjection();
    }
    catch (IException &projError) {
      projError.append(camError);
      throw projError;
    }
  }
}


bool outputValue(ofstream &os, double value) {
  os << ",";

  if (fabs(value) > DBL_EPSILON && value != Null) {
    os << value;
    return true;
  }
  else {
    os << "NA";
    return false;
  }
}


int calcGoodMeasureCount(const ControlPoint *point) {
  int goodMeasureCount = 0;
  for (int i = 0; i < point->GetNumMeasures(); i++) {
    const ControlMeasure *measure = point->GetMeasure(i);
    if (!measure->IsIgnored()) goodMeasureCount++;
  }

  return goodMeasureCount;
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

