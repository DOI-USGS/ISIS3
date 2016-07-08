#ifndef MatrixGraphicsView_H
#define MatrixGraphicsView_H

#include <QGraphicsView>

class QContextMenuEvent;
class QGraphicsScene;
class QResizeEvent;

namespace Isis {
  /**
   * @brief A graphics view that resizes in a more friendly way
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2014-07-14 Kimberly Oyama - Adapted from MosaicGraphicsView.
   *   @history 2016-07-08 Ian Humphrey - Updated documentation and coding standards in preparing
   *                           to add to trunk. Fixes #4095, #4081.
   */
  class MatrixGraphicsView : public QGraphicsView {
      Q_OBJECT

    public:
      MatrixGraphicsView(QGraphicsScene *scene, QWidget *parent = 0);
      virtual ~MatrixGraphicsView();

      /**
       * Sets whether or not to allow resizing of the view.
       *
       * @param enabled Sets whether or not resize zooming is enabled.
       */
      void enableResizeZooming(bool enabled) {
        p_resizeZooming = enabled;
      }

    protected:
      virtual void contextMenuEvent(QContextMenuEvent *event);
      virtual void resizeEvent(QResizeEvent *event);

    private:
      bool p_resizeZooming; //!< Whether or not to allow resizing of the view
  };
}

#endif

