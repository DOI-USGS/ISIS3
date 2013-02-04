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
   *   @history 2012-04-16 Jeannie Backer - Added #include for Pvl class in
   *                           implementation file.
   *   @history 2013-01-02 Steven Lambright - Updated setArrowVisible() to support new coloring
   *                           options. The design of this configuration is wrong/needs fixed, but
   *                           I'm leaving it alone due to time constraints. Fixes #479.
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
      QString snToFileName(QString sn);

      void setArrowsVisible(bool visible, bool colorByMeasureCount, int measureCount,
                            bool colorByJigsawError, double residualMagnitude);

    private slots:
      void buildChildren();

    private:
      QPair<QPointF, QPointF> pointToScene(ControlPoint *);

      ControlNet *m_controlNet;

      MosaicSceneWidget *m_mosaicScene;
      QMap<ControlPoint *, QPair<QPointF, QPointF> > *m_pointToScene;
      QMap<QString, UniversalGroundMap *> *m_cubeToGroundMap;
      SerialNumberList *m_serialNumbers;
  };
}

#endif

