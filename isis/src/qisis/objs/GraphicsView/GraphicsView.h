#include <QGraphicsView>

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class GraphicsView : public QGraphicsView {
     Q_OBJECT
    public:
      GraphicsView(QGraphicsScene *scene, QWidget *parent = 0) :
        QGraphicsView(scene, parent) {}

    protected:
      virtual void resizeEvent(QResizeEvent *event);
  };
}

