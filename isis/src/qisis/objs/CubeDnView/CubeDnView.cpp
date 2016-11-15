/**
 * @file
 * $Date$
 * $Revision$
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include "CubeDnView.h"

#include <QAction>
#include <QDataStream>
#include <QHBoxLayout>
#include <QMap>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QModelIndex>
#include <QSize>
#include <QSizePolicy>
#include <QStatusBar>
#include <QToolBar>
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
#include "HelpTool.h"
#include "HistogramTool.h"
#include "Image.h"
#include "ImageList.h"
#include "IpceTool.h"
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
#include "XmlStackedHandlerReader.h"
#include "ZoomTool.h"

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

    connect(m_workspace, SIGNAL( cubeViewportActivated(MdiCubeViewport *) ),
            this, SLOT( onCubeViewportActivated(MdiCubeViewport *) ) );

    connect(m_workspace, SIGNAL( cubeViewportAdded(MdiCubeViewport *) ),
            this, SLOT( onCubeViewportAdded(MdiCubeViewport *) ) );


    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
    
    //m_toolBar = new QWidget(this);
    
    //QHBoxLayout *toolBarLayout = new QHBoxLayout;
    
    m_permToolBar = new QToolBar("Standard Tools", 0);
    m_permToolBar->setObjectName("permToolBar");
    m_permToolBar->setIconSize(QSize(22, 22));
    //toolBarLayout->addWidget(m_permToolBar);

    m_activeToolBar = new QToolBar("Active Tool", 0);
    m_activeToolBar->setObjectName("activeToolBar");
    m_activeToolBar->setIconSize(QSize(22, 22));
    //toolBarLayout->addWidget(m_activeToolBar);

    m_toolPad = new ToolPad("Tool Pad", 0);
    m_toolPad->setObjectName("toolPad");
    //toolBarLayout->addWidget(m_toolPad);

    //m_toolBar->setLayout(toolBarLayout);

    //layout->addWidget(m_toolBar);
    layout->addWidget(m_workspace);

    // Create tools
    ToolList *tools = new ToolList;

    Tool *defaultActiveTool = NULL;

    tools->append(new RubberBandTool(this));
//  QnetTool *qnetTool = new QnetTool(m_workspace);
    //tools->append(new FileTool(this));
    //tools->append(new QnetFileTool(qnetTool, this));
    tools->append(NULL);
    tools->append(new BandTool(this));

    IpceTool *ipceTool = new IpceTool(directory, this);
    defaultActiveTool = ipceTool;
    tools->append(defaultActiveTool);

    if (directory->project()->activeControl()) {
      ipceTool->setControlNet(directory->project()->activeControl()->controlNet()); 
    }
    //  Pass on Signals emitted from IpceTool
    //  TODO 2016-09-09 TLS Design:  Use a proxy model instead of signals?
    connect(ipceTool, SIGNAL(modifyControlPoint(ControlPoint *)),
            this, SIGNAL(modifyControlPoint(ControlPoint *)));

    connect(ipceTool, SIGNAL(deleteControlPoint(ControlPoint *)),
            this, SIGNAL(deleteControlPoint(ControlPoint *)));

    connect(ipceTool, SIGNAL(createControlPoint(double, double, Cube *, bool)),
            this, SIGNAL(createControlPoint(double, double, Cube *, bool)));

    // Pass on signals emitted from Directory (by way of ControlPointEditWidget)
    // This is done to redraw the control points on the cube viewports
    connect(this, SIGNAL(controlPointAdded(QString)), ipceTool, SLOT(refresh()));

    tools->append(new ZoomTool(this));
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
    tools->append(new HelpTool(this));

    QStatusBar *statusBar = new QStatusBar(this);
    layout->addWidget(statusBar);
    tools->append(new TrackTool(statusBar));

//  QnetNavTool *ntool = new QnetNavTool(qnetTool, this);
//  tools->append(ntool);
//  tools->append(qnetTool);
//
//  connect(qnetTool, SIGNAL(showNavTool()), ntool, SLOT(showNavTool()));

    //QMenuBar *menuBar = new QMenuBar;
    //QMap<QString, QMenu *> subMenus;

    m_separatorAction = new QAction(this);
    m_separatorAction->setSeparator(true);
    
    m_fileMenu = new QMenu;
    m_viewMenu = new QMenu;
    m_optionsMenu = new QMenu;
    m_windowMenu = new QMenu;
    m_helpMenu = new QMenu;

    for (int i = 0; i < tools->count(); i++) {
      Tool *tool = (*tools)[i];

      if (tool) {
        tool->addTo(m_workspace);
        tool->addToPermanent(m_permToolBar);
        tool->addToActive(m_activeToolBar);
        tool->addTo(m_toolPad);

        if (!tool->menuName().isEmpty()) {
          QString menuName = tool->menuName();

          if (menuName == "&File") {
            tool->addTo(m_fileMenu);
          }
          else if (menuName == "&View") {
            tool->addTo(m_viewMenu);
          }
          else if (menuName == "&Options") {
            tool->addTo(m_optionsMenu);
          }
          else if (menuName == "&Window") {
            tool->addTo(m_windowMenu);
          }
          else if (menuName == "&Help") {
            tool->addTo(m_helpMenu);
          }
        }
      }
      else {
        m_permToolBar->addSeparator();
      }
    }

    m_permToolBarActions.append( m_permToolBar->actions() );

    m_activeToolBarAction = new QWidgetAction(this);
    m_activeToolBarAction->setDefaultWidget(m_activeToolBar);

    m_toolPadActions.append( m_toolPad->actions() );

    QSizePolicy policy = sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    policy.setVerticalPolicy(QSizePolicy::Expanding);
    setSizePolicy(policy);
  }


  /**
   * Destructor
   */
  CubeDnView::~CubeDnView() {
    delete m_permToolBar;
    delete m_activeToolBar;
    delete m_toolPad;

    m_permToolBar = 0;
    m_activeToolBar = 0;
    m_toolPad = 0;
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


  /**
   * Returns the suggested size
   *
   * @return @b QSize The size hint
   */
  QSize CubeDnView::sizeHint() const {
    return QSize(800, 600);
  }


  bool CubeDnView::viewportContainsShape(MdiCubeViewport *viewport) {

    ProjectItem *item = m_cubeItemMap.value( viewport->cube() );

    if (!item) {
      return false;
    }

    return item->isShape();
  }


  /**
   * Returns a list of actions appropriate for a file menu.
   *
   * @return @b QList<QAction*> The actions
   */
  QList<QAction *> CubeDnView::fileMenuActions() {
    return m_fileMenu->actions();
  }


  /**
   * Returns a list of actions appropriate for a project menu.
   *
   * @return @b QList<QAction*> The actions
   */
  QList<QAction *> CubeDnView::projectMenuActions() {
    return QList<QAction *>();
  }

  /**
   * Returns a list of actions appropriate for an edit menu.
   *
   * @return @b QList<QAction*> The actions
   */
  QList<QAction *> CubeDnView::editMenuActions() {
    return QList<QAction *>();
  }


  /**
   * Returns a list of actions appropriate for a view menu.
   *
   * @return @b QList<QAction*> The actions
   */
  QList<QAction *> CubeDnView::viewMenuActions() {
    QList<QAction *> result;
    result.append( m_viewMenu->actions() );
    result.append(m_separatorAction);
    result.append( m_windowMenu->actions() );
    return result;
  }


  /**
   * Returns a list of actions appropriate for a settings menu.
   *
   * @return @b QList<QAction*> The actions
   */
  QList<QAction *> CubeDnView::settingsMenuActions() {
    return m_optionsMenu->actions();
  }


  /**
   * Returns a list of actions appropriate for a help menu.
   *
   * @return @b QList<QAction*> The actions
   */
  QList<QAction *> CubeDnView::helpMenuActions() {
    return m_helpMenu->actions();
  }
  

  /**
   * Returns a list of actions for the permanent tool bar.
   *
   * @return @b QList<QAction*> The actions
   */
  QList<QAction *> CubeDnView::permToolBarActions() {
    return m_permToolBar->actions();
  }


  /**
   * Returns a list of actions for the active tool bar.
   *
   * @return @b QList<QAction*> The actions
   */
  QList<QAction *> CubeDnView::activeToolBarActions() {
    QList<QAction *> actions;
    actions.append(m_activeToolBarAction);
    return actions;
  }


  /**
   * Returns a list of actions for the tool pad.
   *
   * @return @b QList<QAction*> The actions
   */
  QList<QAction *> CubeDnView::toolPadActions() {
    return m_toolPad->actions();
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


  void CubeDnView::load(XmlStackedHandlerReader *xmlReader, Project *project) {
    xmlReader->pushContentHandler(new XmlHandler(this, project));
  }


  void CubeDnView::save(QXmlStreamWriter &stream, Project *, FileName) const {
    stream.writeStartElement("cubeDnView");

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


  CubeDnView::XmlHandler::XmlHandler(CubeDnView *cubeDnView, Project *project) {

    m_cubeDnView = cubeDnView;
    m_project = project;
  }


  CubeDnView::XmlHandler::~XmlHandler() {
  }


  bool CubeDnView::XmlHandler::startElement(const QString &namespaceURI,
      const QString &localName, const QString &qName, const QXmlAttributes &atts) {
    bool result = XmlStackedHandler::startElement(namespaceURI, localName, qName, atts);

    if (result) {
      ProjectItemProxyModel *proxy = (ProjectItemProxyModel *) m_cubeDnView->internalModel();
      ProjectItemModel *source = proxy->sourceModel();
      QString id = atts.value("id");

      ProjectItem *item = NULL;
      if (localName == "image") {
        Image *image = m_project->image(id);
        if (image) {
          // Find ProjectItem and append to list
          item = source->findItemData(qVariantFromValue(image));
        }
      }
      else if (localName == "shape") {
        Shape *shape = m_project->shape(id);
        if (shape) {
          item = source->findItemData(qVariantFromValue(shape));
        }
      }
      if (item) {
        proxy->addItem(item);
      }
    }

    return result;
  }


  bool CubeDnView::XmlHandler::endElement(const QString &namespaceURI,
      const QString &localName, const QString &qName) {
    bool result = XmlStackedHandler::endElement(namespaceURI, localName, qName);

    return result;
  }
}
