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

    initializeVitals();

    connect(cnet, SIGNAL(networkModified(ControlNet::ModType)),
            this, SLOT(validateNetwork(ControlNet::ModType)));

    connect(cnet, SIGNAL(newPoint(ControlPoint*)),
            this, SLOT(addPoint(ControlPoint*)));
    connect(cnet, SIGNAL(pointModified(ControlPoint *, ControlPoint::ModType, QVariant, QVariant)),
            this, SLOT(pointModified(ControlPoint *, ControlPoint::ModType, QVariant, QVariant)));
    connect(cnet, SIGNAL(pointDeleted(ControlPoint*)),
            this, SLOT(deletePoint(ControlPoint*)));

    connect(cnet, SIGNAL(newMeasure(ControlMeasure*)),
            this, SLOT(addMeasure(ControlMeasure*)));
    connect(cnet, SIGNAL(measureModified(ControlMeasure *, ControlMeasure::ModType, QVariant, QVariant)),
            this, SLOT(measureModified(ControlMeasure *, ControlMeasure::ModType, QVariant, QVariant)));
    connect(cnet, SIGNAL(measureRemoved(ControlMeasure*)),
            this, SLOT(deleteMeasure(ControlMeasure*)));

    validate();
  }

  void ControlNetVitals::initializeVitals() {

    m_islandList = m_controlNet->GetSerialConnections();

    m_numPointsIgnored = 0;
    m_numPointsLocked = 0;

    m_pointMeasureCounts.clear();
    m_imageMeasureCounts.clear();
    m_pointTypeCounts.clear();

    m_pointTypeCounts.insert(ControlPoint::Free, 0);
    m_pointTypeCounts.insert(ControlPoint::Constrained, 0);
    m_pointTypeCounts.insert(ControlPoint::Fixed, 0);

    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      addPoint(point);
    }

    foreach(QString serial, m_controlNet->GetCubeSerials()) {
      int numValidMeasures = m_controlNet->GetNumberOfValidMeasuresInImage(serial);
      if ( !m_imageMeasureCounts.contains(numValidMeasures) ) {
        m_imageMeasureCounts.insert(numValidMeasures, 1);
      }
      else {
        m_imageMeasureCounts[numValidMeasures]++;
      }
    }
  }


  /**
   * Unlike, deletePoint, this method does modify counters based on measures
   * because the ControlNet does not emit separate measureAdded signals for
   * efficiency reasons.
   */
  void ControlNetVitals::addPoint(ControlPoint *point) {
    std::cout << "Point added" << std::endl;
    if (point->IsIgnored()) {
      m_numPointsIgnored++;
      return;
    }

    if (point->IsEditLocked()) {
      m_numPointsLocked++;
    }

    m_pointTypeCounts[point->GetType()]++;

    int numValidMeasures = point->GetNumValidMeasures();
    if ( !m_pointMeasureCounts.contains(numValidMeasures) ) {
      m_pointMeasureCounts.insert(numValidMeasures, 1);
    }

    else {
      m_pointMeasureCounts[numValidMeasures]++;
    }
    validate();
  }


  void ControlNetVitals::pointModified(ControlPoint *point, ControlPoint::ModType type,
                                       QVariant oldValue, QVariant newValue) {
    switch(type) {
      case ControlPoint::EditLockModified:

        if (oldValue.toBool()) {
          m_numPointsLocked--;
        }

        if (newValue.toBool()) {
          m_numPointsLocked++;
        }

        break;

      case ControlPoint::IgnoredModified:

        if (oldValue.toBool()) {
          m_numPointsIgnored--;
          if (point->IsEditLocked()) {
            m_numPointsLocked++;
          }
          m_pointTypeCounts[point->GetType()]++;
          int numValidMeasures = point->GetNumValidMeasures();
          if ( !m_pointMeasureCounts.contains(numValidMeasures) ) {
            m_pointMeasureCounts.insert(numValidMeasures, 1);
          }
          else {
            m_pointMeasureCounts[numValidMeasures]++;
          }
        }

        if (newValue.toBool()) {
          m_numPointsIgnored++;
          if (point->IsEditLocked()) {
            m_numPointsLocked--;
          }
          m_pointTypeCounts[point->GetType()]--;
          int numValidMeasures = point->GetNumValidMeasures();
          if ( --m_pointMeasureCounts[numValidMeasures] < 1 ) {
            m_pointMeasureCounts.remove(numValidMeasures);
          }
        }

        break;

      case ControlPoint::TypeModified:

        m_pointTypeCounts[ControlPoint::PointType(oldValue.toInt())]--;
        m_pointTypeCounts[ControlPoint::PointType(newValue.toInt())]++;

        break;

      default:
        // no operation
        break;
    }
    validate();

  }

  /**
   * This does not modify any counters based on the measures in the point
   * because separate measureDeleted signals will be emitted by the ControlNet.
   * addPoint does add modify counters based on measures because ControlNet does
   * not emit separate measureAdded signals for efficiency reasons.
   */
  void ControlNetVitals::deletePoint(ControlPoint *point) {
    std::cout << "Point deleted" << std::endl;
    if (point->IsIgnored()) {
      m_numPointsIgnored--;
      validate();
      return;
    }



    if (point->IsEditLocked()) {
      m_numPointsLocked--;
    }

    m_pointTypeCounts[point->GetType()]--;
    validate();

  }

  void ControlNetVitals::addMeasure(ControlMeasure *measure) {
    ControlPoint *point = measure->Parent();
    if (point) {
      // By this time, the measure has been added to its parent point, so the
      // old count is the current count minus one.
      int numValidMeasures = point->GetNumValidMeasures();
      if ( --m_pointMeasureCounts[numValidMeasures - 1] < 1 ) {
        m_pointMeasureCounts.remove(numValidMeasures - 1);
      }
      if ( !m_pointMeasureCounts.contains(numValidMeasures) ) {
        m_pointMeasureCounts.insert(numValidMeasures, 1);
      }
      else {
        m_pointMeasureCounts[numValidMeasures]++;
      }
    }

    QString serial = measure->GetCubeSerialNumber();
    int numValidMeasures = m_controlNet->GetNumberOfValidMeasuresInImage(serial);
    if ( !m_imageMeasureCounts.contains(numValidMeasures) ) {
      m_imageMeasureCounts.insert(numValidMeasures, 1);
    }
    else {
      m_imageMeasureCounts[numValidMeasures]++;
    }

    validate();
  }

  void ControlNetVitals::measureModified(ControlMeasure *measure, ControlMeasure::ModType type, QVariant oldValue, QVariant newValue) {

    switch (type) {
      case ControlMeasure::IgnoredModified:

        if ( !oldValue.toBool() && newValue.toBool() ) {
          return addMeasure(measure);
        }
        else if ( oldValue.toBool() && !newValue.toBool() ) {
          return deleteMeasure(measure);
        }
        break;

      default:
        // No operation.
        break;

    }
    validate();
  }


  void ControlNetVitals::deleteMeasure(ControlMeasure *measure) {

    ControlPoint *point = measure->Parent();
    if (point) {
      // By this time, the measure is still a valid measure in the parent control point.
      int numValidMeasures = point->GetNumValidMeasures();

      if ( --m_pointMeasureCounts[numValidMeasures] < 1 ) {
        m_pointMeasureCounts.remove(numValidMeasures);
      }
      if ( !m_pointMeasureCounts.contains(numValidMeasures - 1) ) {
        m_pointMeasureCounts.insert(numValidMeasures - 1, 1);
      }
      else {
        m_pointMeasureCounts[numValidMeasures - 1]++;
      }
    }

    QString serial = measure->GetCubeSerialNumber();
    int numValidMeasures = m_controlNet->GetNumberOfValidMeasuresInImage(serial);

    if ( --m_pointMeasureCounts[numValidMeasures] < 1 ) {
      m_pointMeasureCounts.remove(numValidMeasures);
    }

    if ( !m_imageMeasureCounts.contains(numValidMeasures - 1) ) {
      m_imageMeasureCounts.insert(numValidMeasures - 1, 1);
    }
    else {
      m_imageMeasureCounts[numValidMeasures - 1]++;
    }

    validate();
  }


  void ControlNetVitals::validateNetwork(ControlNet::ModType type) {
    switch (type) {
      case ControlNet::Swapped:
        initializeVitals();
        break;
      case ControlNet::GraphModified:
        m_islandList = m_controlNet->GetSerialConnections();
        break;
      default:
        // No operation.
        break;
    }

    validate();

  }

  ControlNetVitals::~ControlNetVitals() {
  }


  bool ControlNetVitals::hasIslands() {
    return numIslands() > 1;
  }


  int ControlNetVitals::numIslands() {
    return m_islandList.size();
  }


  const QList< QList<QString> > &ControlNetVitals::getIslands() {
    return m_islandList;
  }


  int ControlNetVitals::numPoints() {
    return m_controlNet->GetNumPoints();
  }


  int ControlNetVitals::numIgnoredPoints() {
    return m_numPointsIgnored;
  }


  int ControlNetVitals::numLockedPoints() {
    return m_numPointsLocked;
  }


  int ControlNetVitals::numFixedPoints() {
    return m_pointTypeCounts[ControlPoint::Fixed];
  }


  int ControlNetVitals::numConstrainedPoints() {
    return m_pointTypeCounts[ControlPoint::Constrained];
  }


  int ControlNetVitals::numFreePoints() {
    return m_pointTypeCounts[ControlPoint::Free];
  }


  int ControlNetVitals::numPointsBelowMeasureThreshold(int num) {
    int count = 0;
    foreach(int measureCount, m_pointMeasureCounts) {
      if (measureCount > num) {
        break;
      }
      count += m_pointMeasureCounts[measureCount];
    }
    return count;
  }

  int ControlNetVitals::numImages() {
    return m_controlNet->GetCubeSerials().size();
  }


  int ControlNetVitals::numMeasures() {
    return m_controlNet->GetNumMeasures();
  }


  int ControlNetVitals::numImagesBelowMeasureThreshold(int num) {
    int count = 0;
    foreach(int measureCount, m_imageMeasureCounts) {
      if (measureCount > num) {
        break;
      }
      count += m_imageMeasureCounts[measureCount];
    }
    return count;
  }


  // REFACTOR
  int ControlNetVitals::numImagesBelowHullTolerance(int tolerance) {
    return 0;
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
      if (point->GetType() == ControlPoint::Fixed) fixedPoints.append(point);
    }
    return fixedPoints;
  }


  QList<ControlPoint*> ControlNetVitals::getConstrainedPoints() {
    QList<ControlPoint*> constrainedPoints;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetType() == ControlPoint::Constrained) constrainedPoints.append(point);
    }
    return constrainedPoints;
  }


  QList<ControlPoint*> ControlNetVitals::getFreePoints() {
    QList<ControlPoint*> freePoints;
    foreach(ControlPoint* point, m_controlNet->GetPoints()) {
      if (point->GetType() == ControlPoint::Free) freePoints.append(point);
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


  void ControlNetVitals::validate() {

    QString status = "";
    QString details = "";
    if (hasIslands()) {
      status = "Broken!";
      details = "This network has " + toString(numIslands()) + " islands.";
    }
    else {

      if (numPointsBelowMeasureThreshold() > 0) {
        status = "Weak!";
        details += "This network has " + toString(numPointsBelowMeasureThreshold()) + " point(s) with less than 3 measures\n";
      }

      if (numImagesBelowMeasureThreshold() > 0) {
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
