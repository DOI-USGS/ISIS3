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
  class CubeDisplayProperties;
  class ControlPoint;
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
   * @author Stacy Alley
   *
   * @internal
   *
   *  @history 2010-10-26 Tracie Sucharski Added missing includes to cpp after
   *                                removing includes from ControlNet.h.
   *  @history 2011-05-07 Steven Lambright Refactored from MosaicItem to
   *                                have far fewer responsibilities.
   *  @history 2011-05-10 Steven Lambright Reduced the amount of useless code
   */
  class MosaicSceneItem : public QGraphicsObject {
      Q_OBJECT
    public:
      MosaicSceneItem(CubeDisplayProperties *, MosaicSceneWidget *parent);
      ~MosaicSceneItem();

      virtual QRectF boundingRect() const;
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget = 0);

      void highlightSelected(QGraphicsPolygonItem *item, QPainter *painter,
                             const QStyleOptionGraphicsItem *option);

      double levelOfDetail() const { return p_lastLevelOfDetail; }
      double emissionAngle() const { return p_emissionAngle; }
      double incidenceAngle() const { return p_incidenceAngle; }
      QColor color() const;
      CubeDisplayProperties *cubeDisplay() { return p_cubeDisplay; }
      QGraphicsSimpleTextItem *getLabel() const { return p_label; }
      int getImageTrans() const { return p_imageTransparency; }

      void reproject();
      void setImageVisible(bool visible);
      void setTreeItemSelected(bool selected);
      void setZValue(qreal z);
      QPointF screenToGround(QPointF point);
      QPointF screenToCam(int x, int y);
      QPointF screenToCam(QPointF p);
      void setSelectedPoint(ControlPoint *p);
      bool contains(const QPointF &) const;
      void updateSelection(bool);

      void scenePropertiesChanged() {
        updateChildren();
      }

    signals:
      void colorChanged();
      void changed(const QList<QRectF> &);

    public slots:
      void cubeDisplayChanged();

    protected:
      virtual bool sceneEvent(QEvent *event);
      virtual QVariant itemChange(GraphicsItemChange change,
                                  const QVariant & value);

    protected:
      virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    private slots:
      void lostCubeDisplay();

    private:
      MosaicSceneItem(const MosaicSceneItem &); //!< cannot copy
      MosaicSceneItem & operator=(const MosaicSceneItem &); //!< cannot assign

      void updateChildren();
      Stretch *getStretch();


      MosaicSceneWidget *p_scene;

      geos::geom::MultiPolygon *p_mp; //!< This item's multipolygon in the 0/360 longitude domain
      geos::geom::MultiPolygon *p_180mp; //!< This item's multipolygon in the -180/180 longitude domain
      QList< QGraphicsPolygonItem * > *p_polygons;

      void createFootprint(); //!< Creates the footprint of this image.
      void drawImage(QPainter *painter, const QStyleOptionGraphicsItem *option);
      QList<int> scanLineIntersections(QPolygon poly, int y, int boxWidth);

      bool midTest(double trueMidX, double trueMidY, double testMidX, double testMidY);
      double getPixelValue(int sample, int line);
      void setupStretch();
      void setFontSize();
      void setFontSize(QFont font);
      void setUpItem(PvlGroup *grp);

      void paintControlPoints(QPainter *painter, const QStyleOptionGraphicsItem *option);

      QColor getColor();

      double p_pixRes; //!< Pixel Resolution of the cube
      double p_emissionAngle;
      double p_incidenceAngle;
      double p_levelOfDetail; //!< Level of detail

      double p_maxPixelValue;
      int p_imageTransparency;
      CubeDisplayProperties *p_cubeDisplay;
      Stretch *p_cubeDnStretch;

      QList<QImage> p_lastImages;
      QRectF p_lastExposedRect;
      double p_lastPaintScale;

      QGraphicsSimpleTextItem *p_label;
      double p_lastLevelOfDetail;
      double p_screenResolution;
      bool p_updateFont;
  };
};

#endif

