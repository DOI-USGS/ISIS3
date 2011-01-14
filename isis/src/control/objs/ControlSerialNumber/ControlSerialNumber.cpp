#include "ControlSerialNumber.h"
#include "ControlMeasure.h"

#include <QHash>
#include <QString>

namespace Isis {

  /**
  * Create an empty SerialNumber object.
  */
  ControlSerialNumber::ControlSerialNumber() {
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


  void ControlSerialNumber::AddMeasure(QString parentPoint,
      ControlMeasure * measure) {
    measure->ConnectControlSN(this);
    (*measures)[parentPoint] = measure;
  }


  void ControlSerialNumber::RemoveMeasure(QString parentPoint) {
    (*measures)[parentPoint]->DisconnectControlSN();
    measures->remove(parentPoint);
  }
}
