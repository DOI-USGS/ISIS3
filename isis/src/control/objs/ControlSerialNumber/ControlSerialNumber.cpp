#include "ControlSerialNumber.h"
#include "ControlMeasure.h"

#include <QList>

namespace Isis {

  /**
  * Create an empty SerialNumber object.
  */
  ControlSerialNumber::ControlSerialNumber() {
    measures = new QList<ControlMeasure *>;
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


  void ControlSerialNumber::AddMeasure(ControlMeasure * measure) {
    measures->append(measure);
  }


  void ControlSerialNumber::RemoveMeasure(ControlMeasure * measure) {
    for (int i = 0; i < measures->size(); i++) {
      if ((*measures)[i] == measure) {
        measures->removeAt(i);
        break;
      }
    }
  }


  void ControlSerialNumber::RemoveMeasure(int index) {
    measures->removeAt(index);
  }

}
