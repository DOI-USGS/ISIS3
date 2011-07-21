#include "Isis.h"

#include <cfloat>
#include <sstream>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "FileList.h"
#include "Progress.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "iException.h"
#include "iTime.h"

using namespace std;
using namespace Isis;


void mergeNetwork(
    ControlNet &baseNet, ControlNet &newNet, bool mergePoints,
    bool overwritePoints, bool overwriteMeasures,
    bool overwriteReference, bool overwriteMissing, PvlObject &cnetLog);
ControlPoint * mergePoint(
    ControlPoint *basePoint, ControlPoint *newPoint,
    bool overwritePoints, bool overwriteMeasures,
    bool overwriteReference, bool overwriteMissing, PvlObject &cnetLog);
void replacePoint(ControlPoint *basePoint, ControlPoint *newPoint,
    PvlObject &pointLog);
void mergeMeasure(
    ControlPoint *basePoint, ControlPoint *newPoint,
    ControlMeasure *baseMeasure, ControlMeasure *newMeasure,
    bool overwriteMeasures, bool overwriteReference, PvlGroup &measureLog);
ControlMeasure * replaceMeasure(ControlPoint *basePoint,
    ControlMeasure *baseMeasure, ControlMeasure *newMeasure);
ControlMeasure * addMeasure(ControlPoint *basePoint,
    ControlMeasure *newMeasure);
void reportConflict(PvlObject &pointLog, iString conflict);
void reportConflict(PvlGroup &measureLog, iString conflict);


bool report;


void IsisMain() {
  // Get user parameters
  UserInterface &ui = Application::GetUserInterface();
  FileList filelist;
  if (ui.GetString("INPUTTYPE") == "LIST") {
    filelist = ui.GetFilename("CLIST");
  }
  else if (ui.GetString("INPUTTYPE") == "CNETS") {
    filelist.push_back(ui.GetFilename("CNET"));
    filelist.push_back(ui.GetFilename("CNET2"));
  }
  Filename outfile(ui.GetFilename("ONET"));

  bool overwritePoints = false;
  bool overwriteMeasures = false;
  bool overwriteReference = false;
  bool overwriteMissing = false;
  if (ui.GetString("DUPLICATEPOINTS") == "MERGE") {
    overwritePoints = ui.GetBoolean("OVERWRITEPOINTS");
    overwriteMeasures = ui.GetBoolean("OVERWRITEMEASURES");
    overwriteReference = ui.GetBoolean("OVERWRITEREFERENCE");
    overwriteMissing = ui.GetBoolean("OVERWRITEMISSING");
  }

  // Creates a Progress
  Progress progress;
  progress.SetMaximumSteps(filelist.size());
  progress.CheckStatus();

  // Set up the output ControlNet with the first Control Net in the list
  ControlNet baseNet(Filename(filelist[0]).Expanded());
  baseNet.SetNetworkId(ui.GetString("NETWORKID"));
  baseNet.SetUserName(Isis::Application::UserName());
  baseNet.SetCreatedDate(Isis::Application::DateTime());
  baseNet.SetModifiedDate(Isis::iTime::CurrentLocalTime());
  baseNet.SetDescription(ui.GetString("DESCRIPTION"));

  progress.CheckStatus();

  Pvl conflictLog;
  report = ui.WasEntered("LOG");

  bool mergePoints = (ui.GetString("DUPLICATEPOINTS") == "MERGE");
  for (int cnetIndex = 1; cnetIndex < (int)filelist.size(); cnetIndex ++) {
    ControlNet newNet(Filename(filelist[cnetIndex]).Expanded());

    // Checks to make sure the ControlNets are valid to merge
    if (baseNet.GetTarget().DownCase() != newNet.GetTarget().DownCase()) {
      string msg = "Input [" + newNet.GetNetworkId() + "] does not target the "
          "same target as other Control Network(s)";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    PvlObject cnetLog(newNet.GetNetworkId());
    mergeNetwork(baseNet, newNet, mergePoints,
        overwritePoints, overwriteMeasures,
        overwriteReference, overwriteMissing, cnetLog);
    if (cnetLog.Objects() > 0) conflictLog.AddObject(cnetLog);

    progress.CheckStatus();
  }

  // Writes out the final Control Net
  baseNet.Write(outfile.Expanded());

  if (report)
    conflictLog.Write(ui.GetFilename("LOG"));
}


void mergeNetwork(
    ControlNet &baseNet, ControlNet &newNet, bool mergePoints,
    bool overwritePoints, bool overwriteMeasures,
    bool overwriteReference, bool overwriteMissing, PvlObject &cnetLog) {

  for (int newIndex = 0; newIndex < newNet.GetNumPoints(); newIndex++) {
    ControlPoint *newPoint = newNet.GetPoint(newIndex);
    if (baseNet.ContainsPoint(newPoint->GetId())) {
      if (mergePoints) {
        ControlPoint *basePoint = baseNet.GetPoint(QString(newPoint->GetId()));
        ControlPoint *outPoint = 
          mergePoint(
              basePoint, newPoint,
              overwritePoints, overwriteMeasures,
              overwriteReference, overwriteMissing, cnetLog);
        baseNet.DeletePoint(basePoint);
        baseNet.AddPoint(outPoint);
      }
      else {
        string msg = "Inputs contain the same ControlPoint. [Id=";
        msg += newPoint->GetId() + "] Set DUPLICATEPOINTS=MERGE to";
        msg += " merge conflicting Control Points.";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
    else {
      ControlPoint *outPoint = new ControlPoint(*newPoint);
      baseNet.AddPoint(outPoint);
    }
  }
}


ControlPoint * mergePoint(
    ControlPoint *basePoint, ControlPoint *newPoint,
    bool overwritePoints, bool overwriteMeasures,
    bool overwriteReference, bool overwriteMissing, PvlObject &cnetLog) {

  ControlPoint *outPoint = new ControlPoint(*basePoint);

  PvlObject pointLog(newPoint->GetId());
  if (overwritePoints) {
    replacePoint(outPoint, newPoint, pointLog);
  }
  else {
    reportConflict(pointLog, "Retained: OVERWRITEPOINTS=false");
  }

  if (overwriteMissing) {
    for (int baseIndex = 0; baseIndex < basePoint->GetNumMeasures(); baseIndex++) {
      ControlMeasure *baseMeasure = basePoint->GetMeasure(baseIndex);
      PvlGroup measureLog(baseMeasure->GetCubeSerialNumber());

      if (!newPoint->HasSerialNumber(baseMeasure->GetCubeSerialNumber())) {
        outPoint->Delete(baseMeasure);
        reportConflict(measureLog, "Removed: OVERWRITEMISSING=true");
      }

      if (measureLog.Keywords() > 0) pointLog.AddGroup(measureLog);
    }
  }

  for (int newIndex = 0; newIndex < newPoint->GetNumMeasures(); newIndex++) {
    ControlMeasure *newMeasure = newPoint->GetMeasure(newIndex);
    if (outPoint->HasSerialNumber(newMeasure->GetCubeSerialNumber())) {
      ControlMeasure *baseMeasure =
          outPoint->GetMeasure(newMeasure->GetCubeSerialNumber());
      PvlGroup measureLog(baseMeasure->GetCubeSerialNumber());
      mergeMeasure(
          basePoint, newPoint,
          baseMeasure, newMeasure,
          overwriteMeasures, overwriteReference, measureLog);
      if (measureLog.Keywords() > 0) pointLog.AddGroup(measureLog);
    }
    else {
      addMeasure(outPoint, newMeasure);
    }
  }

  if (pointLog.Keywords() > 0 || pointLog.Groups() > 0)
    cnetLog.AddObject(pointLog);

  return outPoint;
}


void replacePoint(ControlPoint *basePoint, ControlPoint *newPoint,
    PvlObject &pointLog) {

  if (!basePoint->IsEditLocked()) {
    basePoint->SetId(newPoint->GetId());
    basePoint->SetType(newPoint->GetType());
    basePoint->SetChooserName(newPoint->GetChooserName());
    basePoint->SetEditLock(newPoint->IsEditLocked());
    basePoint->SetIgnored(newPoint->IsIgnored());
    basePoint->SetAprioriSurfacePointSource(
        newPoint->GetAprioriSurfacePointSource());
    basePoint->SetAprioriSurfacePointSourceFile(
        newPoint->GetAprioriSurfacePointSourceFile());
    basePoint->SetAprioriSurfacePointSourceFile(
        newPoint->GetAprioriSurfacePointSourceFile());
    basePoint->SetAprioriRadiusSource(newPoint->GetAprioriRadiusSource());
    basePoint->SetAprioriRadiusSourceFile(
        newPoint->GetAprioriRadiusSourceFile());
    basePoint->SetAprioriSurfacePoint(newPoint->GetAprioriSurfacePoint());
    basePoint->SetAdjustedSurfacePoint(newPoint->GetAdjustedSurfacePoint());
    // TODO basePoint->SetInvalid(newPoint->GetInvalid());
    reportConflict(pointLog, "Replaced: OVERWRITEPOINTS=true");
  }
  else {
    reportConflict(pointLog, "Retained: Edit Lock");
  }
}


void mergeMeasure(
    ControlPoint *basePoint, ControlPoint *newPoint,
    ControlMeasure *baseMeasure, ControlMeasure *newMeasure,
    bool overwriteMeasures, bool overwriteReference, PvlGroup &measureLog) {

  if (!baseMeasure->IsEditLocked()) {
    if (newPoint->GetRefMeasure() == newMeasure) {
      if (overwriteReference) {
        ControlMeasure *outMeasure = replaceMeasure(
            basePoint, baseMeasure, newMeasure);
        basePoint->SetRefMeasure(outMeasure);
        reportConflict(measureLog, "Replaced: OVERWRITEREFERENCE=true");
      }
      else {
        reportConflict(measureLog, "Retained: OVERWRITEREFERENCE=false");
      }
    }
    else if (overwriteMeasures) {
      replaceMeasure(basePoint, baseMeasure, newMeasure);
      reportConflict(measureLog, "Replaced: OVERWRITEMEASURES=true");
    }
    else {
      reportConflict(measureLog, "Retained: OVERWRITEMEASURES=false");
    }
  }
  else {
    reportConflict(measureLog, "Retained: Edit Lock");
  }
}


ControlMeasure * replaceMeasure(ControlPoint *basePoint,
    ControlMeasure *baseMeasure, ControlMeasure *newMeasure) {

  basePoint->Delete(baseMeasure);
  return addMeasure(basePoint, newMeasure);
}


ControlMeasure * addMeasure(ControlPoint *basePoint,
    ControlMeasure *newMeasure) {

  ControlMeasure *outMeasure = new ControlMeasure(*newMeasure);
  basePoint->Add(outMeasure);
  return outMeasure;
}


void reportConflict(PvlObject &pointLog, iString conflict) {
  if (report) {
    PvlKeyword resolution("Resolution", conflict);
    pointLog.AddKeyword(resolution);
  }
}


void reportConflict(PvlGroup &measureLog, iString conflict) {
  if (report) {
    PvlKeyword resolution("Resolution", conflict);
    measureLog.AddKeyword(resolution);
  }
}

