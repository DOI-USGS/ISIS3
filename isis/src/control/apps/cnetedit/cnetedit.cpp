#define GUIHELPERS

#include "Isis.h"

#include <QList>
#include <QMap>
#include <QSet>
#include <QString>
#include <QTextStream>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlNetValidMeasure.h"
#include "ControlPoint.h"
#include "ControlPointList.h"
#include "Cube.h"
#include "FileName.h"
#include "MeasureValidationResults.h"
#include "Progress.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SerialNumber.h"
#include "SerialNumberList.h"
#include "IException.h"

#include "GuiEditFile.h"

using namespace std;
using namespace Isis;

// Deletion test
bool shouldDelete(ControlPoint *point);

// Mutator methods
void ignorePoint(ControlNet &cnet, ControlPoint *point, string cause);
void ignoreMeasure(ControlNet &cnet, ControlPoint *point,
                   ControlMeasure *measure, string cause);
void deletePoint(ControlNet &cnet, int cp);
void deleteMeasure(ControlPoint *point, int cm);

// Edit passes
void populateLog(ControlNet &cnet, bool ignore);

void unlockPoints(ControlNet &cnet, ControlPointList &cpList);
void ignorePoints(ControlNet &cnet, ControlPointList &cpList);
void lockPoints(ControlNet &cnet, ControlPointList &cpList);

void unlockCubes(ControlNet &cnet, SerialNumberList &snl);
void ignoreCubes(ControlNet &cnet, SerialNumberList &snl);
void lockCubes(ControlNet &cnet, SerialNumberList &snl);

void unlockMeasures(ControlNet &cnet,
    QMap< QString, QSet<QString> * > &editMeasures);
void ignoreMeasures(ControlNet &cnet,
    QMap< QString, QSet<QString> * > &editMeasures);
void lockMeasures(ControlNet &cnet,
    QMap< QString, QSet<QString> * > &editMeasures);

void checkAllMeasureValidity(ControlNet &cnet, string cubeList);

// Validity test
MeasureValidationResults validateMeasure(const ControlMeasure *measure,
    Cube *cube, Camera *camera);

// Logging helpers
void logResult(QMap<string, string> *pointsLog, string pointId, string cause);
void logResult(QMap<string, PvlGroup> *measuresLog,
    string pointId, string serial, string cause);
PvlObject createLog(string label, QMap<string, string> *pointsMap);
PvlObject createLog(string label,
    QMap<string, string> *pointsMap, QMap<string, PvlGroup> *measuresMap);

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
bool keepLog;
bool ignoreAll;

QMap<string, string> *ignoredPoints;
QMap<string, PvlGroup> *ignoredMeasures;
QMap<string, string> *retainedReferences;
QMap<string, string> *editLockedPoints;
QMap<string, PvlGroup> *editLockedMeasures;

ControlNetValidMeasure *validator;


// Main program
void IsisMain() {
  // Reset the counts of points and measures deleted
  numPointsDeleted = 0;
  numMeasuresDeleted = 0;

  // Interface for getting user parameters
  UserInterface &ui = Application::GetUserInterface();

  // Get global user parameters
  bool ignore = ui.GetBoolean("IGNORE");
  deleteIgnored = ui.GetBoolean("DELETE");
  preservePoints = ui.GetBoolean("PRESERVE");
  retainRef = ui.GetBoolean("RETAIN_REFERENCE");
  ignoreAll = ui.GetBoolean("IGNOREALL");

  // Data needed to keep track of ignored/deleted points and measures
  keepLog = ui.WasEntered("LOG");
  ignoredPoints = NULL;
  ignoredMeasures = NULL;
  retainedReferences = NULL;
  editLockedPoints = NULL;
  editLockedMeasures = NULL;

  // If the user wants to keep a log, go ahead and populate it with all the
  // existing ignored points and measures
  ControlNet cnet(ui.GetFileName("CNET"));
  if (keepLog && cnet.GetNumPoints() > 0)
    populateLog(cnet, ignore);

  // List has Points Ids
  bool processPoints = false;
  ControlPointList *cpList = NULL;
  if (ui.WasEntered("POINTLIST") && cnet.GetNumPoints() > 0) {
    processPoints = true;
    string pointlistFileName = ui.GetFileName("POINTLIST");
    cpList = new ControlPointList((FileName) pointlistFileName);
  }

  // List has Cube file names
  bool processCubes = false;
  SerialNumberList *cubeSnl = NULL;
  if (ui.WasEntered("CUBELIST") && cnet.GetNumPoints() > 0) {
    processCubes = true;
    string ignorelistFileName = ui.GetFileName("CUBELIST");
    cubeSnl = new SerialNumberList(ignorelistFileName);
  }

  // List has Cube file names
  bool processMeasures = false;
  QMap< QString, QSet<QString> * > *editMeasures = NULL;
  if (ui.WasEntered("MEASURELIST") && cnet.GetNumPoints() > 0) {
    processMeasures = true;
    editMeasures = new QMap< QString, QSet<QString> * >;

    QFile file(ui.GetFileName("MEASURELIST"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      string msg = "Unable to open MEASURELIST [" +
        file.fileName().toStdString() + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    QTextStream in(&file);
    int lineNumber = 1;
    while (!in.atEnd()) {
      QString line = in.readLine();
      QStringList results = line.split(",");
      if (results.size() < 2) {
        string msg = "Line " + IString(lineNumber) + " in the MEASURELIST does "
          "not contain a Point ID and a cube filename separated by a comma";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      if (!editMeasures->contains(results[0]))
        editMeasures->insert(results[0], new QSet<QString>);

      FileName cubeName(results[1].toStdString());
      string sn = SerialNumber::Compose(cubeName.expanded());
      (*editMeasures)[results[0]]->insert(QString::fromStdString(sn));

      lineNumber++;
    }
  }

  if (ui.GetBoolean("UNLOCK")) {
    if (processPoints) unlockPoints(cnet, *cpList);
    if (processCubes) unlockCubes(cnet, *cubeSnl);
    if (processMeasures) unlockMeasures(cnet, *editMeasures);
  }

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
        deletePoint(cnet, cp);
      }
      else {
        for (int cm = point->GetNumMeasures() - 1; cm >= 0; cm--) {
          if (point->GetMeasure(cm)->IsIgnored()) {
            if (cm == point->IndexOfRefMeasure()) {
              // If the reference is ignored, the point must ignored too
              ignorePoint(cnet, point, "Reference measure ignored");
            }
            else {
              // Can't delete the reference without deleting the whole point
              deleteMeasure(point, cm);
            }
          }
        }

        // Check if the number of measures in the point is zero or there are too
        // few measures in the point and we don't want to preserve them.
        if (shouldDelete(point)) {
          deletePoint(cnet, cp);
        }
      }

      progress.CheckStatus();
    }
  }

  if (ignore) {
    if (processPoints) ignorePoints(cnet, *cpList);
    if (processCubes) ignoreCubes(cnet, *cubeSnl);
    if (processMeasures) ignoreMeasures(cnet, *editMeasures);

    // Perform validity check
    if (ui.GetBoolean("CHECKVALID") && cnet.GetNumPoints() > 0) {
      validator = NULL;

      // First validate DefFile's keywords and value type
      Pvl defFile(ui.GetFileName("DEFFILE"));
      Pvl pvlTemplate("$ISIS3DATA/base/templates/cnet_validmeasure/validmeasure.def");
      Pvl pvlResults;
      pvlTemplate.ValidatePvl(defFile, pvlResults);
      if (pvlResults.Groups() > 0 || pvlResults.Keywords() > 0) {
        Application::Log(pvlResults.Group(0));
        string sErrMsg = "Invalid Deffile\n";
        throw IException(IException::User, sErrMsg, _FILEINFO_);
      }

      // Construct the validator from the user-specified definition file
      validator = new ControlNetValidMeasure(&defFile);

      // User also provided a list of all serial numbers corresponding to every
      // cube in the control network
      string cubeList = ui.GetFileName("FROMLIST");
      checkAllMeasureValidity(cnet, cubeList);

      // Delete the validator
      if (validator != NULL) {
        delete validator;
        validator = NULL;
      }

      // Log the DEFFILE to the print file
      Application::Log(defFile.FindGroup("ValidMeasure", Pvl::Traverse));
    }
  }

  if (ui.GetBoolean("LOCK")) {
    if (processPoints) lockPoints(cnet, *cpList);
    if (processCubes) lockCubes(cnet, *cubeSnl);
    if (processMeasures) lockMeasures(cnet, *editMeasures);
  }

  // Log statistics
  if (keepLog) {
    Pvl outputLog;

    outputLog.AddKeyword(PvlKeyword("PointsDeleted", numPointsDeleted));
    outputLog.AddKeyword(PvlKeyword("MeasuresDeleted", numMeasuresDeleted));

    PvlObject lockedLog = createLog(
        "EditLocked", editLockedPoints, editLockedMeasures);
    outputLog.AddObject(lockedLog);

    outputLog.AddObject(createLog("RetainedReferences", retainedReferences));

    // Depending on whether the user chose to delete ignored points and
    // measures, the log will either contain reasons for being ignored, or
    // reasons for being deleted
    PvlObject ignoredLog = createLog(
        deleteIgnored ? "Deleted" : "Ignored", ignoredPoints, ignoredMeasures);
    outputLog.AddObject(ignoredLog);

    // Write the log
    string logFileName = ui.GetFileName("LOG");
    outputLog.Write(logFileName);

    // Delete the structures keeping track of the ignored points and measures
    delete ignoredPoints;
    ignoredPoints = NULL;

    delete ignoredMeasures;
    ignoredMeasures = NULL;

    delete retainedReferences;
    retainedReferences = NULL;

    delete editLockedPoints;
    editLockedPoints = NULL;

    delete editLockedMeasures;
    editLockedMeasures = NULL;
  }

  delete cpList;
  cpList = NULL;

  delete cubeSnl;
  cubeSnl = NULL;

  if (editMeasures != NULL) {
    QList<QString> points = editMeasures->keys();
    for (int i = 0; i < points.size(); i++) delete (*editMeasures)[points[i]];
    delete editMeasures;
    editMeasures = NULL;
  }

  // Write the network
  cnet.Write(ui.GetFileName("ONET"));
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
bool shouldDelete(ControlPoint *point) {
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
void ignorePoint(ControlNet &cnet, ControlPoint *point, string cause) {
  ControlPoint::Status result = point->SetIgnored(true);

  logResult(result == ControlPoint::Success ? ignoredPoints : editLockedPoints,
      point->GetId(), cause);
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
void ignoreMeasure(ControlNet &cnet, ControlPoint *point,
                   ControlMeasure *measure, string cause) {
  ControlMeasure::Status result = measure->SetIgnored(true);

  logResult(
      result == ControlMeasure::Success ? ignoredMeasures : editLockedMeasures,
      point->GetId(), measure->GetCubeSerialNumber(), cause);

  if (ignoreAll && measure->Parent()->GetRefMeasure() == measure) {
    foreach (ControlMeasure *cm, measure->Parent()->getMeasures()) {
      if (!cm->IsIgnored())
        ignoreMeasure(cnet, measure->Parent(), cm, "Reference ignored");
    }
  }
}


/**
 * Delete the point, record how many points and measures have been deleted.
 *
 * @param cnet The Control Network being modified
 * @param cp   Index into the Control Network for the point we wish to delete
 */
void deletePoint(ControlNet &cnet, int cp) {
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
        ignorePoint(cnet, point, "Too few measures");

      // For any measures not ignored, mark their cause for deletion as being
      // caused by the point's deletion
      for (int cm = 0; cm < point->GetNumMeasures(); cm++) {
        ControlMeasure *measure = point->GetMeasure(cm);
        if (!measure->IsIgnored())
          ignoreMeasure(cnet, point, measure, "Point deleted");
      }
    }

    cnet.DeletePoint(cp);
  }
  else {
    for (int cm = 0; cm < point->GetNumMeasures(); cm++) {
      if (point->GetMeasure(cm)->IsEditLocked()) {
        ignorePoint(cnet, point, "EditLocked point skipped");
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
void deleteMeasure(ControlPoint *point, int cm) {
  if (point->Delete(cm) == ControlMeasure::Success) numMeasuresDeleted++;
}


/**
 * Seed the log with points and measures already ignored.
 *
 * @param cnet The Control Network being modified
 */
void populateLog(ControlNet &cnet, bool ignore) {
  ignoredPoints = new QMap<string, string>;
  ignoredMeasures = new QMap<string, PvlGroup>;

  retainedReferences = new QMap<string, string>;

  editLockedPoints = new QMap<string, string>;
  editLockedMeasures = new QMap<string, PvlGroup>;

  Progress progress;
  progress.SetText("Initializing Log File");
  progress.SetMaximumSteps(cnet.GetNumPoints());
  progress.CheckStatus();

  for (int cp = 0; cp < cnet.GetNumPoints(); cp++) {
    ControlPoint *point = cnet.GetPoint(cp);

    if (point->IsIgnored()) {
      ignorePoint(cnet, point, "Ignored from input");
    }

    for (int cm = 0; cm < point->GetNumMeasures(); cm++) {
      ControlMeasure *measure = point->GetMeasure(cm);

      if (measure->IsIgnored()) {
        if (cm == point->IndexOfRefMeasure()) {
          // If the reference is ignored, the point must be ignored too
          if (ignore && !point->IsIgnored()) {
            ignorePoint(cnet, point, "Reference measure ignored");
          }
        }

        ignoreMeasure(cnet, point, measure, "Ignored from input");
      }
    }

    progress.CheckStatus();
  }
}


void unlockPoints(ControlNet &cnet, ControlPointList &cpList) {
  Progress progress;
  progress.SetText("Unlocking Points");
  progress.SetMaximumSteps(cnet.GetNumPoints());
  progress.CheckStatus();

  for (int cp = cnet.GetNumPoints() - 1; cp >= 0; cp--) {
    ControlPoint *point = cnet.GetPoint(cp);
    if (point->IsEditLocked() && cpList.HasControlPoint(point->GetId())) {
      point->SetEditLock(false);
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
void ignorePoints(ControlNet &cnet, ControlPointList &cpList) {
  Progress progress;
  progress.SetText("Comparing Points to POINTLIST");
  progress.SetMaximumSteps(cnet.GetNumPoints());
  progress.CheckStatus();

  for (int cp = cnet.GetNumPoints() - 1; cp >= 0; cp--) {
    ControlPoint *point = cnet.GetPoint(cp);

    // Compare each Point Id listed with the Point in the
    // Control Network for according exclusion
    if (!point->IsIgnored() && cpList.HasControlPoint(point->GetId())) {
      ignorePoint(cnet, point, "Point ID in POINTLIST");
    }

    if (deleteIgnored) {
      //look for previously ignored control points
      if (point->IsIgnored() || point->GetRefMeasure()->IsIgnored()) {
        deletePoint(cnet, cp);
      }
      else {
        //look for previously ignored control measures
        for (int cm = point->GetNumMeasures() - 1; cm >= 0; cm--) {
          if (point->GetMeasure(cm)->IsIgnored() && deleteIgnored) {
            deleteMeasure(point, cm);
          }
        }
        // Check if there are too few measures in the point or the point was
        // previously ignored
        if (shouldDelete(point)) {
          deletePoint(cnet, cp);
        }
      }
    }

    progress.CheckStatus();
  }
}


void lockPoints(ControlNet &cnet, ControlPointList &cpList) {
  Progress progress;
  progress.SetText("Locking Points");
  progress.SetMaximumSteps(cnet.GetNumPoints());
  progress.CheckStatus();

  for (int cp = cnet.GetNumPoints() - 1; cp >= 0; cp--) {
    ControlPoint *point = cnet.GetPoint(cp);
    if (!point->IsEditLocked() && cpList.HasControlPoint(point->GetId())) {
      point->SetEditLock(true);
    }
    progress.CheckStatus();
  }
}


void unlockCubes(ControlNet &cnet, SerialNumberList &snl) {
  Progress progress;
  progress.SetText("Unlocking Cubes");
  progress.SetMaximumSteps(cnet.GetNumPoints());
  progress.CheckStatus();

  for (int cp = cnet.GetNumPoints() - 1; cp >= 0; cp--) {
    ControlPoint *point = cnet.GetPoint(cp);

    for (int cm = point->GetNumMeasures() - 1; cm >= 0; cm--) {
      ControlMeasure *measure = point->GetMeasure(cm);

      string serialNumber = measure->GetCubeSerialNumber();
      if (measure->IsEditLocked() && snl.HasSerialNumber(serialNumber)) {
        measure->SetEditLock(false);
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
void ignoreCubes(ControlNet &cnet, SerialNumberList &snl) {
  Progress progress;
  progress.SetText("Comparing Measures to CUBELIST");
  progress.SetMaximumSteps(cnet.GetNumPoints());
  progress.CheckStatus();

  for (int cp = cnet.GetNumPoints() - 1; cp >= 0; cp--) {
    ControlPoint *point = cnet.GetPoint(cp);

    // Compare each Serial Number listed with the serial number in the
    // Control Measure for according exclusion
    for (int cm = point->GetNumMeasures() - 1; cm >= 0; cm--) {
      ControlMeasure *measure = point->GetMeasure(cm);

      if (!point->IsIgnored() && point->GetMeasure(cm)->IsEditLocked()) {
        ignoreMeasure(cnet, point, measure, "EditLocked measure skipped");
      }

      string serialNumber = measure->GetCubeSerialNumber();

      if (snl.HasSerialNumber(serialNumber)) {
        string cause = "Serial Number in CUBELIST";
        if (cm == point->IndexOfRefMeasure() && retainRef) {
          logResult(retainedReferences, point->GetId(), cause);
        }
        else if (!measure->IsIgnored() || cm == point->IndexOfRefMeasure()) {
          ignoreMeasure(cnet, point, measure, cause);

          if (cm == point->IndexOfRefMeasure() && !point->IsIgnored()) {
            ignorePoint(cnet, point, "Reference measure ignored");
          }
        }
      }

      //also look for previously ignored control measures
      if (deleteIgnored && measure->IsIgnored() &&
          cm != point->IndexOfRefMeasure()) {
        deleteMeasure(point, cm);
      }
    }
    // Check if there are too few measures in the point or the point was
    // previously ignored
    if (shouldDelete(point)) {
      deletePoint(cnet, cp);
    }

    progress.CheckStatus();
  }
}


void lockCubes(ControlNet &cnet, SerialNumberList &snl) {
  Progress progress;
  progress.SetText("Locking Cubes");
  progress.SetMaximumSteps(cnet.GetNumPoints());
  progress.CheckStatus();

  for (int cp = cnet.GetNumPoints() - 1; cp >= 0; cp--) {
    ControlPoint *point = cnet.GetPoint(cp);

    for (int cm = point->GetNumMeasures() - 1; cm >= 0; cm--) {
      ControlMeasure *measure = point->GetMeasure(cm);

      string serialNumber = measure->GetCubeSerialNumber();
      if (!measure->IsEditLocked() && snl.HasSerialNumber(serialNumber)) {
        measure->SetEditLock(true);
      }
    }
    progress.CheckStatus();
  }
}


void unlockMeasures(ControlNet &cnet,
    QMap< QString, QSet<QString> * > &editMeasures) {

  Progress progress;
  progress.SetText("Unlocking Measures");
  progress.SetMaximumSteps(cnet.GetNumPoints());
  progress.CheckStatus();

  for (int cp = cnet.GetNumPoints() - 1; cp >= 0; cp--) {
    ControlPoint *point = cnet.GetPoint(cp);

    QString id = QString::fromStdString(point->GetId());
    if (editMeasures.contains(id)) {
      QSet<QString> *measureSet = editMeasures[id];

      for (int cm = point->GetNumMeasures() - 1; cm >= 0; cm--) {
        ControlMeasure *measure = point->GetMeasure(cm);

        QString serialNumber = QString::fromStdString(
            measure->GetCubeSerialNumber());
        if (measure->IsEditLocked() && measureSet->contains(serialNumber)) {
          measure->SetEditLock(false);
        }
      }
    }
    progress.CheckStatus();
  }
}


void ignoreMeasures(ControlNet &cnet,
    QMap< QString, QSet<QString> * > &editMeasures) {

  Progress progress;
  progress.SetText("Ignoring Measures");
  progress.SetMaximumSteps(cnet.GetNumPoints());
  progress.CheckStatus();

  for (int cp = cnet.GetNumPoints() - 1; cp >= 0; cp--) {
    ControlPoint *point = cnet.GetPoint(cp);

    QString id = QString::fromStdString(point->GetId());
    if (editMeasures.contains(id)) {
      QSet<QString> *measureSet = editMeasures[id];

      // Compare each Serial Number listed with the serial number in the
      // Control Measure for according exclusion
      for (int cm = point->GetNumMeasures() - 1; cm >= 0; cm--) {
        ControlMeasure *measure = point->GetMeasure(cm);

        if (!point->IsIgnored() && point->GetMeasure(cm)->IsEditLocked()) {
          ignoreMeasure(cnet, point, measure, "EditLocked measure skipped");
        }

        QString serialNumber = QString::fromStdString(
            measure->GetCubeSerialNumber());
        if (measureSet->contains(serialNumber)) {
          string cause = "Measure in MEASURELIST";
          if (cm == point->IndexOfRefMeasure() && retainRef) {
            logResult(retainedReferences, point->GetId(), cause);
          }
          else if (!measure->IsIgnored() || cm == point->IndexOfRefMeasure()) {
            ignoreMeasure(cnet, point, measure, cause);

            if (cm == point->IndexOfRefMeasure() && !point->IsIgnored()) {
              ignorePoint(cnet, point, "Reference measure ignored");
            }
          }
        }

        //also look for previously ignored control measures
        if (deleteIgnored && measure->IsIgnored() &&
            cm != point->IndexOfRefMeasure()) {
          deleteMeasure(point, cm);
        }
      }
      // Check if there are too few measures in the point or the point was
      // previously ignored
      if (shouldDelete(point)) {
        deletePoint(cnet, cp);
      }
    }

    progress.CheckStatus();
  }
}


void lockMeasures(ControlNet &cnet,
    QMap< QString, QSet<QString> * > &editMeasures) {

  Progress progress;
  progress.SetText("Locking Measures");
  progress.SetMaximumSteps(cnet.GetNumPoints());
  progress.CheckStatus();

  for (int cp = cnet.GetNumPoints() - 1; cp >= 0; cp--) {
    ControlPoint *point = cnet.GetPoint(cp);

    QString id = QString::fromStdString(point->GetId());
    if (editMeasures.contains(id)) {
      QSet<QString> *measureSet = editMeasures[id];

      for (int cm = point->GetNumMeasures() - 1; cm >= 0; cm--) {
        ControlMeasure *measure = point->GetMeasure(cm);

        QString serialNumber = QString::fromStdString(
            measure->GetCubeSerialNumber());
        if (!measure->IsEditLocked() && measureSet->contains(serialNumber)) {
          measure->SetEditLock(true);
        }
      }
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
void checkAllMeasureValidity(ControlNet &cnet, string cubeList) {
  SerialNumberList serialNumbers(cubeList);

  QList<ControlCubeGraphNode *> graphNodes = cnet.GetCubeGraphNodes();
  Progress progress;
  progress.SetText("Checking Measure Validity");
  progress.SetMaximumSteps(graphNodes.size());
  progress.CheckStatus();

  for (int sn = 0; sn < graphNodes.size(); sn++) {
    ControlCubeGraphNode *graphNode = graphNodes[sn];
    IString serialNumber = graphNode->getSerialNumber();

    Cube *cube = NULL;
    Camera *camera = NULL;
    if (validator->IsCubeRequired()) {
      if (!serialNumbers.HasSerialNumber(serialNumber)) {
        string msg = "Serial Number [" + serialNumber + "] contains no ";
        msg += "matching cube in FROMLIST";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      cube = new Cube;
      cube->open(serialNumbers.FileName(serialNumber));

      if (validator->IsCameraRequired()) {
        try {
          camera = cube->getCamera();
        }
        catch (IException &e) {
          string msg = "Cannot Create Camera for Image:" + cube->getFileName();
          throw IException(e, IException::User, msg, _FILEINFO_);
        }
      }
    }

    QList<ControlMeasure *> measures = graphNode->getMeasures();
    for (int cm = 0; cm < measures.size(); cm++) {
      ControlMeasure *measure = measures[cm];
      ControlPoint *point = measure->Parent();

      if (!measure->IsIgnored()) {
        MeasureValidationResults results =
            validateMeasure(measure, cube, camera);

        if (!results.isValid()) {
          string failure = results.toString().toStdString();
          string cause = "Validity Check " + failure;

          if (measure == point->GetRefMeasure() && retainRef) {
            logResult(retainedReferences, point->GetId(), cause);
          }
          else {
            ignoreMeasure(cnet, point, measure, cause);

            if (measure == point->GetRefMeasure()) {
              ignorePoint(cnet, point, "Reference measure ignored");
            }
          }
        }
      }
    }

    delete cube;
    progress.CheckStatus();
  }

  for (int cp = cnet.GetNumPoints() - 1; cp >= 0; cp--) {
    ControlPoint *point = cnet.GetPoint(cp);

    for (int cm = point->GetNumMeasures() - 1; cm >= 0; cm--) {
      ControlMeasure *measure = point->GetMeasure(cm);

      // Also look for previously ignored control measures
      if (deleteIgnored && measure->IsIgnored() &&
          measure != point->GetRefMeasure()) {
        deleteMeasure(point, cm);
      }
    }

    // Check if there are too few measures in the point or the point was
    // previously ignored
    if (shouldDelete(point))
      deletePoint(cnet, cp);
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
MeasureValidationResults validateMeasure(const ControlMeasure *measure,
    Cube *cube, Camera *camera) {

  MeasureValidationResults results =
      validator->ValidStandardOptions(measure, cube, camera);

  return results;
}


void logResult(QMap<string, string> *pointsLog, string pointId, string cause) {
  if (keepLog) {
    // Label the keyword as the Point ID, and make the cause into the value
    (*pointsLog)[pointId] = cause;
  }
}


void logResult(QMap<string, PvlGroup> *measuresLog,
    string pointId, string serial, string cause) {

  if (keepLog) {
    // Make the keyword label the measure Serial Number, and the cause into the
    // value
    PvlKeyword measureMessage(PvlKeyword(serial, cause));

    // Using a map to make accessing by Point ID a O(1) to O(lg n) operation
    if (measuresLog->contains(pointId)) {
      // If the map already has a group for the given Point ID, simply add the
      // new measure to it
      PvlGroup &pointGroup = (*measuresLog)[pointId];
      pointGroup.AddKeyword(measureMessage);
    }
    else {
      // Else there is no group for the Point ID of the measure being ignored,
      // so make a new group, add the measure, and insert it into the map
      PvlGroup pointGroup(pointId);
      pointGroup.AddKeyword(measureMessage);
      (*measuresLog)[pointId] = pointGroup;
    }
  }
}


PvlObject createLog(string label, QMap<string, string> *pointsMap) {
  PvlObject pointsLog(label);

  QList<string> pointIds = pointsMap->keys();
  for (int i = 0; i < pointIds.size(); i++) {
    string pointId = pointIds.at(i);
    pointsLog.AddKeyword(PvlKeyword(pointId, (*pointsMap)[pointId]));
  }

  return pointsLog;
}


PvlObject createLog(string label,
    QMap<string, string> *pointsMap, QMap<string, PvlGroup> *measuresMap) {

  PvlObject editLog(label);

  PvlObject pointsLog = createLog("Points", pointsMap);
  editLog.AddObject(pointsLog);

  // Get all the groups of measures from the map
  PvlObject measuresLog("Measures");
  QList<PvlGroup> measureGroups = measuresMap->values();

  for (int i = 0; i < measureGroups.size(); i++)
    measuresLog.AddGroup(measureGroups.at(i));

  editLog.AddObject(measuresLog);
  return editLog;
}


/**
 * Helper function to print out template to session log.
 */
void PrintTemp() {
  UserInterface &ui = Application::GetUserInterface();

  // Get template PVL
  Pvl userTemp;
  userTemp.Read(ui.GetFileName("DEFFILE"));

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

