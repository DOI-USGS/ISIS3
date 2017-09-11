#ifndef MosaicItem_H
#define MosaicItem_H

#include <QAbstractGraphicsShapeItem>

class QGraphicsPolygonItem;

namespace geos {
  namespace geom {
    class MultiPolygon;
  }
}

namespace Isis {
  class Camera;
  class Image;
  class MosaicSceneWidget;
  class Projection;
  class PvlGroup;
  class Stretch;
  class UniversalGroundMap;

  /**
   * @brief A single cube in the mosaic scene
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *
   *  @history 2010-10-26 Tracie Sucharski - Added missing includes to cpp after removing includes
   *                          from ControlNet.h.
   *  @history 2011-05-07 Steven Lambright - Refactored from MosaicItem to have far fewer 
   *                          responsibilities.
   *  @history 2011-05-10 Steven Lambright - Reduced the amount of useless code
   *  @history 2011-05-11 Steven Lambright - Reduced the amount of useless code, footprint is now
   *                          gathered from the CubeDisplayProperties so duplicate work is not done.
   *  @history 2011-05-17 Steven Lambright - Labels auto-rotate by 90 degrees once again
   *  @history 2015-05-27 Kristin Berry - Updated to work with longitude domain = 180 correctly.
   *                          Fixes #2208.
   *  @history 2016-05-18 Ian Humphrey - Explicitly made outlines cosmetic so that they always
   *                          appear as 1 pixel wide on screen (Qt4 to Qt5).
   *  @history 2017-07-27 Makayla Shepherd - Fixed a segfault that occurred when closing a cube
   *                          footprint. Fixes #5050.
   */
  class MosaicSceneItem : public QGraphicsObject {
      Q_OBJECT
    public:
      MosaicSceneItem(Image *image, MosaicSceneWidget *parent);
      ~MosaicSceneItem();

      virtual QRectF boundingRect() const;
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget = 0);

      QColor color() const;
      Image *image() { return m_image; }
      QGraphicsSimpleTextItem *getLabel() const { return m_label; }

      void reproject();
      bool contains(const QPointF &) const;
      void updateSelection(bool);

      void scenePropertiesChanged() {
        updateChildren();
      }

    signals:
      void colorChanged();
      void changed(const QList<QRectF> &);
      void mosaicCubeClosed(Image *);

    public slots:
      void cubeDisplayChanged();

    protected:
      virtual bool sceneEvent(QEvent *event);

    protected:
      virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    private slots:
      void lostCubeDisplay();
      void onCloseCube();

    private:
      MosaicSceneItem(const MosaicSceneItem &); //!< cannot copy
      MosaicSceneItem & operator=(const MosaicSceneItem &); //!< cannot assign

      void updateChildren();
      Stretch *getStretch();

      MosaicSceneWidget *m_scene;

      geos::geom::MultiPolygon *m_mp; //!< This item's multipolygon in the 0/360 longitude domain
      geos::geom::MultiPolygon *m_180mp; //!< This item's multipolygon in the -180/180 longitude domain
      QList< QGraphicsPolygonItem * > *m_polygons;
      UniversalGroundMap *groundMap;

      void setupFootprint();
      void drawImage(QPainter *painter, const QStyleOptionGraphicsItem *option);

      double getPixelValue(int sample, int line);
      void setupStretch();

      Image *m_image;
      Stretch *m_cubeDnStretch;

      bool m_showingLabel;
      bool m_ignoreCubeDisplayChanged;

      QGraphicsSimpleTextItem *m_label;
  };
};

#endif

