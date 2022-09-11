#include "MoveDownOneSceneWorkOrder.h"

#include "MosaicSceneWidget.h"

namespace Isis {

  MoveDownOneSceneWorkOrder::MoveDownOneSceneWorkOrder(MosaicSceneWidget *scene, Project *project) :
      MosaicSceneWorkOrder(tr("Send Backward"), scene, project) {
  }


  MoveDownOneSceneWorkOrder::MoveDownOneSceneWorkOrder(Project *project) :
      MosaicSceneWorkOrder(project) {
  }


  MoveDownOneSceneWorkOrder::MoveDownOneSceneWorkOrder(const MoveDownOneSceneWorkOrder &other) :
      MosaicSceneWorkOrder(other) {
  }


  MoveDownOneSceneWorkOrder::~MoveDownOneSceneWorkOrder() {
  }


  MoveDownOneSceneWorkOrder *MoveDownOneSceneWorkOrder::clone() const {
    return new MoveDownOneSceneWorkOrder(*this);
  }


  void MoveDownOneSceneWorkOrder::execute() {
    storeZPositions( scene()->moveDownOne(imageList()) );
  }


  void MoveDownOneSceneWorkOrder::undoExecution() {
    restoreZPositions(true);
  }
}
