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
   *   @history 2012-07-10 Kimberly Oyama - Added the latitude and longitude extents as parameters
   *                           to the constructor to support the new grid functionallity of user
   *                           specified extents (map projection, bounding rectangle of the open
   *                           cubes, manually entered). Also modified the constructor so it can
   *                           handle drawing grids for both planetocentric and planetographic
   *                           latitudes. Fixes #604.
   *   @history 2013-02-06 Steven Lambright - Added a safety check to make sure the increments
   *                           aren't zero or negative. References #1060.
   *   @history 2013-03-06 Steven Lambright - Added support for getting target radii from the
   *                           TargetName mapping keyword if the target radii keywords are missing.
   *                           References #1534.
   *   @history 2016-05-18 Ian Humphrey - Made the latitude and longitude lines explicitly cosmetic
   *                           so they always appear as 1 pixel wide on screen (Qt4 to Qt5).
   */
  class GridGraphicsItem : public QGraphicsItem {
    public:
      GridGraphicsItem(Latitude baseLat, Longitude baseLon,
                       Angle latInc, Angle lonInc,
                       MosaicSceneWidget *projectionSrc, int density,
                       Latitude latMin, Latitude latMax,
                       Longitude lonMin, Longitude lonMax);
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

