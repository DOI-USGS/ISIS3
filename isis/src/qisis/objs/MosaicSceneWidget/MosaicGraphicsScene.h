#ifndef MosaicGraphicsScene_H
#define MosaicGraphicsScene_H

#include <QGraphicsView>

namespace Isis {
  class MosaicSceneWidget;

  /**
   * @brief A graphics scene with improved user-interaction for use with the MosaicSceneWidget
   *
   * The context menu event will behave differently - if items are selected, and you right click
   *   on one of the selected items, the MosaicSceneWidget will be given an opportunity to
   *   handle it (multi-item selection).
   *
   * @author 2012-09-17 Steven Lambright
   *
   * @internal
   */
  class MosaicGraphicsScene : public QGraphicsScene {
      Q_OBJECT

    public:
      MosaicGraphicsScene(MosaicSceneWidget *parent);
      virtual ~MosaicGraphicsScene();

    protected:
      virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent);
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
  };
}

#endif

