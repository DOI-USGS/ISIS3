#include "MosaicGraphicsScene.h"

#include <QGraphicsSceneMouseEvent>

#include "MosaicSceneWidget.h"

namespace Isis {
  MosaicGraphicsScene::MosaicGraphicsScene(MosaicSceneWidget *parent) : QGraphicsScene(parent) {

    m_parent = parent;
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
      //qDebug()<<"MosaicGraphicsScene::mousePressEvent  Right Button";
      // Do nothing on right click... this prevents the loss of selection before a context event
      if (m_parent->isControlNetToolActive()) {
        //qDebug()<<"MosaicGraphicsScene::mousePressEvent  Right Button  cnetTool active";
        QGraphicsScene::mousePressEvent(mouseEvent);
      }
      else {
        mouseEvent->accept();
      }
    }
    else {
      QGraphicsScene::mousePressEvent(mouseEvent);
    }
  }
}

