#include "Isis.h"

#include "Application.h"
#include "ApolloPanTile.h"
#include "ControlNet.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "IException.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

bool comparePointsAlpha(ControlPoint *LHS, ControlPoint *RHS);

void IsisMain() {
  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  // Read in from the old PVL file

  ApolloPanTile originalTile;
  try {
    originalTile.fromPvl(ui.GetFileName("ORIGINALPVL"));
  }
  catch (IException &e) {
    QString msg = "Unable to open original Pvl file [" + ui.GetFileName("ORIGINALPVL") + "].";
    throw IException(e, IException::Io, msg, _FILEINFO_);
  }
  ApolloPanTile newTile = originalTile;
  newTile.clearFiducialMarks();
  newTile.clearTimingMarks();

  // Update fiducial marks

  ControlNet fiducialNet;
  try {
    fiducialNet.ReadControl(ui.GetFileName("FIDUCIALNET"));
  }
  catch (IException &e) {
    QString msg = "Unable to open original fiducial mark network ["
                   + ui.GetFileName("FIDUCIALNET") + "].";
    throw IException(e, IException::Io, msg, _FILEINFO_);
  }
  QList<ControlPoint*> fiducialPoints = fiducialNet.GetPoints();
  qSort(fiducialPoints.begin(), fiducialPoints.end(), comparePointsAlpha);
  int numberOfFiducials = fiducialPoints.size();
  for (int i = 0 ; i < numberOfFiducials; i++) {
    ControlPoint *point = fiducialPoints[i];
    ControlMeasure *measure = point->GetMeasure(0);
    double sample = measure->GetSample();
    double line = measure->GetLine();
    double offset = 1000;
    if (line < 20000) {
      line -= 1000;
    }
    else {
      line -= 24300;
      offset = 24300;
    }
    FiducialMark mark;
    // If the mark was added by the user
    if (QString::compare(point->GetId().left(3), "new", Qt::CaseInsensitive) == 0) {
      int length = 80;
      int height = 60;
      Point2f massCenter((float)sample, (float)line);
      Rect boundingRect((int)sample - length/2, (int)line - height/2, length, height);
      mark = FiducialMark(massCenter, boundingRect);
      if (mark.leftSample() < 20 || newTile.samples() - mark.rightSample() < 20) {
        mark.setValid(false);
      }
    }
    // If the mark already existed
    else {
      QString id = point->GetId();
      int index = id.toInt();
      if (index < 0 || index >= originalTile.numberOfFiducialMarks()) {
        QString msg = "Invalid fiducial mark control point id [" + id + "].\n"
                      + "Modified fiducial mark point ids must be the index of"
                          + "the mark in the original pvl\n"
                      + "New fiducial mark point ids must begin with \"new\"";
        throw IException(IException::Io, msg, _FILEINFO_);
      }
      mark = originalTile.fiducialMark(index);
      mark.adjustLocation(sample, line);
      if (mark.leftSample() < 20 || newTile.samples() - mark.rightSample() < 20) {
        mark.setValid(false);
      }
      else {
        mark.setValid(true);
      }
      mark.setNumber(-1);
      mark.setCalibratedX(0);
      mark.setCalibratedY(0);
      mark.setResidualX(0);
      mark.setResidualY(0);
      mark.computeResidualMagnitude();
    }
    mark.setLineOffset(offset);
    newTile.addFiducialMark(mark);
  }

  // Update timing marks

  ControlNet timingNet;
  try {
    timingNet.ReadControl(ui.GetFileName("TIMINGNET"));
  }
  catch (IException &e) {
    QString msg = "Unable to open original timing mark network ["
                   + ui.GetFileName("TIMINGNET") + "].";
    throw IException(e, IException::Io, msg, _FILEINFO_);
  }
  QList<ControlPoint*> timingPoints = timingNet.GetPoints();
  qSort(timingPoints.begin(), timingPoints.end(), comparePointsAlpha);
  int numberOfTimingPoints = timingPoints.size();
  for (int i = 0; i < numberOfTimingPoints - 1; i+=2) {
    ControlPoint *startPoint = timingPoints[i];
    ControlPoint *stopPoint  = timingPoints[i+1];
    QString startID = startPoint->GetId();
    QString stopID = stopPoint->GetId();
    if (!(QString::compare(startID.right(5), "start", Qt::CaseInsensitive) == 0) ||
        !(QString::compare(stopID.right(4), "stop", Qt::CaseInsensitive) == 0) ) {
      QString msg = "Timing mark control points [" + startID + "] and [" + stopID
                    + "] are not a start/stop pair.\nEvery timing mark must be represented"
                    + "by two control points, their IDs must be the mark's index in the original"
                    + "pvl followed by either \"start\" for the right side or \"stop\" "
                    + "for the left side.";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
    ControlMeasure *startMeasure = startPoint->GetMeasure(0);
    ControlMeasure *stopMeasure  = stopPoint->GetMeasure(0);
    double startSample = startMeasure->GetSample();
    double stopSample  = stopMeasure->GetSample();
    int line = startMeasure->GetLine() - 24900;
    TimingMark mark;
    // If the mark was added by the user
    if ((QString::compare(startID.left(3), "new", Qt::CaseInsensitive) == 0) ||
        (QString::compare(stopID.left(3), "new", Qt::CaseInsensitive) == 0) ) {
      double length = startSample - stopSample;
      int height = 90;
      Point2f massCenter((float)(startSample + stopSample)/2, (float)line);
      Rect boundingRect((int)stopSample, (int)line - height/2, length, height);
      mark = TimingMark(massCenter, boundingRect);
      if (mark.leftSample() < 20 || newTile.samples() - mark.rightSample() < 20) {
        mark.setValid(false);
      }
    }
    // If the mark already existed
    else {
      int index = startID.left(startID.length() - 5).toInt();
      if (!(QString::compare(startID.left(startID.length() - 5), stopID.left(stopID.length() - 4)) == 0) ||
          index >= originalTile.numberOfTimingMarks() ||
          index < 0) {
        QString msg = "Timing mark control point IDs [" + startID + "] and [" + stopID
                      + "] do not have valid prefixs.\nThe prefix for start and stop control "
                      + "points must be the timing mark's index in the original detection.";
      throw IException(IException::Io, msg, _FILEINFO_);
      }
      mark = originalTile.timingMark(index);
      mark.adjustLocation(startSample, stopSample, line);
      if (mark.leftSample() < 20 || newTile.samples() - mark.rightSample() < 20) {
        mark.setValid(false);
      }
      else {
        mark.setValid(true);
      }
      mark.setNumber(-1);
      mark.setValue(-1);
    }
    newTile.addTimingMark(mark);
  }

  // Output the new Pvl

  Pvl outputFile;
  outputFile += newTile.toPvl();
  try {
    outputFile.write(ui.GetFileName("TOPVL"));
  }
  catch (IException &e) {
    QString msg = "Unable to output file [" + ui.GetFileName("TOPVL") + "].";
    throw IException(e, IException::Io, msg, _FILEINFO_);
  }
}


/**
 * Control Net does not properly return a sorted list of points by ID.
 * So this is the comparison function we'll use to sort.
 */
bool comparePointsAlpha(ControlPoint *LHS, ControlPoint *RHS) {
  return LHS->GetId() < RHS->GetId();
}