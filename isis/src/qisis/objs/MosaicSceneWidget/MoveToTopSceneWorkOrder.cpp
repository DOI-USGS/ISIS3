#include "IsisDebug.h"
#include "MoveToTopSceneWorkOrder.h"

#include "MosaicSceneWidget.h"

namespace Isis {

  MoveToTopSceneWorkOrder::MoveToTopSceneWorkOrder(MosaicSceneWidget *scene, Project *project) :
      MosaicSceneWorkOrder(tr("Bring to Front"), scene, project) {
  }


  MoveToTopSceneWorkOrder::MoveToTopSceneWorkOrder(Project *project) :
      MosaicSceneWorkOrder(project) {
  }


  MoveToTopSceneWorkOrder::MoveToTopSceneWorkOrder(const MoveToTopSceneWorkOrder &other) :
      MosaicSceneWorkOrder(other) {
  }


  MoveToTopSceneWorkOrder::~MoveToTopSceneWorkOrder() {
  }


  MoveToTopSceneWorkOrder *MoveToTopSceneWorkOrder::clone() const {
    return new MoveToTopSceneWorkOrder(*this);
  }


  void MoveToTopSceneWorkOrder::syncRedo() {
    storeZPositions( scene()->moveToTop(imageList()) );
  }


  void MoveToTopSceneWorkOrder::syncUndo() {
    restoreZPositions(false);
  }
}
