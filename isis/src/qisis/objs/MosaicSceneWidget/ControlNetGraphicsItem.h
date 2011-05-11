#ifndef ControlNetGraphicsItem_h
#define ControlNetGraphicsItem_h

#include <QGraphicsObject>

namespace Isis {
  class ControlNet;
  class ControlPoint;
  class MosaicSceneWidget;
  class Projection;
  class SerialNumberList;
  class UniversalGroundMap;

  /**
   * @brief Control Network Display on Mosaic Scene
   *
   * @author 2011-05-07 Steven Lambright
   *
   * @internal
   *   @history 2011-05-10 Steven Lambright - Added arrow capabilities for CPs
   */
  class ControlNetGraphicsItem : public QGraphicsObject {
      Q_OBJECT

    public:
      ControlNetGraphicsItem(ControlNet *controlNet,
                             MosaicSceneWidget *mosaicScene);
      virtual ~ControlNetGraphicsItem();

      QRectF boundingRect() const;
      void paint(QPainter *, const QStyleOptionGraphicsItem *,
                 QWidget * widget = 0);
      QString snToFilename(QString sn);

    private slots:
      void buildChildren();

    private:
      QPair<QPointF, QPointF> pointToScene(ControlPoint *);

      ControlNet *p_controlNet;

      MosaicSceneWidget *p_mosaicScene;
      QMap<ControlPoint *, QPair<QPointF, QPointF> > *p_pointToScene;
      QMap<QString, UniversalGroundMap *> *p_cubeToGroundMap;
      SerialNumberList *p_serialNumbers;
  };
}

#endif

