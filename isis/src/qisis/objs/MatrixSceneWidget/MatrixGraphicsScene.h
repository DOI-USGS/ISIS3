#ifndef MatrixGraphicsScene_H
#define MatrixGraphicsScene_H

#include <QGraphicsView>

namespace Isis {
  class MatrixSceneWidget;

  /**
   * @brief A graphics scene with improved user-interaction for use with the MatrixSceneWidget
   *
   * The context menu event will behave differently - if items are selected, and you right click
   *   on one of the selected items, the MatrixSceneWidget will be given an opportunity to
   *   handle it (multi-item selection).
   *
   * @author 2012-09-17 Steven Lambright
   *
   * @internal
   *   @history 2014-07-14 Kimberly Oyama - Adapted from MosaicGraphicsScene.
   */
  class MatrixGraphicsScene : public QGraphicsScene {
      Q_OBJECT

    public:
      MatrixGraphicsScene(MatrixSceneWidget *parent);
      virtual ~MatrixGraphicsScene();

    protected:
      virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent);
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
  };
}

#endif

