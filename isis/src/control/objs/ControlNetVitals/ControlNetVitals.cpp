#include "ControlNetVitals.h"

#include <QList>

#include "IException.h"
#include "IString.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"

namespace Isis {

  ControlNetVitals::ControlNetVitals(ControlNet *cnet) {
    m_controlNet = cnet;
    validate();
  }

  ControlNetVitals::~ControlNetVitals() {
  }

  bool ControlNetVitals::hasIslands() {
    // Replace this with graph call!!!$!@$!@$!@$#@%#@$%#@
    return true;
  }

  int ControlNetVitals::numIslands() {
    // replace this with graph call!#@$!#%#@%*($#)
    return 1;
  }

  QList<QString> ControlNetVitals::getIslands() {
    // TEMP, replace with graph
    QList<QString> list;
    list.append("CASSIS_01.cub");
    return list;
  }

  int ControlNetVitals::numPoints() {
    return m_controlNet->GetNumPoints();
  }

  // REFACTOR
  int ControlNetVitals::numIgnoredPoints() {
    int count = 0;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->IsIgnored()) count++;
    }
    return count;
  }

  int ControlNetVitals::numLockedPoints() {
    return m_controlNet->GetNumEditLockPoints();
  }

  // REFACTOR
  int ControlNetVitals::numFixedPoints() {
    int count = 0;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetPointTypeString() == "Fixed") count++;
    }
    return count;
  }

  // REFACTOR
  int ControlNetVitals::numConstrainedPoints() {
    int count = 0;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetPointTypeString() == "Constrained") count++;
    }
    return count;
  }

  // REFACTOR
  int ControlNetVitals::numFreePoints() {
    int count = 0;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetPointTypeString() == "Free") count++;
    }
    return count;
  }

  // REFACTOR
  int ControlNetVitals::numPointsBelowMeasureThreshold(int num) {
    int count = 0;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetNumMeasures() < num) count++;
    }
    return count;
  }

  int ControlNetVitals::numImages() {
    return m_controlNet->GetCubeSerials().size();
  }

  int ControlNetVitals::numMeasures() {
    return m_controlNet->GetNumMeasures();
  }

  // REFACTOR
  int ControlNetVitals::numImagesBelowMeasureThreshold(int num) {
    int count = 0;
    foreach(QString serial, m_controlNet->GetCubeSerials()) {
      if (m_controlNet->GetMeasuresInCube(serial).size() < num) count++;
    }
    return count;
  }
  // REFACTOR
  int ControlNetVitals::numImagesBelowHullTolerance(int tolerance) {
    return 1;
  }

  QList<QString> ControlNetVitals::getCubeSerials() {
    return m_controlNet->GetCubeSerials();
  }

  QList<ControlPoint*> ControlNetVitals::getAllPoints() {
    return m_controlNet->GetPoints();
  }

  // REFACTOR
  QList<ControlPoint*> ControlNetVitals::getIgnoredPoints() {
    QList<ControlPoint*> ignoredPoints;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->IsIgnored()) ignoredPoints.append(point);
    }
    return ignoredPoints;
  }

  QList<ControlPoint*> ControlNetVitals::getLockedPoints() {
    QList<ControlPoint*> lockedPoints;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->IsEditLocked()) lockedPoints.append(point);
    }
    return lockedPoints;
  }

  QList<ControlPoint*> ControlNetVitals::getFixedPoints() {
    QList<ControlPoint*> fixedPoints;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetPointTypeString() == "Fixed") fixedPoints.append(point);
    }
    return fixedPoints;
  }

  QList<ControlPoint*> ControlNetVitals::getConstrainedPoints() {
    QList<ControlPoint*> constrainedPoints;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetPointTypeString() == "Constrained") constrainedPoints.append(point);
    }
    return constrainedPoints;
  }

  QList<ControlPoint*> ControlNetVitals::getFreePoints() {
    QList<ControlPoint*> freePoints;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetPointTypeString() == "Free") freePoints.append(point);
    }
    return freePoints;
  }

  // REFACTOR
  QList<ControlPoint*> ControlNetVitals::getPointsBelowMeasureThreshold(int num) {
    QList<ControlPoint*> belowThreshold;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetNumMeasures() < num) belowThreshold.append(point);
    }
    return belowThreshold;
  }

  QList<QString> ControlNetVitals::getAllImageSerials() {
    return m_controlNet->GetCubeSerials();
  }

  // REFACTOR
  QList<QString> ControlNetVitals::getImagesBelowMeasureThreshold(int num) {
    QList<QString> imagesBelowThreshold;
    foreach(QString serial, m_controlNet->GetCubeSerials()) {
      if (m_controlNet->GetMeasuresInCube(serial).size() < num) imagesBelowThreshold.append(serial);
    }
    return imagesBelowThreshold;
  }

  // REFACTOR
  QList<QString> ControlNetVitals::getImagesBelowHullTolerance(int num) {
    QList<QString> list;
    list.append("Example.cub");
    return list;
  }

  QString ControlNetVitals::getStatus() {
    return m_status;
  }

  QString ControlNetVitals::getStatusDetails() {
    return m_statusDetails;
  }

  QString ControlNetVitals::getNetworkId() {
    return m_controlNet->GetNetworkId();
  }

  //
  // ImageVitals ControlNetVitals::getImageVitals(QString serial) {
  //   return NULL;
  // }

  void ControlNetVitals::validate() {
    QString status = "";
    QString details = "";
    if (hasIslands()) {
      status = "Broken!";
      details = "This network has " + toString(numIslands()) + " island(s).";
    }
    else {

      if (numPointsBelowMeasureThreshold() < 3) {
        status = "Weak!";
        details += "This network has " + toString(numPointsBelowMeasureThreshold()) + " point(s) with less than 3 measures\n";
      }

      if (numImagesBelowMeasureThreshold() < 3) {
        status = "Weak!";
        details += "This network has " + toString(numImagesBelowMeasureThreshold()) + " image(s) with less than 3 measures\n";
      }

      if (numImagesBelowHullTolerance() > 0) {
        status = "Weak!";
        details += "This network has " + toString(numImagesBelowHullTolerance()) + " image(s) below the Convex Hull Tolerance of 75%\n";
      }

      if (status.isEmpty()) {
        status = "Healthy!";
        details = "This network is healthy.";
      }
    }
    updateStatus(status, details);
  }

  void ControlNetVitals::updateStatus(QString status, QString details) {
    m_status = status;
    m_statusDetails = details;
    emit networkChanged();
  }

}
