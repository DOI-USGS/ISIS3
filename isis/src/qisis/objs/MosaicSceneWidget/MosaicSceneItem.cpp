#include "MosaicSceneItem.h"

#include <iostream>
#include <cfloat>

#include <QApplication>
#include <QBrush>
#include <QEvent>
#include <QGraphicsItem>
#include <QList>
#include <QPainter>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QTreeWidgetItem>

#include "CubeDisplayProperties.h"
#include "FileDialog.h"
#include "Histogram.h"
#include "iString.h"
#include "ImagePolygon.h"
#include "LineManager.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"
#include "PolygonTools.h"
#include "SerialNumber.h"
#include "Statistics.h"
#include "Stretch.h"
#include "Table.h"

using namespace geos::geom;

namespace Isis {
  /**
   * MosaicSceneItem constructor
   *
   *
   * @param cubeFilename
   * @param parent
   */
  MosaicSceneItem::MosaicSceneItem(CubeDisplayProperties *cubeDisplay,
      MosaicSceneWidget *parent) : QGraphicsObject() {
    if (parent->getProjection() == NULL) {
      std::string msg = "Parent does not have projection in MosaicWidget";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    m_cubeDisplay = cubeDisplay;

    connect(m_cubeDisplay, SIGNAL(destroyed(QObject *)),
            this, SLOT(lostCubeDisplay()));
    connect(m_cubeDisplay, SIGNAL(destroyed(QObject *)),
            this, SLOT(deleteLater()));

    m_mp = NULL;
    m_polygons = NULL;
    m_cubeDnStretch = NULL;
    groundMap = NULL;

    m_scene = parent;

    m_polygons = new QList< QGraphicsPolygonItem *>();

    setupFootprint();

    setToolTip(m_cubeDisplay->displayName());

    setAcceptHoverEvents(true);

    m_cubeDisplay->addSupport(CubeDisplayProperties::Color);
    m_cubeDisplay->addSupport(CubeDisplayProperties::Selected);
    m_cubeDisplay->addSupport(CubeDisplayProperties::ShowDNs);
    m_cubeDisplay->addSupport(CubeDisplayProperties::ShowFill);
    m_cubeDisplay->addSupport(CubeDisplayProperties::ShowLabel);
    m_cubeDisplay->addSupport(CubeDisplayProperties::ShowOutline);

    if(parent->userHasTools()) {
      m_cubeDisplay->addSupport(CubeDisplayProperties::Zooming);
    }

    m_cubeDisplay->addSupport(CubeDisplayProperties::ZOrdering);

    connect(m_cubeDisplay, SIGNAL(propertyChanged(CubeDisplayProperties *)),
            this, SLOT(cubeDisplayChanged()));
  }


  /**
   * Mosaic Item destructor
   *
   */
  MosaicSceneItem::~MosaicSceneItem() {
    if(scene())
      scene()->removeItem(this);

    while(m_polygons->size()) {
      delete m_polygons->takeAt(0);
    }
  }


  QRectF MosaicSceneItem::boundingRect() const {
    QRectF boundingRect;

    QGraphicsPolygonItem *polygon;
    foreach(polygon, *m_polygons) {
      boundingRect = boundingRect.united(polygon->boundingRect());

      QGraphicsItem *polyChild;
      foreach(polyChild, polygon->childItems()) {
        if(polyChild->isVisible()) {
          boundingRect = boundingRect.united(
            mapFromItem(polyChild, polyChild->boundingRect()).boundingRect());
        }
      }
    }

    return boundingRect;
  }


  /**
   * Re-paints the item
   *
   * @param painter
   * @param option
   * @param widget
   */
  void MosaicSceneItem::paint(QPainter *painter,
      const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if(m_cubeDisplay &&
       m_cubeDisplay->getValue(CubeDisplayProperties::ShowDNs).toBool()) {
      drawImage(painter, option);
    }
  }


  /**
   *
   */
  void MosaicSceneItem::setupFootprint() {
    if(m_cubeDisplay) {
      m_mp = m_cubeDisplay->footprint();

      try {
        reproject();
      }
      catch(iException &e) {
        m_cubeDisplay->deleteLater();

        iString msg = "Could not project the footprint from cube [" +
            m_cubeDisplay->displayName() + "]";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
      }
    }
  }


  /**
   * Called anytime the user reprojects the cube. (Selects a new
   * map file.) And everytime a mosaic item is created.
   */
  void MosaicSceneItem::reproject() {
    prepareGeometryChange();

    MultiPolygon *mp;
    Projection *proj = m_scene->getProjection();

    // Remove current polygons from the scene
    while(m_polygons->size()) {
      QGraphicsPolygonItem *polyItem = m_polygons->at(0);
      m_scene->getScene()->removeItem(polyItem);
      m_polygons->removeAll(polyItem);

      delete polyItem;
      polyItem = NULL;
    }

    if (proj->Has180Domain()) {
      m_180mp = PolygonTools::To180(m_mp);
      mp = m_180mp;
    }
    else {
      mp = m_mp;
    }

    //----------------------------------------------------------
    // We need to loop thru the num. geom. because some of the
    // cubes will have more than one geom. if it crosses lat/lon
    // boundries.
    //----------------------------------------------------------
    for (unsigned int i = 0; i < mp->getNumGeometries(); i++) {
      const Geometry *geom = mp->getGeometryN(i);
      CoordinateSequence *pts;

      pts = geom->getCoordinates();
      double lat, lon;
      QVector<QPointF> polyPoints;

      //--------------------------------------------------------------
      // We need to convert the footprint polygons from lat/lon to x/y
      // in order to display them in the QGraphicsScene
      //--------------------------------------------------------------
      for (unsigned int j = 0; j < pts->getSize(); j++) {
        lat = pts->getY(j);
        lon = pts->getX(j);
        if (proj->SetUniversalGround(lat, lon)) {
          double x = proj->XCoord();
          double y = -1 * (proj->YCoord());

          polyPoints.push_back(QPointF(x, y));
        }
      }

      setFlag(QGraphicsItem::ItemIsSelectable);

      QGraphicsPolygonItem *polyItem = new QGraphicsPolygonItem(this);
      polyItem->setPolygon(QPolygonF(polyPoints));

      QGraphicsSimpleTextItem *label = new QGraphicsSimpleTextItem(polyItem);
      if(m_cubeDisplay)
        label->setText(m_cubeDisplay->displayName());
      label->setFlag(QGraphicsItem::ItemIsMovable);
      label->setFont(QFont("Helvetica", 10));
      label->setPos(polyItem->polygon().boundingRect().center());
      label->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

      QRectF boundingRect = polyItem->boundingRect();
      if(boundingRect.width() < boundingRect.height())
        label->rotate(90);

      m_polygons->append(polyItem);

      delete pts;
    }

    updateChildren();
  }


  /**
   * Returns the pixel value at the given sample/line.
   *
   *
   * @param sample
   * @param line
   *
   * @return int
   */
  double MosaicSceneItem::getPixelValue(int sample, int line) {
    double pixelValue = 0;

    if(m_cubeDisplay) {
      Brick gryBrick(1, 1, 1, m_cubeDisplay->cube()->getPixelType());
      gryBrick.SetBasePosition((int)(sample + 0.5), (int)(line + 0.5), 1);
      m_cubeDisplay->cube()->read(gryBrick);

      pixelValue = gryBrick[0];
      if (pixelValue == Null) {
        return Null;
      }
      if (pixelValue < 0) pixelValue = 0;
      if (pixelValue > 255) pixelValue = 255;
    }

    return pixelValue;
  }


  /**
   * This method reads in and draws the image associated with this
   * item.
   * @param painter
   */
  void MosaicSceneItem::drawImage(QPainter *painter,
      const QStyleOptionGraphicsItem *option) {
    Stretch *stretch = getStretch();
    QApplication::setOverrideCursor(Qt::WaitCursor);

    try {
      QGraphicsPolygonItem *polygon;
      foreach(polygon, *m_polygons) {
        QPolygonF polyBounding = polygon->polygon();
        QRectF sceneRect = polyBounding.boundingRect();
        QPolygon screenPoly = m_scene->getView()->mapFromScene(sceneRect);
        QRect visibleBox = screenPoly.boundingRect();

        int bbWidth  = (int)visibleBox.width();
        int bbHeight = (int)visibleBox.height();

        int bbLeft = visibleBox.left();
        int bbTop = visibleBox.top();
        int bbRight = visibleBox.right();
        int bbBottom = visibleBox.bottom();

        QImage image(bbWidth, bbHeight, QImage::Format_ARGB32);

        for (int y = bbTop; y <= bbBottom; y++) {
          QRgb *lineData = (QRgb *)image.scanLine(y - bbTop);

          for (int x = bbLeft; x <= bbRight; x++) {
            lineData[x - bbLeft] = qRgba(0, 0, 0, 0);

            // We have an x,y in screen space. Let's translate it to
            //   projected space, ask the polygon if it's in the area,
            QPointF scenePos = m_scene->getView()->mapToScene(
                QPoint(x, y));

            if(polygon->polygon().containsPoint(scenePos, Qt::OddEvenFill)) {
              // This is likely in the cube... use the projection to go to
              //   lat/lon and use that lat/lon to go to cube sample,line
              m_scene->getProjection()->SetCoordinate(scenePos.x(),
                                                      -1 * scenePos.y());

              double lat = m_scene->getProjection()->UniversalLatitude();
              double lon = m_scene->getProjection()->UniversalLongitude();

              if(m_cubeDisplay) {
                if(!groundMap) {
                  groundMap = new UniversalGroundMap(*m_cubeDisplay->cube());
                }

                if(groundMap->SetUniversalGround(lat, lon)) {
                  double dn = Null;

                  if(groundMap->Camera() && groundMap->Camera()->InCube()) {
                    double samp = groundMap->Camera()->Sample();
                    double line = groundMap->Camera()->Line();

                    dn = getPixelValue((int)(samp + 0.5),
                                       (int)(line + 0.5));
                  }
                  else {
                    double samp = groundMap->Projection()->WorldX();
                    double line = groundMap->Projection()->WorldY();

                    dn = getPixelValue((int)(samp + 0.5),
                                       (int)(line + 0.5));
                  }

                  if(!IsSpecial(dn)) {
                    int stretched = (int)stretch->Map(dn);

                    lineData[x - bbLeft] = qRgba(stretched, stretched,
                                                stretched, 255);
                  }
                }
              }
            }
          }
        }

//         m_lastImages.append(image);
        painter->drawImage(polygon->boundingRect(), image);
      }
    }
    catch(iException &e) {
      e.Report();
      e.Clear();
    }

    QApplication::restoreOverrideCursor();
  }


  QColor MosaicSceneItem::color() const {
    return
        m_cubeDisplay->getValue(CubeDisplayProperties::Color).value<QColor>();
  }


  /**
   * Someone changed something in the cube display properties, re-read the
   *   whole thing.
   */
  void MosaicSceneItem::cubeDisplayChanged() {
    m_scene->blockSelectionChange(true);
    updateSelection(false);
    m_scene->blockSelectionChange(false);

    updateChildren();
  }


  /**
   * This filters out events that happen within our polygons. This is necessary
   *   because usually events are filtered based on the bounding box alone.
   *
   * @param watched
   * @param event
   *
   * @return bool
   */
  bool MosaicSceneItem::sceneEvent(QEvent *event) {
    // We need to verify this event is really ours
    QPointF scenePos;

    switch (event->type()) {
      case QEvent::GraphicsSceneContextMenu:
        scenePos = ((QGraphicsSceneContextMenuEvent *)event)->scenePos();
        break;
      case QEvent::GraphicsSceneHoverEnter:
      case QEvent::GraphicsSceneHoverMove:
      case QEvent::GraphicsSceneHoverLeave:
        scenePos = ((QGraphicsSceneHoverEvent *)event)->scenePos();
        break;
      case QEvent::GraphicsSceneMouseMove:
      case QEvent::GraphicsSceneMousePress:
      case QEvent::GraphicsSceneMouseRelease:
      case QEvent::GraphicsSceneMouseDoubleClick:
        scenePos = ((QGraphicsSceneMouseEvent *)event)->scenePos();
        break;
      default:
        break;
    }

    bool ourEvent = true;
    if(!scenePos.isNull()) {
      ourEvent = contains(scenePos);
    }

    if(ourEvent) {
      return QGraphicsObject::sceneEvent(event);
    }
    else {
      event->ignore();
      return true;
    }
  }


  /**
   * Test if we contain the point. Even though our rect is empty, return true
   *   if a child polygon contains it for toolTips and other events.
   */
  bool MosaicSceneItem::contains(const QPointF &p) const {
    if(p.isNull())
      return false;

    QGraphicsPolygonItem * polygon;
    foreach(polygon, *m_polygons) {
      if(polygon->contains(p)) {
        return true;
      }
    }

    return false;
  }


  /**
   * Update the selected state.
   *
   * @param save True if we need to write to the CubeDisplayProperties, false
   *             if we need to read from them.
   */
  void MosaicSceneItem::updateSelection(bool save) {
    QGraphicsPolygonItem * polygon;
    if(save && m_cubeDisplay) {
      bool selected = isSelected();

      foreach(polygon, *m_polygons) {
        selected = selected || polygon->isSelected();
      }

      m_cubeDisplay->setSelected(selected);
      updateSelection(false);
    }
    else if(m_cubeDisplay) {
      bool selected =
          m_cubeDisplay->getValue(CubeDisplayProperties::Selected).toBool();

      if(selected != isSelected()) {
        setSelected(selected);
      }

      foreach(polygon, *m_polygons) {
        if(polygon->isSelected() != selected) {
          polygon->setSelected(selected);
        }
      }
    }

  }


  /**
   * The user right clicked on us (or otherwise requested a context menu).
   *
   * Give it to them.
   */
  void MosaicSceneItem::contextMenuEvent(
      QGraphicsSceneContextMenuEvent *event) {
    if(m_cubeDisplay) {
      QMenu menu;

      QAction *title = menu.addAction(m_cubeDisplay->displayName());
      title->setEnabled(false);
      menu.addSeparator();

      QList<CubeDisplayProperties *> cubeDisplays;
      cubeDisplays.append(m_cubeDisplay);

      QList<QAction *> displayActs =
          CubeDisplayProperties::getSupportedDisplayActions(cubeDisplays);

      QAction *displayAct;
      foreach(displayAct, displayActs) {
        menu.addAction(displayAct);
      }

      QList<QAction *> zoomActs =
          CubeDisplayProperties::getSupportedZoomActions(cubeDisplays);

      QList<QAction *> zActs =
          CubeDisplayProperties::getSupportedZOrderActions(cubeDisplays);

      if((zoomActs.size() || zActs.size()) && displayActs.size()) {
        menu.addSeparator();
      }

      QAction *zoomAct;
      foreach(zoomAct, zoomActs) {
        menu.addAction(zoomAct);
      }

      QAction *zAct;
      foreach(zAct, zActs) {
        menu.addAction(zAct);
      }

      menu.addSeparator();
      QAction *removeAction = menu.addAction("Close Cube");
      connect(removeAction, SIGNAL(triggered()),
              m_cubeDisplay, SLOT(deleteLater()));

      menu.exec(event->screenPos());
    }
  }


  void MosaicSceneItem::lostCubeDisplay() {
    m_cubeDisplay = NULL;
  }


  /**
   * This applies the cubeDisplayProperties and selectability. It's called
   *   updateChildren because the child items are the visually displayed
   *   items on the scene.
   */
  void MosaicSceneItem::updateChildren() {
    setFlag(QGraphicsItem::ItemIsSelectable, m_scene->cubesSelectable());

    QList<QRectF> regionsChanged;

    if(m_cubeDisplay) {
      QGraphicsPolygonItem * polygon;
      foreach(polygon, *m_polygons) {
        // Fill
        if(m_cubeDisplay->getValue(CubeDisplayProperties::ShowFill).toBool())
          polygon->setBrush(color());
        else
          polygon->setBrush(Qt::NoBrush);

        // Outline
        QColor opaqueColor(color());
        opaqueColor.setAlpha(255);
        if(m_cubeDisplay->getValue(CubeDisplayProperties::ShowOutline).toBool())
          polygon->setPen(opaqueColor);
        else
          polygon->setPen(Qt::NoPen);

        polygon->setFlag(QGraphicsItem::ItemIsSelectable,
                        m_scene->cubesSelectable());

        // Children (label is the only child)
        QGraphicsItem *polyChild;
        foreach(polyChild, polygon->childItems()) {
          polyChild->setVisible(
              m_cubeDisplay->getValue(
                CubeDisplayProperties::ShowLabel).toBool());

          // Qt documentation was lacking the enum that this matches to, so this
          //   is the best I could do
          if(polyChild->type() == 9) {
            QGraphicsSimpleTextItem * text =
                (QGraphicsSimpleTextItem *)polyChild;
            text->setBrush(opaqueColor);
          }
        }
      }

      update();
      emit changed(regionsChanged);
    }
  }


  /**
   * This gets a Stretch object that will work for the 
   *   cubeDisplay converting from DN to screen pixel.
   *
   * The first time this is called the stretch is calculated, later calls
   *   re-use the original object. Ownership remains at the class scope.
   */
  Stretch *MosaicSceneItem::getStretch() {
    if (m_cubeDnStretch != NULL || !m_cubeDisplay) return m_cubeDnStretch;

    LineManager mgr(*m_cubeDisplay->cube());

    mgr.begin();
    Statistics stats;

    const int skip = 0;

    while(mgr ++) {
      m_cubeDisplay->cube()->read(mgr);
      stats.AddData(mgr.DoubleBuffer(), mgr.size());

      for(int i = 0; i < skip; i++)
        mgr ++;
    }

    m_cubeDnStretch = new Stretch();
    m_cubeDnStretch->AddPair(stats.BestMinimum(), 0.0);
    m_cubeDnStretch->AddPair(stats.BestMaximum(), 255.0);

    m_cubeDnStretch->SetNull(0.0);
    m_cubeDnStretch->SetLis(0.0);
    m_cubeDnStretch->SetLrs(0.0);
    m_cubeDnStretch->SetHis(255.0);
    m_cubeDnStretch->SetHrs(255.0);
    m_cubeDnStretch->SetMinimum(0.0);
    m_cubeDnStretch->SetMaximum(255.0);

    return m_cubeDnStretch;
  }
}
