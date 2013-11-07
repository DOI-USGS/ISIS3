#include "MosaicGraphicsScene.h"

#include <QGraphicsSceneMouseEvent>

#include "MosaicSceneWidget.h"

namespace Isis {
  MosaicGraphicsScene::MosaicGraphicsScene(MosaicSceneWidget *parent) : QGraphicsScene(parent) {
  }

  MosaicGraphicsScene::~MosaicGraphicsScene() {
  }


  void MosaicGraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent) {
    if (selectedItems().count() < 2 ||
        !qobject_cast<MosaicSceneWidget *>(parent())->contextMenuEvent(contextMenuEvent)) {
      QGraphicsScene::contextMenuEvent(contextMenuEvent);
    }
  }


  void MosaicGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (mouseEvent->button() == Qt::RightButton) {
      // Do nothing on right click... this prevents the loss of selection before a context event
      mouseEvent->accept();
    }
    else {
      QGraphicsScene::mousePressEvent(mouseEvent);
    }
  }
}

