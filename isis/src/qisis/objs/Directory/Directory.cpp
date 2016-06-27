/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
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
#include "Directory.h"


#include <QAction>
#include <QApplication>
#include <QDockWidget>
#include <QGridLayout>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QSplitter>
#include <QStringList>
#include <QXmlStreamWriter>

#include <QtDebug>
#include <QMessageBox>


#include "CloseProjectWorkOrder.h"
#include "CnetEditorViewWorkOrder.h"
#include "CnetEditorWidget.h"
#include "Control.h"
#include "ControlDisplayProperties.h"
#include "ControlList.h"
//#include "ControlNetEditor.h"  //tjw
//#include "ControlPointEditWidget.h"  //tjw
//#include "CubeDnView.h"  //tjw
#include "CubeDnViewWorkOrder.h"
#include "ExportControlNetWorkOrder.h"
#include "ExportImagesWorkOrder.h"
#include "FileName.h"
//#include "Footprint2DView.h" //tjw
#include "Footprint2DViewWorkOrder.h"
#include "HistoryTreeWidget.h"
#include "IException.h"
#include "IString.h"
#include "ImageFileListViewWorkOrder.h"
#include "ImportControlNetWorkOrder.h"
#include "ImportImagesWorkOrder.h"
#include "ImageFileListWidget.h"
#include "JigsawWorkOrder.h"
//#include "MatrixSceneWidget.h"  //tjw
#include "MatrixViewWorkOrder.h"
#include "MosaicSceneWidget.h"
#include "OpenProjectWorkOrder.h"
#include "OpenRecentProjectWorkOrder.h"


//#include "Project.h"  //tjw
//#include "ProjectItem.h"  //tjw
//#include "ProjectItemModel.h"  //tjw
//#include "ProjectItemTreeView.h"  //tjw
#include "RenameProjectWorkOrder.h"
#include "SaveProjectWorkOrder.h"
#include "SaveProjectAsWorkOrder.h"
#include "SensorGetInfoWorkOrder.h"
#include "SensorInfoWidget.h"
//#include "TargetInfoWidget.h"  //tjw
#include "TargetGetInfoWorkOrder.h"
#include "WarningTreeWidget.h"
#include "WorkOrder.h"
#include "Workspace.h"
#include "XmlStackedHandler.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {

  /**
   * @brief The Constructor
   * @throws IException::Programmer To handle the event that a Project cannot be created.
   * @throws IException::Programmer To handle the event that a Directory cannot be created
   * because the WorkOrders we are attempting to add to the Directory are corrupt.
   */
  Directory::Directory(QObject *parent) : QObject(parent) {
    //qDebug()<<"Directory::Directory";

    //m_controlPointEditWidget = NULL;  //tjw

    try {
      //m_project = new Project(*this);  //tjw
    }
    catch (IException &e) {
      throw IException(e, IException::Programmer,
          "Could not create directory because Project could not be created.",
          _FILEINFO_);
    }

    //connect( m_project, SIGNAL(imagesAdded(ImageList *) ),
             //this, SLOT(imagesAddedToProject(ImageList *) ) );

    //connect( m_project, SIGNAL(targetsAdded(TargetBodyList *) ),
             //this, SLOT(targetsAddedToProject(TargetBodyList *) ) );

    //connect( m_project, SIGNAL(guiCamerasAdded(GuiCameraList *) ),
             //this, SLOT(guiCamerasAddedToProject(GuiCameraList *) ) );

    // <<<<<<< .mine
// =======
//     connect( m_project, SIGNAL(projectLoaded(Project *) ),
//              this, SLOT(updateRecentProjects(Project *) ) );
// 
// >>>>>>> .r5959

    //m_projectItemModel = new ProjectItemModel(this);  //tjw
    //m_projectItemModel->addProject(m_project);  //tjw

    try {
      /*  //tjw
      createWorkOrder<CnetEditorViewWorkOrder>();
      createWorkOrder<CubeDnViewWorkOrder>();
      createWorkOrder<Footprint2DViewWorkOrder>();
      createWorkOrder<MatrixViewWorkOrder>();
      createWorkOrder<SensorGetInfoWorkOrder>();
      createWorkOrder<TargetGetInfoWorkOrder>();
      createWorkOrder<ImageFileListViewWorkOrder>();

      m_exportControlNetWorkOrder = createWorkOrder<ExportControlNetWorkOrder>();
      m_exportImagesWorkOrder = createWorkOrder<ExportImagesWorkOrder>();
      m_importControlNetWorkOrder = createWorkOrder<ImportControlNetWorkOrder>();
      m_importImagesWorkOrder = createWorkOrder<ImportImagesWorkOrder>();
      m_openProjectWorkOrder = createWorkOrder<OpenProjectWorkOrder>();
      m_saveProjectWorkOrder = createWorkOrder<SaveProjectWorkOrder>();
      m_saveProjectAsWorkOrder = createWorkOrder<SaveProjectAsWorkOrder>();
      m_openRecentProjectWorkOrder = createWorkOrder<OpenRecentProjectWorkOrder>();
      m_runJigsawWorkOrder = createWorkOrder<JigsawWorkOrder>();
      m_closeProjectWorkOrder = createWorkOrder<CloseProjectWorkOrder>();
      m_renameProjectWorkOrder = createWorkOrder<RenameProjectWorkOrder>();
      */
    }
    catch (IException &e) {
      throw IException(e, IException::Programmer,
          "Could not create directory because work orders are corrupt.",
           _FILEINFO_);
    }

    initializeActions();

  }


  /**
   * @brief The Destructor.
   */
  Directory::~Directory() {

    //delete m_project;  //tjw
    //m_project = NULL;  //tjw

    foreach (WorkOrder *workOrder, m_workOrders) {
      delete workOrder;
    }

    m_workOrders.clear();
  }


  /**
   * @brief Get the list of actions that the Directory can provide for the file menu.
   * @return @b QList<QAction *> Returns a list of file menu actions.
   */
  QList<QAction *> Directory::fileMenuActions() {
    return m_fileMenuActions;
  }


  /**
   * @brief Get the list of actions that the Directory can provide for the project menu.
   * @return @b QList<QAction *> Returns a list of project menu actions.
   */
  QList<QAction *> Directory::projectMenuActions() {
    return m_projectMenuActions;
  }

  
  /**
   * @brief Get the list of actions that the Directory can provide for the edit menu.
   * @return @b QList<QAction *> Returns a list of edit menu actions.
   */
  QList<QAction *> Directory::editMenuActions() {
    return m_editMenuActions;
  }


  /**
   * @brief Get the list of actions that the Directory can provide for the view menu.
   * @return @b QList<QAction *>  Returns a list of view menu actions.
   */
  QList<QAction *> Directory::viewMenuActions() {
    return m_viewMenuActions;
  }

  
  /**
   * @brief Get the list of actions that the Directory can provide for the settings menu.
   * @return @b QList<QAction *>  Returns a list of menu actions for the settings.
   */
  QList<QAction *> Directory::settingsMenuActions() {
    return m_settingsMenuActions;
  }


  /**
   * @brief Get the list of actions that the Directory can provide for the help menu.
   * @return @b QList<QAction *> Returns a list of help menu actions.
   */
  QList<QAction *> Directory::helpMenuActions() {
    return m_helpMenuActions;
  }


  /**
   * @brief Get the list of actions that the Directory can provide for the permanent Tool Bar.
   * @return @b QList<QAction *> Returns a list of permanent tool bar menu actions.
   */
  QList<QAction *> Directory::permToolBarActions() {
    return m_permToolBarActions;
  }


  /**
   * @brief Get the list of actions that the Directory can provide for the active Tool Bar.
   * @return @b QList<QAction *>  Returns a list of active Tool Bar actions.
   */
  QList<QAction *> Directory::activeToolBarActions() {
    return m_activeToolBarActions;
  }

  
  /**
   * @brief Get the list of actions that the Directory can provide for the Tool Pad.
   * @return @b QList<QAction *>  Returns a list of Tool Pad actions.
   */
  QList<QAction *> Directory::toolPadActions() {
    return m_toolPadActions;
  }

  
  /**
   * @description Initializes the actions that the Directory can provide to a main window.
   */
  void Directory::initializeActions() {
    // Menus are created temporarily to convinently organize the actions.
    QMenu *fileMenu = new QMenu();

    //fileMenu->addAction(m_importControlNetWorkOrder->clone());
    //fileMenu->addAction(m_importImagesWorkOrder->clone());

    QAction *openProjectAction = m_openProjectWorkOrder->clone();
    openProjectAction->setIcon(QIcon(":open") );
    fileMenu->addAction(openProjectAction);

    m_permToolBarActions.append(openProjectAction);

    QMenu *recentProjectsMenu = fileMenu->addMenu("&Recent Projects");
    int nRecentProjects = m_recentProjects.size();

    for (int i = 0; i < nRecentProjects; i++) {
      FileName projectFileName = m_recentProjects.at(i);
      if (!projectFileName.fileExists() )
        continue;

      QAction *openRecentProjectAction = m_openRecentProjectWorkOrder->clone();

      openRecentProjectAction->setData(m_recentProjects.at(i) );
      openRecentProjectAction->setText(m_recentProjects.at(i) );

      if ( !( (OpenRecentProjectWorkOrder*)openRecentProjectAction )
           ->isExecutable(m_recentProjects.at(i) ) )
        continue;

      recentProjectsMenu->addAction(openRecentProjectAction);
    }

    fileMenu->addSeparator();

    QAction *saveAction = m_saveProjectWorkOrder->clone();
    saveAction->setShortcut(Qt::Key_S | Qt::CTRL);
    saveAction->setIcon( QIcon(":save") );

    //tjw
    //connect( project()->undoStack(), SIGNAL( cleanChanged(bool) ),
    //         saveAction, SLOT( setDisabled(bool) ) );

    fileMenu->addAction(saveAction);
    m_permToolBarActions.append(saveAction);

    QAction *saveAsAction = m_saveProjectAsWorkOrder->clone();
    saveAsAction->setIcon(QIcon(":saveAs"));
    fileMenu->addAction(saveAsAction);
    m_permToolBarActions.append(saveAsAction);

    fileMenu->addSeparator();

    QMenu *importMenu = fileMenu->addMenu("&Import");
    importMenu->addAction(m_importControlNetWorkOrder->clone() );
    importMenu->addAction(m_importImagesWorkOrder->clone() );
    
    QMenu *exportMenu = fileMenu->addMenu("&Export");
    exportMenu->addAction(m_exportControlNetWorkOrder->clone() );
    exportMenu->addAction(m_exportImagesWorkOrder->clone() );

    fileMenu->addSeparator();

    fileMenu->addAction(m_closeProjectWorkOrder->clone() );

    m_fileMenuActions.append( fileMenu->actions() );

    
    QMenu *projectMenu = new QMenu();
    projectMenu->addAction(m_renameProjectWorkOrder->clone());
    projectMenu->addAction(m_runJigsawWorkOrder->clone() );
    
    m_projectMenuActions.append( projectMenu->actions() );

    m_editMenuActions = QList<QAction *>();
    m_viewMenuActions = QList<QAction *>();
    m_settingsMenuActions = QList<QAction *>();
    m_helpMenuActions = QList<QAction *>();    
  }

  
  /**
   * @description This method sets up the main menu at the top of the window (File, Settings, ...)
   * @param menuBar The menu area to populate.
   */
  void Directory::populateMainMenu(QMenuBar *menuBar) {
    QMenu *fileMenu = menuBar->findChild<QMenu *>("fileMenu");
    if (fileMenu) {
      fileMenu->addAction(m_importControlNetWorkOrder->clone());
      fileMenu->addAction(m_importImagesWorkOrder->clone());
      QAction *openProjectAction = m_openProjectWorkOrder->clone();
      openProjectAction->setIcon(QIcon(":open") );
      fileMenu->addAction(openProjectAction);

      QMenu *recentProjectsMenu = fileMenu->addMenu("Recent P&rojects");
      int nRecentProjects = m_recentProjects.size();
      for (int i = 0; i < nRecentProjects; i++) {
        FileName projectFileName = m_recentProjects.at(i);
        if (!projectFileName.fileExists() )
          continue;

        QAction *openRecentProjectAction = m_openRecentProjectWorkOrder->clone();

        openRecentProjectAction->setData(m_recentProjects.at(i) );
        openRecentProjectAction->setText(m_recentProjects.at(i) );

        if ( !( (OpenRecentProjectWorkOrder*)openRecentProjectAction )
             ->isExecutable(m_recentProjects.at(i) ) )
          continue;

        recentProjectsMenu->addAction(openRecentProjectAction);
      }

      fileMenu->addSeparator();
      fileMenu->addAction(m_openProjectWorkOrder->clone());
      fileMenu->addSeparator();

      m_permToolBarActions.append(m_openProjectWorkOrder->clone());

      QAction *saveAction = m_saveProjectWorkOrder->clone();
      saveAction->setShortcut(Qt::Key_S | Qt::CTRL);
      saveAction->setIcon( QIcon(":save") );

      //tjw
      //connect( project()->undoStack(), SIGNAL( cleanChanged(bool) ),
      //         saveAction, SLOT( setDisabled(bool) ) );

      fileMenu->addAction(saveAction);
      QAction *addAction = m_saveProjectAsWorkOrder->clone();
      addAction->setIcon(QIcon(":saveAs") );
      fileMenu->addAction(addAction);

      fileMenu->addSeparator();
      fileMenu->addAction(m_exportControlNetWorkOrder->clone());
      fileMenu->addAction(m_exportImagesWorkOrder->clone());

      QMenu *importMenu = fileMenu->addMenu("&Import");
      importMenu->addAction(m_importControlNetWorkOrder->clone() );
      importMenu->addAction(m_importImagesWorkOrder->clone() );

      QMenu *exportMenu = fileMenu->addMenu("&Export");

      exportMenu->addAction(m_exportControlNetWorkOrder->clone() );
      exportMenu->addAction(m_exportImagesWorkOrder->clone() );

      fileMenu->addSeparator();

      fileMenu->addAction(m_closeProjectWorkOrder->clone() );

      fileMenu->addSeparator();
    }

    QMenu *projectMenu = menuBar->findChild<QMenu *>("projectMenu");
    if (projectMenu) {
      projectMenu->addAction( m_renameProjectWorkOrder->clone() );
      projectMenu->addAction( m_runJigsawWorkOrder->clone() );
    }
  }


  /**
   * @brief Set up the history info in the history dockable widget.
   * @param historyContainer The widget to fill.
   */
  void Directory::setHistoryContainer(QDockWidget *historyContainer) {
    /*  //tjw
    if (!m_historyTreeWidget) {
      m_historyTreeWidget = new HistoryTreeWidget( project() );
    }
    historyContainer->setWidget(m_historyTreeWidget);
    */
  }


  /**
   * @brief Set up the warning info in the warning dockable widget.
   * @param warningContainer The widget to fill.
   */
  void Directory::setWarningContainer(QDockWidget *warningContainer) {
    if (!m_warningTreeWidget) {
      m_warningTreeWidget = new WarningTreeWidget;
    }
    warningContainer->setWidget(m_warningTreeWidget);
  }
  

  /**
   * @brief Add recent projects to the recent projects list.
   * @param recentProjects List of projects to add to list.
   */
  void Directory::setRecentProjectsList(QStringList recentProjects) {
    m_recentProjects.append(recentProjects);
  }


  /**
   * @brief Public accessor for the list of recent projects.
   * @return @b QStringList List of recent projects.
   */
   QStringList Directory::recentProjectsList() {
     return m_recentProjects;
  }

  
  /**
   * @brief Add the widget for the cnet editor view to the window.
   * @param network Control net to edit.
   * @return @b (CnetEditorWidget *) The view to add to the window.
   */
  CnetEditorWidget *Directory::addCnetEditorView(Control *network) {
    QString title = tr("Cnet Editor View %1").arg( network->displayProperties()->displayName() );

    FileName configFile("$HOME/.Isis/" + QApplication::applicationName() + "/" + title + ".config");

    // TODO: This layout should be inside of the cnet editor widget, but I put it here to not
    //     conflict with current work in the cnet editor widget code.
    QWidget *result = new QWidget;
    QGridLayout *resultLayout = new QGridLayout;
    result->setLayout(resultLayout);

    int row = 0;

    QMenuBar *menuBar = new QMenuBar;
    resultLayout->addWidget(menuBar, row, 0, 1, 2);
    row++;

    CnetEditorWidget *mainWidget =
        new CnetEditorWidget( network->controlNet(), configFile.expanded() );
    resultLayout->addWidget(mainWidget, row, 0, 1, 2);
    row++;

    // Populate the menu...
    QMap< QAction *, QList< QString > > actionMap = mainWidget->menuActions();
    QMapIterator< QAction *, QList< QString > > actionMapIterator(actionMap);

    QMap<QString, QMenu *> topLevelMenus;

    while ( actionMapIterator.hasNext() ) {
      actionMapIterator.next();
      QAction *actionToAdd = actionMapIterator.key();
      QList< QString > location = actionMapIterator.value();

      QMenu *menuToPutActionInto = NULL;

      if ( location.count() ) {
        QString topLevelMenuTitle = location.takeFirst();
        if (!topLevelMenus[topLevelMenuTitle]) {
          topLevelMenus[topLevelMenuTitle] = menuBar->addMenu(topLevelMenuTitle);
        }

        menuToPutActionInto = topLevelMenus[topLevelMenuTitle];
      }

      foreach (QString menuName, location) {
        bool foundSubMenu = false;
        foreach ( QAction *possibleSubMenu, menuToPutActionInto->actions() ) {
          if (!foundSubMenu &&
              possibleSubMenu->menu() && possibleSubMenu->menu()->title() == menuName) {
            foundSubMenu = true;
            menuToPutActionInto = possibleSubMenu->menu();
          }
        }

        if (!foundSubMenu) {
          menuToPutActionInto = menuToPutActionInto->addMenu(menuName);
        }
      }

      menuToPutActionInto->addAction(actionToAdd);
    }

    QTabWidget *treeViews = new QTabWidget;
    treeViews->addTab( mainWidget->pointTreeView(), tr("Point View") );
    treeViews->addTab( mainWidget->serialTreeView(), tr("Serial View") );
    treeViews->addTab( mainWidget->connectionTreeView(), tr("Connection View") );
    resultLayout->addWidget(treeViews, row, 0, 1, 1);

    QTabWidget *filterViews = new QTabWidget;
    filterViews->addTab( mainWidget->pointFilterWidget(), tr("Filter Points and Measures") );
    filterViews->addTab( mainWidget->serialFilterWidget(), tr("Filter Images and Points") );
    filterViews->addTab( mainWidget->connectionFilterWidget(), tr("Filter Connections") );
    resultLayout->addWidget(filterViews, row, 1, 1, 1);
    row++;

    connect( result, SIGNAL( destroyed(QObject *) ),
             this, SLOT( cleanupCnetEditorViewWidgets() ) );

    m_cnetEditorViewWidgets.append(mainWidget);

    result->setWindowTitle(title);
    result->setObjectName(title);

    emit newWidgetAvailable(result);

    return mainWidget;
  }


  /**
   * @brief Add the qview workspace to the window.
   * @return @b (CubeDnView*) The work space to display.
   */
  /*  //tjw
  CubeDnView *Directory::addCubeDnView() {

    CubeDnView *result = new CubeDnView();
    result->setModel(m_projectItemModel);
    m_cubeDnViewWidgets.append(result);
    connect( result, SIGNAL( destroyed(QObject *) ),
             this, SLOT( cleanupCubeDnViewWidgets() ) );
    
    result->setWindowTitle("Cube DN View");
    result->setWindowTitle( tr("Cube DN View %1").arg(m_cubeDnViewWidgets.count() ) );
    
    emit newWidgetAvailable(result);
    return result;

  }
*/

  /**
   * @brief Add the qmos view widget to the window.
   * @return @b (Footprint2DView*) A pointer to the Footprint2DView to display.
   */

  //tjw


//  Footprint2DView *Directory::addFootprint2DView() {
//    Footprint2DView *result = new Footprint2DView();
//    result->setModel(m_projectItemModel);
//    m_footprint2DViewWidgets.append(result);
//    result->setWindowTitle( tr("Footprint View %1").arg( m_footprint2DViewWidgets.count() ) );
    
//    connect( result, SIGNAL( destroyed(QObject *) ),
//             this, SLOT( cleanupFootprint2DViewWidgets() ) );
    
//    emit newWidgetAvailable(result);
//    return result;
    
    /*
    //qDebug()<<"Directory::addFootprint2DView";
    MosaicSceneWidget *result = new MosaicSceneWidget(NULL, true, true, this);

    connect( result, SIGNAL( destroyed(QObject *) ),
             this, SLOT( cleanupFootprint2DViewWidgets() ) );

    m_footprint2DViewWidgets.append(result);

    result->setWindowTitle( tr("Footprint View %1").arg( m_footprint2DViewWidgets.count() ) );
    result->setObjectName(result->windowTitle());
    result->setObjectName( result->windowTitle() );

    emit newWidgetAvailable(result);

    return result;
    */

//  }




  /**
   * @brief Add the matrix view widget to the window.
   * @return @b (MatrixSceneWidget*) The widget to view.
   */
  /*
  MatrixSceneWidget *Directory::addMatrixView() {
    MatrixSceneWidget *result = new MatrixSceneWidget(NULL, true, true, this);

    connect( result, SIGNAL( destroyed(QObject *) ),
             this, SLOT( cleanupMatrixViewWidgets() ) );

    m_matrixViewWidgets.append(result);

    result->setWindowTitle( tr("Matrix View %1").arg( m_matrixViewWidgets.count() ) );
    result->setObjectName( result->windowTitle() );

    emit newWidgetAvailable(result);

    return result;
  }
*/

  /**
   * @brief Add target body data view widget to the window.
   * @return (TargetInfoWidget*) The widget to view.
   */
  /*
  TargetInfoWidget *Directory::addTargetInfoView(TargetBodyQsp target) {
    TargetInfoWidget *result = new TargetInfoWidget(target.data(), this);

    connect( result, SIGNAL( destroyed(QObject *) ),
             this, SLOT( cleanupTargetInfoWidgets() ) );

    m_targetInfoWidgets.append(result);

    result->setWindowTitle( tr("%1").arg(target->displayProperties()->displayName() ) );
    result->setObjectName( result->windowTitle() );

    emit newWidgetAvailable(result);

    return result;
  }
*/

  /**
   * @brief Add sensor data view widget to the window.
   * @return @b (SensorInfoWidget*) The widget to view.
   */
  SensorInfoWidget *Directory::addSensorInfoView(GuiCameraQsp camera) {
    SensorInfoWidget *result = new SensorInfoWidget(camera.data(), this);

    connect( result, SIGNAL( destroyed(QObject *) ),
             this, SLOT( cleanupSensorInfoWidgets() ) );

    m_sensorInfoWidgets.append(result);

    result->setWindowTitle( tr("%1").arg(camera->displayProperties()->displayName() ) );
    result->setObjectName( result->windowTitle() );

    emit newWidgetAvailable(result);

    return result;
  }


  /**
   * @brief Add an imageFileList widget to the window.
   * @return @b (ImageFileListWidget *) A pointer to the widget to add to the window.
   */
  ImageFileListWidget *Directory::addImageFileListView() {
    //qDebug()<<"Directory::addImageFileListView";
    ImageFileListWidget *result = new ImageFileListWidget(this);

    connect( result, SIGNAL( destroyed(QObject *) ),
             this, SLOT( cleanupFileListWidgets() ) );

    m_fileListWidgets.append(result);

    result->setWindowTitle( tr("File List %1").arg( m_fileListWidgets.count() ) );
    result->setObjectName( result->windowTitle() );

    emit newWidgetAvailable(result);

    return result;
  }


  /**
   * @brief  Adds a control net editor to the window.
   * @param cnet A pointer to a control network to pass to the control point edit widget.
   * @param The name of the control network.
   */
  void Directory::addControlPointEditor(ControlNet *cnet, QString cnetFilename) {
     //tjw
    /*
    if (m_controlPointEditWidget == NULL) {
      //  TODO  Need parent for controlPointWidget
      m_controlPointEditWidget = new ControlPointEditWidget(m_footprint2DViewWidgets.at(0)); 
      m_controlPointEditWidget->setWindowTitle(tr("Control Point Editor"));
      m_controlPointEditWidget->setObjectName(m_controlPointEditWidget->windowTitle());

      m_controlPointEditWidget->setControlNet(cnet, cnetFilename);

      //  TODO  Need active imagelist - simply use first for now
      SerialNumberList *snList = new SerialNumberList();
      *snList = project()->images().at(0)->serialNumberList();
      m_controlPointEditWidget->setSerialNumberList(snList);

//    connect(m_controlPointEditWidget, SIGNAL(destroyed(QObject *)),
//            this, SLOT(cleanupControlPointEdit()));
      emit newWidgetAvailable(m_controlPointEditWidget);
      //qDebug()<<"Directory::addControlPointEditor  after emit newWidgetAvailable";

      m_cnetEditor = new ControlNetEditor(snList, cnet, m_controlPointEditWidget);
    }
    //  Update control network
    else {

    }

    updateControlNetEditConnections();
    */
  }


  /**
   * @brief Adds a ProjectItemTreeView to the window.
   * @return @b (ProjectItemTreeView *) The added view.
   */
  /* //tjw
  ProjectItemTreeView *Directory::addProjectItemTreeView() {
    ProjectItemTreeView *result = new ProjectItemTreeView(); 
    result->setModel(m_projectItemModel);
    
    return result;
  }
  */

  /**
   * @brief Gets the ProjectItemModel for this directory.
   * @return @b (ProjectItemModel *) Returns a pointer to the ProjectItemModel.
   */
  /*
  ProjectItemModel *Directory::model() {
    return m_projectItemModel;
  }

  */

  /**
   * @brief Returns a pointer to the warning widget.
   * @return @b (QWidget *)  The WarningTreeWidget pointer.
   */
  QWidget *Directory::warningWidget() {
    return m_warningTreeWidget;
  }


  /**
   * @brief Removes pointers to deleted CnetEditorWidget objects.
   */
  void Directory::cleanupCnetEditorViewWidgets() {
    //m_cnetEditorViewWidgets.removeAll(NULL);  //tjw
  }


  /**
   * @brief Removes pointers to deleted CubeDnView objects.
   */
  void Directory::cleanupCubeDnViewWidgets() {
    //m_cubeDnViewWidgets.removeAll(NULL);  //tjw
  }


  /**
   * @brief Reomoves pointers to deleted ImageFileListWidget objects.
   */
  void Directory::cleanupFileListWidgets() {
    m_fileListWidgets.removeAll(NULL);

  }


  /**
   * @brief Removes pointers to deleted Footprint2DView objects.
   */
  void Directory::cleanupFootprint2DViewWidgets() {
    //m_footprint2DViewWidgets.removeAll(NULL);  //tjw
  }


  /**
   * @brief Delete the ControlPointEditWidget and set it's pointer to NULL.
   */
  void Directory::cleanupControlPointEditorWidget() {
    //delete m_controlPointEditWidget;  //tjw
    //m_controlPointEditWidget = NULL;
  }


  /**
   * @brief Removes pointers to deleted MatrixSceneWidget objects.
   */
  void Directory::cleanupMatrixViewWidgets() {
    //m_matrixViewWidgets.removeAll(NULL);//tjw
  }


  /**
   * @brief Removes pointers to deleted SensorInfoWidget objects.
   */
  void Directory::cleanupSensorInfoWidgets() {
    m_sensorInfoWidgets.removeAll(NULL);
  }


  /**
   * @brief Removes pointers to deleted TargetInfoWidget objects.
   */
  void Directory::cleanupTargetInfoWidgets() {
    //m_targetInfoWidgets.removeAll(NULL);  //tjw
  }


  /**
   * @brief  Adds a new Project object to the list of recent projects if it has not
   * already been added.
   * @param project A pointer to the Project to add.
   */
  void Directory::updateRecentProjects(Project *project) {
    //if ( !m_recentProjects.contains( project->projectRoot() ) )  //tjw
    //  m_recentProjects.insert( 0, project->projectRoot() );      //tjw
  }


  /**
   * @brief Gets the Project for this directory.
   * @return @b (Project *) Returns a pointer to the Project.
   */

  /*
  Project *Directory::project() const {
    return m_project;
  }
*/

  /**
   * @brief Returns a list of all the control network views for this directory.
   * @return @b QList<CnetEditorWidget *> A pointer list of all the CnetEditorWidget objects.
   */
  QList<CnetEditorWidget *> Directory::cnetEditorViews() {
    QList<CnetEditorWidget *> results;

    foreach (CnetEditorWidget *widget, m_cnetEditorViewWidgets) {
      results.append(widget);
    }

    return results;
  }


  /**
   * @brief Accessor for the list of CubeDnViews currently available.
   * @return @b QList<CubeDnView *> The list CubeDnView objects.
   */
  /*  //tjw
  QList<CubeDnView *> Directory::cubeDnViews() {
    QList<CubeDnView *> results;

    foreach (CubeDnView *widget, m_cubeDnViewWidgets) {
      results.append(widget);
    }

    return results;
  }
*/

  /**
   * @brief Accessor for the list of MatrixSceneWidgets currently available.
   * @return @b QList<MatrixSceneWidget *> The list of MatrixSceneWidget objects.
   */
  /* //tjw
  QList<MatrixSceneWidget *> Directory::matrixViews() {
    QList<MatrixSceneWidget *> results;

    foreach (MatrixSceneWidget *widget, m_matrixViewWidgets) {
      results.append(widget);
    }

    return results;
  }

  */
  
  /**
   * @brief Accessor for the list of SensorInfoWidgets currently available.
   * @return QList<SensorInfoWidget *> The list of SensorInfoWidget objects.
   */
  QList<SensorInfoWidget *> Directory::sensorInfoViews() {
    QList<SensorInfoWidget *> results;

    foreach (SensorInfoWidget *widget, m_sensorInfoWidgets) {
      results.append(widget);
    }

    return results;
  }


  /**
   * @brief Accessor for the list of TargetInfoWidgets currently available.
   * @return @b QList<TargetInfoWidget *> The list of TargetInfoWidget objects.
   */
  /*
  QList<TargetInfoWidget *> Directory::targetInfoViews() {
    QList<TargetInfoWidget *> results;

    foreach (TargetInfoWidget *widget, m_targetInfoWidgets) {
      results.append(widget);
    }

    return results;
  }

*/
  /**
   * @brief Accessor for the list of Footprint2DViews currently available.
   * @return QList<Footprint2DView *> The list of MosaicSceneWidget objects.
   */
  /*
  QList<Footprint2DView *> Directory::footprint2DViews() {
    QList<Footprint2DView *> results;

    foreach (Footprint2DView *view, m_footprint2DViewWidgets) {
      results.append(view);
    }

    return results;
  }

*/
  /**
   * @brief Accessor for the list of ImageFileListWidgets currently available.
   * @return QList<ImageFileListWidget *> The list of ImageFileListWidgets.
   */
  QList<ImageFileListWidget *> Directory::imageFileListViews() {
    QList<ImageFileListWidget *> results;

    foreach (ImageFileListWidget *widget, m_fileListWidgets) {
      results.append(widget);
    }

    return results;
  }

  /**
   * @brief Gets the ControlPointEditWidget associated with the Directory.
   * @return @b (ControlPointEditWidget *) Returns a pointer to the ControlPointEditWidget.
   */
  /*
  ControlPointEditWidget *Directory::controlPointEditorView() {

    return m_controlPointEditWidget;
  }
*/

  /**
   * @brief Gets the ControlNetEditor associated with this the Directory.
   * @return @b (ControlNetEditor *) Returns a pointer to the ControlNetEditor.
   */
  /*
   * //tjw
  ControlNetEditor *Directory::controlNetEditor() {
    return m_cnetEditor;
  }
  */


  /**
   * @brief Returns a list of progress bars associated with this Directory.
   * @return @b QList<QProgressBar *>
   */
  QList<QProgressBar *> Directory::progressBars() {
    QList<QProgressBar *> result;
    return result;
  }


  /**
   * @brief Displays a Warning
   * @param text The text to be displayed as a warning.
   */
  void Directory::showWarning(QString text) {
    m_warningTreeWidget->showWarning(text);
  }


  /**
   * @brief Creates an Action to redo the last action.
   * @return @b (QAction *) Returns an action pointer to redo the last action.
   */
  /*  //tjw
  QAction *Directory::redoAction() {
    return project()->undoStack()->createRedoAction(this);
  }

  */

  /**
   * @brief Creates an Action to undo the last action.
   * @return @b (QAction *) Returns an action pointer to undo the last action.
   */
  /*  //tjw
  QAction *Directory::undoAction() {
    return project()->undoStack()->createUndoAction(this);
  }
*/

  /**
   * @brief Loads the Directory from an XML file.
   * @param xmlReader  The reader that takes in and parses the XML file.
   */
  void Directory::load(XmlStackedHandlerReader *xmlReader) {
    xmlReader->pushContentHandler( new XmlHandler(this) );
  }


  /**
   * @brief Save the directory to an XML file.
   * @param stream  The XML stream writer
   * @param newProjectRoot The FileName of the project this Directory is attached to.
   */
  void Directory::save(QXmlStreamWriter &stream, FileName newProjectRoot) const {
    stream.writeStartElement("directory");

    if ( !m_fileListWidgets.isEmpty() ) {
      stream.writeStartElement("fileListWidgets");

      //foreach (ImageFileListWidget *fileListWidget, m_fileListWidgets) {  //tjw
      //  fileListWidget->save(stream, project(), newProjectRoot);
      //}

      stream.writeEndElement();
    }

    //if ( !m_footprint2DViewWidgets.isEmpty() ) {
    //  //qDebug()<<"Writing footprints";
    //  stream.writeStartElement("footprintViews");

    //  foreach (MosaicSceneWidget *footprint2DViewWidget, m_footprint2DViewWidgets) {
    //    footprint2DViewWidget->save(stream, project(), newProjectRoot);
    //  }

    //  stream.writeEndElement();
    //}

    stream.writeEndElement();
  }


  /**
   * @brief This function sets the Directory pointer for the Directory::XmlHandler class
   * @param directory The new directory we are setting XmlHandler's member variable to.
   */
  Directory::XmlHandler::XmlHandler(Directory *directory) {
    m_directory = directory;
  }


  /**
   * @brief The Destructor for Directory::XmlHandler
   */
  Directory::XmlHandler::~XmlHandler() {
  }


  /**
   * @description The XML reader invokes this method at the start of every element in the
   * XML document.  This method expects <footprint2DView/> and <imageFileList/>
   * elements.
   * A quick example using this function:
   *     startElement("xsl","stylesheet","xsl:stylesheet",attributes)
   *
   * @param namespaceURI The Uniform Resource Identifier of the element's namespace
   * @param localName The local name string
   * @param qName The XML qualified string (or empty, if QNames are not available).
   * @param atts The XML attributes attached to each element
   * @return @b bool  Returns True signalling to the reader the start of a valid XML element.  If
   * False is returned, something bad happened.
   *
   */
  bool Directory::XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
                                           const QString &qName, const QXmlAttributes &atts) {
    bool result = XmlStackedHandler::startElement(namespaceURI, localName, qName, atts);

    if (result) {
      if (localName == "footprint2DView") {
        //m_directory->addFootprint2DView()->load( reader() );
      }
      else if (localName == "imageFileList") {
        m_directory->addImageFileListView()->load( reader() );
      }
    }

    return result;
  }


  /**
   * @description Reformat actionPairings to be user friendly for use in menus.
   *
   * actionPairings is:
   *    Widget A ->
   *      Action 1
   *      Action 2
   *      Action 3
   *    Widget B ->
   *      Action 1
   *      Action 3
   *      NULL
   *      Action 4
   *    ...
   *
   * We convert this into a list of actions, that when added to a menu, looks like:
   *   Action 1 -> Widget A
   *               Widget B
   *   Action 2 on Widget A
   *   Action 3 -> Widget A
   *               Widget B
   *   ----------------------
   *   Action 4 on Widget B
   *
   * The NULL separators aren't 100% yet, but work a good part of the time.
   *
   * This works by doing a data transformation and then using the resulting data structures
   *   to populate the menu.
   *
   * actionPairings is transformed into:
   *   restructuredData:
   *     Action 1 -> (Widget A, QAction *)
   *                 (Widget B, QAction *)
   *     Action 2 -> (Widget A, QAction *)
   *     Action 3 -> (Widget A, QAction *)
   *                 (Widget B, QAction *)
   *     Action 4 -> (Widget B, QAction *)
   *
   *   and
   *
   *   sortedActionTexts - A list of unique (if not empty) strings:
   *     "Action 1"
   *     "Action 2"
   *     "Action 3"
   *     ""
   *     "Action 4"
   *
   * This transformation is done by looping linearly through actionPairings and for each action in
   *   the pairings we add to the restructured data and append the action's text to
   *   sortedActionTexts.
   *
   * We loop through sortedActionTexts and populate the menu based based on the current (sorted)
   *   action text. If the action text is NULL (we saw a separator in the input), we add a NULL
   *   (separator) to the resulting list of actions. If it isn't NULL, we create a menu or utilize
   *   the action directly depending on if there are multiple actions with the same text.
   *
   * @param actionPairings A list of action pairings.
   * @return @b QList<QAction *> A list of actions that can be added to a menu.
   *
   */
  QList<QAction *> Directory::restructureActions(
      QList< QPair< QString, QList<QAction *> > > actionPairings) {
    QList<QAction *> results;

    QStringList sortedActionTexts;

    // This is a map from the Action Text to the actions and their widget titles
    QMap< QString, QList< QPair<QString, QAction *> > > restructuredData;

    QPair< QString, QList<QAction *> > singleWidgetPairing;
    foreach (singleWidgetPairing, actionPairings) {
      QString widgetTitle = singleWidgetPairing.first;
      QList<QAction *> widgetActions = singleWidgetPairing.second;

      foreach (QAction *widgetAction, widgetActions) {
        if (widgetAction) {
          QString actionText = widgetAction->text();

          restructuredData[actionText].append( qMakePair(widgetTitle, widgetAction) );

          if ( !sortedActionTexts.contains(actionText) ) {
            sortedActionTexts.append(actionText);
          }
        }
        else {
          // Add separator
          if ( !sortedActionTexts.isEmpty() && !sortedActionTexts.last().isEmpty() ) {
            sortedActionTexts.append("");
          }
        }
      }
    }

    if ( sortedActionTexts.count() && sortedActionTexts.last().isEmpty() ) {
      sortedActionTexts.removeLast();
    }

    foreach (QString actionText, sortedActionTexts) {
      if ( actionText.isEmpty() ) {
        results.append(NULL);
      }
      else {
        // We know this list isn't empty because we always appended to the value when we
        //   accessed a particular key.
        QList< QPair<QString, QAction *> > actions = restructuredData[actionText];

        if (actions.count() == 1) {
          QAction *finalAct = actions.first().second;
          QString widgetTitle = actions.first().first;

          finalAct->setText( tr("%1 on %2").arg(actionText).arg(widgetTitle) );
          results.append(finalAct);
        }
        else {
          QAction *menuAct = new QAction(actionText, NULL);

          QMenu *menu = new QMenu;
          menuAct->setMenu(menu);

          QList<QAction *> actionsInsideMenu;

          QPair<QString, QAction *> widgetTitleAndAction;
          foreach (widgetTitleAndAction, actions) {
            QString widgetTitle = widgetTitleAndAction.first;
            QAction *action = widgetTitleAndAction.second;

            action->setText(widgetTitle);
            actionsInsideMenu.append(action);
          }

          qSort(actionsInsideMenu.begin(), actionsInsideMenu.end(), &actionTextLessThan);

          QAction *allAct = new QAction(tr("All"), NULL);

          foreach (QAction *actionInMenu, actionsInsideMenu) {
            connect( allAct, SIGNAL( triggered() ),
                     actionInMenu, SIGNAL( triggered() ) );
            menu->addAction(actionInMenu);
          }

          menu->addSeparator();
          menu->addAction(allAct);

          results.append(menuAct);
        }
      }
    }

    return results;
  }


  /**
   * @brief This is for determining the ordering of the descriptive text of
   * for the actions.
   * @param lhs  The first QAction argument.
   * @param rhs  The second QAction argument.
   * @return @b bool Returns True if the text for the lhs QAction is less than
   * the text for the rhs QAction.  Returns False otherwise.
   */
  bool Directory::actionTextLessThan(QAction *lhs, QAction *rhs) {
    return lhs->text().localeAwareCompare( rhs->text() ) < 0;

  }


  /**
   * @description Updates the SIGNAL/SLOT connections for the cotrol net editor.
   */
  //tjw - commented this function out
  void Directory::updateControlNetEditConnections() {
    //if (m_controlPointEditWidget && m_footprint2DViewWidgets.size() == 1) {
//    connect(m_footprint2DViewWidgets.at(0), SIGNAL(controlPointSelected(ControlPoint *)),
//            m_controlPointEdit, SLOT(loadControlPoint(ControlPoint *)));

      //connect(m_cnetEditor, SIGNAL(controlPointCreated(ControlPoint *)), //tjw
      //        m_controlPointEditWidget, SLOT(setEditPoint(ControlPoint *)));

      // MosaicControlTool->MosaicSceneWidget->ControlNetEditor

      //connect( m_footprint2DViewWidgets.at(0), SIGNAL( deleteControlPoint(QString) ),  //tjw
      //         m_cnetEditor, SLOT( deleteControlPoint(QString) ) );
      // ControlNetEditor->MosaicSceneWidget->MosaicControlTool

      //connect( m_cnetEditor, SIGNAL( controlPointDeleted() ),  //tjw
      //         m_footprint2DViewWidgets.at(0), SIGNAL( controlPointDeleted() ) );


      // TODO Figure out which footprint view has the "active" cnet.  //tjw
      //qDebug() << "\t\tMos items: " << m_footprint2DViewWidgets.at(0);

      //connect(m_controlPointEditWidget, SIGNAL(controlPointChanged(QString)),  //tjw
      //        m_footprint2DViewWidgets.at(0), SIGNAL(controlPointChanged(QString)));


      //connect(m_controlPointEditWidget, SIGNAL(controlPointAdded(QString)),  //tjw
      //        m_footprint2DViewWidgets.at(0), SIGNAL(controlPointAdded(QString)));
      //}
  }
}
