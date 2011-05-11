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

#include "ControlMeasure.h"
#include "ControlPoint.h"
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

    p_cubeDisplay = cubeDisplay;

    connect(p_cubeDisplay, SIGNAL(destroyed(QObject *)),
            this, SLOT(lostCubeDisplay()));
    connect(p_cubeDisplay, SIGNAL(destroyed(QObject *)),
            this, SLOT(deleteLater()));

    p_imageTransparency = 180;
    p_pixRes = 0;
    p_emissionAngle = 0;
    p_incidenceAngle = 0;
    p_lastPaintScale = 0;

    p_mp = NULL;
    p_polygons = NULL;
    p_cubeDnStretch = NULL;

    p_scene = parent;

    p_polygons = new QList< QGraphicsPolygonItem *>();

    createFootprint();

    setToolTip(p_cubeDisplay->displayName());

    setAcceptHoverEvents(true);

    p_cubeDisplay->addSupport(CubeDisplayProperties::Color);
    p_cubeDisplay->addSupport(CubeDisplayProperties::Selected);
    p_cubeDisplay->addSupport(CubeDisplayProperties::ShowDNs);
    p_cubeDisplay->addSupport(CubeDisplayProperties::ShowFill);
    p_cubeDisplay->addSupport(CubeDisplayProperties::ShowLabel);
    p_cubeDisplay->addSupport(CubeDisplayProperties::ShowOutline);

    if(parent->userHasTools()) {
      p_cubeDisplay->addSupport(CubeDisplayProperties::Zooming);
    }

    p_cubeDisplay->addSupport(CubeDisplayProperties::ZOrdering);

    connect(p_cubeDisplay, SIGNAL(propertyChanged(CubeDisplayProperties *)),
            this, SLOT(cubeDisplayChanged()));
  }


  /**
   * Mosaic Item destructor
   *
   */
  MosaicSceneItem::~MosaicSceneItem() {
    if(scene())
      scene()->removeItem(this);

    while(p_polygons->size()) {
      delete p_polygons->takeAt(0);
    }
  }


  /**
   * This method is called from the constructor when there is a
   * group passed as one of the header args.
   * When this method is called it means a saved project file has
   * been read in and this sets up the item back to the way the
   * user had set it up before the save project command.
   *
   * @param grp
   */
  void MosaicSceneItem::setUpItem(PvlGroup *grp) {
  }


  QRectF MosaicSceneItem::boundingRect() const {
    QRectF boundingRect;

    QGraphicsPolygonItem *polygon;
    foreach(polygon, *p_polygons) {
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
    if(p_cubeDisplay &&
       p_cubeDisplay->getValue(CubeDisplayProperties::ShowDNs).toBool()) {
      drawImage(painter, option);
    }
  }


  /**
   * This method gets the footprint polygon of the cube
   *
   */
  void MosaicSceneItem::createFootprint() {
    // The Archive group should never be searched for anything ever!
    //Get the product ID and set the label text to that.
    //PvlKeyword keyword = cube.Label()->FindObject("IsisCube").FindGroup("Archive").FindKeyword("ProductId");
    //p_label->setPlainText(keyword[0].ToQt());
    //p_label->setText(keyword[0].ToQt());

    //Get the camera stats table from the cube.
    //This is how we know the cube's resolution.
    //This is the reason we need to run camstats
    //with the attach option set to yes.
    if(!p_cubeDisplay)
      return;

    try {
      Table table("CameraStatistics", p_cubeDisplay->cube()->Filename());
      //Table table("CameraStatistics", p_filename.Name());
      for (int i = 0; i < table.Records(); i++) {
        for (int j = 0; j < table[i].Fields(); j++) {
          QString label;

          if (table[i][j].IsText()) {
            label = QString::fromStdString((std::string)table[i][j]);
            label.truncate(10);
          }

          // Get the average resolution for this mosaic item.
          if (table[i][j].IsText() && label.compare("Resolution") == 0) {
            if (j + 3 < table[i].Fields()) {
              if (table[i][j+3].IsInteger()) {
              }
              else if (table[i][j+3].IsDouble()) {
                p_pixRes = (double)table[i][j+3];
              }
              else if (table[i][j+3].IsText()) {
              }
            }
          }

          // Get the average emission angle for this mosaic item.
          if (table[i][j].IsText() && label.compare("EmissionAn") == 0) {
            if (j + 3 < table[i].Fields()) {
              if (table[i][j+3].IsInteger()) {
              }
              else if (table[i][j+3].IsDouble()) {
                p_emissionAngle = (double)table[i][j+3];
              }
              else if (table[i][j+3].IsText()) {
              }
            }
          }

          // Get the average incidence angle for this mosaic item.
          if (table[i][j].IsText() && label.compare("IncidenceA") == 0) {
            if (j + 3 < table[i].Fields()) {
              if (table[i][j+3].IsInteger()) {
              }
              else if (table[i][j+3].IsDouble()) {
                p_incidenceAngle = (double)table[i][j+3];
              }
              else if (table[i][j+3].IsText()) {
              }
            }
          }

        } // end for table[i].Fields
      } // end for table.Records

    }
    catch (iException &e) {
      iException::Message(iException::Io,
          "Could not find the CameraStatistics Table. "
          "Please run camstats with the attach option", _FILEINFO_);
      e.Report(false);
      return;
    }

    try {
      ImagePolygon poly;
      p_cubeDisplay->cube()->Read(poly);

      p_mp = (geos::geom::MultiPolygon *)poly.Polys()->clone();

      reproject();
    }
    catch (...) {
      p_cubeDisplay->deleteLater();

      iString msg = "Could not read the footprint from cube [" +
          p_cubeDisplay->displayName() + "]. Please make "
          "sure footprintinit has been run";
      throw iException::Message(iException::Io, msg, _FILEINFO_);
    }
  }


  /**
   * Called anytime the user reprojects the cube. (Selects a new
   * map file.) And everytime a mosaic item is created.
   */
  void MosaicSceneItem::reproject() {
    geos::geom::MultiPolygon *mp;
    Projection *proj = p_scene->getProjection();

    // Remove current polygons from the scene
    while(p_polygons->size()) {
      QGraphicsPolygonItem *polyItem = p_polygons->at(0);
      p_scene->getScene()->removeItem(polyItem);
      p_polygons->removeAll(polyItem);

      delete polyItem;
      polyItem = NULL;
    }

    if (proj->Has180Domain()) {
      p_180mp = PolygonTools::To180(p_mp);
      mp = p_180mp;
    }
    else {
      mp = p_mp;
    }

    //----------------------------------------------------------
    // We need to loop thru the num. geom. because some of the
    // cubes will have more than one geom. if it crosses lat/lon
    // boundries.
    //----------------------------------------------------------
    for (unsigned int i = 0; i < mp->getNumGeometries(); i++) {
      const geos::geom::Geometry *geom = mp->getGeometryN(i);
      geos::geom::CoordinateSequence *pts;

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
      if(p_cubeDisplay)
        label->setText(p_cubeDisplay->displayName());
      label->setFlag(QGraphicsItem::ItemIsMovable);
      label->setFont(QFont("Helvetica", 10));
      label->setPos(polyItem->polygon().boundingRect().center());
      label->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

      p_polygons->append(polyItem);

      delete pts;
    }

    updateChildren();
  }


  /**
   * Translates the screen coordinates(x,y) to camera coordinates.
   *
   *
   * @param x
   * @param y
   *
   * @return QPointF
   */
  QPointF MosaicSceneItem::screenToCam(int x, int y) {
    QPointF camPoints;
    QPointF scenePoint = scene()->views().last()->mapToScene(QPoint(x, y));
    Projection *proj = p_scene->getProjection();

    if (!proj->SetWorld(scenePoint.x(), -scenePoint.y())) {
      camPoints.setX(-1);
      camPoints.setY(-1);
      return camPoints;
    }

    double lat = proj->UniversalLatitude();
    double lon = proj->UniversalLongitude();

    if(p_cubeDisplay) {
      Camera *cam = p_cubeDisplay->cube()->Camera();
      if (!cam->SetUniversalGround(lat, lon)) {
        camPoints.setX(-1);
        camPoints.setY(-1);
        return camPoints;
      }

      camPoints.setX(cam->Sample() + 0.5);
      camPoints.setY(cam->Line() + 0.5);
    }

    return camPoints;
  }


  /**
   * Translates the screen coordinates(x,y) to line/sample.
   *
   *
   * @param x
   * @param y
   *
   * @return QPointF
   */
  QPointF MosaicSceneItem::screenToCam(QPointF p) {
    QPointF camPoints;
    Projection *proj = p_scene->getProjection();

    if (!proj->SetWorld(p.x(), -p.y())) {
      camPoints.setX(-1);
      camPoints.setY(-1);
      return camPoints;
    }

    double lat = proj->UniversalLatitude();
    double lon = proj->UniversalLongitude();

    if(p_cubeDisplay) {
      Camera *cam = p_cubeDisplay->cube()->Camera();
      if (!cam->SetUniversalGround(lat, lon)) {
        camPoints.setX(-1);
        camPoints.setY(-1);
        return camPoints;
      }

      camPoints.setX(cam->Sample());
      camPoints.setY(cam->Line());
    }

    return camPoints;
  }


  /**
   * Translates screen points to lat/lon for reporting in the
   * lower right corner of the qmos window.
   *
   *
   * @param point
   *
   * @return QPointF
   */
  QPointF MosaicSceneItem::screenToGround(QPointF point) {
    QPointF groundPoints;
    Projection *proj = p_scene->getProjection();

    if (!proj->SetWorld(point.x(), -point.y())) {
      groundPoints.setX(-1);
      groundPoints.setY(-1);
      return groundPoints;
    }

    double lat = proj->Latitude();
    double lon = proj->Longitude();
    groundPoints.setX(lat);
    groundPoints.setY(lon);

    return groundPoints;
  }


  /**
   * Returns true if the distance between two points is less than
   * .5, otherwise returns false.
   *
   *
   * @param trueMidX
   * @param trueMidY
   * @param testMidX
   * @param testMidY
   *
   * @return bool
   */
  bool MosaicSceneItem::midTest(double trueMidX, double trueMidY,
                           double testMidX, double testMidY) {
    //since sqrt is expensive, we will just compare dist to .5^2.
    //double dist = sqrt(pow(testMidX - trueMidX,2) + pow(testMidY - trueMidY,2));
    double dist = (testMidX - trueMidX) * (testMidX - trueMidX) +
                  (testMidY - trueMidY) * (testMidY - trueMidY);
    if (dist < .5 * .5) {
      return true;
    }
    else {
      return false;
    }
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

    if(p_cubeDisplay) {
      Brick gryBrick(1, 1, 1, p_cubeDisplay->cube()->PixelType());
      gryBrick.SetBasePosition((int)(sample + 0.5), (int)(line + 0.5), 1);
      p_cubeDisplay->cube()->Read(gryBrick);

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
  void MosaicSceneItem::drawImage(QPainter *painter, const QStyleOptionGraphicsItem *option) {
    //if paint is not enabled, then just draw the last image and return
    //the paint is only disable when in zoom mode and the mouse button is down.

    //If the exposed rect is the same as the last exposed rect, then don't draw again.
    //just draw the last image and return
//     QRectF exposed(option->exposedRect);
//
//     QRectF exposedOverlap = exposed.intersected(p_lastExposedRect);
//
//     QPoint test1(0, 0);
//     QPoint test2(2, 2);
//     QPointF scenePointDifference =
//       p_scene->getView()->mapToScene(test1) -
//       p_scene->getView()->mapToScene(test2);
//
//     double sceneScale =
//         scenePointDifference.x() * scenePointDifference.x() +
//         scenePointDifference.y() * scenePointDifference.y();
//
//     if(fabs(p_lastPaintScale - sceneScale) < 0.00001)
//       sceneScale = p_lastPaintScale;
//
//     if(exposedOverlap == exposed &&
//        p_lastPaintScale == sceneScale) {
//       for(int i = 0; i < p_polygons->size(); i++) {
//         QImage *image = &p_lastImages[i];
//         painter->drawImage(p_polygons->at(i)->polygon().boundingRect(), *image);
//       }
//       return;
//     }
//
//     p_lastImages.clear();
//     p_lastExposedRect = option->exposedRect;
//
//     p_lastPaintScale = sceneScale;

    Stretch *stretch = getStretch();
    QApplication::setOverrideCursor(Qt::WaitCursor);

    try {
      QGraphicsPolygonItem *polygon;
      foreach(polygon, *p_polygons) {
        QPolygonF polyBounding = polygon->polygon();
        QRectF sceneRect = polyBounding.boundingRect();
        QPolygon screenPoly = p_scene->getView()->mapFromScene(sceneRect);
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
            QPointF scenePos = p_scene->getView()->mapToScene(
                QPoint(x, y));

            if(polygon->polygon().containsPoint(scenePos, Qt::OddEvenFill)) {
              // This is likely in the cube... use the projection to go to
              //   lat/lon and use that lat/lon to go to cube sample,line
              p_scene->getProjection()->SetWorld(scenePos.x(),
                                                 -1 * scenePos.y());

              double lat = p_scene->getProjection()->UniversalLatitude();
              double lon = p_scene->getProjection()->UniversalLongitude();

              if(p_cubeDisplay) {
                Camera *cam = p_cubeDisplay->cube()->Camera();
                if(cam->SetUniversalGround(lat, lon) &&
                   cam->InCube()) {
                  double samp = cam->Sample();
                  double line = cam->Line();

                  double dn = getPixelValue((int)(samp + 0.5),
                                            (int)(line + 0.5));
                  int stretched = (int)stretch->Map(dn);

                  lineData[x - bbLeft] = qRgba(stretched, stretched,
                                               stretched, 255);
                }
              }
            }
          }
        }

        p_lastImages.append(image);
        painter->drawImage(polygon->boundingRect(), image);
      }
    }
    catch(iException &e) {
      e.Report();
      e.Clear();
    }

    QApplication::restoreOverrideCursor();
  }


  /**
   * This method returns a list of x values where the polygon
   * intersects with the bounding box.
   *
   * @param poly
   * @param line
   * @param boxWidth
   *
   * @return QList<int>
   */
  QList<int> MosaicSceneItem::scanLineIntersections(QPolygon poly, int y, int boxWidth) {
    QList <int> inter;
    for (int i = 0; i < poly.size() - 1; i++) {
      int n = i + 1;

      int yMax = qMax(poly.point(i).y(), poly.point(n).y());
      int yMin = qMin(poly.point(i).y(), poly.point(n).y());

      if (y < yMin) continue;
      if (y > yMax) continue;
      if (yMin == yMax) continue;

      if ((poly.point(n).x() - poly.point(i).x()) == 0) {
        inter.push_back(poly.point(n).x());
      }
      else {
        double slope = (double)(poly.point(n).y() - poly.point(i).y()) / (poly.point(n).x() - poly.point(i).x());
        double x = (y - poly.point(i).y()) / slope + poly.point(i).x();
        inter.push_back((int)(x + 0.5));
      }
    }

    qSort(inter.begin(), inter.end());

    QList<int> uniqueList;
    for (int i = 0; i < inter.size(); i++) {
      if (uniqueList.size() == 0) {
        uniqueList.push_back(inter[i]);
      }
      else if (inter[i] != uniqueList.last()) {
        uniqueList.push_back(inter[i]);
      }
    }
    if (uniqueList.size() == 3) {
      uniqueList.removeAt(1);
    }

    return uniqueList;
  }


  QColor MosaicSceneItem::color() const {
    return
        p_cubeDisplay->getValue(CubeDisplayProperties::Color).value<QColor>();
  }


  /**
   * This method sets the z values for this item and gives it's
   * children, if there are any, the same z value.
   *
   *
   * @param z
   */
  void MosaicSceneItem::setZValue(qreal z) {
    QGraphicsItem::setZValue(z);
  }


  QVariant MosaicSceneItem::itemChange(GraphicsItemChange change,
      const QVariant & value) {
    return QGraphicsObject::itemChange(change, value);
  }

  void MosaicSceneItem::cubeDisplayChanged() {
    p_scene->blockSelectionChange(true);
    updateSelection(false);
    p_scene->blockSelectionChange(false);

    updateChildren();
  }


  /**
   * Overloaded methoded for convenience.
   * Set the font size of the item's label.
   *
   *
   * @param font
   */
  void MosaicSceneItem::setFontSize(QFont font) {
    p_label->setFont(font);
    p_updateFont = true;
    update();
  }


  /**
   * This filters out events that happen within our bounding box but not on
   *   one of the polygons.
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


  bool MosaicSceneItem::contains(const QPointF &p) const {
    if(p.isNull())
      return false;

    QGraphicsPolygonItem * polygon;
    foreach(polygon, *p_polygons) {
      if(polygon->contains(p)) {
        return true;
      }
    }

    return false;
  }


  void MosaicSceneItem::updateSelection(bool save) {
    QGraphicsPolygonItem * polygon;
    if(save && p_cubeDisplay) {
      bool selected = isSelected();

      foreach(polygon, *p_polygons) {
        selected = selected || polygon->isSelected();
      }

      p_cubeDisplay->setSelected(selected);
    }
    else if(p_cubeDisplay) {
      bool selected =
          p_cubeDisplay->getValue(CubeDisplayProperties::Selected).toBool();

      if(selected != isSelected()) {
        setSelected(selected);
      }

      foreach(polygon, *p_polygons) {
        if(polygon->isSelected() != selected) {
          polygon->setSelected(selected);
        }
      }
    }

  }


  void MosaicSceneItem::contextMenuEvent(
      QGraphicsSceneContextMenuEvent *event) {
    if(p_cubeDisplay) {
      QMenu menu;

      QAction *title = menu.addAction(p_cubeDisplay->displayName());
      title->setEnabled(false);
      menu.addSeparator();

      QList<CubeDisplayProperties *> cubeDisplays;
      cubeDisplays.append(p_cubeDisplay);

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
              p_cubeDisplay, SLOT(deleteLater()));

      menu.exec(event->screenPos());
    }
  }


  void MosaicSceneItem::lostCubeDisplay() {
    p_cubeDisplay = NULL;
  }


  void MosaicSceneItem::updateChildren() {
    setFlag(QGraphicsItem::ItemIsSelectable, p_scene->cubesSelectable());

    QList<QRectF> regionsChanged;

    if(p_cubeDisplay) {
      QGraphicsPolygonItem * polygon;
      foreach(polygon, *p_polygons) {
        // Fill
        if(p_cubeDisplay->getValue(CubeDisplayProperties::ShowFill).toBool())
          polygon->setBrush(color());
        else
          polygon->setBrush(Qt::NoBrush);

        // Outline
        QColor opaqueColor(color());
        opaqueColor.setAlpha(255);
        if(p_cubeDisplay->getValue(CubeDisplayProperties::ShowOutline).toBool())
          polygon->setPen(opaqueColor);
        else
          polygon->setPen(Qt::NoPen);

        polygon->setFlag(QGraphicsItem::ItemIsSelectable,
                        p_scene->cubesSelectable());

        // Children (label is the only child)
        QGraphicsItem *polyChild;
        foreach(polyChild, polygon->childItems()) {
          polyChild->setVisible(
              p_cubeDisplay->getValue(
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



  Stretch *MosaicSceneItem::getStretch() {
    if (p_cubeDnStretch != NULL || !p_cubeDisplay) return p_cubeDnStretch;

    LineManager mgr(*p_cubeDisplay->cube());

    mgr.begin();
    Statistics stats;

    const int skip = 0;

    while(mgr ++) {
      p_cubeDisplay->cube()->Read(mgr);
      stats.AddData(mgr.DoubleBuffer(), mgr.size());

      for(int i = 0; i < skip; i++)
        mgr ++;
    }

    p_cubeDnStretch = new Stretch();
    p_cubeDnStretch->AddPair(stats.BestMinimum(), 0.0);
    p_cubeDnStretch->AddPair(stats.BestMaximum(), 255.0);

    p_cubeDnStretch->SetNull(0.0);
    p_cubeDnStretch->SetLis(0.0);
    p_cubeDnStretch->SetLrs(0.0);
    p_cubeDnStretch->SetHis(255.0);
    p_cubeDnStretch->SetHrs(255.0);
    p_cubeDnStretch->SetMinimum(0.0);
    p_cubeDnStretch->SetMaximum(255.0);

    return p_cubeDnStretch;
  }
}

