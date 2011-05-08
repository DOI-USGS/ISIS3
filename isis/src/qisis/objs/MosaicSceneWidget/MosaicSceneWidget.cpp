#include "MosaicSceneWidget.h"

#include <QAction>
#include <QEvent>
#include <QFileDialog>
//#include <QGLWidget> This is necessary for OpenGL acceleration
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMessageBox>
#include <QProgressBar>
#include <QRubberBand>
#include <QScrollBar>
#include <QStatusBar>
#include <QToolBar>

#include "Camera.h"
#include "Cube.h"
#include "CubeDisplayProperties.h"
#include "Distance.h"
#include "GraphicsView.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MosaicControlNetTool.h"
#include "MosaicFindTool.h"
#include "MosaicGraphicsView.h"
#include "MosaicPanTool.h"
#include "MosaicSceneItem.h"
#include "MosaicSelectTool.h"
#include "MosaicTrackTool.h"
#include "MosaicZoomTool.h"
#include "ProgressBar.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "PvlObject.h"
#include "Pvl.h"

namespace Isis {
  MosaicSceneWidget::MosaicSceneWidget(QStatusBar *status,
                                       QWidget *parent) : QWidget(parent) {
    p_mosaicSceneItems = new QList<MosaicSceneItem *>;

    p_graphicsScene = new QGraphicsScene(this);
    p_graphicsScene->installEventFilter(this);

    p_graphicsView = new MosaicGraphicsView(p_graphicsScene, this);
    p_graphicsView->setScene(p_graphicsScene);
    p_graphicsView->setOptimizationFlag(QGraphicsView::DontSavePainterState,
                                        true);
    p_graphicsView->setInteractive(true);
    p_graphicsView->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    p_graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    // This enables OpenGL acceleration
//     p_graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));

    QLayout * sceneLayout = new QHBoxLayout;
    sceneLayout->addWidget(p_graphicsView);
    sceneLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(sceneLayout);

    p_projection = NULL;
    p_mapAction = NULL;

//     p_projectionFootprint = new QGraphicsPolygonItem();
//     p_projectionFootprint->hide();

    p_cubesSelectable = true;
    p_customRubberBandEnabled = false;
    p_customRubberBand = NULL;
    p_rubberBandOrigin = NULL;
    p_outlineRect = NULL;

    // Create the tools we want
    p_tools = new QList<MosaicTool *>;
    p_tools->append(new MosaicSelectTool(this));
    p_tools->append(new MosaicZoomTool(this));
    p_tools->append(new MosaicPanTool(this));
    p_tools->append(new MosaicControlNetTool(this));
    p_tools->append(new MosaicFindTool(this));
    if(status)
      p_tools->append(new MosaicTrackTool(this, status));

    p_tools->at(0)->activate(true);

    blockSelectionChange(false);

    p_userToolControl = false;
    p_ownProjection = false;

    p_progress = new ProgressBar;
    p_progress->setVisible(false);

    getView()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    getView()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(getView()->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(sendVisibleRectChanged()));
    connect(getView()->verticalScrollBar() , SIGNAL(valueChanged(int)),
            this, SLOT(sendVisibleRectChanged()));
    connect(getView()->horizontalScrollBar(), SIGNAL(rangeChanged(int, int)),
            this, SLOT(sendVisibleRectChanged()));
    connect(getView()->verticalScrollBar() , SIGNAL(rangeChanged(int, int)),
            this, SLOT(sendVisibleRectChanged()));
  }

  MosaicSceneWidget::~MosaicSceneWidget() {
    p_outlineRect = NULL; // The scene will clean this up

    if(p_tools) {
      MosaicTool *tool;
      foreach(tool, *p_tools) {
        delete tool;
        tool = NULL;
      }

      delete p_tools;
      p_tools = NULL;
    }

    if(p_ownProjection && p_projection) {
      delete p_projection;
    }
    p_projection = NULL;
  }


  /**
   * This method takes ownership of proj
   */
  void MosaicSceneWidget::setProjection(Projection *proj) {
    PvlGroup mapping(proj->Mapping());

    if(p_mapAction) {
      PvlKeyword projectionKeyword = mapping.FindKeyword("ProjectionName");
      QString projName = QString::fromStdString(projectionKeyword[0]);
      p_mapAction->setText(projName);
    }

    Projection *old = p_projection;
    p_projection = proj;

    reprojectItems();
    emit projectionChanged(p_projection);

    if(old && p_ownProjection) {
      delete old;
      old = NULL;
    }

    p_ownProjection = false;
  }



  void MosaicSceneWidget::setOutlineRect(QRectF outline) {
    if(outline.united(getView()->sceneRect()) != getView()->sceneRect())
      outline = QRectF();

    if(!p_outlineRect) {
      p_outlineRect = getScene()->addRect(outline,
                                          QPen(Qt::black),
                                          Qt::NoBrush);
      p_outlineRect->setZValue(DBL_MAX);
    }
    else {
      p_outlineRect->setRect(outline);
    }

    if(!p_userToolControl)
      refit();
  }


  Projection *MosaicSceneWidget::createInitialProjection(
      CubeDisplayProperties * cubeDisplay) {
    Projection *proj = NULL;
    Cube *cube = cubeDisplay->cube();

    try {
      proj = ProjectionFactory::CreateFromCube(*cube->Label());
    }
    catch(iException &e) {
      Camera * cam = cube->Camera();

      Pvl mappingPvl("$base/templates/maps/equirectangular.map");

      // The template is missing most information... let's fill it in
      PvlGroup &mappingGrp = mappingPvl.FindGroup("Mapping");

      mappingGrp += PvlKeyword("TargetName", cam->Target());
      mappingGrp += PvlKeyword("LatitudeType", "Planetocentric");
      mappingGrp += PvlKeyword("LongitudeDirection", "PositiveEast");
      mappingGrp += PvlKeyword("LongitudeDomain", "360");
      mappingGrp += PvlKeyword("CenterLatitude", "0");
      mappingGrp += PvlKeyword("CenterLongitude", "180");
      mappingGrp += PvlKeyword("MinimumLatitude", "-90");
      mappingGrp += PvlKeyword("MaximumLatitude", "90");
      mappingGrp += PvlKeyword("MinimumLongitude", "0");
      mappingGrp += PvlKeyword("MaximumLongitude", "360");

      Distance radii[3];
      cam->Radii(radii);

      mappingGrp += PvlKeyword("EquatorialRadius", radii[0].GetMeters(),
                               "meters");
      mappingGrp += PvlKeyword("PolarRadius", radii[2].GetMeters(),
                               "meters");

      proj = ProjectionFactory::Create(mappingPvl);
      e.Clear();
    }

    return proj;
  }

  /**
   * Returns a list of all the cubes selected in the scene
   *
   * @return QList<Cube *>
   */
  QList<CubeDisplayProperties *> MosaicSceneWidget::getSelectedCubes() const {
    QList<CubeDisplayProperties *> cubes;

    MosaicSceneItem *mosaicItem;
    foreach(mosaicItem, *p_mosaicSceneItems) {
      if(mosaicItem->isSelected()) {
        cubes.append(mosaicItem->cubeDisplay());
      }
    }

    return cubes;
  }


  void MosaicSceneWidget::addToPermanent(QToolBar *perm) {
    if(!p_mapAction) {
      p_mapAction = new QAction(this);
      p_mapAction->setToolTip("Select Map File");
      p_mapAction->setText("Select Map File");

      if(p_projection) {
        PvlKeyword projectionKeyword =
            p_projection->Mapping().FindKeyword("ProjectionName");
        QString projName = QString::fromStdString(projectionKeyword[0]);
        p_mapAction->setText(projName);
      }
    }

    perm->addAction(p_mapAction);

    connect(p_mapAction, SIGNAL(triggered()), this, SLOT(askNewProjection()));
  }


  void MosaicSceneWidget::addTo(QToolBar *toolbar) {
    MosaicTool *tool;
    foreach(tool, *p_tools) {
      tool->addTo(toolbar);
    }

    p_userToolControl = true;

    getView()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    getView()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    getView()->enableResizeZooming(false);
  }


  void MosaicSceneWidget::addTo(QMenu *menu) {
    MosaicTool *tool;
    foreach(tool, *p_tools) {
      tool->addTo(menu);
    }
  }


  void MosaicSceneWidget::addTo(Qisis::ToolPad *toolPad) {
    MosaicTool *tool;
    foreach(tool, *p_tools) {
      tool->addTo(toolPad);
    }
  }


  void MosaicSceneWidget::enableRubberBand(bool enable) {
    p_customRubberBandEnabled = enable;
  }


  void MosaicSceneWidget::blockSelectionChange(bool block) {
    if(block) {
      disconnect(getScene(), SIGNAL(selectionChanged()),
                 this, SLOT(onSelectionChanged()));
    }
    else {
      connect(getScene(), SIGNAL(selectionChanged()),
              this, SLOT(onSelectionChanged()));
    }
  }


  QProgressBar *MosaicSceneWidget::getProgress() {
    return p_progress;
  }


  PvlObject MosaicSceneWidget::toPvl() const {
    PvlObject output("MosaicScene");

    if(p_projection) {
      output += p_projection->Mapping();

      MosaicTool *tool;
      foreach(tool, *p_tools) {
        if(tool->projectPvlObjectName() != "") {
          PvlObject toolObj = tool->toPvl();
          toolObj.SetName(tool->projectPvlObjectName());
          output += toolObj;
        }
      }
    }
    else {
      throw iException::Message(iException::User,
          "Cannot save a scene without a projection to a project file",
          _FILEINFO_);
    }

    return output;
  }

  void MosaicSceneWidget::fromPvl(PvlObject project) {
    Pvl tmp;
    tmp += project.FindGroup("Mapping");
    setProjection(ProjectionFactory::Create(tmp));
    p_ownProjection = true;

    MosaicTool *tool;
    foreach(tool, *p_tools) {
      if(tool->projectPvlObjectName() != "") {
        if(project.HasObject(tool->projectPvlObjectName())) {
          tool->fromPvl(project.FindObject(tool->projectPvlObjectName()));
        }
      }
    }
  }


  QRectF MosaicSceneWidget::cubesBoundingRect() const {
    QRectF boundingRect;

    MosaicSceneItem * mosaicItem;
    foreach(mosaicItem, *p_mosaicSceneItems) {
      if(boundingRect.isEmpty())
        boundingRect = mosaicItem->boundingRect();
      else
        boundingRect = boundingRect.united(mosaicItem->boundingRect());
    }

    if(p_outlineRect)
      boundingRect = boundingRect.united(p_outlineRect->boundingRect());

    return boundingRect;
  }

  MosaicSceneItem *MosaicSceneWidget::addCube(CubeDisplayProperties *cube) {
    if(p_projection == NULL) {
      setProjection(createInitialProjection(cube));
      p_ownProjection = true;
    }

    // Verify we don't have this cube already
    try {
      cubeToMosaic(cube);
      return NULL; // We have one already, ignore the add request
    }
    catch(iException &e) {
      e.Clear();
    }

    MosaicSceneItem *mosItem = new MosaicSceneItem(cube, this);

    connect(mosItem, SIGNAL(changed(const QList<QRectF> &)),
            p_graphicsView, SLOT(updateScene(const QList<QRectF> &)));

    // We want everything to have a unique Z value so we can manage the z order
    //   well.
    mosItem->setZValue(maximumZ() + 1);

    getScene()->addItem(mosItem);
    p_mosaicSceneItems->append(mosItem);

    connect(mosItem, SIGNAL(destroyed(QObject *)),
            this, SLOT(removeMosItem(QObject *)));

    connect(cube, SIGNAL(moveDownOne()),
            this, SLOT(moveDownOne()));
    connect(cube, SIGNAL(moveToBottom()),
            this, SLOT(moveToBottom()));
    connect(cube, SIGNAL(moveUpOne()),
            this, SLOT(moveUpOne()));
    connect(cube, SIGNAL(moveToTop()),
            this, SLOT(moveToTop()));
    connect(cube, SIGNAL(zoomFit()),
            this, SLOT(fitInView()));

    return mosItem;
  }


  qreal MosaicSceneWidget::maximumZ() {
    // 0 is okay for a maximum even if everything is below
    qreal maxZ = 0;
    MosaicSceneItem *mosaicItem;
    foreach(mosaicItem, *p_mosaicSceneItems) {
      if(mosaicItem->zValue() > maxZ)
        maxZ = mosaicItem->zValue();
    }

    return maxZ;
  }


  qreal MosaicSceneWidget::minimumZ() {
    // 0 is okay for a minimum even if everything is above
    qreal minZ = 0;
    MosaicSceneItem *mosaicItem;
    foreach(mosaicItem, *p_mosaicSceneItems) {
      if(mosaicItem->zValue() < minZ)
        minZ = mosaicItem->zValue();
    }

    return minZ;
  }

  void MosaicSceneWidget::recalcSceneRect() {
    if(p_projection) {
      double minX, minY, maxX, maxY;
      p_projection->XYRange(minX, maxX, minY, maxY);

      QRectF projRect(minX, minY, maxX - minX, maxY - minY);
      QRectF cubesBounding = cubesBoundingRect();

      QRectF bounding = projRect.united(cubesBounding);

      if(p_outlineRect && p_outlineRect->isVisible())
        bounding = bounding.united(p_outlineRect->boundingRect());

      getView()->setSceneRect(bounding);
    }
  }

  void MosaicSceneWidget::addCubes(QList<CubeDisplayProperties *> cubes) {
    QList<QGraphicsItem *> sceneItems;

    if(p_userToolControl)
      p_progress->setText("Loading primary scene");
    else
      p_progress->setText("Loading secondary scene");

    p_progress->setRange(0, cubes.size() - 1);
    p_progress->setValue(0);
    p_progress->setVisible(true);

    CubeDisplayProperties *cube;
    foreach(cube, cubes) {
      try {
        MosaicSceneItem *item = addCube(cube);

        if(item)
          sceneItems.append(item);
      }
      catch(iException &e) {
        e.Report(false);
        e.Clear();
      }

      p_progress->setValue(p_progress->value() + 1);
    }

    recalcSceneRect();
    refit();

    p_progress->setVisible(false);
    emit cubesChanged();
  }


  void MosaicSceneWidget::removeMosItem(QObject *mosItem) {
    MosaicSceneItem *castedMosItem = (MosaicSceneItem*) mosItem;
    p_mosaicSceneItems->removeAll(castedMosItem);
    recalcSceneRect();
    emit cubesChanged();
  }


  /**
   * This method creates the reference footprint if defined in the map file.
   *
   */
  void MosaicSceneWidget::createReferenceFootprint() {
    return; // this currently is not implemented
    double x = 0;
    double y = 0;

    QVector<QPointF> footprintPoints;
    QPolygonF footprintPoly;

    // need to create a polygon from the min/max lat/long numbers in the map
    //   file.
    try {
      PvlGroup mapping(p_projection->Mapping());
      PvlKeyword minLatKeyword = mapping.FindKeyword("MinimumLatitude");
      Latitude minLat(minLatKeyword[0], mapping, Angle::Degrees);
      PvlKeyword minLonKeyword = mapping.FindKeyword("MinimumLongitude");
      Longitude minLon(minLonKeyword[0], mapping, Angle::Degrees);
      PvlKeyword maxLatKeyword = mapping.FindKeyword("MaximumLatitude");
      Latitude maxLat(maxLatKeyword[0], mapping, Angle::Degrees);
      PvlKeyword maxLonKeyword = mapping.FindKeyword("MaximumLongitude");
      Longitude maxLon(maxLonKeyword[0], mapping, Angle::Degrees);

      Angle increment(1, Angle::Degrees);
      if(p_projection->SetUniversalGround(minLat.GetDegrees(),
         minLon.GetDegrees())) {
        x = p_projection->XCoord();
        y = -1 * (p_projection->YCoord());
        footprintPoints.push_back(QPointF(x, y));
      }

      for(Angle lat = minLat + increment; lat < maxLat; lat += increment) {
        if(p_projection->SetUniversalGround(lat.GetDegrees(),
           minLon.GetDegrees())) {
          x = p_projection->XCoord();
          y = -1 * (p_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }
      for(Angle lon = minLon + increment; lon < maxLon; lon += increment) {
        if(p_projection->SetUniversalGround(maxLat.GetDegrees(),
           lon.GetDegrees())) {
          x = p_projection->XCoord();
          y = -1 * (p_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }
      for(Angle lat = maxLat; lat > minLat + increment; lat -= increment) {
        if(p_projection->SetUniversalGround(lat.GetDegrees(),
           maxLon.GetDegrees())) {
          x = p_projection->XCoord();
          y = -1 * (p_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }
      for(Angle lon = maxLon; lon > minLon + increment; lon -= increment) {
        if(p_projection->SetUniversalGround(minLat.GetDegrees(),
           lon.GetDegrees())) {
          x = p_projection->XCoord();
          y = -1 * (p_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }

      //Now close the polygon.
      if(p_projection->SetUniversalGround(minLat.GetDegrees(),
         minLon.GetDegrees())) {
        x = p_projection->XCoord();
        y = -1 * (p_projection->YCoord());
        footprintPoints.push_back(QPointF(x, y));
      }
      footprintPoly = QPolygonF(footprintPoints);
      p_projectionFootprint->setPolygon(footprintPoly);
      p_projectionFootprint->setBrush(QBrush(QColor(255, 255, 0, 100)));
      p_projectionFootprint->setPen(QColor(Qt::black));
      p_projectionFootprint->setZValue(-FLT_MAX);
      p_projectionFootprint->setFlag(QGraphicsItem::ItemIsSelectable, false);
      p_graphicsScene->addItem(p_projectionFootprint);
      //p_graphicsView->fitInView(p_footprintItem, Qt::KeepAspectRatio);
      p_projectionFootprint->show();

    }
    catch(Isis::iException &e) {
      std::string msg = e.Errors();
      QMessageBox::information(this, "Error", QString::fromStdString(msg),
                               QMessageBox::Ok);
      //p_showReference->setChecked(Qt::Unchecked);
      return;
    }
  }


  /**
   * This method refits t:he items in the graphics view.
   *
   */
  void MosaicSceneWidget::refit() {
    QRectF sceneRect = cubesBoundingRect();

    if(sceneRect.isEmpty())
      return;

    double xPadding = sceneRect.width() * 0.10;
    double yPadding = sceneRect.height() * 0.10;

    sceneRect.adjust(-xPadding, -yPadding, xPadding, yPadding);
    getView()->fitInView(sceneRect, Qt::KeepAspectRatio);
  }


  void MosaicSceneWidget::setCubesSelectable(bool selectable) {
    if(p_cubesSelectable != selectable) {
      p_cubesSelectable = selectable;

      MosaicSceneItem *mosaicSceneItem;
      foreach(mosaicSceneItem, *p_mosaicSceneItems) {
        mosaicSceneItem->scenePropertiesChanged();
      }
    }
  }


  void MosaicSceneWidget::askNewProjection() {
    QString mapFile = QFileDialog::getOpenFileName(this,
            "Select Map File",
            ".",
            "Map Files (*.map);;All Files (*)");
    if(mapFile.isEmpty()) return;

    try {
      Isis::Pvl pvl;
      pvl.Read(mapFile.toStdString());

      PvlGroup &mapping = pvl.FindGroup("Mapping", Pvl::Traverse);

      if(!mapping.HasKeyword("MinimumLatitude"))
        mapping += PvlKeyword("MinimumLatitude", -90);

      if(!mapping.HasKeyword("MaximumLatitude"))
        mapping += PvlKeyword("MaximumLatitude", 90);

      if(!mapping.HasKeyword("MinimumLongitude")) {
        if(mapping["LongitudeDomain"][0] == "360")
          mapping += PvlKeyword("MinimumLongitude", 0);
        else
          mapping += PvlKeyword("MinimumLongitude", -180);
      }

      if(!mapping.HasKeyword("MaximumLongitude")) {
        if(mapping["LongitudeDomain"][0] == "360")
          mapping += PvlKeyword("MaximumLongitude", 360);
        else
          mapping += PvlKeyword("MaximumLongitude", 180);
      }

      Isis::Projection *proj = Isis::ProjectionFactory::Create(pvl);
      setProjection(proj);
      p_ownProjection = true;
    }
    catch(Isis::iException &e) {
      std::string msg = e.Errors();
      QMessageBox::information(this, "Error", QString::fromStdString(msg), QMessageBox::Ok);
      return;
    }
  }


  void MosaicSceneWidget::sendVisibleRectChanged() {
    QPointF topLeft = getView()->mapToScene(0, 0);
    QPointF bottomRight = getView()->mapToScene(
        (int)getView()->width(),
        (int)getView()->height());

    QRectF visibleRect(topLeft, bottomRight);
    emit visibleRectChanged(visibleRect);
  }


  bool MosaicSceneWidget::eventFilter(QObject *obj, QEvent *event) {
    bool stopProcessingEvent = true;

    switch(event->type()) {
      case QMouseEvent::GraphicsSceneMousePress: {
        if(p_customRubberBandEnabled) {
          // Intiate the rubber banding!
          if(!p_customRubberBand) {
            p_customRubberBand = new QRubberBand(QRubberBand::Rectangle,
                                                 getView());
          }

          if(!p_rubberBandOrigin) {
            p_rubberBandOrigin = new QPoint;
          }

          *p_rubberBandOrigin = getView()->mapFromScene(
              ((QGraphicsSceneMouseEvent *)event)->scenePos());
          p_customRubberBand->setGeometry(QRect(*p_rubberBandOrigin, QSize()));
          p_customRubberBand->show();
        }

        emit mouseButtonPress(
              ((QGraphicsSceneMouseEvent *)event)->scenePos(),
              ((QGraphicsSceneMouseEvent *)event)->button());

        stopProcessingEvent = false;
        break;
      }

      case QMouseEvent::GraphicsSceneMouseRelease: {
        bool signalEmitted = false;
        if(p_customRubberBandEnabled && p_rubberBandOrigin &&
           p_customRubberBand) {
          if(p_customRubberBand->geometry().width() +
             p_customRubberBand->geometry().height() > 10) {
            emit rubberBandComplete(
                getView()->mapToScene(
                    p_customRubberBand->geometry()).boundingRect(),
                ((QGraphicsSceneMouseEvent *)event)->button());
            signalEmitted = true;
          }

          delete p_rubberBandOrigin;
          p_rubberBandOrigin = NULL;

          delete p_customRubberBand;
          p_customRubberBand = NULL;
        }

        if(!signalEmitted) {
          stopProcessingEvent = false;
          emit mouseButtonRelease(
                ((QGraphicsSceneMouseEvent *)event)->scenePos(),
                ((QGraphicsSceneMouseEvent *)event)->button());
        }
        break;
      }

      case QMouseEvent::GraphicsSceneMouseDoubleClick:
        emit mouseDoubleClick(
              ((QGraphicsSceneMouseEvent *)event)->scenePos());
        stopProcessingEvent = false;
        break;

      case QMouseEvent::GraphicsSceneMouseMove:
        if(p_customRubberBandEnabled && p_rubberBandOrigin &&
           p_customRubberBand) {
          QPointF scenePos = ((QGraphicsSceneMouseEvent *)event)->scenePos();
          QPoint screenPos = getView()->mapFromScene(scenePos);

          QRect rubberBandRect =
              QRect(*p_rubberBandOrigin, screenPos).normalized();

          p_customRubberBand->setGeometry(rubberBandRect);
        }
        else {
          stopProcessingEvent = false;
        }

        emit mouseMove(
              ((QGraphicsSceneMouseEvent *)event)->scenePos());
        break;

      case QEvent::GraphicsSceneWheel:
        emit mouseWheel(
              ((QGraphicsSceneWheelEvent *)event)->scenePos(),
              ((QGraphicsSceneWheelEvent *)event)->delta());
          stopProcessingEvent = true;

      case QMouseEvent::Enter:
        emit mouseEnter();
        stopProcessingEvent = false;
        break;

      case QMouseEvent::Leave:
        emit mouseLeave();
        stopProcessingEvent = false;
        break;

      case QEvent::GraphicsSceneHelp: {
        double maxZ = DBL_MIN;
        MosaicSceneItem *mosaicSceneItem;
        foreach(mosaicSceneItem, *p_mosaicSceneItems) {
          if(maxZ < mosaicSceneItem->zValue()) {
            if(mosaicSceneItem->contains(
              ((QGraphicsSceneHelpEvent*)event)->scenePos())) {
              setToolTip(mosaicSceneItem->toolTip());
              maxZ = mosaicSceneItem->zValue();
              stopProcessingEvent = false; // We want the tooltip to proceed
            }
          }
        }

        if(maxZ == DBL_MIN) {
          setToolTip("");
        }
        break;
      }

      default:
        stopProcessingEvent = false;
        break;
    }

    return stopProcessingEvent;
  }


  /**
   * Reprojects all the items in the view.
   * Also makes sure to resize the view rectangle to fit the newly
   * projected footprints.
   *
   */
  void MosaicSceneWidget::reprojectItems() {
    if(p_mosaicSceneItems->size() == 0)
      return;

    if(p_userToolControl)
      p_progress->setText("Reprojecting primary scene");
    else
      p_progress->setText("Reprojecting secondary scene");

    // This gives some pretty graphics as thing work
    const int reprojectsPerUpdate = qMax(1, p_mosaicSceneItems->size() / 20);

    p_progress->setRange(0,
        (p_mosaicSceneItems->size() - 1) / reprojectsPerUpdate + 1);
    p_progress->setValue(0);
    p_progress->setVisible(true);

    MosaicSceneItem *mosaicSceneItem;

    int progressCountdown = reprojectsPerUpdate;
    foreach(mosaicSceneItem, *p_mosaicSceneItems) {
      mosaicSceneItem->reproject();

      progressCountdown --;
      if(progressCountdown == 0) {
        p_progress->setValue(p_progress->value() + 1);
        progressCountdown = reprojectsPerUpdate;
        refit();
      }
    }

    p_progress->setValue(p_progress->maximum());

    recalcSceneRect();
    refit();
    p_progress->setVisible(false);
  }


  /**
   * This slot is changes the level of detail at which mosaic
   * footprints will be allowed to have transparency.  This is
   * adjustable because it affects the speed of the re-paint
   * calls.
   *
   * @param detail
   */
  void MosaicSceneWidget::setLevelOfDetail(int detail) {
    MosaicSceneItem *mosaicSceneItem;
    foreach(mosaicSceneItem, *p_mosaicSceneItems) {
      mosaicSceneItem->setLevelOfDetail(detail * 0.005);
      mosaicSceneItem->update();
    }
  }


  void MosaicSceneWidget::moveDownOne() {
    CubeDisplayProperties *cube = (CubeDisplayProperties *)sender();
    MosaicSceneItem *item = cubeToMosaic(cube);
    MosaicSceneItem *nextDown = getNextItem(item, false);
    if(nextDown) {
      qreal newZValue = nextDown->zValue() - 1;

      MosaicSceneItem *mosaicSceneItem;
      foreach(mosaicSceneItem, *p_mosaicSceneItems) {
        if(mosaicSceneItem->zValue() <= newZValue) {
          mosaicSceneItem->setZValue(mosaicSceneItem->zValue() - 1);
        }
      }

      item->setZValue(newZValue);
    }

    getNextItem(item, false);
  }


  void MosaicSceneWidget::moveToBottom() {
    CubeDisplayProperties *cube = (CubeDisplayProperties *)sender();
    MosaicSceneItem *item = cubeToMosaic(cube);
    qreal minZ = minimumZ();

    if(item->zValue() != minZ) {
      // We know min-1 isn't already used
      item->setZValue(minZ - 1);
    }
  }


  void MosaicSceneWidget::moveUpOne() {
    CubeDisplayProperties *cube = (CubeDisplayProperties *)sender();
    MosaicSceneItem *item = cubeToMosaic(cube);
    MosaicSceneItem *nextUp = getNextItem(item, true);

    if(nextUp) {
      qreal newZValue = nextUp->zValue() + 1;

      MosaicSceneItem *mosaicSceneItem;
      foreach(mosaicSceneItem, *p_mosaicSceneItems) {
        if(mosaicSceneItem->zValue() >= newZValue) {
          mosaicSceneItem->setZValue(mosaicSceneItem->zValue() + 1);
        }
      }

      item->setZValue(newZValue);
    }
  }


  void MosaicSceneWidget::moveToTop() {
    CubeDisplayProperties *cube = (CubeDisplayProperties *)sender();
    MosaicSceneItem *item = cubeToMosaic(cube);
    qreal maxZ = maximumZ();

    if(item->zValue() != maxZ) {
      // We know min-1 isn't already used
      item->setZValue(maxZ + 1);
    }
  }


  void MosaicSceneWidget::fitInView() {
    if(p_userToolControl) {
      CubeDisplayProperties *cube = (CubeDisplayProperties *)sender();
      MosaicSceneItem *item = cubeToMosaic(cube);
      QRectF boundingBox = item->boundingRect();

      double xPadding = boundingBox.width() * 0.10;
      double yPadding = boundingBox.height() * 0.10;

      boundingBox.setLeft(boundingBox.left() - xPadding);
      boundingBox.setRight(boundingBox.right() + xPadding);

      boundingBox.setTop(boundingBox.top() - yPadding);
      boundingBox.setBottom(boundingBox.bottom() + yPadding);

      getView()->fitInView(boundingBox, Qt::KeepAspectRatio);
      getView()->centerOn(boundingBox.center());
    }
  }


  void MosaicSceneWidget::onSelectionChanged() {
    MosaicSceneItem *mosaicSceneItem;
    foreach(mosaicSceneItem, *p_mosaicSceneItems) {
      mosaicSceneItem->updateSelection(true);
    }
  }


  //! Implemented because we want invisible items too
  MosaicSceneItem *MosaicSceneWidget::getNextItem(MosaicSceneItem *item,
                                                      bool up) {
    MosaicSceneItem *nextZValueItem = NULL;
    MosaicSceneItem *mosaicSceneItem;

    QMap<qreal, bool> zvals;

    foreach(mosaicSceneItem, *p_mosaicSceneItems) {
      if(mosaicSceneItem != item &&
         mosaicSceneItem->boundingRect().intersects(item->boundingRect())) {
        // Does this item qualify as above or below at all?
        if( (up  && mosaicSceneItem->zValue() > item->zValue()) ||
            (!up && mosaicSceneItem->zValue() < item->zValue())) {
          // It is in the correct direction, set the initial guess if we don't
          //   have one or test if it's better
          if(!nextZValueItem) {
            nextZValueItem = mosaicSceneItem;
          }
          else {
            // We know it qualifies, we wan't to know if it's closer than
            //   nextZValueItem
            if( (up &&
                      mosaicSceneItem->zValue() < nextZValueItem->zValue()) ||
                (!up &&
                      mosaicSceneItem->zValue() > nextZValueItem->zValue())) {
              nextZValueItem = mosaicSceneItem;
            }
          }
        }
      }
    }

    return nextZValueItem;
  }


  MosaicSceneItem *MosaicSceneWidget::cubeToMosaic(
      CubeDisplayProperties *cubeDisplay) {
    MosaicSceneItem *mosaicSceneItem;
    foreach(mosaicSceneItem, *p_mosaicSceneItems) {
      if(mosaicSceneItem->cubeDisplay() == cubeDisplay)
        return mosaicSceneItem;
    }

    iString msg = "Cube is not in the mosaic";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  QStringList MosaicSceneWidget::cubeFilenames() {
    QStringList cubes;

    MosaicSceneItem *mosaicSceneItem;
    foreach(mosaicSceneItem, *p_mosaicSceneItems) {
      if(mosaicSceneItem->cubeDisplay())
        cubes.append(mosaicSceneItem->cubeDisplay()->fileName());
    }

    return cubes;
  }


  QList<CubeDisplayProperties *> MosaicSceneWidget::cubeDisplays() {
    QList<CubeDisplayProperties *> displays;

    MosaicSceneItem *mosaicSceneItem;
    foreach(mosaicSceneItem, *p_mosaicSceneItems) {
      if(mosaicSceneItem->cubeDisplay())
        displays.append(mosaicSceneItem->cubeDisplay());
    }

    return displays;
  }
}

