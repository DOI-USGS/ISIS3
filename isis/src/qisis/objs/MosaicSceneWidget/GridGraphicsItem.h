#ifndef GridGraphicsItem_h
#define GridGraphicsItem_h

#include <QGraphicsItem>

#include <QScopedPointer>

class QPointF;

template<typename A> class QList;

namespace Isis {
  class Angle;
  class GroundGrid;
  class Latitude;
  class Longitude;
  class MosaicSceneWidget;
  class UniversalGroundMap;

  /**
   * @brief The visual display of the find point
   *
   * @author 2011-05-07 Steven Lambright
   *
   * @internal
   *   @history 2011-05-10 Steven Lambright - Fixed problem with boundingRect()
   */
  class GridGraphicsItem : public QGraphicsItem {
    public:
      GridGraphicsItem(Latitude baseLat, Longitude baseLon,
                       Angle latInc, Angle lonInc,
                       MosaicSceneWidget *projectionSrc, int density);
      virtual ~GridGraphicsItem();

      void paint(QPainter *, const QStyleOptionGraphicsItem *,
                 QWidget * widget = 0);

      virtual QRectF boundingRect() const;

    private:
      QRectF calcRect() const;
      QRectF rect() const;
      void setRect(QRectF newBoundingRect);

    private:
      QRectF m_boundingRect;
  };
}

#endif

