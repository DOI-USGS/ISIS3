#ifndef MosaicGraphicsView_H
#define MosaicGraphicsView_H

#include <QGraphicsView>
#include <QSize>

namespace Isis {
  /**
   * @brief A graphics view that resizes in a more friendly way
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2017-01-27 Tracie Sucharski - Added member variable to save the size of the graphics
   *                          view, because there is a bug in Qt's QResizeEvent::oldSize() method.
   *                          This only happens when this is used from ipce in a QMdiSubWindow.
   *                          See https://bugreports.qt.io/browse/QTBUG-32446.
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
      QSize m_oldSize;
  };
}

#endif

