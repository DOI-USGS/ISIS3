#include "MatrixGraphicsScene.h"

#include <QGraphicsSceneMouseEvent>

#include "MatrixSceneWidget.h"

namespace Isis {
  MatrixGraphicsScene::MatrixGraphicsScene(MatrixSceneWidget *parent) : QGraphicsScene(parent) {
  }

  MatrixGraphicsScene::~MatrixGraphicsScene() {
  }


  void MatrixGraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent) {
    if ( selectedItems().count() < 2 ||
         !qobject_cast<MatrixSceneWidget *>( parent() )->contextMenuEvent(contextMenuEvent) ) {
      QGraphicsScene::contextMenuEvent(contextMenuEvent);
    }
  }


  void MatrixGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (mouseEvent->button() == Qt::RightButton) {
      // Do nothing on right click... this prevents the loss of selection before a context event
      mouseEvent->accept();
    }
    else {
      QGraphicsScene::mousePressEvent(mouseEvent);
      emit selectionChanged();
    }
  }
}

