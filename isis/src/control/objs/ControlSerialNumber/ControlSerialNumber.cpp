#include "IsisDebug.h"

#include "ControlSerialNumber.h"

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "iException.h"
#include "iString.h"

#include <QHash>
#include <QString>

namespace Isis {

  /**
  * Create an empty SerialNumber object.
  */
  ControlSerialNumber::ControlSerialNumber(iString sn) {
    nullify();

    serialNumber = new iString(sn);
    measures = new QHash<ControlPoint *, ControlMeasure *>;
    connections = new QHash< ControlSerialNumber *, QList< ControlMeasure * > >;
  }


  ControlSerialNumber::ControlSerialNumber(const ControlSerialNumber &other) {
    nullify();

    serialNumber = new iString(*other.serialNumber);
    measures = new QHash<ControlPoint *, ControlMeasure *>;
    connections = new QHash< ControlSerialNumber *, QList< ControlMeasure * > >;

    *measures = *other.measures;
  }


  void ControlSerialNumber::nullify() {
    serialNumber = NULL;
    measures = NULL;
    connections = NULL;
  }


  /**
   * Destroy a SerialNumber object.
  */
  ControlSerialNumber::~ControlSerialNumber() {
    if (serialNumber) {
      delete serialNumber;
      serialNumber = NULL;
    }

    if (measures) {
      delete measures;
      measures = NULL;
    }

    if (connections) {
      delete connections;
      connections = NULL;
    }
  }


  /**
   * @param point The ControlPoint to check for
   *
   * @returns true if the point is contained, false otherwise
   */
  bool ControlSerialNumber::contains(ControlPoint *point) {
    return measures->contains(point);
  }


  /**
   * Adds a measure
   *
   * @param measure The ControlMeasure to add
   */
  void ControlSerialNumber::addMeasure(ControlMeasure *measure) {
    ASSERT(measure != NULL);
    if (measure->GetCubeSerialNumber() != *serialNumber) {
      iString msg = "Attempted to add Control Measure with Cube Serial Number ";
      msg += "[" + measure->GetCubeSerialNumber() + "] does not match Serial ";
      msg += "Number [" + *serialNumber + "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    measure->associatedCSN = this;
    updateConnections(&ControlSerialNumber::addConnection, measure);
    (*measures)[measure->Parent()] = measure;
  }


  void ControlSerialNumber::removeMeasure(ControlMeasure *measure) {
    measures->remove(measure->Parent());
    updateConnections(&ControlSerialNumber::removeConnection, measure);
    measure->associatedCSN = NULL;
  }


  void ControlSerialNumber::updateConnections(
    void (ControlSerialNumber::*updateFunc)(ControlMeasure *),
    ControlMeasure *measure) {
    QList< ControlPoint * > keys = measures->keys();
    for (int i = 0; i < keys.size(); i++) {
      ControlPoint *cp = keys[i];
      for (int j = 0; j < cp->GetNumMeasures(); j++) {
        ControlMeasure *cm = cp->GetMeasure(j);
        if (cm->GetCubeSerialNumber() != *serialNumber) {
          ASSERT(cm->ControlSN() != NULL);

          if (cm->ControlSN()) {
            (this->*updateFunc)(cm);
            (cm->ControlSN()->*updateFunc)(measure);
          }
        }
      }
    }
  }


  void ControlSerialNumber::addConnection(ControlMeasure *measure) {
    ASSERT(measure);
    ControlSerialNumber *csn = measure->ControlSN();
    ASSERT(csn);

    if (connections->contains(csn)) {
      QList< ControlMeasure * > & measureList = (*connections)[csn];
      if (measureList.contains(measure)) {
        iString msg = "Connection already exists!";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }

      measureList.append(measure);
    }
    else {
      QList< ControlMeasure * > measureList;
      measureList.append(measure);
      connections->insert(csn, measureList);
    }
  }


  void ControlSerialNumber::removeConnection(ControlMeasure *measure) {
    ASSERT(measure);
    ControlSerialNumber *csn = measure->ControlSN();
    ASSERT(csn);

    if (!connections->contains(csn)) {
      iString msg = "Can not remove non-existent connection!";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    QList< ControlMeasure * > & measureList = (*connections)[csn];
    ASSERT(measureList.count(measure) == 1);
    measureList.removeAt(measureList.indexOf(measure));

    if (!measureList.size())
      connections->remove(csn);
  }


  int ControlSerialNumber::size() {
    return measures->size();
  }


  iString ControlSerialNumber::getSerialNumber() {
    return *serialNumber;
  }


  QList< ControlMeasure * > ControlSerialNumber::getMeasures() const {
    return measures->values();
  }


  ControlMeasure *ControlSerialNumber::getMeasure(ControlPoint *point) {
    if (!measures->contains(point)) {
      iString msg = "point [";
      msg += (iString) point->GetId();
      msg += "] not found in the ControlSerialNumber";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return (*measures)[point];
  }


  const ControlMeasure *ControlSerialNumber::getMeasure(
    ControlPoint *point) const {
    if (!measures->contains(point)) {
      iString msg = "point [";
      msg += (iString) point->GetId();
      msg += "] not found in the ControlSerialNumber";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return measures->value(point);
  }


  ControlMeasure *ControlSerialNumber::operator[](ControlPoint *point) {
    return getMeasure(point);
  }


  const ControlMeasure *ControlSerialNumber::operator[](
    ControlPoint *point) const {
    return getMeasure(point);
  }


  const ControlSerialNumber &ControlSerialNumber::operator=(
    ControlSerialNumber other) {
    if (this == &other)
      return *this;

    if (serialNumber) {
      delete serialNumber;
      serialNumber = NULL;
    }

    if (measures) {
      delete measures;
      measures = NULL;
    }

    if (connections) {
      delete connections;
      connections = NULL;
    }

    serialNumber = new iString;
    measures = new QHash< ControlPoint *, ControlMeasure *>;
    connections = new QHash< ControlSerialNumber *, QList< ControlMeasure * > >;

    *serialNumber = *other.serialNumber;
    *measures = *other.measures;
    *connections = *other.connections;

    return *this;
  }
}
