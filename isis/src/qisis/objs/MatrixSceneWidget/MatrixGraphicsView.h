#ifndef MatrixGraphicsView_H
#define MatrixGraphicsView_H

#include <QGraphicsView>

namespace Isis {
  /**
   * @brief A graphics view that resizes in a more friendly way
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2014-07-14 Kimberly Oyama - Adapted from MosaicGraphicsView.
   */
  class MatrixGraphicsView : public QGraphicsView {
      Q_OBJECT

    public:
      MatrixGraphicsView(QGraphicsScene *scene, QWidget *parent = 0);
      virtual ~MatrixGraphicsView();

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

