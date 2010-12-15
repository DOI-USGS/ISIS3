#ifndef MosaicItem_H
#define MosaicItem_H

#include <QBitmap>
#include <QColor>
#include <QFont>
#include <QGraphicsPolygonItem>
#include <QGraphicsSimpleTextItem>
#include <QImage>
#include <QList>
#include <QMap>
#include <QPointF>
#include <QPolygonF>
#include <QRectF>
#include <QTreeWidgetItem>

#include <geos/geom/MultiPolygon.h>
#include <geos/geom/CoordinateSequence.h>

#include "MosaicWidget.h"

#include "Filename.h"
#include "Cube.h"
#include "Stretch.h"

namespace Isis {
  class ControlNet;
  class Projection;
  class PvlGroup;
  class UniversalGroundMap;
}

namespace Qisis {

  /**
   * @brief
   *
   * @ingroup Visualization Tools
   *
   * @author Stacy Alley
   *
   * @internal
   *
   *  @history 2010-10-26 Tracie Sucharski Added missing includes to cpp after
   *                                removing includes from ControlNet.h.
   */
  class MosaicItem : public QGraphicsPolygonItem {

    public:
      MosaicItem(const QString &cubeFilename, MosaicWidget *parent, Isis::PvlGroup *group = 0);
      ~MosaicItem();

      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget = 0);
      void qt_graphicsItem_highlightSelected(QGraphicsItem *item, QPainter
                                             *painter, const
                                             QStyleOptionGraphicsItem *option);



      /**
       * Returns the cube filename.
       *
       *
       * @return Isis::Filename
       */
      Isis::Filename filename() const {
        return p_filename;
      };


      /**
       *
       *
       *
       * @return std::string
       */
      std::string serialNumber() {
        return p_serialNumber;
      };


      /**
       *
       *
       *
       * @return QList<QPointF>
       */
      QList<QPointF> controlPoints() {
        return p_controlPoints;
      };
      QMap<QString, QPointF> pointsMap() {
        return p_sceneToPointMap;
      };


      /**
       * Returns this item's maximum y value.
       *
       *
       * @return double
       */
      double YMaximum() const {
        return p_ymax;
      };


      /**
       * Returns this items' maximum x value.
       *
       *
       * @return double
       */
      double XMaximum() const {
        return p_xmax;
      };


      /**
       * Returns this item's minimum y value.
       *
       *
       * @return double
       */
      double YMinimum() const {
        return p_ymin;
      };


      /**
       * Returns this item's minimum x value.
       *
       *
       * @return double
       */
      double XMinimum() const {
        return p_xmin;
      };


      /**
       * Returns this item's pixel resolution.
       *
       *
       * @return double
       */
      double pixelResolution() const {
        return p_pixRes;
      };


      /**
       *
       *
       *
       * @return double
       */
      double levelOfDetail() const {
        return p_lastLevelOfDetail;
      };

      /**
       * Returns this item's emission angle.
       *
       *
       * @return double
       */
      double emissionAngle() const {
        return p_emissionAngle;
      };

      /**
       * Returns this item's incidence angle.
       *
       *
       * @return double
       */
      double incidenceAngle() const {
        return p_incidenceAngle;
      };



      /**
       * Returns this item's footprint color.
       *
       *
       * @return QColor
       */
      QColor color() const {
        return p_color;
      };


      /**
       *
       *
       *
       * @return Isis::Projection*
       */
      Isis::Projection *getProj() const {
        return p_proj;
      }
      Isis::Cube *cube() {
        return &p_cube;
      }


      /**
       *
       *
       *
       * @return Isis::UniversalGroundMap*
       */
      Isis::UniversalGroundMap *getGroundMap() const {
        return p_groundMap;
      }


      /**
       *
       *
       *
       * @return QGraphicsTextItem*
       */
      QGraphicsSimpleTextItem *getLabel() const {
        return p_label;
      }


      /**
       *
       *
       *
       * @return int
       */
      int getImageTrans() const {
        return p_imageTransparency;
      }


      /**
       *
       *
       *
       * @return bool
       */
      bool crossBoundry() const {
        return p_crossesBoundry;
      }


      /**
       *
       *
       *
       * @return MosaicItem*
       */
      MosaicItem *getSecondItem() const {
        return p_secondItem;
      }

      /**
       * Returns a bool indicating whether or not the tree item for
       * this mosaic item is selected.
       *
       *
       * @return bool
       */
      bool isTreeItemSelected() {
        return p_treeItem->isSelected();
      };

      QTreeWidgetItem *treeWidgetItem() {
        return p_treeItem;
      };

      void setLabelVisible(bool visible);
      void setOutlineVisible(bool visible);
      void setFootprintVisible(bool visible);
      void displayControlPoints(Isis::ControlNet *cn);
      void setControlPointsVisible(bool visible);

      void reproject();
      Isis::PvlGroup saveState();
      void setColor(QColor color);
      void setTransparency(int alpha);
      void setItemVisible(bool visible);
      void setImageVisible(bool visible);
      void setTreeItemSelected(bool selected);
      void setLevelOfDetail(double detail);
      void setZValue(qreal z);
      void setSelected(bool selected);
      QPointF screenToGround(QPointF point);
      QPointF screenToCam(int x, int y);
      QPointF screenToCam(QPointF p);
      void setSelectedPoint(QPointF p);
    protected:
      /**
       * Returns the tree item associated with this item.
       *
       *
       * @return QTreeWidgetItem*
       */
      QTreeWidgetItem *treeItem() {
        return p_treeItem;
      };
      bool sceneEventFilter(QGraphicsItem *watched, QEvent *event);

    private:
      MosaicItem(MosaicItem *parent);  //!< Private constructor
      geos::geom::MultiPolygon *p_mp; //!< This item's multipolygon in the 0/360 longitude domain
      geos::geom::MultiPolygon *p_180mp; //!< This item's multipolygon in the -180/180 longitude domain
      QTreeWidgetItem *p_treeItem; //!< Tree item associated with this mosaic item
      Isis::Filename p_filename; //!< Cube filename

      void createFootprint(); //!< Creates the footprint of this image.
      void drawImage(QPainter *painter, const QStyleOptionGraphicsItem *option);
      QList<int> scanLineIntersections(QPolygon poly, int y, int boxWidth);

      bool midTest(double trueMidX, double trueMidY, double testMidX, double testMidY);
      double getPixelValue(int sample, int line);
      void getStretch();
      void setFontSize();
      void setFontSize(QFont font);
      void setEnableRepaint(bool paint);
      void setUpItem(Isis::PvlGroup *grp);

      void paintOutline(QPainter *painter);
      void paintFootprint(QPainter *painter);
      void paintControlPoints(QPainter *painter, const QStyleOptionGraphicsItem *option);
      void paintLabel(const QStyleOptionGraphicsItem *option);
      //Isis::Brick *p_gryBrick;//!< Bricks for gray band

      static QColor randomColor(); //!< Selects a color for the footprint polygon
      QColor p_color; //!< This item's footprint color
      QPolygonF p_footprintPoly;

      double p_xmin; //!< min x value of item
      double p_xmax; //!< max x value of item
      double p_ymin; //!< min y value of item
      double p_ymax; //!< max y value of item
      double p_pixRes; //!< Pixel Resolution of the cube
      double p_emissionAngle;
      double p_incidenceAngle;
      double p_levelOfDetail; //!< Level of detail

      MosaicWidget *p_parent;  //!< Parent widget
      friend class MosaicWidget; //!< Made friends so parent has access to private members
      MosaicItem *p_secondItem;

      Isis::Camera *p_camera;
      Isis::Projection *p_proj;
      Isis::UniversalGroundMap *p_groundMap;
      double p_maxPixelValue;
      int p_imageTransparency;
      Isis::Cube p_cube;
      Isis::Stretch p_stretch;

      QImage p_image;

      QImage p_lastImage;
      QRectF p_lastExposedRect;

      //QGraphicsTextItem *p_label;
      QGraphicsSimpleTextItem *p_label;
      QFont p_labelFont;
      //QFont p_font;
      double p_lastLevelOfDetail;
      double p_screenResolution;
      bool p_updateFont;
      bool p_enablePaint;
      bool p_crossesBoundry;
      bool p_controlPointsVisible;
      QList<QPointF> p_controlPoints;
      std::string p_serialNumber;
      QMap<QString, QPointF> p_sceneToPointMap;
      QPointF p_selectedPoint;
      Isis::ControlNet *p_controlNet;
  };
};

#endif

