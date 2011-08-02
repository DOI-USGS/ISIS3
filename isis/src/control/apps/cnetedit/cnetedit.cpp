#define GUIHELPERS

#include "Isis.h"

#include <QList>
#include <QMap>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlNetValidMeasure.h"
#include "ControlPoint.h"
#include "ControlPointList.h"
#include "Cube.h"
#include "MeasureValidationResults.h"
#include "Progress.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SerialNumberList.h"
#include "iException.h"

#include "GuiEditFile.h"

using namespace std;

using namespace Isis;

// Deletion test
bool ShouldDelete(ControlPoint *point);

// Mutator methods
void IgnorePoint(ControlNet &cnet, ControlPoint *point, string cause);
void IgnoreMeasure(ControlNet &cnet, ControlPoint *point,
                   ControlMeasure *measure, string cause);
void DeletePoint(ControlNet &cnet, int cp);
//void DeleteMeasure(ControlNet & cnet, ControlPoint * point, int cm);
void DeleteMeasure(ControlPoint *point, int cm);

// Edit passes
void PopulateLog(ControlNet &cnet);
void ProcessControlPoints(string fileName, ControlNet &cnet);
void ProcessControlMeasures(string fileName, ControlNet &cnet);
void CheckAllMeasureValidity(ControlNet &cnet, string cubeList);

// Validity test
MeasureValidationResults ValidateMeasure(const ControlMeasure *measure,
    SerialNumberList &serialNumbers);

void PrintTemp();
void EditDefFile();

map<string, void *> GuiHelpers() {
  map<string, void *> helper;
  helper["PrintTemp"]   = (void *) PrintTemp;
  helper["EditDefFile"] = (void *) EditDefFile;
  return helper;
}

// Global variables
int numPointsDeleted;
int numMeasuresDeleted;

bool deleteIgnored;
bool preservePoints;
bool retainRef;
bool comments;
bool keepLog;
PvlObject *ignoredPoints;
QMap<string, PvlGroup> * ignoredMeasures;
PvlObject *commentPoints;
PvlObject * editLockedMeasures;
PvlObject * editLockedPoints;

ControlNetValidMeasure *validator;


// Main program
void IsisMain() {
  // Reset the counts of points and measures deleted
  numPointsDeleted = 0;
  numMeasuresDeleted = 0;
  comments = false;

  // Interface for getting user parameters
  UserInterface &ui = Application::GetUserInterface();

  // Get global user parameters
  deleteIgnored  = ui.GetBoolean("DELETE");
  preservePoints = ui.GetBoolean("PRESERVE");
  retainRef      = ui.GetBoolean("RETAIN_REFERENCE");

  // Data needed to keep track of ignored/deleted points and measures
  keepLog = ui.WasEntered("LOG");
  ignoredPoints = NULL;
  ignoredMeasures = NULL;
  commentPoints = NULL;
  editLockedMeasures = NULL;
  editLockedPoints = NULL;

  // If the user wants to keep a log, go ahead and populate it with all the
  // existing ignored points and measures
  ControlNet cnet(ui.GetFilename("CNET"));
  if (keepLog && cnet.GetNumPoints() > 0)
    PopulateLog(cnet);

  /*
   * As a first pass, just try and delete anything that's already ignored
   * in the Control Network, if the user wants to delete ignored points and
   * measures.  Originally, this check was performed last, only if the user
   * didn't specify any other deletion methods.  However, performing this
   * check first will actually improve the running time in cases where there
   * are already ignored points and measures in the input network. The added
   * cost of doing this check here actually doesn't add to the running time at
   * all, because these same checks would need to have been done later
   * regardless.
   */
  if (deleteIgnored && cnet.GetNumPoints() > 0) {

    Progress progress;
    progress.SetText("Deleting Ignored in Input");
    progress.SetMaximumSteps(cnet.GetNumPoints());
    progress.CheckStatus();

    for (int cp = cnet.GetNumPoints() - 1; cp >= 0; cp--) {
      ControlPoint *point = cnet.GetPoint(cp);
      if (point->IsIgnored()) {
        DeletePoint(cnet, cp);
      }
      else {
        for (int cm = point->GetNumMeasures() - 1; cm >= 0; cm--) {
          if (point->GetMeasure(cm)->IsIgnored()) {
            if (cm == point->IndexOfRefMeasure()) {
              // If the reference is ignored, the point must ignored too
              IgnorePoint(cnet, point, "Reference measure ignored");
            }
            else {
              // Can't delete the reference without deleting the whole point
              //DeleteMeasure(cnet, point, cm);
              DeleteMeasure(point, cm);
            }
          }
        }

        // Check if the number of measures in the point is zero or there are too
        // few measures in the point and we don't want to preserve them.
        if (ShouldDelete(point)) {
          DeletePoint(cnet, cp);
        }
      }

      progress.CheckStatus();
    }
  }

  //List has Points Ids
  if (ui.WasEntered("POINTLIST") && cnet.GetNumPoints() > 0) {
    string pointlistFilename = ui.GetFilename("POINTLIST");
    ProcessControlPoints(pointlistFilename, cnet);
  }

  //List has Cube file names
  if (ui.WasEntered("CUBELIST") && cnet.GetNumPoints() > 0) {
    string ignorelistFilename = ui.GetFilename("CUBELIST");
    ProcessControlMeasures(ignorelistFilename, cnet);
  }

  // Perform validity check
  if (ui.GetBoolean("CHECKVALID") && cnet.GetNumPoints() > 0) {
    validator = NULL;

    // First validate DefFile's keywords and value type
    Pvl defFile(ui.GetFilename("DEFFILE"));
    Pvl pvlTemplate("$ISIS3DATA/base/templates/cnet_validmeasure/validmeasure.def");
    Pvl pvlResults;
    pvlTemplate.ValidatePvl(defFile, pvlResults);
    if (pvlResults.Groups() > 0 || pvlResults.Keywords() > 0) {
      Application::Log(pvlResults.Group(0));
      string sErrMsg = "Invalid Deffile\n";
      throw Isis::iException::Message(Isis::iException::User, sErrMsg, _FILEINFO_);
    }

    // Construct the validator from the user-specified definition file
    validator = new ControlNetValidMeasure(&defFile);

    // User also provided a list of all serial numbers corresponding to every
    // cube in the control network
    string cubeList = ui.GetFilename("FROMLIST");
    CheckAllMeasureValidity(cnet, cubeList);

    // Delete the validator
    if (validator != NULL) {
      delete validator;
      validator = NULL;
    }

    // Log the DEFFILE to the print file
    Application::Log(defFile.FindGroup("ValidMeasure", Pvl::Traverse));
  }

  // Log statistics
  if (keepLog) {
    Pvl outputLog;
    
    outputLog.AddKeyword(PvlKeyword("PointsDeleted", numPointsDeleted));
    outputLog.AddKeyword(PvlKeyword("MeasuresDeleted", numMeasuresDeleted));

    outputLog.AddObject(*editLockedPoints);
    outputLog.AddObject(*editLockedMeasures);
    if (comments) {
      outputLog.AddObject(*commentPoints);
    }
    // Depending on whether the user chose to delete ignored points and
    // measures, the log will either contain reasons for being ignored, or
    // reasons for being deleted
    PvlObject editLog((deleteIgnored) ? "Deleted" : "Ignored");
    editLog.AddObject(*ignoredPoints);

    // Get all the groups of measures from the map
    PvlObject measuresLog("Measures");
    QList<PvlGroup> measureGroups = ignoredMeasures->values();

    for (int i = 0; i < measureGroups.size(); i++) {
      measuresLog.AddGroup(measureGroups.at(i));
    }

    editLog.AddObject(measuresLog);
    outputLog.AddObject(editLog);

    // Write the log
    string logFilename = ui.GetFilename("LOG");
    outputLog.Write(logFilename);

    // Delete the structures keeping track of the ignored points and measures
    if (ignoredPoints != NULL) {
      delete ignoredPoints;
      ignoredPoints = NULL;
    }
    if (ignoredMeasures != NULL) {
      delete ignoredMeasures;
      ignoredMeasures = NULL;
    }
    if (commentPoints != NULL) {
      delete commentPoints;
      commentPoints = NULL;
    }
    if (editLockedPoints != NULL) {
      delete editLockedPoints;
      editLockedPoints = NULL;
    }
    if (editLockedMeasures != NULL) {
      delete editLockedMeasures;
      editLockedMeasures = NULL;
    }
  }

  // Write the network
  cnet.Write(ui.GetFilename("ONET"));
}


/**
 * After any modification to a point's measures or ignored status, this check
 * should be performed to determine if the changes should result in the point's
 * deletion.
 *
 * @param point The Control Point recently modified
 *
 * @return Whether or not the point should be deleted
 */
bool ShouldDelete(ControlPoint *point) {
  // If the point only has one measure, then unless it's a fixed point or the
  // user wishes to preserve such points, it should be deleted. As a side
  // effect, this check will also delete empty points that satisfy this
  // condition without having to do the next check
  if ((point->GetNumMeasures() < 2 && !preservePoints) &&
      point->GetType() != ControlPoint::Fixed)
    return true;

  // A point without any measures should always be deleted
  if (point->GetNumMeasures() == 0)
    return true;

  // If the user wants to delete ignored points, and this point is ignored,
  // then it follows that it should be deleted
  if (point->IsIgnored() && deleteIgnored)
    return true;

  // Otherwise, the point looks good
  return false;
}


/**
 * Set the point at the given index in the control network to ignored, and add
 * a new keyword to the list of ignored points with a cause for the ignoring,
 * if the user wished to keep a log.
 *
 * @param cnet  The Control Network being modified
 * @param point The Control Point we wish to ignore
 * @param cause A prose description of why the point was ignored (for logging)
 */
void IgnorePoint(ControlNet &cnet, ControlPoint *point, string cause) {
  ControlPoint::Status result = point->SetIgnored(true);
  if (keepLog) {
    if (result == ControlPoint::Success) {
      // Label the keyword as the Point ID, and make the cause into the value
      ignoredPoints->AddKeyword(PvlKeyword(point->GetId(), cause));
    }
    else if (point->IsEditLocked() && cause == "EditLocked point skipped") {
      editLockedPoints->AddKeyword(PvlKeyword(point->GetId(),cause));
    }
  }
}


/**
 * Set the measure to be ignored, and add a new keyword to the list of ignored
 * measures if the user wished to keep a log.
 *
 * @param cnet    The Control Network being modified
 * @param point   The Control Point of the Control Measure we wish to ignore
 * @param measure The Control Measure we wish to ignore
 * @param cause   A prose description of why the measure was ignored (for
 *                logging)
 */
void IgnoreMeasure(ControlNet &cnet, ControlPoint *point,
                   ControlMeasure *measure, string cause) {

  ControlMeasure::Status result = measure->SetIgnored(true);
  if (keepLog && result == ControlMeasure::Success) {
    // Make the keyword label the measure Serial Number, and the cause into
    // the value
    PvlKeyword ignoredMeasure(
      PvlKeyword(measure->GetCubeSerialNumber(), cause));

    // Using a map to make accessing by Point ID a O(1) to O(lg n) operation
    if (ignoredMeasures->contains(point->GetId())) {
      // If the map already has a group for the given Point ID, simply add the
      // new measure to it
      PvlGroup &pointGroup = (*ignoredMeasures)[point->GetId()];
      pointGroup.AddKeyword(ignoredMeasure);
    }
    else {
      // Else there is no group for the Point ID of the measure being ignored,
      // so make a new group, add the measure, and insert it into the map
      
      PvlGroup pointGroup(point->GetId());
      pointGroup.AddKeyword(ignoredMeasure);
      (*ignoredMeasures)[point->GetId()] = pointGroup;
    }
  }
  else if (measure->IsEditLocked() && cause == "EditLocked measure skipped") {
    editLockedMeasures->AddKeyword(PvlKeyword(point->GetId(), cause));
  }
}


/**
 * Delete the point, record how many points and measures have been deleted.
 *
 * @param cnet The Control Network being modified
 * @param cp   Index into the Control Network for the point we wish to delete
 */
void DeletePoint(ControlNet &cnet, int cp) {
  ControlPoint *point = cnet.GetPoint(cp);

  // Do the edit lock check up front so we don't accidentally log that a point
  // was deleted when in fact it was not
  if (!point->IsEditLocked()) {
    numMeasuresDeleted += point->GetNumMeasures();
    numPointsDeleted++;

    if (keepLog) {
      // If the point's being deleted but it wasn't set to ignore, it can only
      // be because the point has two few measures remaining
      if (!point->IsIgnored())
        IgnorePoint(cnet, point, "Too few measures");

      // For any measures not ignored, mark their cause for deletion as being
      // caused by the point's deletion
      for (int cm = 0; cm < point->GetNumMeasures(); cm++) {
        ControlMeasure *measure = point->GetMeasure(cm);
        if (!measure->IsIgnored())
          IgnoreMeasure(cnet, point, measure, "Point deleted");
      }
    }

    cnet.DeletePoint(cp);
  } 
  else {
    for (int cm = 0; cm < point->GetNumMeasures(); cm++) {
      if (point->GetMeasure(cm)->IsEditLocked()) {
        IgnorePoint(cnet, point, "EditLocked point skipped");
      }
    }
  }
}


/**
 * Delete the measure, increment the count of measures deleted.
 *
 * @param cnet  The Control Network being modified
 * @param point The Control Point of the Control Measure we wish to delete
 * @param cm    Index into the Control Network for the measure we wish to delete
 */
void DeleteMeasure(ControlPoint *point, int cm) {
  if (point->Delete(cm) == ControlMeasure::Success) numMeasuresDeleted++;
}


/**
 * Seed the log with points and measures already ignored.
 *
 * @param cnet The Control Network being modified
 */
void PopulateLog(ControlNet &cnet) {
  ignoredPoints = new PvlObject("Points");
  ignoredMeasures = new QMap<string, PvlGroup>;
  
  commentPoints = new PvlObject("RetainedPointReference");
  
  editLockedMeasures = new PvlObject("EditLockedMeasures");
  editLockedPoints = new PvlObject("EditLockedPoints");
  

  Progress progress;
  progress.SetText("Initializing Log File");
  progress.SetMaximumSteps(cnet.GetNumPoints());
  progress.CheckStatus();

  for (int cp = 0; cp < cnet.GetNumPoints(); cp++) {
    ControlPoint *point = cnet.GetPoint(cp);

    if (point->IsIgnored()) {
      IgnorePoint(cnet, point, "Ignored from input");
    }

    for (int cm = 0; cm < point->GetNumMeasures(); cm++) {
      ControlMeasure *measure = point->GetMeasure(cm);

      if (measure->IsIgnored()) {
        if (cm == point->IndexOfRefMeasure()) {
          // If the reference is ignored, the point must ignored too
          if (!point->IsIgnored()) {
            IgnorePoint(cnet, point, "Reference measure ignored");
          }
        }

        IgnoreMeasure(cnet, point, measure, "Ignored from input");
      }
    }

    progress.CheckStatus();
  }
}


/**
 * Iterates over the points in the Control Network looking for a match in the
 * list of Control Points to be ignored.  If a match is found, ignore the
 * point, and if the DELETE option was selected, the point will then be deleted
 * from the network.
 *
 * @param fileName Name of the file containing the list of Control Points
 * @param cnet     The Control Network being modified
 */
void ProcessControlPoints(string fileName, ControlNet &cnet) {
  ControlPointList cpList(fileName);

  Progress progress;
  progress.SetText("Comparing Points to POINTLIST");
  progress.SetMaximumSteps(cnet.GetNumPoints());
  progress.CheckStatus();

  for (int cp = cnet.GetNumPoints() - 1; cp >= 0; cp--) {
    ControlPoint *point = cnet.GetPoint(cp);

    // Compare each Point Id listed with the Point in the
    // Control Network for according exclusion
    if (!point->IsIgnored() && cpList.HasControlPoint(point->GetId())) {
      IgnorePoint(cnet, point, "Point ID in POINTLIST");
    }

    if (deleteIgnored) {
      //look for previously ignored control points
      if (point->IsIgnored() || point->GetRefMeasure()->IsIgnored()) {
        DeletePoint(cnet, cp);
      }
      else {
        //look for previously ignored control measures
        for (int cm = point->GetNumMeasures() - 1; cm >= 0; cm--) {
          if (point->GetMeasure(cm)->IsIgnored() && deleteIgnored) {
            DeleteMeasure(point, cm);
          }
        }
        // Check if there are too few measures in the point or the point was
        // previously ignored
        if (ShouldDelete(point)) {
          DeletePoint(cnet, cp);
        }
      }
    }

    progress.CheckStatus();
  }
}

/**
 * Iterates over the list of Control Measures in the Control Network and
 * compares measure serial numbers with those in the input list of serial
 * numbers to be ignored.  If a match is found, ignore the measure.  If the
 * DELETE option was selected, the measure will then be deleted from the
 * network.
 *
 * @param fileName Name of the file containing the list of Serial Numbers to be
 *                 ignored
 * @param cnet     The Control Network being modified
 */
void ProcessControlMeasures(string fileName, ControlNet &cnet) {
  SerialNumberList snl = fileName;

  Progress progress;
  progress.SetText("Comparing Measures to CUBELIST");
  progress.SetMaximumSteps(cnet.GetNumPoints());
  progress.CheckStatus();

  for (int cp = cnet.GetNumPoints() - 1; cp >= 0; cp --) {
    ControlPoint *point = cnet.GetPoint(cp);

    // Compare each Serial Number listed with the serial number in the
    // Control Measure for according exclusion
    for (int cm = point->GetNumMeasures() - 1; cm >= 0; cm--) {
      ControlMeasure *measure = point->GetMeasure(cm);
      
      if (!point->IsIgnored() && point->GetMeasure(cm)->IsEditLocked()) {
        IgnoreMeasure(cnet, point, measure, "EditLocked measure skipped");
      }
    
      string serialNumber = measure->GetCubeSerialNumber();
      
      if (snl.HasSerialNumber(serialNumber)){
        if (cm == point->IndexOfRefMeasure() && retainRef) {
          comments = true;
          commentPoints->AddKeyword( 
            PvlKeyword(point->GetId(),
                "Reference Measure is in the Ignore CubeList"));
        } 
        else if (!measure->IsIgnored() || (cm == point->IndexOfRefMeasure() &&
            !retainRef)) {
          IgnoreMeasure(cnet, point, measure, "Serial Number in CUBELIST");
  
          if (cm == point->IndexOfRefMeasure() && !point->IsIgnored()) {
            IgnorePoint(cnet, point, "Reference measure ignored");
          } 
        }
      }

      //also look for previously ignored control measures
      if (deleteIgnored && measure->IsIgnored() &&
          cm != point->IndexOfRefMeasure()) {
        DeleteMeasure(point, cm);
      }
    }
    // Check if there are too few measures in the point or the point was
    // previously ignored
    if (ShouldDelete(point)) {
      DeletePoint(cnet, cp);
    }

    progress.CheckStatus();
  }
}


/**
 * Compare each measure in the Control Network against tolerances specified in
 * the input DEFFILE.  Ignore any measure whose values fall outside the valid
 * tolerances, and delete it if the user specified to do so.
 *
 * @param cnet     The Control Network being modified
 * @param cubeList Name of the file containing the list of all Serial Numbers
 *                 in the network
 */
void CheckAllMeasureValidity(ControlNet &cnet, string cubeList) {
  SerialNumberList serialNumbers(cubeList);

  Progress progress;
  progress.SetText("Checking Measure Validity");
  progress.SetMaximumSteps(cnet.GetNumPoints());
  progress.CheckStatus();

  for (int cp = cnet.GetNumPoints() - 1; cp >= 0; cp--) {
    ControlPoint *point = cnet.GetPoint(cp);

    // Compare each Serial Number listed with the serial number in the
    // Control Measure for according exclusion
    for (int cm = point->GetNumMeasures() - 1; cm >= 0; cm--) {
      ControlMeasure *measure = point->GetMeasure(cm);

      if (!measure->IsIgnored()) {
        MeasureValidationResults results =
            ValidateMeasure(measure, serialNumbers);

        if (!results.isValid()) {
          if (cm == point->IndexOfRefMeasure() && retainRef) {
            comments = true;
            if (commentPoints->HasKeyword(point->GetId())) {
              PvlKeyword & key = commentPoints->FindKeyword(point->GetId());
              key += "\nReference Measure is not Validated";
            }
            else {
              commentPoints->AddKeyword(
                PvlKeyword(point->GetId(), "Reference Measure is not Validated"));
            }
          }
          else {
            string failure = results.toString().toStdString();
            IgnoreMeasure(cnet, point, measure, "Validity Check " + failure);
  
            if (cm == point->IndexOfRefMeasure()) {
              IgnorePoint(cnet, point, "Reference measure ignored");
            }
          }
        }
      }

      //also look for previously ignored control measures
      if (deleteIgnored && measure->IsIgnored() &&
          cm != point->IndexOfRefMeasure()) {
        DeleteMeasure(point, cm);
      }
    }

    // Check if there are too few measures in the point or the point was
    // previously ignored
    if (ShouldDelete(point)) {
      DeletePoint(cnet, cp);
    }
    progress.CheckStatus();
  }
}


/**
 * Test an individual measure against the user-specified tolerances and return
 * the result.
 *
 * @param curMeasure The measure currently being tested
 * @param cubeName   Name of the cube whose serial number matches that of the
 *                   current measure
 *
 * @return The results of validating the measure as an object containing the
 *         validity and a formatted error (or success) message
 */
MeasureValidationResults ValidateMeasure(const ControlMeasure *measure,
    SerialNumberList &serialNumbers) {

  Cube *measureCube = NULL;

  if (validator->IsCubeRequired()) {
    string serialNumber = measure->GetCubeSerialNumber();
    if (!serialNumbers.HasSerialNumber(serialNumber)) {
      string msg = "Serial Number [" + serialNumber + "] contains no ";
      msg += "matching cube in FROMLIST";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    measureCube = new Cube;
    measureCube->open(serialNumbers.Filename(serialNumber));
  }

  MeasureValidationResults results =
      validator->ValidStandardOptions(measure, measureCube);

  delete measureCube;

  return results;
}


/**
 * Helper function to print out template to session log.
 */
void PrintTemp() {
  UserInterface &ui = Application::GetUserInterface();

  // Get template PVL
  Pvl userTemp;
  userTemp.Read(ui.GetFilename("DEFFILE"));

  // Write template file out to the log
  Isis::Application::GuiLog(userTemp);
}

/**
 * Helper function to be able to edit the Deffile. 
 * Opens an editor to edit the file. 
 * 
 * @author Sharmila Prasad (5/23/2011)
 */
void EditDefFile(void) {
  UserInterface &ui = Application::GetUserInterface();
  string sDefFile = ui.GetAsString("DEFFILE");
  
  GuiEditFile::EditFile(ui, sDefFile);
}
