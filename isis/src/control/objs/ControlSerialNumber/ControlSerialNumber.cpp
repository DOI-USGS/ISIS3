#include "ControlSerialNumber.h"

#include "ControlMeasure.h"
#include "iException.h"
#include "iString.h"

#include <QHash>

namespace Isis {

  /**
  * Create an empty SerialNumber object.
  */
  ControlSerialNumber::ControlSerialNumber(QString sn) {
    serialNumber = sn;
    measures = new QHash<QString, ControlMeasure *>;
  }

  /**
   * Destroy a SerialNumber object.
  */
  ControlSerialNumber::~ControlSerialNumber() {
    if (measures != NULL) {
      delete measures;
      measures = NULL;
    }
  }


  void ControlSerialNumber::AddMeasure(QString parentPointId,
      ControlMeasure * measure) {
    if (measure->GetCubeSerialNumber() != iString(serialNumber)) {
      iString msg = "Attempted to add Control Measure with Cube Serial Number [";
      msg += measure->GetCubeSerialNumber() + "] does not match Serial Number [";
      msg += iString(serialNumber) + "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    measure->ConnectControlSN(this);
    (*measures)[parentPoint] = measure;
  }


  void ControlSerialNumber::RemoveMeasure(QString parentPoint) {
    (*measures)[parentPoint]->DisconnectControlSN();
    measures->remove(parentPoint);
  }


  QString ControlSerialNumber::GetSerialNumber() {
    return serialNumber;
  }
}
