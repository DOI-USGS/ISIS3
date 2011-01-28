#include "ControlSerialNumber.h"

#include "ControlMeasure.h"
#include "iException.h"
#include "iString.h"

#include <QHash>
#include <QString>

namespace Isis {

  /**
  * Create an empty SerialNumber object.
  */
  ControlSerialNumber::ControlSerialNumber(iString sn) {
    Nullify();

    serialNumber = new iString(sn);
    measures = new QHash<QString, ControlMeasure *>;
  }


  ControlSerialNumber::ControlSerialNumber(const ControlSerialNumber &other) {
    Nullify();

    serialNumber = new iString(*other.serialNumber);
    measures = new QHash<QString, ControlMeasure *>;

    *measures = *other.measures;
  }


  void ControlSerialNumber::Nullify() {
    serialNumber = NULL;
    measures = NULL;
  }


  /**
   * Destroy a SerialNumber object.
  */
  ControlSerialNumber::~ControlSerialNumber() {
    if (serialNumber) {
      delete serialNumber;
      serialNumber = NULL;
    }

    if (measures != NULL) {
      delete measures;
      measures = NULL;
    }
  }


  void ControlSerialNumber::AddMeasure(iString parentPointId,
                                       ControlMeasure *measure) {
    if (measure->GetCubeSerialNumber() != *serialNumber) {
      iString msg = "Attempted to add Control Measure with Cube Serial Number ";
      msg += "[" + measure->GetCubeSerialNumber() + "] does not match Serial ";
      msg += "Number [" + *serialNumber + "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    measure->ConnectControlSN(this);
    (*measures)[parentPointId] = measure;
  }


  bool ControlSerialNumber::Contains(iString parentPointId) {
    return measures->contains(parentPointId);
  }


  void ControlSerialNumber::RemoveMeasure(iString parentPointId) {
    (*measures)[parentPointId]->DisconnectControlSN();
    measures->remove(parentPointId);
  }


  int ControlSerialNumber::GetNumMeasures() {
    return measures->size();
  }


  iString ControlSerialNumber::GetSerialNumber() {
    return *serialNumber;
  }


  QList< QString > ControlSerialNumber::GetPointIds() const {
    return measures->keys();
  }
  
  
  QList< ControlMeasure * > ControlSerialNumber::GetMeasures() const {
    return measures->values();
  }
  
  
  ControlMeasure *ControlSerialNumber::GetMeasure(iString pointId) {
    if (!measures->contains(pointId)) {
      iString msg = "point Id [";
      msg += (iString) pointId;
      msg += "] not found in the ControlSerialNumber";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return (*measures)[pointId];
  }


  const ControlMeasure *ControlSerialNumber::GetMeasure(iString pointId) const {
    if (!measures->contains(pointId)) {
      iString msg = "point Id [";
      msg += (iString) pointId;
      msg += "] not found in the ControlSerialNumber";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return measures->value(pointId);
  }


  ControlMeasure *ControlSerialNumber::operator[](iString pointId) {
    return GetMeasure(pointId);
  }


  const ControlMeasure *ControlSerialNumber::operator[](iString pointId) const {
    return GetMeasure(pointId);
  }

  const ControlSerialNumber &ControlSerialNumber::operator=(
    ControlSerialNumber other) {
    if (this == &other)
      return *this;

    if (serialNumber) {
      delete serialNumber;
      serialNumber = NULL;
    }

    if (measures != NULL) {
      delete measures;
      measures = NULL;
    }

    serialNumber = new iString;
    measures = new QHash<QString, ControlMeasure *>;

    *serialNumber = *other.serialNumber;
    *measures = *other.measures;

    return *this;
  }
}
