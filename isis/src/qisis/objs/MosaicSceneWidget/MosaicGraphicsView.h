#ifndef MosaicGraphicsView_H
#define MosaicGraphicsView_H

#include <QGraphicsView>

namespace Isis {
  /**
   * @brief A graphics view that resizes in a more friendly way
   *
   */
  class MosaicGraphicsView : public QGraphicsView {
      Q_OBJECT

    public:
      MosaicGraphicsView(QGraphicsScene *scene, QWidget *parent = 0);
      virtual ~MosaicGraphicsView();

      void enableResizeZooming(bool enabled) {
        p_resizeZooming = enabled;
      }

    protected:
      virtual void contextMenuEvent(QContextMenuEvent *event);
      virtual void resizeEvent(QResizeEvent *event);

    private:
      bool p_resizeZooming;
  };
}

#endif

