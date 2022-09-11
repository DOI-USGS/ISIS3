#include "MoveUpOneSceneWorkOrder.h"

#include "MosaicSceneWidget.h"

namespace Isis {

  MoveUpOneSceneWorkOrder::MoveUpOneSceneWorkOrder(MosaicSceneWidget *scene, Project *project) :
      MosaicSceneWorkOrder(tr("Bring Forward"), scene, project) {
  }


  MoveUpOneSceneWorkOrder::MoveUpOneSceneWorkOrder(Project *project) :
      MosaicSceneWorkOrder(project) {
  }


  MoveUpOneSceneWorkOrder::MoveUpOneSceneWorkOrder(const MoveUpOneSceneWorkOrder &other) :
      MosaicSceneWorkOrder(other) {
  }


  MoveUpOneSceneWorkOrder::~MoveUpOneSceneWorkOrder() {
  }


  MoveUpOneSceneWorkOrder *MoveUpOneSceneWorkOrder::clone() const {
    return new MoveUpOneSceneWorkOrder(*this);
  }


  void MoveUpOneSceneWorkOrder::execute() {
    storeZPositions( scene()->moveUpOne(imageList()) );
  }


  void MoveUpOneSceneWorkOrder::undoExecution() {
    restoreZPositions(true);
  }
}
