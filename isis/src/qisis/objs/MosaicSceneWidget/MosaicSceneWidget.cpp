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
#include <QToolTip>

#include "Camera.h"
#include "Cube.h"
#include "CubeDisplayProperties.h"
#include "Distance.h"
#include "GraphicsView.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MosaicAreaTool.h"
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
    m_mosaicSceneItems = new QList<MosaicSceneItem *>;

    m_graphicsScene = new QGraphicsScene(this);
    m_graphicsScene->installEventFilter(this);

    m_graphicsView = new MosaicGraphicsView(m_graphicsScene, this);
    m_graphicsView->setScene(m_graphicsScene);
    m_graphicsView->setInteractive(true);
    m_graphicsView->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    m_graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    // This enables OpenGL acceleration
//     m_graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));

    QLayout * sceneLayout = new QHBoxLayout;
    sceneLayout->addWidget(m_graphicsView);
    sceneLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(sceneLayout);

    m_projection = NULL;
    m_mapAction = NULL;

//     m_projectionFootprint = new QGraphicsPolygonItem();
//     m_projectionFootprint->hide();

    m_cubesSelectable = true;
    m_customRubberBandEnabled = false;
    m_customRubberBand = NULL;
    m_rubberBandOrigin = NULL;
    m_outlineRect = NULL;

    // Create the tools we want
    m_tools = new QList<MosaicTool *>;
    m_tools->append(new MosaicSelectTool(this));
    m_tools->append(new MosaicZoomTool(this));
    m_tools->append(new MosaicPanTool(this));
    m_tools->append(new MosaicControlNetTool(this));
    m_tools->append(new MosaicAreaTool(this));
    m_tools->append(new MosaicFindTool(this));
    if(status)
      m_tools->append(new MosaicTrackTool(this, status));

    m_tools->at(0)->activate(true);

    blockSelectionChange(false);

    m_userToolControl = false;
    m_ownProjection = false;

    m_progress = new ProgressBar;
    m_progress->setVisible(false);

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
    m_outlineRect = NULL; // The scene will clean this up

    if(m_tools) {
      MosaicTool *tool;
      foreach(tool, *m_tools) {
        delete tool;
        tool = NULL;
      }

      delete m_tools;
      m_tools = NULL;
    }

    if(m_ownProjection && m_projection) {
      delete m_projection;
    }
    m_projection = NULL;
  }


  void MosaicSceneWidget::setProjection(const PvlGroup &mapping) {
    Pvl tmp;
    tmp += mapping;
   
    if(!mapping.HasKeyword("EquatorialRadius")) { 
      PvlGroup radii = Projection::TargetRadii(mapping["TargetName"]);
      tmp.FindGroup("Mapping") += radii["EquatorialRadius"];
      tmp.FindGroup("Mapping") += radii["PolarRadius"];
    }

    setProjection(ProjectionFactory::Create(tmp));
    m_ownProjection = true;
  }

  /**
   * This method takes ownership of proj
   */
  void MosaicSceneWidget::setProjection(Projection *proj) {
    PvlGroup mapping(proj->Mapping());

    if(m_mapAction) {
      PvlKeyword projectionKeyword = mapping.FindKeyword("ProjectionName");
      QString projName = QString::fromStdString(projectionKeyword[0]);
      m_mapAction->setText(projName);
    }

    Projection *old = m_projection;
    m_projection = proj;

    reprojectItems();
    emit projectionChanged(m_projection);

    if(old && m_ownProjection) {
      delete old;
      old = NULL;
    }

    m_ownProjection = false;
  }



  void MosaicSceneWidget::setOutlineRect(QRectF outline) {
    if(outline.united(getView()->sceneRect()) != getView()->sceneRect())
      outline = QRectF();

    if(!m_outlineRect) {
      m_outlineRect = getScene()->addRect(outline,
                                          QPen(Qt::black),
                                          Qt::NoBrush);
      m_outlineRect->setZValue(DBL_MAX);
    }
    else {
      m_outlineRect->setRect(outline);
    }

    if(!m_userToolControl)
      refit();
  }


  PvlGroup MosaicSceneWidget::createInitialProjection(
      CubeDisplayProperties * cubeDisplay) {
    Projection *proj = NULL;
    Cube *cube = cubeDisplay->cube();
    Pvl *label = cube->getLabel();

    try {
      proj = ProjectionFactory::CreateFromCube(*label);
      return proj->Mapping();
    }
    catch(iException &e) {
      Pvl mappingPvl("$base/templates/maps/equirectangular.map");
      PvlGroup &mappingGrp = mappingPvl.FindGroup("Mapping");
      mappingGrp += PvlKeyword("LatitudeType", "Planetocentric");
      mappingGrp += PvlKeyword("LongitudeDirection", "PositiveEast");
      mappingGrp += PvlKeyword("LongitudeDomain", "360");
      mappingGrp += PvlKeyword("CenterLatitude", "0");
      mappingGrp += PvlKeyword("CenterLongitude", "180");
      mappingGrp += PvlKeyword("MinimumLatitude", "-90");
      mappingGrp += PvlKeyword("MaximumLatitude", "90");
      mappingGrp += PvlKeyword("MinimumLongitude", "0");
      mappingGrp += PvlKeyword("MaximumLongitude", "360");

      try {
        Camera * cam = cube->getCamera();
        Distance radii[3];
        cam->Radii(radii);

        mappingGrp += PvlKeyword("TargetName", cam->Target());
        mappingGrp += PvlKeyword("EquatorialRadius", radii[0].GetMeters(),
                                 "meters");
        mappingGrp += PvlKeyword("PolarRadius", radii[2].GetMeters(),
                                 "meters");

      }
      catch(iException &e) {
        mappingGrp +=
            label->FindGroup("Instrument", Pvl::Traverse)["TargetName"];
      }

      e.Clear();
      return mappingGrp;
    }
  }

  /**
   * Returns a list of all the cubes selected in the scene
   *
   * @return QList<Cube *>
   */
  QList<CubeDisplayProperties *> MosaicSceneWidget::getSelectedCubes() const {
    QList<CubeDisplayProperties *> cubes;

    MosaicSceneItem *mosaicItem;
    foreach(mosaicItem, *m_mosaicSceneItems) {
      if(mosaicItem->isSelected()) {
        cubes.append(mosaicItem->cubeDisplay());
      }
    }

    return cubes;
  }


  void MosaicSceneWidget::addToPermanent(QToolBar *perm) {
    if(!m_mapAction) {
      m_mapAction = new QAction(this);
      m_mapAction->setToolTip("Select Map File");
      m_mapAction->setText("Select Map File");

      if(m_projection) {
        PvlKeyword projectionKeyword =
            m_projection->Mapping().FindKeyword("ProjectionName");
        QString projName = QString::fromStdString(projectionKeyword[0]);
        m_mapAction->setText(projName);
      }
    }

    perm->addAction(m_mapAction);

    connect(m_mapAction, SIGNAL(triggered()), this, SLOT(askNewProjection()));
  }


  void MosaicSceneWidget::addTo(QToolBar *toolbar) {
    MosaicTool *tool;
    foreach(tool, *m_tools) {
      tool->addTo(toolbar);
    }

    m_userToolControl = true;

    getView()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    getView()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    getView()->enableResizeZooming(false);
  }


  void MosaicSceneWidget::addTo(QMenu *menu) {
    MosaicTool *tool;
    foreach(tool, *m_tools) {
      tool->addTo(menu);
    }
  }


  void MosaicSceneWidget::addTo(ToolPad *toolPad) {
    MosaicTool *tool;
    foreach(tool, *m_tools) {
      tool->addTo(toolPad);
    }
  }


  void MosaicSceneWidget::enableRubberBand(bool enable) {
    m_customRubberBandEnabled = enable;
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
    return m_progress;
  }


  PvlObject MosaicSceneWidget::toPvl() const {
    PvlObject output("MosaicScene");

    if(m_projection) {
      output += m_projection->Mapping();

      MosaicTool *tool;
      foreach(tool, *m_tools) {
        if(tool->projectPvlObjectName() != "") {
          PvlObject toolObj = tool->toPvl();
          toolObj.SetName(tool->projectPvlObjectName());
          output += toolObj;
        }
      }

      PvlObject zOrders("ZOrdering");
      foreach(MosaicSceneItem * mosaicSceneItem, *m_mosaicSceneItems) {
        PvlKeyword zValue("ZValue");
        zValue += (iString)mosaicSceneItem->cubeDisplay()->fileName();
        zValue += mosaicSceneItem->zValue();
        zOrders += zValue;
      }

      output += zOrders;
    }
    else {
      throw iException::Message(iException::User,
          "Cannot save a scene without a projection to a project file",
          _FILEINFO_);
    }

    return output;
  }

  void MosaicSceneWidget::fromPvl(PvlObject project) {
    setProjection(project.FindGroup("Mapping"));

    recalcSceneRect();

    MosaicTool *tool;
    foreach(tool, *m_tools) {
      if(tool->projectPvlObjectName() != "") {
        if(project.HasObject(tool->projectPvlObjectName())) {
          tool->fromPvl(project.FindObject(tool->projectPvlObjectName()));
        }
      }

      if (project.HasObject("ZOrdering")) {
        PvlObject &zOrders = project.FindObject("ZOrdering");

        for (int zOrderIndex = 0;
             zOrderIndex < zOrders.Keywords();
             zOrderIndex ++) {
          PvlKeyword &zOrder = zOrders[zOrderIndex];

          QString filenameToFind = zOrder[0];

          bool found = false;
          for (int itemIndex = 0;
               itemIndex < m_mosaicSceneItems->size() && !found;
               itemIndex++) {
            MosaicSceneItem * mosaicSceneItem =
                (*m_mosaicSceneItems)[itemIndex];

            if (mosaicSceneItem->cubeDisplay()->fileName() == filenameToFind) {
              mosaicSceneItem->setZValue(zOrder[1]);
              found = true;
            }
          }
        }
      }
    }
  }


  QRectF MosaicSceneWidget::cubesBoundingRect() const {
    QRectF boundingRect;

    MosaicSceneItem * mosaicItem;
    foreach(mosaicItem, *m_mosaicSceneItems) {
      if(boundingRect.isEmpty())
        boundingRect = mosaicItem->boundingRect();
      else
        boundingRect = boundingRect.united(mosaicItem->boundingRect());
    }

    if(m_outlineRect)
      boundingRect = boundingRect.united(m_outlineRect->boundingRect());

    return boundingRect;
  }

  MosaicSceneItem *MosaicSceneWidget::addCube(CubeDisplayProperties *cube) {
    if(m_projection == NULL) {
      setProjection(createInitialProjection(cube));
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
            m_graphicsView, SLOT(updateScene(const QList<QRectF> &)));

    // We want everything to have a unique Z value so we can manage the z order
    //   well.
    mosItem->setZValue(maximumZ() + 1);

    getScene()->addItem(mosItem);
    m_mosaicSceneItems->append(mosItem);

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
    foreach(mosaicItem, *m_mosaicSceneItems) {
      if(mosaicItem->zValue() > maxZ)
        maxZ = mosaicItem->zValue();
    }

    return maxZ;
  }


  qreal MosaicSceneWidget::minimumZ() {
    // 0 is okay for a minimum even if everything is above
    qreal minZ = 0;
    MosaicSceneItem *mosaicItem;
    foreach(mosaicItem, *m_mosaicSceneItems) {
      if(mosaicItem->zValue() < minZ)
        minZ = mosaicItem->zValue();
    }

    return minZ;
  }

  void MosaicSceneWidget::recalcSceneRect() {
    if(m_projection) {
      double minX, minY, maxX, maxY;
      m_projection->XYRange(minX, maxX, minY, maxY);

      QRectF projRect(minX, minY, maxX - minX, maxY - minY);
      QRectF cubesBounding = cubesBoundingRect();

      QRectF bounding = projRect.united(cubesBounding);

      if(m_outlineRect && m_outlineRect->isVisible())
        bounding = bounding.united(m_outlineRect->boundingRect());

      getView()->setSceneRect(bounding);
    }
  }

  void MosaicSceneWidget::addCubes(QList<CubeDisplayProperties *> cubes) {
    QList<QGraphicsItem *> sceneItems;

    if(m_userToolControl)
      m_progress->setText("Loading primary scene");
    else
      m_progress->setText("Loading secondary scene");

    m_progress->setRange(0, cubes.size() - 1);
    m_progress->setValue(0);
    m_progress->setVisible(true);

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

      m_progress->setValue(m_progress->value() + 1);
    }

    recalcSceneRect();
    refit();

    m_progress->setVisible(false);
    emit cubesChanged();
  }


  void MosaicSceneWidget::removeMosItem(QObject *mosItem) {
    MosaicSceneItem *castedMosItem = (MosaicSceneItem*) mosItem;
    m_mosaicSceneItems->removeAll(castedMosItem);
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
      PvlGroup mapping(m_projection->Mapping());
      PvlKeyword minLatKeyword = mapping.FindKeyword("MinimumLatitude");
      Latitude minLat(minLatKeyword[0], mapping, Angle::Degrees);
      PvlKeyword minLonKeyword = mapping.FindKeyword("MinimumLongitude");
      Longitude minLon(minLonKeyword[0], mapping, Angle::Degrees);
      PvlKeyword maxLatKeyword = mapping.FindKeyword("MaximumLatitude");
      Latitude maxLat(maxLatKeyword[0], mapping, Angle::Degrees);
      PvlKeyword maxLonKeyword = mapping.FindKeyword("MaximumLongitude");
      Longitude maxLon(maxLonKeyword[0], mapping, Angle::Degrees);

      Angle increment(1, Angle::Degrees);
      if(m_projection->SetUniversalGround(minLat.GetDegrees(),
         minLon.GetDegrees())) {
        x = m_projection->XCoord();
        y = -1 * (m_projection->YCoord());
        footprintPoints.push_back(QPointF(x, y));
      }

      for(Angle lat = minLat + increment; lat < maxLat; lat += increment) {
        if(m_projection->SetUniversalGround(lat.GetDegrees(),
           minLon.GetDegrees())) {
          x = m_projection->XCoord();
          y = -1 * (m_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }
      for(Angle lon = minLon + increment; lon < maxLon; lon += increment) {
        if(m_projection->SetUniversalGround(maxLat.GetDegrees(),
           lon.GetDegrees())) {
          x = m_projection->XCoord();
          y = -1 * (m_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }
      for(Angle lat = maxLat; lat > minLat + increment; lat -= increment) {
        if(m_projection->SetUniversalGround(lat.GetDegrees(),
           maxLon.GetDegrees())) {
          x = m_projection->XCoord();
          y = -1 * (m_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }
      for(Angle lon = maxLon; lon > minLon + increment; lon -= increment) {
        if(m_projection->SetUniversalGround(minLat.GetDegrees(),
           lon.GetDegrees())) {
          x = m_projection->XCoord();
          y = -1 * (m_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }

      //Now close the polygon.
      if(m_projection->SetUniversalGround(minLat.GetDegrees(),
         minLon.GetDegrees())) {
        x = m_projection->XCoord();
        y = -1 * (m_projection->YCoord());
        footprintPoints.push_back(QPointF(x, y));
      }
      footprintPoly = QPolygonF(footprintPoints);
      m_projectionFootprint->setPolygon(footprintPoly);
      m_projectionFootprint->setBrush(QBrush(QColor(255, 255, 0, 100)));
      m_projectionFootprint->setPen(QColor(Qt::black));
      m_projectionFootprint->setZValue(-FLT_MAX);
      m_projectionFootprint->setFlag(QGraphicsItem::ItemIsSelectable, false);
      m_graphicsScene->addItem(m_projectionFootprint);
      //m_graphicsView->fitInView(m_footprintItem, Qt::KeepAspectRatio);
      m_projectionFootprint->show();

    }
    catch(iException &e) {
      std::string msg = e.Errors();
      QMessageBox::information(this, "Error", QString::fromStdString(msg),
                               QMessageBox::Ok);
      //m_showReference->setChecked(Qt::Unchecked);
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
    if(m_cubesSelectable != selectable) {
      m_cubesSelectable = selectable;

      MosaicSceneItem *mosaicSceneItem;
      foreach(mosaicSceneItem, *m_mosaicSceneItems) {
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
      Pvl pvl;
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

      setProjection(mapping);
    }
    catch(iException &e) {
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
        if(m_customRubberBandEnabled) {
          // Intiate the rubber banding!
          if(!m_customRubberBand) {
            m_customRubberBand = new QRubberBand(QRubberBand::Rectangle,
                                                 getView());
          }

          if(!m_rubberBandOrigin) {
            m_rubberBandOrigin = new QPoint;
          }

          *m_rubberBandOrigin = getView()->mapFromScene(
              ((QGraphicsSceneMouseEvent *)event)->scenePos());
          m_customRubberBand->setGeometry(QRect(*m_rubberBandOrigin, QSize()));
          m_customRubberBand->show();
        }

        emit mouseButtonPress(
              ((QGraphicsSceneMouseEvent *)event)->scenePos(),
              ((QGraphicsSceneMouseEvent *)event)->button());

        stopProcessingEvent = false;
        break;
      }

      case QMouseEvent::GraphicsSceneMouseRelease: {
        bool signalEmitted = false;
        if(m_customRubberBandEnabled && m_rubberBandOrigin &&
           m_customRubberBand) {
          if(m_customRubberBand->geometry().width() +
             m_customRubberBand->geometry().height() > 10) {
            emit rubberBandComplete(
                getView()->mapToScene(
                    m_customRubberBand->geometry()).boundingRect(),
                ((QGraphicsSceneMouseEvent *)event)->button());
            signalEmitted = true;
          }

          delete m_rubberBandOrigin;
          m_rubberBandOrigin = NULL;

          delete m_customRubberBand;
          m_customRubberBand = NULL;
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
        if(m_customRubberBandEnabled && m_rubberBandOrigin &&
           m_customRubberBand) {
          QPointF scenePos = ((QGraphicsSceneMouseEvent *)event)->scenePos();
          QPoint screenPos = getView()->mapFromScene(scenePos);

          QRect rubberBandRect =
              QRect(*m_rubberBandOrigin, screenPos).normalized();

          m_customRubberBand->setGeometry(rubberBandRect);
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
        break;

      case QMouseEvent::Enter:
        emit mouseEnter();
        stopProcessingEvent = false;
        break;

      case QMouseEvent::Leave:
        emit mouseLeave();
        stopProcessingEvent = false;
        break;

      case QEvent::GraphicsSceneHelp: {
        setToolTip("");
        bool toolTipFound = false;

        QGraphicsItem *sceneItem;
        foreach(sceneItem, getScene()->items()) {
          if(!toolTipFound) {
            if(sceneItem->contains(
              ((QGraphicsSceneHelpEvent*)event)->scenePos()) &&
              sceneItem->toolTip().size() > 0) {
              setToolTip(sceneItem->toolTip());
              toolTipFound = true;
            }
          }
        }

        if(toolTipFound) {
          stopProcessingEvent = true;
          QToolTip::showText(
              ((QGraphicsSceneHelpEvent*)event)->screenPos(),
              toolTip());
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
    if(m_mosaicSceneItems->size() == 0)
      return;

    if(m_userToolControl)
      m_progress->setText("Reprojecting primary scene");
    else
      m_progress->setText("Reprojecting secondary scene");

    // This gives some pretty graphics as thing work
    const int reprojectsPerUpdate = qMax(1, m_mosaicSceneItems->size() / 20);

    m_progress->setRange(0,
        (m_mosaicSceneItems->size() - 1) / reprojectsPerUpdate + 1);
    m_progress->setValue(0);
    m_progress->setVisible(true);

    MosaicSceneItem *mosaicSceneItem;

    int progressCountdown = reprojectsPerUpdate;
    foreach(mosaicSceneItem, *m_mosaicSceneItems) {
      try {
        mosaicSceneItem->reproject();
      }
      catch(iException &e) {
        iString msg = "The file [";

        if(mosaicSceneItem->cubeDisplay())
          msg += (iString)mosaicSceneItem->cubeDisplay()->displayName();

        msg += "] is being removed due to not being able to project";

        iException &tmp = iException::Message(iException::Programmer,
                                              msg, _FILEINFO_);
        tmp.Report();
        tmp.Clear();
        mosaicSceneItem->cubeDisplay()->deleteLater();
      }

      progressCountdown --;
      if(progressCountdown == 0) {
        m_progress->setValue(m_progress->value() + 1);
        progressCountdown = reprojectsPerUpdate;
        refit();
      }
    }

    m_progress->setValue(m_progress->maximum());

    recalcSceneRect();
    refit();
    m_progress->setVisible(false);
  }


  void MosaicSceneWidget::moveDownOne() {
    CubeDisplayProperties *cube = (CubeDisplayProperties *)sender();
    MosaicSceneItem *item = cubeToMosaic(cube);
    MosaicSceneItem *nextDown = getNextItem(item, false);
    if(nextDown) {
      qreal newZValue = nextDown->zValue() - 1;

      MosaicSceneItem *mosaicSceneItem;
      foreach(mosaicSceneItem, *m_mosaicSceneItems) {
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
      foreach(mosaicSceneItem, *m_mosaicSceneItems) {
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
    if(m_userToolControl) {
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
    foreach(mosaicSceneItem, *m_mosaicSceneItems) {
      mosaicSceneItem->updateSelection(true);
    }
  }


  //! Implemented because we want invisible items too
  MosaicSceneItem *MosaicSceneWidget::getNextItem(MosaicSceneItem *item,
                                                      bool up) {
    MosaicSceneItem *nextZValueItem = NULL;
    MosaicSceneItem *mosaicSceneItem;

    QMap<qreal, bool> zvals;

    foreach(mosaicSceneItem, *m_mosaicSceneItems) {
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
    foreach(mosaicSceneItem, *m_mosaicSceneItems) {
      if(mosaicSceneItem->cubeDisplay() == cubeDisplay)
        return mosaicSceneItem;
    }

    iString msg = "Cube is not in the mosaic";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  QStringList MosaicSceneWidget::cubeFilenames() {
    QStringList cubes;

    MosaicSceneItem *mosaicSceneItem;
    foreach(mosaicSceneItem, *m_mosaicSceneItems) {
      if(mosaicSceneItem->cubeDisplay())
        cubes.append(mosaicSceneItem->cubeDisplay()->fileName());
    }

    return cubes;
  }


  QList<CubeDisplayProperties *> MosaicSceneWidget::cubeDisplays() {
    QList<CubeDisplayProperties *> displays;

    MosaicSceneItem *mosaicSceneItem;
    foreach(mosaicSceneItem, *m_mosaicSceneItems) {
      if(mosaicSceneItem->cubeDisplay())
        displays.append(mosaicSceneItem->cubeDisplay());
    }

    return displays;
  }
}

