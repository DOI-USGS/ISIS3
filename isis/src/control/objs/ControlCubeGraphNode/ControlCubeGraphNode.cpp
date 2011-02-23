#include "IsisDebug.h"

#include "ControlCubeGraphNode.h"

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
  ControlCubeGraphNode::ControlCubeGraphNode(iString sn) {
    nullify();

    serialNumber = new iString(sn);
    measures = new QHash<ControlPoint *, ControlMeasure *>;
    connections = new QHash < ControlCubeGraphNode *,
    QList< ControlMeasure * > >;
  }


  ControlCubeGraphNode::ControlCubeGraphNode(const ControlCubeGraphNode &other) {
    nullify();

    serialNumber = new iString(*other.serialNumber);
    measures = new QHash<ControlPoint *, ControlMeasure *>;
    connections = new QHash < ControlCubeGraphNode *,
    QList< ControlMeasure * > >;

    *measures = *other.measures;
  }


  void ControlCubeGraphNode::nullify() {
    serialNumber = NULL;
    measures = NULL;
    connections = NULL;
  }


  /**
   * Destroy a SerialNumber object.
  */
  ControlCubeGraphNode::~ControlCubeGraphNode() {
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
  bool ControlCubeGraphNode::contains(ControlPoint *point) {
    return measures->contains(point);
  }


  /**
   * Adds a measure
   *
   * @param measure The ControlMeasure to add
   */
  void ControlCubeGraphNode::addMeasure(ControlMeasure *measure) {
    ASSERT(measure != NULL);
    if (measure->GetCubeSerialNumber() != *serialNumber) {
      iString msg = "Attempted to add Control Measure with Cube Serial Number ";
      msg += "[" + measure->GetCubeSerialNumber() + "] does not match Serial ";
      msg += "Number [" + *serialNumber + "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    measure->associatedCSN = this;
    updateConnections(&ControlCubeGraphNode::addConnection, measure);
    (*measures)[measure->Parent()] = measure;
  }


  void ControlCubeGraphNode::removeMeasure(ControlMeasure *measure) {
    measures->remove(measure->Parent());
    updateConnections(&ControlCubeGraphNode::removeConnection, measure);
    measure->associatedCSN = NULL;
  }


  void ControlCubeGraphNode::updateConnections(void
      (ControlCubeGraphNode::*updateFunc)(ControlMeasure *),
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


  void ControlCubeGraphNode::addConnection(ControlMeasure *measure) {
    ASSERT(measure);
    ControlCubeGraphNode *csn = measure->ControlSN();
    ASSERT(csn);

    if (connections->contains(csn)) {
      QList< ControlMeasure * > & measureList = (*connections)[csn];
      if (!measureList.contains(measure))
        measureList.append(measure);
    }
    else {
      QList< ControlMeasure * > measureList;
      measureList.append(measure);
      connections->insert(csn, measureList);
    }
  }


  void ControlCubeGraphNode::removeConnection(ControlMeasure *measure) {
    ASSERT(measure);
    ControlCubeGraphNode *csn = measure->ControlSN();
    ASSERT(csn);

    if (connections->contains(csn)) {
      QList< ControlMeasure * > & measureList = (*connections)[csn];
      ASSERT(measureList.count(measure) == 1);
      measureList.removeAt(measureList.indexOf(measure));

      if (!measureList.size())
        connections->remove(csn);
    }
  }


  int ControlCubeGraphNode::size() {
    return measures->size();
  }


  iString ControlCubeGraphNode::getSerialNumber() {
    return *serialNumber;
  }


  QList< ControlMeasure * > ControlCubeGraphNode::getMeasures() const {
    return measures->values();
  }


  QList< QString > ControlCubeGraphNode::getAdjacentSerials() const {
    QList< QString > adjacentSerials;

    QList< ControlCubeGraphNode * > neighbors = connections->keys();
    for (int i = 0; i < neighbors.size(); i++)
      adjacentSerials.append(neighbors[i]->getSerialNumber());

    return adjacentSerials;
  }


  bool ControlCubeGraphNode::isConnected(ControlCubeGraphNode *other) const {
    return connections->contains(other);
  }


  ControlMeasure *ControlCubeGraphNode::getMeasure(ControlPoint *point) {
    if (!measures->contains(point)) {
      iString msg = "point [";
      msg += (iString) point->GetId();
      msg += "] not found in the ControlCubeGraphNode";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return (*measures)[point];
  }


  const ControlMeasure *ControlCubeGraphNode::getMeasure(
    ControlPoint *point) const {
    if (!measures->contains(point)) {
      iString msg = "point [";
      msg += (iString) point->GetId();
      msg += "] not found in the ControlCubeGraphNode";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return measures->value(point);
  }


  ControlMeasure *ControlCubeGraphNode::operator[](ControlPoint *point) {
    return getMeasure(point);
  }


  const ControlMeasure *ControlCubeGraphNode::operator[](
    ControlPoint *point) const {
    return getMeasure(point);
  }


  const ControlCubeGraphNode &ControlCubeGraphNode::operator=(
    ControlCubeGraphNode other) {
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
    connections = new QHash< ControlCubeGraphNode *, QList< ControlMeasure * > >;

    *serialNumber = *other.serialNumber;
    *measures = *other.measures;
    *connections = *other.connections;

    return *this;
  }
}
