/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CubeDnView.h"

#include <QAction>
#include <QDebug>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QMap>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMenuBar>
#include <QModelIndex>
#include <QSize>
#include <QStatusBar>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QXmlStreamWriter>

#include "AdvancedTrackTool.h"
#include "BandTool.h"
#include "BlinkTool.h"
#include "Cube.h"
#include "CubeAttribute.h"
#include "Directory.h"
#include "EditTool.h"
#include "FeatureNomenclatureTool.h"
#include "FileName.h"
#include "FileTool.h"
#include "FindTool.h"
#include "HistogramTool.h"
#include "Image.h"
#include "ImageList.h"
#include "ControlNetTool.h"
#include "IString.h"
#include "MatchTool.h"
#include "MdiCubeViewport.h"
#include "MeasureTool.h"
#include "PanTool.h"
#include "Project.h"
#include "ProjectItem.h"
#include "ProjectItemModel.h"
#include "ProjectItemProxyModel.h"
#include "RubberBandTool.h"
#include "ScatterPlotTool.h"
#include "Shape.h"
#include "SpatialPlotTool.h"
#include "SpecialPixelTool.h"
#include "SpectralPlotTool.h"
#include "StatisticsTool.h"
#include "StereoTool.h"
#include "StretchTool.h"
#include "SunShadowTool.h"
#include "TrackTool.h"
#include "ToolList.h"
#include "ToolPad.h"

#include "ViewportMdiSubWindow.h"
#include "Workspace.h"
#include "WindowTool.h"
#include "ZoomTool.h"

#include "ProjectItemViewMenu.h"

namespace Isis {
  /**
   * Constructs the view, initializing the tools.
   *
   * @param parent (QWidget *) Pointer to parent widget
   */
  CubeDnView::CubeDnView(Directory *directory, QWidget *parent) :
                 AbstractProjectItemView(parent) {
    connect( internalModel()->selectionModel(),
        SIGNAL( currentChanged(const QModelIndex &, const QModelIndex &) ),
             this, SLOT( onCurrentChanged(const QModelIndex &) ) );

    connect( internalModel(), SIGNAL( itemAdded(ProjectItem *) ),
             this, SLOT( onItemAdded(ProjectItem *) ) );

    m_cubeItemMap = QMap<Cube *, ProjectItem *>();

    m_workspace = new Workspace(false, this);
    m_workspace->mdiArea()->setActivationOrder(QMdiArea::StackingOrder);

    m_directory = directory;

    // Since this is a QMainWindow, set the workspace as the central widget.
    setCentralWidget(m_workspace);

    createActions(directory);

    connect(m_workspace, SIGNAL( cubeViewportActivated(MdiCubeViewport *) ),
            this, SLOT( onCubeViewportActivated(MdiCubeViewport *) ) );

    connect(m_workspace, SIGNAL( cubeViewportAdded(MdiCubeViewport *) ),
            this, SLOT( onCubeViewportAdded(MdiCubeViewport *) ) );
  }


  void CubeDnView::createActions(Directory *directory) {

    m_permToolBar = new QToolBar("Standard Tools", this);
    m_permToolBar->setObjectName("permToolBar");
    m_permToolBar->setIconSize(QSize(22, 22));
    addToolBar(m_permToolBar);

    m_activeToolBar = new QToolBar("Active Tool", this);
    m_activeToolBar->setObjectName("activeToolBar");
    m_activeToolBar->setIconSize(QSize(22, 22));
    addToolBar(m_activeToolBar);

    m_toolPad = new ToolPad("Tool Pad", this);
    m_toolPad->setObjectName("toolPad");
    addToolBar(Qt::RightToolBarArea, m_toolPad);

    // Create tools
    ToolList *tools = new ToolList;

    tools->append(new RubberBandTool(this));

    // 2018-07-02 Kaitlyn Lee - Commented this out; not sure why it was here
    //tools->append(NULL);

    ControlNetTool *controlNetTool = new ControlNetTool(directory, this);
    tools->append(controlNetTool);

    if (directory->project()->activeControl()) {
      controlNetTool->setControlNet(directory->project()->activeControl()->controlNet());
    }
    //  Pass on Signals emitted from ControlNetTool
    //  TODO 2016-09-09 TLS Design:  Use a proxy model instead of signals?
    connect(controlNetTool, SIGNAL(modifyControlPoint(ControlPoint *, QString)),
            this, SIGNAL(modifyControlPoint(ControlPoint *, QString)));

    connect(controlNetTool, SIGNAL(deleteControlPoint(ControlPoint *)),
            this, SIGNAL(deleteControlPoint(ControlPoint *)));

    connect(controlNetTool, SIGNAL(createControlPoint(double, double, Cube *, bool)),
            this, SIGNAL(createControlPoint(double, double, Cube *, bool)));

    // Pass on signals emitted from Directory (by way of ControlPointEditWidget)
    // This is done to redraw the control points on the cube viewports
    connect(this, SIGNAL(controlPointAdded(QString)), controlNetTool, SLOT(paintAllViewports()));

    //  Pass on redrawMeasure signal from Directory, so the control measures are redrawn on all
    //  CubeViewports.
    connect(this, SIGNAL(redrawMeasures()), controlNetTool, SLOT(paintAllViewports()));

    tools->append(new BandTool(this));
    ZoomTool *zoomTool = new ZoomTool(this);
    tools->append(zoomTool);
    tools->append(new PanTool(this));
    tools->append(new StretchTool(this));
    tools->append(new FindTool(this));
    tools->append(new BlinkTool(this));
    tools->append(new AdvancedTrackTool(this));
    tools->append(new EditTool(this));
    tools->append(new WindowTool(this));
    tools->append(new MeasureTool(this));
    tools->append(new SunShadowTool(this));
    tools->append(new FeatureNomenclatureTool(this));
    tools->append(new SpecialPixelTool(this));
    tools->append(new SpatialPlotTool(this));
    tools->append(new SpectralPlotTool(this));
    tools->append(new ScatterPlotTool(this));
    tools->append(new HistogramTool(this));
    tools->append(new StatisticsTool(this));
    tools->append(new StereoTool(this));
    tools->append(new TrackTool(statusBar()));

    m_separatorAction = new QAction(this);
    m_separatorAction->setSeparator(true);

    m_viewMenu = new ProjectItemViewMenu("&View");
    connect(m_viewMenu, SIGNAL(menuClosed()), this, SLOT(disableActions()));
    menuBar()->addMenu(m_viewMenu);

    m_optionsMenu = new ProjectItemViewMenu("&Options");
    connect(m_optionsMenu, SIGNAL(menuClosed()), this, SLOT(disableActions()));
    menuBar()->addMenu(m_optionsMenu);

    m_windowMenu = new ProjectItemViewMenu("&Window");
    connect(m_windowMenu, SIGNAL(menuClosed()), this, SLOT(disableActions()));
    menuBar()->addMenu(m_windowMenu);

    for (int i = 0; i < tools->count(); i++) {
      Tool *tool = (*tools)[i];

      if (tool) {
        tool->addTo(m_workspace);
        tool->addToPermanent(m_permToolBar);
        tool->addToActive(m_activeToolBar);
        tool->addTo(m_toolPad);

        if (!tool->menuName().isEmpty()) {
          QString menuName = tool->menuName();

          if (menuName == "&View") {
            tool->addTo(m_viewMenu);
          }
          else if (menuName == "&Options") {
            tool->addTo(m_optionsMenu);
          }
          else if (menuName == "&Window") {
            tool->addTo(m_windowMenu);
          }
        }
      }
      else {
        m_permToolBar->addSeparator();
      }
    }

    // Store the actions and widgets for easy enable/disable.
    foreach (QAction *action, findChildren<QAction *>()) {
      // Remove the edit tool's save button shortcut because the ipce main window
      // already has one and this causes an ambiquous shortcut error.
      if (action->toolTip() == "Save") {
        action->setShortcut(QKeySequence());
      }
      // The active toolbar's actions are inside of a container that is a QWidgetAction.
      // We want to skip adding this because we want to disable the active toolbar's
      // actions separately to skip the combo boxes.
      if (QString(action->metaObject()->className()) == "QWidgetAction") {
        continue;
      }
      addAction(action);
    }

    // There was a problem with disabling/enabling the combo boxes. The only way to
    // get this to work was to skip disabling the combo boxes. We also skip QWidgets
    // because the combo boxes are contained inside of a QWidget.
    foreach (QWidget *child, m_activeToolBar->findChildren<QWidget *>()) {
      if (QString(child->metaObject()->className()).contains("ComboBox") ||
          QString(child->metaObject()->className()).contains("Widget")) {
        continue;
      }
      m_childWidgets.append(child);
    }

    // On default, actions are disabled until the cursor enters the view.
    disableActions();

    zoomTool->activate(true);
  }


  /**
   * Disables actions when the cursor leaves the view. Overriden method
   * If a project item view menu or toolpad action menu is visible, i.e. clicked on,
   * this causes a leave event. We want the actions to still be enabled when a
   * menu is visible.
   *
   * @param event The leave event
   */
  void CubeDnView::leaveEvent(QEvent *event) {
    if (m_optionsMenu->isVisible() || m_viewMenu->isVisible() || m_windowMenu->isVisible()) {
      return;
    }
    // Find the toolpad actions (buttons) with menus and check if they are visible
    foreach (QToolButton *button, findChildren<QToolButton *>()) {
      if (button->menu() && button->menu()->isVisible()) {
        return;
      }
    }
    disableActions();
  }


  /**
   * Disables toolbars and toolpad actions/widgets. Overriden method.
   */
  void CubeDnView::disableActions() {
    foreach (QAction *action, actions()) {
      action->setDisabled(true);
    }
    foreach (QWidget *widget, m_childWidgets) {
      widget->setDisabled(true);
    }
  }


  /**
   * Enables toolbars and toolpad actions/widgets. Overriden method.
   * If an active control network has not been set, do not enable the cnet tool.
   */
  void CubeDnView::enableActions() {
    foreach (QAction *action, actions()) {
      if (action->objectName() == "ControlNetTool" && !m_directory->project()->activeControl()) {
        continue;
      }
      action->setEnabled(true);
    }
    foreach (QWidget *widget, m_childWidgets) {
      widget->setEnabled(true);
    }
  }


  /**
   * A slot function that is called when directory emits a signal that an active
   * control network is set. It enables the control network editor tool in the
   * toolpad and loads the network.
   *
   * @param value The boolean that holds if a control network has been set.
   */
  void CubeDnView::enableControlNetTool(bool value) {
    foreach (QAction *action, m_toolPad->actions()) {
      if (action->objectName() == "ControlNetTool") {
        action->setEnabled(value);
        if (value) {
          ControlNetTool *cnetTool = static_cast<ControlNetTool *>(action->parent());
          cnetTool->loadNetwork();
        }
      }
    }
  }

  /**
   * Destructor
   */
  CubeDnView::~CubeDnView() {
    delete m_permToolBar;
    delete m_activeToolBar;
    delete m_toolPad;
    delete m_viewMenu;
    delete m_optionsMenu;
    delete m_windowMenu;


    m_permToolBar = 0;
    m_activeToolBar = 0;
    m_toolPad = 0;
    m_viewMenu = 0;
    m_optionsMenu = 0;
    m_windowMenu = 0;
  }


  /**
   * Adds an item to the view. Filters out items that are not Images or Shapes
   * or ImageLists.
   *
   * @param[in] item (ProjectItem *) The item to add.
   */
  void CubeDnView::addItem(ProjectItem *item) {
    if ( !item->isImageList() && !item->isImage() && !item->isShapeList() && !item->isShape()) {
      return;
    }

    AbstractProjectItemView::addItem(item);
  }


  bool CubeDnView::viewportContainsShape(MdiCubeViewport *viewport) {

    ProjectItem *item = m_cubeItemMap.value( viewport->cube() );

    if (!item) {
      return false;
    }

    return item->isShape();
  }

  /**
   * Slot to connect to the currentChanged() signal from a selection
   * model. If the new current item is an image the corresponding
   * subwindow is activated.
   *
   * @param[in] current (const QModelIndex &) The new current index
   */
  void CubeDnView::onCurrentChanged(const QModelIndex &current) {

    ProjectItem *item = internalModel()->itemFromIndex(current);

    if (!item) {
      return;
    }

    if (!item->isImage()) {
      return;
    }

    setWorkspaceActiveCube(item->image());
  }


  /**
   * Slot to connect to the cubeViewportActivated signal from the
   * Workspace. Updates the selection model to reflect the activated
   * viewport.
   *
   * @param[in] viewport (MdiCubeViewport *) The viewport
   */
  void CubeDnView::onCubeViewportActivated(MdiCubeViewport *viewport) {
    if ( !isVisible() ) {
      return;
    }

    if (!viewport) {
      return;
    }

    ProjectItem *item = m_cubeItemMap.value( viewport->cube() );

    if (!item) {
      return;
    }

    internalModel()->selectionModel()->setCurrentIndex(item->index(),
                                                       QItemSelectionModel::SelectCurrent);
  }


  /**
   * Slot to connect to the viewportAdded signal from a
   * Workspace. Removes the corresponding item from the model when its
   * viewport is closed.
   *
   * @param[in] viewport (MdiCubeViewport *) The added viewport
   */
  void CubeDnView::onCubeViewportAdded(MdiCubeViewport *viewport) {
    connect(viewport, SIGNAL( destroyed(QObject *) ),
            this, SLOT( onCubeViewportDeleted(QObject *) ) );
  }


  /**
   * Slot to connect to the destroyed signal from a viewport. Removes
   * the viewports corresponding item from the internal model.
   *
   * @param[in] obj (QObject *) The deleted viewport
   */
  void CubeDnView::onCubeViewportDeleted(QObject *obj) {
    MdiCubeViewport *viewport = qobject_cast<MdiCubeViewport *>(obj);

    if (!viewport) {
      return;
    }

    if ( ProjectItemProxyModel *proxyModel =
             qobject_cast<ProjectItemProxyModel *>( internalModel() ) ) {
      proxyModel->removeItem( m_cubeItemMap.value( viewport->cube() ) );
    }
  }


  /**
   * Slot to connect to the itemAdded signal from a
   * ProjectItemModel. Adds the image or shape to the Workspace and the item to
   * an internal map.
   *
   * @param[in] item (ProjectItem *) The added item
   */
  void CubeDnView::onItemAdded(ProjectItem *item) {

    if (!item) {
      return;
    }
    Cube *cube;
    if (item->isImage()) {
      cube = item->image()->cube();
    }
    else if (item->isShape()) {
      cube = item->shape()->cube();
    }
    else {
      return;
    }
    if (m_workspace->cubeToMdiWidget(cube)) {
      return;
    }
    m_workspace->addCubeViewport(cube);
    m_cubeItemMap.insert(cube, item);
  }


  /**
   * Returns the cube of the active viewport in the Workspace, or a
   * null pointer if no viewports are active.
   *
   * @return @b Cube* The active cube
   */
  Cube *CubeDnView::workspaceActiveCube() {
    QMdiArea *mdiArea = m_workspace->mdiArea();
    ViewportMdiSubWindow *subWindow = qobject_cast<ViewportMdiSubWindow *>( mdiArea->currentSubWindow() );
    if (!subWindow) {
      return 0;
    }
    MdiCubeViewport *viewport = subWindow->viewport();
    return viewport->cube();
  }


  /**
   * Raises the subwindow corresponding with an image to the top.
   */
  void CubeDnView::setWorkspaceActiveCube(Image *image) {
    if (!image) {
      return;
    }

    QWidget *mdiWidget = m_workspace->cubeToMdiWidget(image->cube());

    if (!mdiWidget) {
      return;
    }

    QMdiSubWindow *subWindow = qobject_cast<QMdiSubWindow *>( mdiWidget->parent() );

    if (!subWindow) {
      return;
    }

    subWindow->raise();

    // Activating the subwindow activates the view, which is annoying
    //m_workspace->mdiArea()->setActiveSubWindow(subWindow);
  }


  void CubeDnView::save(QXmlStreamWriter &stream, Project *, FileName) const {
    stream.writeStartElement("cubeDnView");
    stream.writeAttribute("objectName", objectName());

    foreach (MdiCubeViewport *cvp, *(m_workspace->cubeViewportList())) {
      ProjectItem *item = m_cubeItemMap.value(cvp->cube());
      if (item->isImage()) {
        stream.writeStartElement("image");
        stream.writeAttribute("id", item->image()->id());
      }
      else if (item->isShape()) {
        stream.writeStartElement("shape");
        stream.writeAttribute("id", item->shape()->id());
      }
      stream.writeEndElement();
    }
    stream.writeEndElement();
  }
}
