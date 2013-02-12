#include "MosaicSceneWidget.h"

#include <QAction>
#include <QBuffer>
#include <QCoreApplication>
#include <QDataStream>
#include <QEvent>
#include <QFileDialog>
//#include <QGLWidget> This is necessary for OpenGL acceleration
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QProgressBar>
#include <QRubberBand>
#include <QScrollBar>
#include <QStatusBar>
#include <QToolBar>
#include <QToolButton>
#include <QToolTip>

#include "Camera.h"
#include "Cube.h"
#include "CubeDisplayProperties.h"
#include "Distance.h"
#include "FileName.h"
#include "GraphicsView.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MosaicAreaTool.h"
#include "MosaicControlNetTool.h"
#include "MosaicFindTool.h"
#include "MosaicGraphicsView.h"
#include "MosaicGridTool.h"
#include "MosaicPanTool.h"
#include "MosaicSceneItem.h"
#include "MosaicSelectTool.h"
#include "MosaicTrackTool.h"
#include "MosaicZoomTool.h"
#include "ProgressBar.h"
#include "Projection.h"
#include "ProjectionConfigDialog.h"
#include "ProjectionFactory.h"
#include "PvlObject.h"
#include "Pvl.h"
#include "TextFile.h"
#include "Target.h"
#include "ToolPad.h"

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
    m_mapButton = NULL;
    m_quickMapAction = NULL;

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
    m_tools->append(new MosaicGridTool(this));
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

    setWhatsThis("This is the mosaic world view. The opened cubes will be "
        "shown here, but you cannot zoom in. You can select cubes by dragging "
        "a box over them, zoom to a particular cube by right clicking on it "
        "and selecting 'Zoom Fit', and many other actions are available.");
  }

  MosaicSceneWidget::~MosaicSceneWidget() {
    m_outlineRect = NULL; // The scene will clean this up

    if(m_tools) {
      foreach(MosaicTool *tool, *m_tools) {
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

    if (m_mapButton) {
      PvlKeyword projectionKeyword = mapping.FindKeyword("ProjectionName");
      QString projName = projectionKeyword[0];
      m_mapButton->setText(tr("View/Edit %1 Projection").arg(projName));
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
    Pvl *label = cube->label();

    try {
      proj = ProjectionFactory::CreateFromCube(*label);
      return proj->Mapping();
    }
    catch(IException &) {
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
        Camera * cam = cube->camera();
        Distance radii[3];
        cam->radii(radii);

        mappingGrp += PvlKeyword("TargetName", cam->target()->name());
        mappingGrp += PvlKeyword("EquatorialRadius", toString(radii[0].meters()),
                                 "meters");
        mappingGrp += PvlKeyword("PolarRadius", toString(radii[2].meters()),
                                 "meters");

      }
      catch(IException &) {
        mappingGrp +=
            label->FindGroup("Instrument", Pvl::Traverse)["TargetName"];
      }

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
    m_mapButton = new QToolButton(this);
    m_mapButton->setText(tr("View/Edit/Load Map File"));
    m_mapButton->setToolTip(tr("View/Edit/Load Map File"));
    m_mapButton->setIcon(QIcon(FileName("$base/icons/ographic.png").expanded()));
    m_mapButton->setWhatsThis(tr("This is the projection used by the mosaic "
        "scene. Cubes can not be shown in the scene without a projection, so "
        "if one is not selected, a default of Equirectangular will be used. "
        "The selected file should be a map file, examples are available in "
        "$base/templates/maps."));
    m_mapButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(m_mapButton, SIGNAL(clicked()), this, SLOT(configProjectionParameters()));

    if(m_projection) {
      PvlKeyword projectionKeyword =
          m_projection->Mapping().FindKeyword("ProjectionName");
      QString projName = projectionKeyword[0];
      m_mapButton->setText(projName);
    }

    m_quickMapAction = new QAction(tr("Quick Load Map"), this);
    m_quickMapAction->setToolTip(tr("Quick Load Map"));
    m_quickMapAction->setIcon(QIcon(FileName("$base/icons/quickopen.png").expanded()));
    m_quickMapAction->setWhatsThis(tr("This is the projection used by the mosaic "
        "scene. Cubes can not be shown in the scene without a projection, so "
        "if one is not selected, a default of Equirectangular will be used."));
    connect(m_quickMapAction, SIGNAL(triggered()), this, SLOT(quickConfigProjectionParameters()));

    perm->addWidget(m_mapButton);
    perm->addAction(m_quickMapAction);
  }


  void MosaicSceneWidget::addTo(QToolBar *toolbar) {
    MosaicTool *tool;
    foreach(tool, *m_tools) {
      tool->addTo(toolbar);
    }

    m_userToolControl = true;

    setWhatsThis("This is the mosaic scene. The opened cubes will be "
        "shown here. You can fully interact with the files shown here.");

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

      QBuffer dataBuffer;
      dataBuffer.open(QIODevice::ReadWrite);
      QDataStream transformStream(&dataBuffer);
      transformStream << getView()->transform();
      dataBuffer.seek(0);

      PvlObject mosaicScenePosition("SceneVisiblePosition");
      mosaicScenePosition += PvlKeyword("ViewTransform",
                                        QString(dataBuffer.data().toHex()));
      PvlKeyword scrollPos("ScrollPosition");
      scrollPos += toString(getView()->horizontalScrollBar()->value());
      scrollPos += toString(getView()->verticalScrollBar()->value());
      mosaicScenePosition += scrollPos;

      output += mosaicScenePosition;

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
        zValue += mosaicSceneItem->cubeDisplay()->fileName();
        zValue += toString(mosaicSceneItem->zValue());
        zOrders += zValue;
      }

      output += zOrders;
    }
    else {
      throw IException(IException::User,
          "Cannot save a scene without a projection to a project file",
          _FILEINFO_);
    }

    return output;
  }


  /**
   * Call this method after loading any cubes when loading a project.
   *
   * @param project The project Pvl
   */
  void MosaicSceneWidget::fromPvl(const PvlObject &project) {
    MosaicTool *tool;
    foreach(tool, *m_tools) {
      if(tool->projectPvlObjectName() != "") {
        if(project.HasObject(tool->projectPvlObjectName())) {
          const PvlObject &toolSettings(
              project.FindObject(tool->projectPvlObjectName()));
          tool->fromPvl(toolSettings);
        }
      }

      if (project.HasObject("ZOrdering")) {
        const PvlObject &zOrders = project.FindObject("ZOrdering");

        for (int zOrderIndex = 0;
             zOrderIndex < zOrders.Keywords();
             zOrderIndex ++) {
          const PvlKeyword &zOrder = zOrders[zOrderIndex];

          QString filenameToFind = zOrder[0];

          bool found = false;
          for (int itemIndex = 0;
               itemIndex < m_mosaicSceneItems->size() && !found;
               itemIndex++) {
            MosaicSceneItem * mosaicSceneItem =
                (*m_mosaicSceneItems)[itemIndex];

            if (mosaicSceneItem->cubeDisplay()->fileName() == filenameToFind) {
              mosaicSceneItem->setZValue(toDouble(zOrder[1]));
              found = true;
            }
          }
        }
      }

      if (project.HasObject("SceneVisiblePosition")) {
        const PvlObject &positionInfo =
            project.FindObject("SceneVisiblePosition");

        QByteArray hexValues(positionInfo["ViewTransform"][0].toAscii());
        QDataStream transformStream(QByteArray::fromHex(hexValues));

        QTransform viewTransform;
        transformStream >> viewTransform;
        getView()->setTransform(viewTransform);

        getView()->horizontalScrollBar()->setValue(
            toDouble(positionInfo["ScrollPosition"][0]));
        getView()->verticalScrollBar()->setValue(
            toDouble(positionInfo["ScrollPosition"][1]));
      }
    }
  }


  /**
   * Call this method before loading any cubes when loading a project.
   *
   * @param project The project Pvl
   */
  void MosaicSceneWidget::preloadFromPvl(const PvlObject &project) {
    setProjection(project.FindGroup("Mapping"));
    recalcSceneRect();
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


  MosaicSceneItem *MosaicSceneWidget::cubeToMosaic(
      CubeDisplayProperties *cubeDisplay) {
    MosaicSceneItem *mosaicSceneItem;
    foreach(mosaicSceneItem, *m_mosaicSceneItems) {
      if(mosaicSceneItem->cubeDisplay() == cubeDisplay)
        return mosaicSceneItem;
    }

    IString msg = "Cube is not in the mosaic";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  QStringList MosaicSceneWidget::cubeFileNames() {
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


  QList<QAction *> MosaicSceneWidget::getExportActions() {
    QList<QAction *> exportActs;

    QAction *exportView = new QAction(this);
    exportView->setText("&Export View...");
    connect(exportView, SIGNAL(activated()), this, SLOT(exportView()));

    QAction *saveList = new QAction(this);
    saveList->setText("Save Entire Cube List (ordered by &view)...");
    connect(saveList, SIGNAL(activated()), this, SLOT(saveList()));

    exportActs.append(exportView);
    exportActs.append(saveList);

    return exportActs;
  }


  QList<QAction *> MosaicSceneWidget::getViewActions() {
    QList<QAction *> viewActs;

    foreach(MosaicTool *tool, *m_tools) {
      QList<QAction *> toolViewActs = tool->getViewActions();
      viewActs.append(toolViewActs);
    }

    return viewActs;
  }


  QWidget * MosaicSceneWidget::getControlNetHelp(QWidget *cnetToolContainer) {
    QScrollArea *cnetHelpWidgetScrollArea = new QScrollArea;

    QWidget *cnetHelpWidget = new QWidget;

    QVBoxLayout *cnetHelpLayout = new QVBoxLayout;
    cnetHelpWidget->setLayout(cnetHelpLayout);

    QLabel *title = new QLabel("<h2>Control Networks</h2>");
    cnetHelpLayout->addWidget(title);

    QPixmap previewPixmap;

    if (cnetToolContainer) {
      previewPixmap = QPixmap::grabWidget(cnetToolContainer).scaled(
          QSize(500, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else {
      ToolPad tmpToolPad("Example Tool Pad", NULL);
      MosaicControlNetTool tmpTool(NULL);
      tmpTool.addTo(&tmpToolPad);

      tmpToolPad.resize(QSize(32, 32));

      previewPixmap = QPixmap::grabWidget(&tmpToolPad);
    }

    QLabel *previewWrapper = new QLabel;
    previewWrapper->setPixmap(previewPixmap);
    cnetHelpLayout->addWidget(previewWrapper);

    QLabel *overview = new QLabel("The mosaic scene can display control points "
        "in addition to the usual cube footprints. This feature is currently "
        "offered as one of the Mosaic Scene's tools. To open a network, click "
        "on the control network tool. It will immediately prompt you for a "
        "control network file if one is not open. Only control points for "
        "which the latitude and longitude can be established will be "
        "displayed. Other control points will be ignored by qmos.<br><br>"
        "<b>Warning: Opening large control networks is slow.</b>"
        "<h3>Control Network Tool Options</h3>"
        "<ul>"
          "<li>The control network tool opens control networks in two ways. "
          "First, if you select the control network tool and no network is "
          "open, then it will prompt you for one. Second, there is an open "
          "network button in the active tool area.</li>"
          "<li>The control network tool can toggle whether or not control "
          "points are displayed on the screen using the 'Display' button. "
          "Control points are always on top and colored based on their "
          "ignored, locked and type values.</li>"
          "<li>This tool can also change the color of your files based on "
          "connectivity through control points. This is available through the "
          "'Color Islands' button. When you press color islands, all of the "
          "current cube coloring information is lost and re-done based on "
          "how the control network connects the files. Each set of connected "
          "cubes are colored differently; generally speaking, islands are not "
          "a good thing to have in your control network.</li>"
          "<li>This tool will color your files on a per-image basis if you "
          "click color images, effectively reversing color islands.</li>"
          "<li>The show movement option only displays data when the control "
          "network has adjusted values. This means that show movement only "
          "works after you have done a jigsaw solution on the control network. "
          "This displays arrows emanating from the apriori latitude/longitude "
          "and pointing at the adjusted latitude/longitude.</li>");
    overview->setWordWrap(true);
    cnetHelpLayout->addWidget(overview);

    cnetHelpWidgetScrollArea->setWidget(cnetHelpWidget);

    return cnetHelpWidgetScrollArea;
  }


  QWidget * MosaicSceneWidget::getGridHelp(QWidget *gridToolContainer) {
    QScrollArea *gridHelpWidgetScrollArea = new QScrollArea;

    QWidget *gridHelpWidget = new QWidget;

    QVBoxLayout *gridHelpLayout = new QVBoxLayout;
    gridHelpWidget->setLayout(gridHelpLayout);

    QLabel *title = new QLabel("<h2>Map Grid Tool</h2>");
    gridHelpLayout->addWidget(title);

    QPixmap previewPixmap;

    if (gridToolContainer) {
      previewPixmap = QPixmap::grabWidget(gridToolContainer).scaled(
          QSize(500, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else {
      ToolPad tmpToolPad("Example Tool Pad", NULL);
      MosaicGridTool tmpTool(NULL);
      tmpTool.addTo(&tmpToolPad);

      tmpToolPad.resize(QSize(32, 32));

      previewPixmap = QPixmap::grabWidget(&tmpToolPad);
    }

    QLabel *previewWrapper = new QLabel;
    previewWrapper->setPixmap(previewPixmap);
    gridHelpLayout->addWidget(previewWrapper);

    QLabel *overview = new QLabel("Superimpose a map grid over the area of "
        "displayed footprints in the 'mosaic scene.'"
        "<h2>Overview</h2>"
        "<ul>"
          "<li>The Map Grid Tool is activated by selecting the 'cross-hatch' "
              "icon or typing 'g' at the keyboard."
          "</li>"
          "<li>The parameter options are displayed in the configuration dialog. "
              "Hitting the 'Options' button will open the dialog. Checking "
              "'Auto Grid' will draw a grid based on the open cubes. Hitting "
              "'Show Grid' will display or hide the grid."
          "</li>"
          "<li>The map grid is defined by the loaded Map File (just as the "
              "footprints and image data are), the opened cubes, or the grid "
              "tool parameters."
          "</li>"
          "<li>If a Map File has not been selected, the default "
              "Equirectangular projection will be used. The resulting grid "
              "lines in the default 'Equi' map file will be drawn for the "
              "full global range (latitude range = -90,90; longitude range = "
              "0,360) at the default latitude and longitude increment values."
          "</li>"
          "<li>"
              "If the grid lines are not immediately visible, try to "
              "'zoom out' in the 'mosaic scene' window and modify the "
              "Latitude and Longitude Increment parameters."
          "</li>"
        "</ul>"
        "<strong>Options:</strong>"
        "<ul>"
        "<li>The 'Show Grid' option draws (checked) or clears (unchecked) the grid."
        "</li>"
        "<li>The 'Auto Grid' option draws a grid with extents and increments "
            "determined by the selected extent types. The values displayed in the dialog "
            "will reflect those used to draw the grid."
        "</li>"
        "<li>The expected units for each entry are displayed on the right of the "
            "dialog."
        "</li>"
        "<li>The 'Extent Type' combo boxes allow you to pick the source of the "
            "grid extents (from the projection, from the open cubes <default>, or manually "
            "entered.)"
        "</li>"
        "<li>The 'Auto Apply' checkbox allows you to see real time updates in the "
            "grid when you change the parameters."
        "</li>"
        "<li> Depending on the projection, the grid may not behave as expected. For instance, "
            "with a polarstereographic projection, the pole will not be included in the 'Auto "
            "Grid' if it is not in the cube region. In this case the 'Manual' option for latitude "
            "extents allows you to force the grid to the pole."
        "</li>"
        "</ul>");
    overview->setWordWrap(true);
    gridHelpLayout->addWidget(overview);

    gridHelpWidgetScrollArea->setWidget(gridHelpWidget);

    return gridHelpWidgetScrollArea;
  }


  QWidget * MosaicSceneWidget::getLongHelp(QWidget *sceneContainer) {
    QScrollArea *longHelpWidgetScrollArea = new QScrollArea;

    QWidget *longHelpWidget = new QWidget;

    QVBoxLayout *longHelpLayout = new QVBoxLayout;
    longHelpWidget->setLayout(longHelpLayout);

    QLabel *title = new QLabel("<h2>Mosaic Scene</h2>");
    longHelpLayout->addWidget(title);

    if (sceneContainer) {
      QPixmap previewPixmap = QPixmap::grabWidget(sceneContainer).scaled(
          QSize(500, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);

      QLabel *previewWrapper = new QLabel;
      previewWrapper->setPixmap(previewPixmap);
      longHelpLayout->addWidget(previewWrapper);
    }

    QLabel *overview = new QLabel("The mosaic scene displays cube footprints "
        "to show you where your files are on a target and how they overlap. "
        "The scene always represents projected image space and cannot show raw "
        "or unprojected images; images will be projected on the fly."
        "<h3>Tools</h3>"
            "<p>You can interact with the mosaic scene in different ways using "
            "the tools. The tools are usually in a toolbar next to the scene. "
            "The tools define what is displayed and what happens when you "
            "click in the mosaic scene. The tools include:</p>"
              "<ul><li>Select Tool</li>"
              "<li>Zoom Tool</li>"
              "<li>Pan Tool</li>"
              "<li>Control Network Tool</li>"
              "<li>Show Area Tool</li>"
              "<li>Find Tool</li></ul>"
        "<h3>Context Menus</h3>"
            "You can right click on anything in the mosaic scene and be given "
            "options relevant to what you clicked on. Some typical actions are "
            "changing which cubes are displayed on top of other cubes and the "
            "color of a cube. The right click menus only affect the item you "
            "clicked on, not what was selected.");
    overview->setWordWrap(true);
    longHelpLayout->addWidget(overview);

    longHelpWidgetScrollArea->setWidget(longHelpWidget);

    return longHelpWidgetScrollArea;
  }


  QWidget * MosaicSceneWidget::getMapHelp(QWidget *mapContainer) {
    QScrollArea *mapHelpWidgetScrollArea = new QScrollArea;

    QWidget *mapHelpWidget = new QWidget;

    QVBoxLayout *mapHelpLayout = new QVBoxLayout;
    mapHelpWidget->setLayout(mapHelpLayout);

    QLabel *title = new QLabel(tr("<h2>Map File</h2>"));
    mapHelpLayout->addWidget(title);

    if (mapContainer) {
      QPixmap previewPixmap = QPixmap::grabWidget(mapContainer).scaled(
          QSize(500, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);

      QLabel *previewWrapper = new QLabel;
      previewWrapper->setPixmap(previewPixmap);
      mapHelpLayout->addWidget(previewWrapper);
    }

    QLabel *overviewMapIcon = new QLabel;

    overviewMapIcon->setPixmap(
        QIcon(FileName("$base/icons/ographic.png").expanded()).pixmap(32, 32));
    mapHelpLayout->addWidget(overviewMapIcon);

    QLabel *defaultMapFile = new QLabel(tr(
        "<h3>Default Map File</h3>"
        "The mosaic scene's projection is defined by a \"Map File\" that consists of keywords "
        "that describe the map layout to be used. If a cube or a list of cubes are "
        "loaded before a map file is selected, the default map file defines the "
        "equirectangular projection, planetocentric latitude, positive longitude east, 360 "
        "longitude domain, latitude range=90S-90N, longitude range=0-360E. The radius will "
        "default to the IAU standards (ellipsoid or sphere) for the specific planetary body "
        "defined for the \"TargetName\" in the labels of the image cube(s)."));

    defaultMapFile->setWordWrap(true);
    mapHelpLayout->addWidget(defaultMapFile);

    QLabel *userDefinedMapFileOverview = new QLabel(tr(
        "<h3>User Defined Map File</h3>"
        "You can load an existing \"Map File\" before loading images into %1 by selecting the "
        "\"View/Edit/Load Map File\" option. You will be greeted with a dialog box that will "
        "enable you to select an existing map file by clicking on \"Load Map File.\" Once "
        "the map file is selected, the contents is displayed in the dialog box where "
        "modifications can be made as well. If the modified map file is to be used later, "
        "save the map file by clicking on \"Save Map File\" button.")
        .arg(QCoreApplication::applicationName()));

    userDefinedMapFileOverview->setWordWrap(true);
    mapHelpLayout->addWidget(userDefinedMapFileOverview);

    QLabel *userDefinedMapFileQuickLoad = new QLabel(tr(
        "The \"Quick Load Map\" option (lightning icon) allows you to efficiently select a "
        "prepared \"Map File\" without an immediate need to view or edit the contents."));

    userDefinedMapFileQuickLoad->setWordWrap(true);
    mapHelpLayout->addWidget(userDefinedMapFileQuickLoad);

    QLabel *userDefinedMapFileAnyTime = new QLabel(tr(
        "At any point, you have access to the \"View/Edit\" functionality to modify or load a "
        "different map file."));

    userDefinedMapFileAnyTime->setWordWrap(true);
    mapHelpLayout->addWidget(userDefinedMapFileAnyTime);

    QString mapProjWorkshopUrl("http://isis.astrogeology.usgs.gov/IsisWorkshop/"
        "index.php/Learning_About_Map_Projections");
    QLabel *preparingMapFile = new QLabel(tr(
        "<h3>Preparing a Map File</h3>"
        "Please refer to Isis applications such as 'maptemplate' or 'mosrange' for more details "
        "on creating a custom map file that defines the desired projection, latitude "
        "system, and longitude direction and domain. This program will use the latitude range "
        "and longitude range if they exist in the loaded file. A choice of map templates that can be used as "
        "a starting point for supported map projections can be found in $base/templates/maps (refer "
        "to maptemplate or mosrange for more details and information on the required parameters "
        "for a projection). Note that through the file name selection box, $base will need "
        "to be replaced with the specific Isis3 system path. The website: "
        "<a href='%1'>%1</a> also provides useful information about map projections.")
        .arg(mapProjWorkshopUrl));

    preparingMapFile->setOpenExternalLinks(true);
    preparingMapFile->setWordWrap(true);
    mapHelpLayout->addWidget(preparingMapFile);

    QLabel *mapFileDisplayResults = new QLabel(tr(
        "<h3>Display Results with the Map File</h3>"
        "The footprints and image data that are displayed in the mosaic scene are defined by the "
        "loaded \"Map File\" regardless of whether the opened cubes are Level1 (raw "
        "camera space) or Level2 (map projected). The associated footprint polygons for "
        "Level2 cubes will be re-mapped as needed based on the loaded map file."));

    mapFileDisplayResults->setWordWrap(true);
    mapHelpLayout->addWidget(mapFileDisplayResults);

    QLabel *editingMapFileOverview = new QLabel(tr(
        "<h3>Editing a Map File</h3>"
        "Editing a map file is possible through the dialog box displayed by %1. The edits are "
        "applied to the current session and will be included with a 'Saved Project' (refer to "
        "the help under File-Save Project or Save Project as)."));

    editingMapFileOverview->setWordWrap(true);
    mapHelpLayout->addWidget(editingMapFileOverview);

    QLabel *saveMapFileToDiskBullet = new QLabel(tr(
        "<ul>"
          "<li>"
            "To save or write the changes to a map file on disk, choose 'Save Map File' button. "
            "Map files can be saved to an existing map file (overwrites) or to a new file. This "
            "program always saves <strong>exactly</strong> what you see, the text, in the dialog "
            "box."
          "</li>"
        "</ul>"));

    saveMapFileToDiskBullet->setWordWrap(true);
    mapHelpLayout->addWidget(saveMapFileToDiskBullet);

    QLabel *mapFileValidityBullet = new QLabel(tr(
        "<ul>"
          "<li>"
            "As you modify the contents of a loaded map file in the dialog box, the entry is "
            "verified as you type with a bold black indicator message displaying whether the "
            "text is valid or is not valid. If you want to see the actual error messages, "
            "select the 'Show Errors' box and the errors will be displayed in red font "
            "along with the black bolded message. The errors will update "
            "as you type."
          "</li>"
        "</ul>"));

    mapFileValidityBullet->setWordWrap(true);
    mapHelpLayout->addWidget(mapFileValidityBullet);

    QLabel *mapFileCommentsBullet = new QLabel(tr(
        "<ul>"
          "<li>"
            "Map files may contain 'commented-out' lines (text that starts with \"#\" at "
            "the beginning of the line). These are referred to as \"unnecessary\""
            "or \"unknown\" keywords, they are simply ignored. If these lines are to be saved to "
            "the output map file on disk, click 'Save Map File' BEFORE clicking 'Ok' or 'Apply.' "
            "The comments are removed from the dialog box when you hit 'Ok' or 'Apply,' if they "
            "are just above \"End_Group\" or follow \"End_Group\" or \"End\".<br/><br/>"

            "If you want these comments retained, make sure they are immediately above a valid "
            "keyword inside of \"Group = Mapping.\" Note that any lines (commented or not) will "
            "not be saved if they are placed outside of \"Group = Mapping\" and \"End_Group\"."
          "</li>"
        "</ul>"));

    mapFileCommentsBullet->setWordWrap(true);
    mapHelpLayout->addWidget(mapFileCommentsBullet);

    mapHelpWidgetScrollArea->setWidget(mapHelpWidget);

    return mapHelpWidgetScrollArea;
  }


  QWidget * MosaicSceneWidget::getPreviewHelp(QWidget *worldViewContainer) {
    QScrollArea *previewHelpWidgetScrollArea = new QScrollArea;

    QWidget *previewHelpWidget = new QWidget;

    QVBoxLayout *previewHelpLayout = new QVBoxLayout;
    previewHelpWidget->setLayout(previewHelpLayout);

    QLabel *title = new QLabel("<h2>Mosaic World View</h2>");
    previewHelpLayout->addWidget(title);

    if (worldViewContainer) {
      QPixmap previewPixmap = QPixmap::grabWidget(worldViewContainer).scaled(
          QSize(500, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);

      QLabel *previewWrapper = new QLabel;
      previewWrapper->setPixmap(previewPixmap);
      previewHelpLayout->addWidget(previewWrapper);
    }

    QLabel *overview = new QLabel("The mosaic world view displays cube "
        "footprints to show you where your files are on a target and their "
        "general arrangement. The world view does not have tools like "
        "mosaic scenes do, but otherwise are very similar.");
    overview->setWordWrap(true);
    previewHelpLayout->addWidget(overview);

    previewHelpWidgetScrollArea->setWidget(previewHelpWidget);

    return previewHelpWidgetScrollArea;
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
    catch(IException &) {
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

      QRectF projRect(minX, -maxY, maxX - minX, maxY - minY);
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
      catch(IException &e) {
        e.print();
      }

      m_progress->setValue(m_progress->value() + 1);
    }

    recalcSceneRect();
    refit();

    m_progress->setVisible(false);
    emit cubesChanged();
  }


  /**
   * Saves the scene as a png, jpg, or tif file.
   */
  void MosaicSceneWidget::exportView() {
    QString output =
      QFileDialog::getSaveFileName((QWidget *)parent(),
                                   "Choose output file",
                                   QDir::currentPath() + "/untitled.png",
                                   QString("Images (*.png *.jpg *.tif)"));
    if(output.isEmpty()) return;

    // Use png format is the user did not add a suffix to their output filename.
    if(QFileInfo(output).suffix().isEmpty()) {
      output = output + ".png";
    }

    QString format = QFileInfo(output).suffix();
    QPixmap pm = QPixmap::grabWidget(getScene()->views().last());

    std::string formatString = format.toStdString();
    if(!pm.save(output, formatString.c_str())) {
      QMessageBox::information(this, "Error",
                               "Unable to save [" + output + "]");
    }
  }


  void MosaicSceneWidget::saveList() {
    QString output =
        QFileDialog::getSaveFileName((QWidget *)parent(),
          "Choose output file",
          QDir::currentPath() + "/files.lis",
          QString("List File (*.lis);;Text File (*.txt);;All Files (*.*)"));
    if(output.isEmpty()) return;

    TextFile file(output, "overwrite");

    QList<MosaicSceneItem *> sorted = *m_mosaicSceneItems;
    qSort(sorted.begin(), sorted.end(), zOrderGreaterThan);

    MosaicSceneItem *sceneItem;
    foreach(sceneItem, sorted) {
      file.PutLine( sceneItem->cubeDisplay()->fileName() );
    }
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
      Latitude minLat(toDouble(minLatKeyword[0]), mapping, Angle::Degrees);
      PvlKeyword minLonKeyword = mapping.FindKeyword("MinimumLongitude");
      Longitude minLon(toDouble(minLonKeyword[0]), mapping, Angle::Degrees);
      PvlKeyword maxLatKeyword = mapping.FindKeyword("MaximumLatitude");
      Latitude maxLat(toDouble(maxLatKeyword[0]), mapping, Angle::Degrees);
      PvlKeyword maxLonKeyword = mapping.FindKeyword("MaximumLongitude");
      Longitude maxLon(toDouble(maxLonKeyword[0]), mapping, Angle::Degrees);

      Angle increment(1, Angle::Degrees);
      if(m_projection->SetUniversalGround(minLat.degrees(),
         minLon.degrees())) {
        x = m_projection->XCoord();
        y = -1 * (m_projection->YCoord());
        footprintPoints.push_back(QPointF(x, y));
      }

      for(Angle lat = minLat + increment; lat < maxLat; lat += increment) {
        if(m_projection->SetUniversalGround(lat.degrees(),
           minLon.degrees())) {
          x = m_projection->XCoord();
          y = -1 * (m_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }
      for(Angle lon = minLon + increment; lon < maxLon; lon += increment) {
        if(m_projection->SetUniversalGround(maxLat.degrees(),
           lon.degrees())) {
          x = m_projection->XCoord();
          y = -1 * (m_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }
      for(Angle lat = maxLat; lat > minLat + increment; lat -= increment) {
        if(m_projection->SetUniversalGround(lat.degrees(),
           maxLon.degrees())) {
          x = m_projection->XCoord();
          y = -1 * (m_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }
      for(Angle lon = maxLon; lon > minLon + increment; lon -= increment) {
        if(m_projection->SetUniversalGround(minLat.degrees(),
           lon.degrees())) {
          x = m_projection->XCoord();
          y = -1 * (m_projection->YCoord());
          footprintPoints.push_back(QPointF(x, y));
        }
      }

      //Now close the polygon.
      if(m_projection->SetUniversalGround(minLat.degrees(),
         minLon.degrees())) {
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
    catch(IException &e) {
      QString msg = e.toString();
      QMessageBox::information(this, "Error", msg, QMessageBox::Ok);
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


  /**
   * This happens when the user clicks on the map action (the button that is named after the
   *   current projection). This method pops up a modal configuration dialog for the map file.
   */
  void MosaicSceneWidget::configProjectionParameters() {
    ProjectionConfigDialog configDialog(this);
    configDialog.exec();
  }


  void MosaicSceneWidget::quickConfigProjectionParameters() {
    ProjectionConfigDialog configDialog(this);
    configDialog.setQuickConfig(true);
    configDialog.exec();
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
        event->accept();
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
      catch(IException &e) {
        IString msg = "The file [";

        if(mosaicSceneItem->cubeDisplay())
          msg += (IString)mosaicSceneItem->cubeDisplay()->displayName();

        msg += "] is being removed due to not being able to project";

        IException tmp(e, IException::Programmer, msg, _FILEINFO_);
        tmp.print();
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

  bool MosaicSceneWidget::zOrderGreaterThan(MosaicSceneItem *first,
                                            MosaicSceneItem *second) {
    return first->zValue() > second->zValue();
  }
}
