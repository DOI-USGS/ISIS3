#include "NetworkVitals.h"

#include <QList>

#include "IException.h"
#include "IString.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"

namespace Isis {

  NetworkVitals::NetworkVitals(ControlNet *cnet) {
    m_controlNet = cnet;
    validate();
  }

  NetworkVitals::~NetworkVitals() {
  }

  bool NetworkVitals::hasIslands() {
    // Replace this with graph call!!!$!@$!@$!@$#@%#@$%#@
    return true;
  }

  int NetworkVitals::numIslands() {
    // replace this with graph call!#@$!#%#@%*($#)
    return 1;
  }

  QList<QString> NetworkVitals::getIslands() {
    // TEMP, replace with graph
    QList<QString> list;
    list.append("CASSIS_01.cub");
    return list;
  }

  int NetworkVitals::numPoints() {
    return m_controlNet->GetNumPoints();
  }

  // REFACTOR
  int NetworkVitals::numIgnoredPoints() {
    int count = 0;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->IsIgnored()) count++;
    }
    return count;
  }

  int NetworkVitals::numLockedPoints() {
    return m_controlNet->GetNumEditLockPoints();
  }

  // REFACTOR
  int NetworkVitals::numFixedPoints() {
    int count = 0;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetPointTypeString() == "Fixed") count++;
    }
    return count;
  }

  // REFACTOR
  int NetworkVitals::numConstraintedPoints() {
    int count = 0;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetPointTypeString() == "Constrained") count++;
    }
    return count;
  }

  // REFACTOR
  int NetworkVitals::numFreePoints() {
    int count = 0;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetPointTypeString() == "Free") count++;
    }
    return count;
  }

  // REFACTOR
  int NetworkVitals::numPointsBelowMeasureThreshold(int num) {
    int count = 0;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetNumMeasures() < num) count++;
    }
    return count;
  }

  int NetworkVitals::numImages() {
    return m_controlNet->GetCubeSerials().size();
  }

  int NetworkVitals::numMeasures() {
    return m_controlNet->GetNumMeasures();
  }

  // REFACTOR
  int NetworkVitals::numImagesBelowMeasureThreshold(int num) {
    int count = 0;
    foreach(QString serial, m_controlNet->GetCubeSerials()) {
      if (m_controlNet->GetMeasuresInCube(serial).size() < num) count++;
    }
    return count;
  }
  // REFACTOR
  int NetworkVitals::numImagesBelowHullTolerance(int tolerance) {
    return 1;
  }

  QList<QString> NetworkVitals::getCubeSerials() {
    return m_controlNet->GetCubeSerials();
  }

  QList<ControlPoint*> NetworkVitals::getAllPoints() {
    return m_controlNet->GetPoints();
  }

  // REFACTOR
  QList<ControlPoint*> NetworkVitals::getIgnoredPoints() {
    QList<ControlPoint*> ignoredPoints;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->IsIgnored()) ignoredPoints.append(point);
    }
    return ignoredPoints;
  }

  QList<ControlPoint*> NetworkVitals::getLockedPoints() {
    QList<ControlPoint*> lockedPoints;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->IsEditLocked()) lockedPoints.append(point);
    }
    return lockedPoints;
  }

  QList<ControlPoint*> NetworkVitals::getFixedPoints() {
    QList<ControlPoint*> fixedPoints;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetPointTypeString() == "Fixed") fixedPoints.append(point);
    }
    return fixedPoints;
  }

  QList<ControlPoint*> NetworkVitals::getConstrainedPoints() {
    QList<ControlPoint*> constrainedPoints;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetPointTypeString() == "Constrained") constrainedPoints.append(point);
    }
    return constrainedPoints;
  }

  QList<ControlPoint*> NetworkVitals::getFreePoints() {
    QList<ControlPoint*> freePoints;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetPointTypeString() == "Free") freePoints.append(point);
    }
    return freePoints;
  }

  // REFACTOR
  QList<ControlPoint*> NetworkVitals::getPointsBelowMeasureThreshold(int num) {
    QList<ControlPoint*> belowThreshold;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetNumMeasures() < num) belowThreshold.append(point);
    }
    return belowThreshold;
  }

  QList<QString> NetworkVitals::getAllImageSerials() {
    return m_controlNet->GetCubeSerials();
  }

  // REFACTOR
  QList<QString> NetworkVitals::getImagesBelowMeasureThreshold(int num) {
    QList<QString> imagesBelowThreshold;
    foreach(QString serial, m_controlNet->GetCubeSerials()) {
      if (m_controlNet->GetMeasuresInCube(serial).size() < num) imagesBelowThreshold.append(serial);
    }
    return imagesBelowThreshold;
  }

  // REFACTOR
  QList<QString> NetworkVitals::getImagesBelowHullTolerance(int num) {
    QList<QString> list;
    list.append("Example.cub");
    return list;
  }

  QString NetworkVitals::getStatus() {
    return m_status;
  }

  QString NetworkVitals::getStatusDetails() {
    return m_statusDetails;
  }

  QString NetworkVitals::getNetworkId() {
    return m_controlNet->GetNetworkId();
  }

  //
  // ImageVitals NetworkVitals::getImageVitals(QString serial) {
  //   return NULL;
  // }

  void NetworkVitals::validate() {
    QString status = "";
    QString details = "";
    if (hasIslands()) {
      status = "Broken!";
      details = "This network has " + toString(numIslands()) + " islands.";
    }
    else {

      if (numPointsBelowMeasureThreshold() < 3) {
        status = "Weak!";
        details += "This network has " + toString(numPointsBelowMeasureThreshold()) + " points with less than 3 measures\n";
      }

      if (numImagesBelowMeasureThreshold() < 3) {
        status = "Weak!";
        details += "This network has " + toString(numImagesBelowMeasureThreshold()) + " images with less than 3 measures\n";
      }

      if (numImagesBelowHullTolerance() > 0) {
        status = "Weak!";
        details += "This network has " + toString(numImagesBelowHullTolerance()) + " images below the Convex Hull Tolerance of 75%\n";
      }

      if (status.isEmpty()) {
        status = "Healthy!";
        details = "This network is healthy.";
      }
    }
    updateStatus(status, details);
  }

  void NetworkVitals::updateStatus(QString status, QString details) {
    m_status = status;
    m_statusDetails = details;
    emit networkChanged();
  }

}
