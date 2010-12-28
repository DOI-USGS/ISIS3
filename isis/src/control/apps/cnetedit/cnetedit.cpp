#define GUIHELPERS

#include "Isis.h"

#include <QList>
#include <QMap>

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "ControlNet.h"
#include "ControlNetValidMeasure.h"
#include "ControlPointList.h"
#include "Cube.h"
#include "iException.h"
#include "MeasureValidationResults.h"
#include "Progress.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SerialNumberList.h"

using namespace std;
using namespace Isis;

void IgnorePoint(ControlNet & cnet, int cp, string cause);
void IgnoreMeasure(ControlNet & cnet, int cp, int cm, string cause);
void DeletePoint(ControlNet & cnet, int cp);
void DeleteMeasure(ControlNet & cnet, int cp, int cm);

void PopulateLog(ControlNet & cnet);
void ProcessControlPoints(std::string psFileName, ControlNet &pcCnet);
void ProcessControlMeasures(std::string psFileName, ControlNet &pcCnet);
void CheckAllMeasureValidity(ControlNet & cnet, std::string cubeList);

MeasureValidationResults ValidateMeasure(const ControlMeasure & curMeasure, std::string filename);

void PrintTemp();

map <string, void *> GuiHelpers() {
  map <string, void *> helper;
  helper ["PrintTemp"] = (void *) PrintTemp;
  return helper;
}

int numPointsDeleted;
int numMeasuresDeleted;

bool deleteIgnored;
bool preservePoints;

bool keepLog;
PvlObject * ignoredPoints;
QMap<string, PvlGroup> * ignoredMeasures;

ControlNetValidMeasure * validator;


// Main program
void IsisMain() {

  // Reset the counts of points and measures deleted
  numPointsDeleted = 0;
  numMeasuresDeleted = 0;

  // Interface for getting user parameters
  UserInterface &ui = Application::GetUserInterface();

  // Get global user parameters
  deleteIgnored = ui.GetBoolean("DELETE");
  preservePoints = ui.GetBoolean("PRESERVE");

  // Data needed to keep track of ignored/deleted points and measures
  keepLog = ui.WasEntered("LOG");
  ignoredPoints = NULL;
  ignoredMeasures = NULL;

  // If the user wants to keep a log, go ahead and populate it with all the
  // existing ignored points and measures
  ControlNet cnet(ui.GetFilename("CNET"));
  if(keepLog)
    PopulateLog(cnet);

  /*
   * As a first pass, just try and delete anything that's already ignored
   * in the Control Network, if the user wants to delete ignored points and
   * measures.  Originally, this check was performed last, only if the user
   * didn't specify any other deletion methods.  However, performing this
   * check first will actually improve the running time in cases where there
   * are already ignored points and measures in the input network.  The added
   * cost of doing this check here actually doesn't add to the running time at
   * all, because these same checks would need to have been done later
   * regardless.
   */
  if(deleteIgnored) {
    for(int cp = cnet.Size() - 1; cp >= 0; cp--) {
      if(cnet[cp].IsIgnored()) {
        DeletePoint(cnet, cp);
      }
      else {
        for(int cm = cnet[cp].Size() - 1; cm >= 0; cm--) {
          if(cnet[cp][cm].IsIgnored()) {
            if(cm == cnet[cp].GetReferenceIndex()) {
              // If the reference is ignored, the point must ignored too
              IgnorePoint(cnet, cp, "Reference measure ignored");
            }
            else {
              // Can't delete the reference without deleting the whole point
              DeleteMeasure(cnet, cp, cm);
            }
          }
        }

        // Check if the number of measures in the point is zero or there are too
        // few measures in the point and we don't want to preserve them.
        if(((cnet[cp].Size() < 2 && !preservePoints) && cnet[cp].GetType() != ControlPoint::Ground)
            || cnet[cp].Size() == 0 || cnet[cp].IsIgnored()) {
          DeletePoint(cnet, cp);
        }
      }
    }
  }

  //List has Points Ids
  if(ui.WasEntered("POINTLIST")) {
    std::string pointlistFilename = ui.GetFilename("POINTLIST");
    ProcessControlPoints(pointlistFilename, cnet);
  }

  //List has Cube file names
  if(ui.WasEntered("CUBELIST")) {
    std::string ignorelistFilename = ui.GetFilename("CUBELIST");
    ProcessControlMeasures(ignorelistFilename, cnet);
  }

  // Perform validity check
  if(ui.GetBoolean("CHECKVALID")) {
    validator = NULL;

    // Construct the validator from the user-specified definition file
    Pvl defFile(ui.GetFilename("DEFFILE"));
    validator = new ControlNetValidMeasure(&defFile);

    // User also provided a list of all serial numbers corresponding to every
    // cube in the control network
    std::string cubeList = ui.GetFilename("FROMLIST");
    CheckAllMeasureValidity(cnet, cubeList);

    // Delete the validator
    if(validator != NULL) {
      delete validator;
      validator = NULL;
    }

    // Log the DEFFILE to the print file
    Application::Log(defFile.FindGroup("ValidMeasure", Pvl::Traverse));
  }

  // Log statistics
  if(keepLog) {
    Pvl outputLog;
    outputLog.AddKeyword(PvlKeyword("PointsDeleted", numPointsDeleted));
    outputLog.AddKeyword(PvlKeyword("MeasuresDeleted", numMeasuresDeleted));

    // Depending on whether the user chose to delete ignored points and
    // measures, the log will either contain reasons for being ignored, or
    // reasons for being deleted
    PvlObject editLog((deleteIgnored) ? "Deleted" : "Ignored");
    editLog.AddObject(*ignoredPoints);

    // Get all the groups of measures from the map
    PvlObject measuresLog("Measures");
    QList<PvlGroup> measureGroups = ignoredMeasures->values();

    for(int i = 0; i < measureGroups.size(); i++) {
      measuresLog.AddGroup(measureGroups.at(i));
    }

    editLog.AddObject(measuresLog);
    outputLog.AddObject(editLog);

    // Write the log
    std::string logFilename = ui.GetFilename("LOG");
    outputLog.Write(logFilename);

    // Delete the structures keeping track of the ignored points and measures
    if(ignoredPoints != NULL) {
      delete ignoredPoints;
      ignoredPoints = NULL;
    }
    if(ignoredMeasures != NULL) {
      delete ignoredMeasures;
      ignoredMeasures = NULL;
    }
  }

  // Write the network
  cnet.Write(ui.GetFilename("ONET"));
}


// Set the point at the given index in the control network to ignored, and add
// a new keyword to the list of ignored points with a cause for the ignoring,
// if the user wished to keep a log
void IgnorePoint(ControlNet & cnet, int cp, string cause) {
  cnet[cp].SetIgnore(true);

  if(keepLog) {
    // Label the keyword as the Point ID, and make the cause into the value
    ignoredPoints->AddKeyword(
        PvlKeyword(cnet[cp].GetId(), cause));
  }
}


// Set the measure to be ignored, and add a new keyword to the list of ignored
// measures if the user wished to keep a log
void IgnoreMeasure(ControlNet & cnet, int cp, int cm, string cause) {
  cnet[cp][cm].SetIgnore(true);

  if(keepLog) {
    // Make the keyword label the measure Serial Number, and the cause into
    // the value
    PvlKeyword ignoredMeasure(
        PvlKeyword(cnet[cp][cm].GetCubeSerialNumber(), cause));

    // Using a map to make accessing by Point ID a O(1) to O(lg n) operation
    if(ignoredMeasures->contains(cnet[cp].GetId())) {
      // If the map already has a group for the given Point ID, simply add the
      // new measure to it
      PvlGroup & pointGroup = (*ignoredMeasures)[cnet[cp].GetId()];
      pointGroup.AddKeyword(ignoredMeasure);
    }
    else {
      // Else there is no group for the Point ID of the measure being ignored,
      // so make a new group, add the measure, and insert it into the map
      PvlGroup pointGroup(cnet[cp].GetId());
      pointGroup.AddKeyword(ignoredMeasure);
      (*ignoredMeasures)[cnet[cp].GetId()] = pointGroup;
    }
  }
}


// Delete the point, record how many points and measures have been deleted
void DeletePoint(ControlNet & cnet, int cp) {
  numMeasuresDeleted += cnet[cp].Size();
  numPointsDeleted++;

  if(keepLog) {
    // If the point's being deleted but it wasn't set to ignore, it can only be
    // because the point has two few measures remaining
    if(!cnet[cp].IsIgnored())
      IgnorePoint(cnet, cp, "Too few measures");

    // For any measures not ignored, mark their cause for deletion as being
    // caused by the point's deletion
    for(int cm = 0; cm < cnet[cp].Size(); cm++) {
      if(!cnet[cp][cm].IsIgnored())
        IgnoreMeasure(cnet, cp, cm, "Point deleted");
    }
  }

  cnet.Delete(cp);
}


// Delete the measure, increment the count of measures deleted
void DeleteMeasure(ControlNet & cnet, int cp, int cm) {
  numMeasuresDeleted++;

  ControlPoint point = cnet[cp];
  point.Delete(cm);
  cnet.UpdatePoint(point);
}


// Seed the log with points and measures already ignored
void PopulateLog(ControlNet & cnet) {
  ignoredPoints = new PvlObject("Points");
  ignoredMeasures = new QMap<string, PvlGroup>;

  for(int cp = 0; cp < cnet.Size(); cp++) {
    if(cnet[cp].IsIgnored()) {
      IgnorePoint(cnet, cp, "Ignored from input");
    }

    for(int cm = 0; cm < cnet[cp].Size(); cm++) {
      if(cnet[cp][cm].IsIgnored()) {
        if(cm == cnet[cp].GetReferenceIndex()) {
          // If the reference is ignored, the point must ignored too
          if(!cnet[cp].IsIgnored()) {
            IgnorePoint(cnet, cp, "Reference measure ignored");
          }
        }

        IgnoreMeasure(cnet, cp, cm, "Ignored from input");
      }
    }
  }
}


/**
 * Reads the Control Points list and matches with the control
 * network. If match was successful, ignore the point. If
 * Delete option was chosen, delete the point
 *
 * @param psFileName - Filename with Control Points
 * @param pcCnet     - holds the input Control Network
 * @param pcPvlLog   - Pvl for which control points stats have
 *                     to be added
 *
 * @return none
 */
void ProcessControlPoints(std::string psFileName, ControlNet &pcCnet) {
  ControlPointList cpList(psFileName);

  for(int cp = pcCnet.Size() - 1; cp >= 0; cp --) {

    // Compare each Point Id listed with the Point in the
    // Control Network for according exclusion
    if(!pcCnet[cp].IsIgnored() && cpList.HasControlPoint(pcCnet[cp].GetId())) {
      IgnorePoint(pcCnet, cp, "Point ID in POINTLIST");
    }

    if(deleteIgnored) {
      //look for previously ignored control points
      if(pcCnet[cp].IsIgnored() ||
          pcCnet[cp][pcCnet[cp].GetReferenceIndex()].IsIgnored()) {
        DeletePoint(pcCnet, cp);
      }
      else {
        //look for previously ignored control measures
        for(int cm = pcCnet[cp].Size() - 1; cm >= 0; cm--) {
          if(pcCnet[cp][cm].IsIgnored() && deleteIgnored) {
            DeleteMeasure(pcCnet, cp, cm);
          }
        }
        // Check if there are too few measures in the point or the point was previously ignored
        if(((pcCnet[cp].Size() < 2 && !preservePoints) && pcCnet[cp].GetType() != ControlPoint::Ground)
            || pcCnet[cp].Size() == 0 || (pcCnet[cp].IsIgnored() && deleteIgnored)) {
          DeletePoint(pcCnet, cp);
        }
      }
    }
  }
}

/**
 * Reads the Cube file list and creates the serial number of the
 * Cubes. If Control Measure serial# matches with the control
 * network,ignore the point. If Delete option was chosen, delete
 * the Measure
 *
 * @param psFileName - Filename with Cube File names
 * @param pcCnet     - holds the input Control Network
 *
 * @return none
 */
void ProcessControlMeasures(std::string psFileName, ControlNet &pcCnet) {
  SerialNumberList snl = psFileName;

  for(int cp = pcCnet.Size() - 1; cp >= 0; cp --) {

    // Compare each Serial Number listed with the serial number in the
    // Control Measure for according exclusion
    for(int cm = pcCnet[cp].Size() - 1; cm >= 0 ; cm--) {
      std::string serialNumber = pcCnet[cp][cm].GetCubeSerialNumber();
      if(!pcCnet[cp][cm].IsIgnored() && snl.HasSerialNumber(serialNumber)) {
        IgnoreMeasure(pcCnet, cp, cm, "Serial Number in CUBELIST");

        if(cm == pcCnet[cp].GetReferenceIndex() && !pcCnet[cp].IsIgnored()) {
          IgnorePoint(pcCnet, cp, "Reference measure ignored");
        }
      }

      //also look for previously ignored control measures
      if(deleteIgnored && pcCnet[cp][cm].IsIgnored() && cm != pcCnet[cp].GetReferenceIndex()) {
        DeleteMeasure(pcCnet, cp, cm);
      }
    }
    // Check if there are too few measures in the point or the point was previously ignored
    if(((pcCnet[cp].Size() < 2 && !preservePoints) && pcCnet[cp].GetType() != ControlPoint::Ground)
        || pcCnet[cp].Size() == 0 || (pcCnet[cp].IsIgnored() && deleteIgnored)) {
      DeletePoint(pcCnet, cp);
    }
  }
}


void CheckAllMeasureValidity(ControlNet & cnet, std::string cubeList) {
  SerialNumberList serialNumbers(cubeList);

  Progress progress;
  progress.SetText("Checking Measure Validity");
  progress.SetMaximumSteps(cnet.Size());
  progress.CheckStatus();

  for(int cp = cnet.Size() - 1; cp >= 0; cp--) {

    // Compare each Serial Number listed with the serial number in the
    // Control Measure for according exclusion
    for(int cm = cnet[cp].Size() - 1; cm >= 0; cm--) {

      std::string serialNumber = cnet[cp][cm].GetCubeSerialNumber();
      if(!serialNumbers.HasSerialNumber(serialNumber)) {
        std::string msg = "Serial Number [" + serialNumber + "] contains no ";
        msg += "matching cube in FROMLIST [" + cubeList + "]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }

      if(!cnet[cp][cm].IsIgnored()) {
        MeasureValidationResults results = 
          ValidateMeasure(cnet[cp][cm], serialNumbers.Filename(serialNumber));
        if(!results.isValid()) {
          string failure = results.toString().toStdString();
          IgnoreMeasure(cnet, cp, cm, "Validity Check " + failure);

          if(cm == cnet[cp].GetReferenceIndex()) {
            IgnorePoint(cnet, cp, "Reference measure ignored");
          }
        }
      }

      //also look for previously ignored control measures
      if(deleteIgnored && cnet[cp][cm].IsIgnored() && cm != cnet[cp].GetReferenceIndex()) {
        DeleteMeasure(cnet, cp, cm);
      }
    }

    // Check if there are too few measures in the point or the point was previously ignored
    if(((cnet[cp].Size() < 2 && !preservePoints) && cnet[cp].GetType() != ControlPoint::Ground)
        || cnet[cp].Size() == 0 || (cnet[cp].IsIgnored() && deleteIgnored)) {
      DeletePoint(cnet, cp);
    }

    progress.CheckStatus();
  }
}


MeasureValidationResults ValidateMeasure(const ControlMeasure & curMeasure, std::string cubeName) {
  Cube curCube;
  curCube.Open(cubeName);

  MeasureValidationResults results = validator->ValidStandardOptions(
      curMeasure.GetSample(), curMeasure.GetLine(), &curCube);

  return results;
}


// Helper function to print out template to session log
void PrintTemp() {
  UserInterface &ui = Application::GetUserInterface();

  // Get template pvl
  Pvl userTemp;
  userTemp.Read(ui.GetFilename("DEFFILE"));

  //Write template file out to the log
  Isis::Application::GuiLog(userTemp);
}
