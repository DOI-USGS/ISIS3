#include "MoveToBottomSceneWorkOrder.h"

#include "MosaicSceneWidget.h"

namespace Isis {

  MoveToBottomSceneWorkOrder::MoveToBottomSceneWorkOrder(MosaicSceneWidget *scene,
      Project *project) :
      MosaicSceneWorkOrder(tr("Send to Back"), scene, project) {
  }


  MoveToBottomSceneWorkOrder::MoveToBottomSceneWorkOrder(Project *project) :
      MosaicSceneWorkOrder(project) {
  }


  MoveToBottomSceneWorkOrder::MoveToBottomSceneWorkOrder(
      const MoveToBottomSceneWorkOrder &other) : MosaicSceneWorkOrder(other) {
  }


  MoveToBottomSceneWorkOrder::~MoveToBottomSceneWorkOrder() {
  }


  MoveToBottomSceneWorkOrder *MoveToBottomSceneWorkOrder::clone() const {
    return new MoveToBottomSceneWorkOrder(*this);
  }


  void MoveToBottomSceneWorkOrder::execute() {
    storeZPositions( scene()->moveToBottom(imageList()) );
  }


  void MoveToBottomSceneWorkOrder::undoExecution() {
    restoreZPositions(false);
  }
}
