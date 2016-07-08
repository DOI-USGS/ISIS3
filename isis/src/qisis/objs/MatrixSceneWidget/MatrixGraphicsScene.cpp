#include "MatrixGraphicsScene.h"

#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>

#include "MatrixSceneWidget.h"

namespace Isis {
  /**
   * Constructor.
   *
   * Constructs the MatrixGraphicsScene.
   *
   * @param parent Pointer to parent widget, MatrixSceneWidget.
   */
  MatrixGraphicsScene::MatrixGraphicsScene(MatrixSceneWidget *parent) : QGraphicsScene(parent) {
  }


  /**
   * Destructor.
   */
  MatrixGraphicsScene::~MatrixGraphicsScene() {
  }


  /**
   * Handles context menu events for the matrix graphics scene.
   *
   * @param contextMenuEvent Pointer to the context menu event that is being captured.
   */
  void MatrixGraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent) {
    if ( selectedItems().count() < 2 ||
         !qobject_cast<MatrixSceneWidget *>( parent() )->contextMenuEvent(contextMenuEvent) ) {
      QGraphicsScene::contextMenuEvent(contextMenuEvent);
    }
  }


  /**
   * Handles mouse press events for the matrix graphics scene.
   *
   * @param mouseEvent Pointer to the mouse event that is being captured.
   */
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

