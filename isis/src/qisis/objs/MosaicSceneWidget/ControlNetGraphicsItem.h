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
   *   @history 2014-06-02 Tracie Sucharski - Added documentation to the header file regarding
   *                           pointToScene method and member variable.
   *   @history 2016-10-20 Tracie Sucharski & Kim Oyama - Added functionality for IPCE, including
   *                           findClosestControlPoint method and making buildChildren and
   *                           clearControlPointGraphicsItem public slots.
   *                           Fixes #4479
   *   @history 2017-08-15 Tracie Sucharski - Fixed possible seg fault in findClosestControlPoint()
   *                           when clicking away from a control point.  Changed
   *                           findClosestControlPoint to take the point location to search for
   *                           a control point.  Removed mouseReleaseEvent as it is not being used.
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

      ControlPoint *findClosestControlPoint(QPointF locationPoint);

    public slots:
      void buildChildren();
      void clearControlPointGraphicsItem(QString pointId);

    private:
      //  Returns apriori x/y in first point, adjusted x/y in 2nd point
      QPair<QPointF, QPointF> pointToScene(ControlPoint *);

      ControlNet *m_controlNet;

      MosaicSceneWidget *m_mosaicScene;

      //  m_pointToScene contains lat/lon coordinates, 1st QPointF = apriori; 2nd QPointF = adjusted.
      //  There will always be and apriori point, but if not adjusted exists, then it is set to the
      //  apriori.
      QMap<ControlPoint *, QPair<QPointF, QPointF> > *m_pointToScene;

      QMap<QString, UniversalGroundMap *> *m_cubeToGroundMap;
      SerialNumberList *m_serialNumbers;
  };
}

#endif

