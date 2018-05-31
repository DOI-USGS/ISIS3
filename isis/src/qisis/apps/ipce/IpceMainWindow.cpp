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
#include "IpceMainWindow.h"

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QDockWidget>
#include <QMap>
#include <QMapIterator>
#include <QMdiArea>
#include <QObject>
#include <QRegExp>
#include <QStringList>
#include <QtWidgets>
#include <QSettings>
#include <QSize>
#include <QStatusBar>
#include <QStringList>
#include <QDateTime>
#include <QTreeView>
#include <QVariant>

#include "AbstractProjectItemView.h"
#include "Directory.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "MosaicSceneWidget.h"
#include "ProgressWidget.h"
#include "Project.h"
#include "ProjectItemModel.h"
#include "ProjectItemTreeView.h"
#include "OpenProjectWorkOrder.h"
#include "SensorInfoWidget.h"
#include "TargetInfoWidget.h"
#include "TemplateEditorWidget.h"
#include "ViewSubWindow.h"

namespace Isis {
  /**
   * Construct the main window. This will create a Directory, the menus, and the dock areas.
   *
   * @param parent The Qt-relationship parent widget (usually NULL in this case)
   *
   * @internal
   *   @history 2016-11-09 Tyler Wilson - Moved the if-block which loads a project from the
   *                             command line from the start of the constructor to the end
   *                             because if there were warnings and errors, they were not
   *                             being output to the Warnings widget since the project is loaded
   *                             before the GUI is constructed.  Fixes #4488
   *   @history 2016-11-09 Ian Humphrey - Added default readSettings() call to load initial
   *                           default project window state. References #4358.
   */
  IpceMainWindow::IpceMainWindow(QWidget *parent) :
      QMainWindow(parent) {
    m_maxThreadCount = -1;

    QMdiArea *centralWidget = new QMdiArea;
    centralWidget->setActivationOrder(QMdiArea::StackingOrder);

    connect(centralWidget, SIGNAL( subWindowActivated(QMdiSubWindow *) ),
            this, SLOT( onSubWindowActivated(QMdiSubWindow *) ) );

    setCentralWidget(centralWidget);
    setDockNestingEnabled(true);

    m_activeView = NULL;

    try {
      m_directory = new Directory(this);
      connect(m_directory, SIGNAL( newWidgetAvailable(QWidget *) ),
              this, SLOT( addView(QWidget *) ) );
      connect(m_directory, SIGNAL(viewClosed(QWidget *)), this, SLOT(removeView(QWidget *)));
      connect(m_directory, SIGNAL( directoryCleaned() ),
              this, SLOT( removeAllViews() ) );
      connect(m_directory->project(), SIGNAL(projectLoaded(Project *)),
              this, SLOT(readSettings(Project *)));
      connect(m_directory, SIGNAL( newWarning() ),
              this, SLOT( raiseWarningTab() ) );
    }
    catch (IException &e) {
      throw IException(e, IException::Programmer,
          "Could not create Directory.", _FILEINFO_);
    }

    m_projectDock = new QDockWidget("Project", this, Qt::SubWindow);
    m_projectDock->setObjectName("projectDock");
    m_projectDock->setFeatures(QDockWidget::DockWidgetMovable |
                              QDockWidget::DockWidgetFloatable);
    m_projectDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    ProjectItemTreeView *projectTreeView = m_directory->addProjectItemTreeView();
    projectTreeView->setInternalModel( m_directory->model() );
    projectTreeView->treeView()->expandAll();
    projectTreeView->installEventFilter(this);
    m_projectDock->setWidget(projectTreeView);

    addDockWidget(Qt::LeftDockWidgetArea, m_projectDock, Qt::Horizontal);

    m_warningsDock = new QDockWidget("Warnings", this, Qt::SubWindow);
    m_warningsDock->setObjectName("m_warningsDock");
    m_warningsDock->setFeatures(QDockWidget::DockWidgetClosable |
                         QDockWidget::DockWidgetMovable |
                         QDockWidget::DockWidgetFloatable);
    m_warningsDock->setWhatsThis(tr("This shows notices and warnings from all operations "
                          "on the current project."));
    m_warningsDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    m_directory->setWarningContainer(m_warningsDock);
    addDockWidget(Qt::BottomDockWidgetArea, m_warningsDock);

    QDockWidget *historyDock = new QDockWidget("History", this, Qt::SubWindow);
    historyDock->setObjectName("historyDock");
    historyDock->setFeatures(QDockWidget::DockWidgetClosable |
                         QDockWidget::DockWidgetMovable |
                         QDockWidget::DockWidgetFloatable);
    historyDock->setWhatsThis(tr("This shows all operations performed on the current project."));
    historyDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, historyDock);
    m_directory->setHistoryContainer(historyDock);
    tabifyDockWidget(m_warningsDock, historyDock);

//  QDockWidget *progressDock = new QDockWidget("Progress", this, Qt::SubWindow);
//  progressDock->setObjectName("progressDock");
//  progressDock->setFeatures(QDockWidget::DockWidgetClosable |
//                       QDockWidget::DockWidgetMovable |
//                       QDockWidget::DockWidgetFloatable);
//  progressDock->setAllowedAreas(Qt::BottomDockWidgetArea);
//  //m_directory->setProgressContainer(progressDock);
//  addDockWidget(Qt::BottomDockWidgetArea, progressDock);
//  tabifyDockWidget(historyDock, progressDock);

    historyDock->raise();


    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::North);
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    statusBar()->showMessage("Ready");
    statusBar()->addWidget(m_directory->project()->progress());

    foreach (QProgressBar *progressBar, m_directory->progressBars()) {
      statusBar()->addWidget(progressBar);
    }

    createMenus();
    initializeActions();
    updateMenuActions();

    m_permToolBar = new QToolBar(this);
    m_activeToolBar = new QToolBar(this);
    m_toolPad = new QToolBar(this);

    QSize iconSize(25, 45);

    m_permToolBar->setIconSize(iconSize);
    m_activeToolBar->setIconSize(iconSize);
    m_toolPad->setIconSize(iconSize);

    m_permToolBar->setObjectName("PermanentToolBar");
    m_activeToolBar->setObjectName("ActiveToolBar");
    m_toolPad->setObjectName("ToolPad");

    addToolBar(m_permToolBar);
    addToolBar(m_activeToolBar);
    addToolBar(m_toolPad);
    updateToolBarActions();

    setTabbedViewMode();
    centralWidget->setTabsMovable(true);
    centralWidget->setTabsClosable(true);

    QStringList args = QCoreApplication::arguments();

    if (args.count() == 2) {
      OpenProjectWorkOrder *workorder = new OpenProjectWorkOrder(m_directory->project());
      workorder->execute();
    }

    // ken testing  If this is used, we will not need to call updateMenuActions() or updateToolBar()
    // above.  They are both called from setActiveView.
    //  setActiveView(projectTreeView);
    // ken testing
  }


  /**
   * This is connected from Directory's newWidgetAvailable signal and called when re-attaching a
   * view which was detached from the MDI main window.
   *
   * @param[in] newWidget (QWidget *)
   */
  void IpceMainWindow::addView(QWidget *newWidget) {
    if ( qobject_cast<SensorInfoWidget *>(newWidget) ||
         qobject_cast<TargetInfoWidget *>(newWidget) ||
         qobject_cast<TemplateEditorWidget *>(newWidget)) {
      QDockWidget *dock = new QDockWidget( newWidget->windowTitle() );
      dock->setAttribute(Qt::WA_DeleteOnClose, true);
      dock->setWidget(newWidget);
      dock->setObjectName(newWidget->windowTitle());
      splitDockWidget(m_projectDock, dock, Qt::Vertical);
    }
    else {
      if ( QMdiArea *mdiArea = qobject_cast<QMdiArea *>( centralWidget() ) ) {
        mdiArea->addSubWindow(newWidget);
        newWidget->show();
        mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(newWidget));
        setActiveView(qobject_cast<AbstractProjectItemView *>(newWidget));
      }
    }
  }


  /**
   * @description This slot is connected from Directory::viewClosed(QWidget *) signal.  It will 
   * close the given QMdiSubWindow from the QMdiArea.  This will also delete the widget contained 
   * within the subwindow which is an AbstractProjectItemView. 
   * 
   * @param view QWidget* 
   *
   */
  void IpceMainWindow::removeView(QWidget *view) {

    QMdiArea *mdiArea = qobject_cast<QMdiArea *>( centralWidget() );
    if (mdiArea){
      // Get all QMdiSubWindows, then find subwindow that hold widget
      QList<QMdiSubWindow *> subWindowList = mdiArea->subWindowList();
      foreach (QMdiSubWindow *sub, subWindowList) {
        if (sub->widget() == view) {
          sub->close();
          break;
        }
      }
//    QMdiSubWindow *window = qobject_cast<QMdiSubWindow *>(view);
//    qDebug()<<"IpceMainWindow::removeView activewindow = "<<window;
//    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(view));
//    qDebug()<<"IpceMainWindow::removeView";
//    mdiArea->closeActiveSubWindow();
      delete view;
    }
  }


  /**
   * Removes All Views in main window, connected to directory signal directoryCleaned()
   */
  void IpceMainWindow::removeAllViews() {
    setWindowTitle("ipce");
    QMdiArea *mdiArea = qobject_cast<QMdiArea *>( centralWidget() );
    if (mdiArea){
//    QMdiSubWindow* window = new QMdiSubWindow();
//    window->show();
//    window->activateWindow();
//    mdiArea->addSubWindow(window);
      mdiArea->closeAllSubWindows();
//    delete window;
    }
    if (!m_detachedViews.isEmpty()) {
      foreach ( QMainWindow* view, m_detachedViews ) {
        view->close();
      }
    }

    QList<QDockWidget *> docks = tabifiedDockWidgets(m_projectDock);
    if(docks.count() > 1) {
      foreach ( QDockWidget* widget, docks ) {
        if(widget != m_projectDock) {
          delete widget;
        }
      }
    }
  }


  /**
   * Cleans up the directory.
   */
  IpceMainWindow::~IpceMainWindow() {
    m_directory->deleteLater();
  }


  /**
   * Sets the active view and updates the toolbars and menus.
   *
   * @param[in] view (AbstractProjectItemView *) The active view.
   */
  void IpceMainWindow::setActiveView(AbstractProjectItemView *view) {
    m_activeView = view;
    updateMenuActions();
    updateToolBarActions();
  }


  /**
   * Clears all the menus, then populates the menus with QActions from
   * several sources. The QActions come from an internal list of
   * QActions, the Directory, and the active view.
   */
  void IpceMainWindow::updateMenuActions() {

    m_fileMenu->clear();
    // Get Directory FileMenu actions
    foreach ( QAction *action, m_directory->fileMenuActions() ) {
      m_fileMenu->addAction(action);
    }
    m_fileMenu->addSeparator();
    // Get FileMenu actions for the active view (eg. CubeDnView, Footprint2DView)
    if (m_activeView) {
      foreach ( QAction *action, m_activeView->fileMenuActions() ) {
        m_fileMenu->addAction(action);
      }
    }
    m_fileMenu->addSeparator();
    // Get FileMenu actions from the ipceMainWindow, Exit is the only action
    foreach ( QAction *action, m_fileMenuActions ) {
      m_fileMenu->addAction(action);
    }

    m_projectMenu->clear();
    //  Get Project menu actions from Directory
    foreach ( QAction *action, m_directory->projectMenuActions() ) {
      m_projectMenu->addAction(action);
    }
    m_projectMenu->addSeparator();
    //  Get Project menu actions from the active view
    if (m_activeView) {
      foreach ( QAction *action, m_activeView->projectMenuActions() ) {
        m_projectMenu->addAction(action);
      }
    }
    m_projectMenu->addSeparator();
    //  Get Project menu actions from IpceMainWindow
    foreach ( QAction *action, m_projectMenuActions ) {
      m_projectMenu->addAction(action);
    }

    m_editMenu->clear();
    // Get Edit menu actions from Directory
    foreach ( QAction *action, m_directory->editMenuActions() ) {
      m_editMenu->addAction(action);
    }
    m_editMenu->addSeparator();
    // Get Edit menu actions from active view
    if (m_activeView) {
      foreach ( QAction *action, m_activeView->editMenuActions() ) {
        m_editMenu->addAction(action);
      }
    }
    m_editMenu->addSeparator();
    // Get Edit menu actions from IpceMainWindow
    foreach ( QAction *action, m_editMenuActions ) {
      m_editMenu->addAction(action);
    }

    m_viewMenu->clear();
    // Get View menu actions from Directory
    foreach ( QAction *action, m_directory->viewMenuActions() ) {
      m_viewMenu->addAction(action);
    }
    m_viewMenu->addSeparator();
    // Get View menu actions from IpceMainWindow
    foreach ( QAction *action, m_viewMenuActions ) {
      m_viewMenu->addAction(action);
    }
    m_viewMenu->addSeparator();
    // Get View menu actions from active view
    if (m_activeView) {
      foreach ( QAction *action, m_activeView->viewMenuActions() ) {
        m_viewMenu->addAction(action);
      }
    }

    m_settingsMenu->clear();
    // Get Settings menu actions from Directory
    foreach ( QAction *action, m_directory->settingsMenuActions() ) {
      m_settingsMenu->addAction(action);
    }
    m_settingsMenu->addSeparator();
    // Get Settings menu actions from active view
    if (m_activeView) {
      foreach ( QAction *action, m_activeView->settingsMenuActions() ) {
        m_settingsMenu->addAction(action);
      }
    }
    m_settingsMenu->addSeparator();
    // Get Settings menu actions from IpceMainWindow
    foreach ( QAction *action, m_settingsMenuActions ) {
      m_settingsMenu->addAction(action);
    }

    m_helpMenu->clear();
    // Get Help menu actions from Directory
    foreach ( QAction *action, m_directory->helpMenuActions() ) {
      m_helpMenu->addAction(action);
    }
    m_helpMenu->addSeparator();
    // Get Help menu actions from active view
    if (m_activeView) {
      foreach ( QAction *action, m_activeView->helpMenuActions() ) {
        m_helpMenu->addAction(action);
      }
    }
    m_helpMenu->addSeparator();
    // Get Help menu actions from IpceMainWindow
    foreach ( QAction *action, m_helpMenuActions ) {
      m_helpMenu->addAction(action);
    }
  }


  /**
   * Clears the tool bars and repopulates them with QActions from
   * several sources. Actions are taken from an internal list of
   * QActions, the Directory, and the active view.
   */
  void IpceMainWindow::updateToolBarActions() {

    m_permToolBar->clear();
    foreach ( QAction *action, m_directory->permToolBarActions() ) {
      m_permToolBar->addAction(action);
    }
    foreach (QAction *action, m_permToolBarActions) {
      if (action->text() == "&Save Active Control Network") {
        m_permToolBar->addSeparator();
      }
      m_permToolBar->addAction(action); 
      if (action->text() == "&Save Active Control Network") {
        m_permToolBar->addSeparator();
      }
    }
    m_permToolBar->addSeparator();
    if (m_activeView) {
      foreach ( QAction *action, m_activeView->permToolBarActions() ) {
        m_permToolBar->addAction(action);
      }
    }

    m_activeToolBar->clear();
    if (m_activeView) {
      foreach ( QAction *action, m_activeView->activeToolBarActions() ) {
        m_activeToolBar->addAction(action);
      }
    }

    m_toolPad->clear();
    if (m_activeView) {
      foreach ( QAction *action, m_activeView->toolPadActions() ) {
        m_toolPad->addAction(action);
      }
    }
  }


  /**
   * Filters out events from views so they can be handled by the main
   * window. Filters out DragEnter Drop and ContextMenu events from
   * views.
   *
   * @param[in] watched (QObject *) The object being filtered.
   * @param[in] event (QEvent *) The event that may be filtered.
   */
  bool IpceMainWindow::eventFilter(QObject *watched, QEvent *event) {
    if ( AbstractProjectItemView *view = qobject_cast<AbstractProjectItemView *>(watched) ) {
      if (event->type() == QEvent::DragEnter) {
        return true;
      }
      else if (event->type() == QEvent::Drop) {
        return true;
      }
      else if (event->type() == QEvent::ContextMenu) {
        QMenu contextMenu;

        QList<QAction *> viewActions = view->contextMenuActions();

        if ( !viewActions.isEmpty() ) {
          foreach (QAction *action, viewActions) {
            if (action) {
              contextMenu.addAction(action);
            }
            else {
              contextMenu.addSeparator();
            }
          }
          contextMenu.addSeparator();
        }

        QList<QAction *> workOrders = m_directory->supportedActions( view->currentItem() );

        if ( !workOrders.isEmpty() ) {
          foreach (QAction *action, workOrders) {
            contextMenu.addAction(action);
          }
          contextMenu.addSeparator();
        }

        contextMenu.exec( static_cast<QContextMenuEvent *>(event)->globalPos() );

        return true;
      }
    }

    return QMainWindow::eventFilter(watched, event);
  }


  /**
   * This method takes the max thread count setting and asks
   * QtConcurrent to respect it.
   */
  void IpceMainWindow::applyMaxThreadCount() {
    if (m_maxThreadCount <= 1) {
      // Allow QtConcurrent to use every core and starve the GUI thread
      QThreadPool::globalInstance()->setMaxThreadCount(QThread::idealThreadCount());
    }
    else {
      // subtract 1 to account for the GUI thread
      QThreadPool::globalInstance()->setMaxThreadCount(m_maxThreadCount - 1);
    }
  }


  /**
   * Initializes the internal lists of actions of the main window for
   * use in the menus and toolbars.
   */
  void IpceMainWindow::initializeActions() {
    QAction *exitAction = new QAction("E&xit", this);
    exitAction->setIcon( QIcon::fromTheme("window-close") );
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    m_fileMenuActions.append(exitAction);
    m_permToolBarActions.append(exitAction);

    QAction *saveNet = new QAction("&Save Active Control Network", this);
    saveNet->setIcon( QIcon::fromTheme("document-save") );
    saveNet->setShortcut(Qt::CTRL + Qt::Key_S);
    saveNet->setToolTip("Save current active control network");
    saveNet->setStatusTip("Save current active control network");
    QString whatsThis = "<b>Function:</b> Saves the current active<i>"
                        "control network</i>";
    saveNet->setWhatsThis(whatsThis);
    connect(saveNet, SIGNAL(triggered()), m_directory, SLOT(saveActiveControl()));
    m_permToolBarActions.append(saveNet);

//  m_saveAsNet = new QAction(QPixmap(toolIconDir() + "/mActionFileSaveAs.png"),
//                            "Save Control Network &As...",
//                            m_matchTool);
//  m_saveAsNet->setToolTip("Save current control network to chosen file");
//  m_saveAsNet->setStatusTip("Save current control network to chosen file");
//  whatsThis = "<b>Function:</b> Saves the current <i>"
//      "control network</i> under chosen filename";
//  m_saveAsNet->setWhatsThis(whatsThis);
//  connect(m_saveAsNet, SIGNAL(triggered()), this, SLOT(saveAsNet()));




    QAction *undoAction = m_directory->undoAction();
    undoAction->setShortcut(Qt::Key_Z | Qt::CTRL);

    QAction *redoAction = m_directory->redoAction();
    redoAction->setShortcut(Qt::Key_Z | Qt::CTRL | Qt::SHIFT);

    m_editMenuActions.append(undoAction);
    m_editMenuActions.append(redoAction);


    QAction *viewModeAction = new QAction("Toggle View Mode", this);
    connect(viewModeAction, SIGNAL( triggered() ),
            this, SLOT( toggleViewMode() ) );
    m_viewMenuActions.append(viewModeAction);

    m_cascadeViewsAction = new QAction("Cascade Views", this);
    connect(m_cascadeViewsAction, SIGNAL( triggered() ),
            centralWidget(), SLOT( cascadeSubWindows() ) );
    m_viewMenuActions.append(m_cascadeViewsAction);

    m_tileViewsAction = new QAction("Tile Views", this);
    connect(m_tileViewsAction, SIGNAL( triggered() ),
            centralWidget(), SLOT( tileSubWindows() ) );
    m_viewMenuActions.append(m_tileViewsAction);

    QAction *detachActiveViewAction = new QAction("Detach Active View", this);
    connect(detachActiveViewAction, SIGNAL( triggered() ),
            this, SLOT( detachActiveView() ) );
    m_viewMenuActions.append(detachActiveViewAction);

    QAction *threadLimitAction = new QAction("Set Thread &Limit", this);
    connect(threadLimitAction, SIGNAL(triggered()),
            this, SLOT(configureThreadLimit()));

    m_settingsMenuActions.append(m_directory->project()->userPreferenceActions());
    m_settingsMenuActions.append(threadLimitAction);

    QAction *activateWhatsThisAct = new QAction("&What's This", this);
    activateWhatsThisAct->setShortcut(Qt::SHIFT | Qt::Key_F1);
    activateWhatsThisAct->setIcon(
        QPixmap(FileName("$base/icons/contexthelp.png").expanded()));
    activateWhatsThisAct->setToolTip("Activate What's This and click on parts "
        "this program to see more information about them");
    connect(activateWhatsThisAct, SIGNAL(triggered()), this, SLOT(enterWhatsThisMode()));

    m_helpMenuActions.append(activateWhatsThisAct);

    readSettings(m_directory->project() );
  }


  /**
   * Creates the main menus of the menu bar.
   */
  void IpceMainWindow::createMenus() {
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->setObjectName("fileMenu");

    m_projectMenu = menuBar()->addMenu(tr("&Project"));
    m_projectMenu->setObjectName("projectMenu");

    // Allow tool tips to be displayed for the project menu's actions (e.g. "Bundle Adjustment")
    // This is a work around for Qt's what this text not working on disabled actions
    // (even though the Qt documentation says it should work on disabled QAction's).
    m_projectMenu->setToolTipsVisible(true);

    m_editMenu = menuBar()->addMenu(tr("&Edit"));
    m_editMenu->setObjectName("editMenu");

    m_viewMenu = menuBar()->addMenu("&View");
    m_viewMenu->setObjectName("viewMenu");

    m_settingsMenu = menuBar()->addMenu("&Settings");
    m_settingsMenu->setObjectName("settingsMenu");

    m_helpMenu = menuBar()->addMenu("&Help");
    m_helpMenu->setObjectName("helpMenu");
  }


  /**
   * Write the window positioning and state information out to a
   * config file. This allows us to restore the settings when we
   * create another main window (the next time this program is run).
   *
   * The state will be saved according to the currently loaded project and its name.
   *
   * When no project is loaded (i.e. the default "Project" is open), the config file used is
   * $HOME/.Isis/$APPNAME/$APPNAME_Project.config.
   * When a project, ProjectName, is loaded, the config file used is
   * $HOME/.Isis/$APPNAME/$APPNAME_ProjectName.config.
   *
   * @param[in] project Pointer to the project that is currently loaded (default is "Project")
   *
   * @internal
   *   @history 2016-11-09 Ian Humphrey - Settings are now written according to the loaded project.
   *                           References #4358.
   *   @history 2017-10-17 Tyler Wilson Added a [recent projects] group for the saving and
   *                           restoring of recently opened projects.  References #4492.
   */
  void IpceMainWindow::writeSettings(const Project *project) const {

    // Ensure that we are not using a NULL pointer
    if (!project) {
      QString msg = "Cannot write settings with a NULL Project pointer.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    QString appName = QApplication::applicationName();
    QSettings projectSettings(
        FileName("$HOME/.Isis/" + appName + "/" + appName + "_" + project->name() + ".config")
          .expanded(),
        QSettings::NativeFormat);

    QSettings globalSettings(
        FileName("$HOME/.Isis/" + appName + "/" + appName + "_" + "Project.config")
          .expanded(),
        QSettings::NativeFormat);

    projectSettings.setValue("geometry", saveGeometry());
    projectSettings.setValue("windowState", saveState());
    projectSettings.setValue("size", size());
    projectSettings.setValue("pos", pos());

    projectSettings.setValue("maxThreadCount", m_maxThreadCount);

    globalSettings.setValue("maxThreadCount", m_maxThreadCount);
    globalSettings.setValue("maxRecentProjects",m_maxRecentProjects);

    globalSettings.beginGroup("recent_projects");
    QStringList keys = globalSettings.allKeys();
    QMap<QString,QString> recentProjects;

    foreach (QString key,keys) {
      recentProjects[key]=globalSettings.value(key).toString();
    }

    QList<QString> projectPaths = recentProjects.values();

    if (keys.count() >= m_maxRecentProjects) {

      //Clear out the recent projects before repopulating this group
      globalSettings.remove("");

      //If the currently open project is a project that has been saved and is not within the current
      //list of recently open projects, then remove the oldest project from the list.
      if (!project->projectRoot().contains("tmpProject") &&
          !projectPaths.contains(project->projectRoot()) ) {
        QString s=keys.first();
        recentProjects.remove( s );
      }

      //If the currently open project is already contained within the list,
      //then remove the earlier reference.
      if (projectPaths.contains(project->projectRoot())) {
        QString key = recentProjects.key(project->projectRoot());
        recentProjects.remove(key);

      }

      QMap<QString,QString>::iterator i;

      //Iterate through the recentProjects QMap and set the <key,val> pairs.
      for (i=recentProjects.begin();i!=recentProjects.end();i++) {
          globalSettings.setValue(i.key(),i.value());
      }

      //Get a unique time value for generating a key
      long t0 = QDateTime::currentMSecsSinceEpoch();

      QString projName = project->name();
      QString t0String=QString::number(t0);

      //Save the project location
      if (!project->projectRoot().contains("tmpProject") ) {
              globalSettings.setValue(t0String+"%%%%%"+projName,project->projectRoot());
      }
    }

    //The numer of recent open projects is less than m_maxRecentProjects
    else {

      long t0 = QDateTime::currentMSecsSinceEpoch();
      QString projName = project->name();
      QString t0String=QString::number(t0);

      if (!project->projectRoot().contains("tmpProject") &&
          !projectPaths.contains( project->projectRoot())) {
        globalSettings.setValue(t0String+"%%%%%"+projName,project->projectRoot());
      }
    }
    globalSettings.endGroup();
  }


  /**
   * Read the window positioning and state information from the config file.
   *
   * When running ipce without opening a project, the config file read is
   * $HOME/.Isis/$APPNAME/$APPNAME_Project.config
   * Otherwise, when running ipce and opening a project (ProjectName), the config file read is
   * $HOME/.Isis/$APPNAME/$APPNAME_ProjectName.config
   *
   * @param[in] project (Project *) The project that was loaded.
   *
   * @internal
   *   @history Ian Humphrey - Settings are now read on a project name basis. References #4358.
   *   @history Tyler Wilson 2017-11-02 - Settings now read recent projects.  References #4492.
   *   @history Tyler Wilson 2017-11-13 - Commented out a resize call near the end because it
   *                                      was messing with the positions of widgets after a
   *                                      project was loaded.  Fixes #5075.
   */
  void IpceMainWindow::readSettings(Project *project) {
    // Ensure that the Project pointer is not NULL
    if (!project) {
      QString msg = "Cannot read settings with a NULL Project pointer.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    if (project->name() == "Project") {
      setWindowTitle("ipce");
    }
    else {
      setWindowTitle( project->name() );
      QString projName = project->name();
      setWindowTitle(projName );
    }
    QString appName = QApplication::applicationName();

    QSettings settings(
        FileName("$HOME/.Isis/" + appName + "/" + appName + "_" + project->name() + ".config")
        .expanded(), QSettings::NativeFormat);

    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    QStringList projectNameList;
    QStringList projectPathList;
    settings.beginGroup("recent_projects");
    QStringList keys = settings.allKeys();

    QRegExp underscore("%%%%%");

    foreach (QString key, keys) {
      QString childKey = "recent_projects/"+key;
      QString projectPath = settings.value(key).toString();
      QString projectName = projectPath.split("/").last();
      projectPathList.append(projectPath) ;
      projectNameList.append(projectName);
    }

    settings.endGroup();

    QStringList projectPathReverseList;

    for (int i = projectPathList.count()-1;i>=0;i--) {
      projectPathReverseList.append(projectPathList[i]);
    }

    QStringList projectPathListTruncated;

    int i =0;

    foreach (QString proj,projectPathReverseList) {
      if (i <= m_maxRecentProjects) {
        projectPathListTruncated.append(proj);
        i++;
      }
      else
        break;
     }


    m_directory->setRecentProjectsList(projectPathListTruncated);
    m_directory->updateRecentProjects();

    // The geom/state isn't enough for main windows to correctly remember
    //   their position and size, so let's restore those on top of
    //   the geom and state.
    if (!settings.value("pos").toPoint().isNull())
      move(settings.value("pos").toPoint());

    m_maxThreadCount = settings.value("maxThreadCount", m_maxThreadCount).toInt();
    applyMaxThreadCount();

  }


  /**
   * Handle the close event by writing the window positioning and
   * state information before forwarding the event to the QMainWindow.
   */
  void IpceMainWindow::closeEvent(QCloseEvent *event) {

    // The active control is checked here for modification because this was the simplest solution
    // vs changing the project clean state every time the control is modified or saved.
    if (!m_directory->project()->isClean() || (m_directory->project()->activeControl() &&
                                               m_directory->project()->activeControl()->isModified())) {
      QMessageBox *box = new QMessageBox(QMessageBox::NoIcon, QString("Current Project Has Unsaved Changes"),
                             QString("Would you like to save your current project?"),
                             NULL, qobject_cast<QWidget *>(parent()), Qt::Dialog);
      QPushButton *save = box->addButton("Save", QMessageBox::AcceptRole);
      box->addButton("Don't Save", QMessageBox::RejectRole);
      QPushButton *cancel = box->addButton("Cancel", QMessageBox::NoRole);
      box->exec();

      if (box->clickedButton() == (QAbstractButton*)cancel) {
        event->ignore();
        return;
      }
      else if (box->clickedButton() == (QAbstractButton*)save) {
        m_directory->project()->save();
      }
    }
    writeSettings(m_directory->project());
    m_directory->project()->clear();

    QMainWindow::closeEvent(event);
  }


  /**
   * Ask the user how many threads to use in this program. This
   * includes the GUI thread.
   */
  void IpceMainWindow::configureThreadLimit() {
    bool ok = false;

    QStringList options;

    int current = 0;
    options << tr("Use all available");

    for(int i = 1; i < 24; i++) {
      QString option = tr("Use %1 threads").arg(i + 1);

      options << option;
      if(m_maxThreadCount == i + 1)
        current = i;
    }

    QString res = QInputDialog::getItem(NULL, tr("Concurrency"),
        tr("Set the number of threads to use"),
        options, current, false, &ok);

    if (ok) {
      m_maxThreadCount = options.indexOf(res) + 1;

      if (m_maxThreadCount <= 1)
        m_maxThreadCount = -1;

      applyMaxThreadCount();
    }
  }


  /**
   * Activate the What's This? cursor. This is useful for he What's
   * This? action in the help menu.
   */
  void IpceMainWindow::enterWhatsThisMode() {
    QWhatsThis::enterWhatsThisMode();
  }


  /**
   * Slot to connect to the subWindowActivated signal from the central
   * QMdiArea. Updates the active view to the active sub window, or
   * sets it to null if the active window is not an AbstractProjectItemView.
   *
   * @param[in] window (QMdiSubWindow *) The active sub window.
   */
  void IpceMainWindow::onSubWindowActivated(QMdiSubWindow * window) {
    if (window) {
      setActiveView( qobject_cast<AbstractProjectItemView *>( window->widget() ) );
    }
    else {
      setActiveView(0);
    }
  }


  /**
   * Toggles the view mode of the central QMdiArea between tabbed and
   * sub window mode.
   */
  void IpceMainWindow::toggleViewMode() {
    QMdiArea *mdiArea = qobject_cast<QMdiArea *>( centralWidget() );
    if (mdiArea->viewMode() == QMdiArea::SubWindowView) {
      setTabbedViewMode();
    }
    else {
      setSubWindowViewMode();
    }
  }


  /**
   * Sets the QMdiArea in the central widget to the tabbed view mode
   * and updates the appropriate actions.
   */
  void IpceMainWindow::setTabbedViewMode() {
    QMdiArea *mdiArea = qobject_cast<QMdiArea *>( centralWidget() );
    mdiArea->setViewMode(QMdiArea::TabbedView);
    m_cascadeViewsAction->setEnabled(false);
    m_tileViewsAction->setEnabled(false);
  }


  /**
   * Sets the QMdiArea in the central widget to the sub window view
   * mode and updates the appropriate actions.
   */
  void IpceMainWindow::setSubWindowViewMode() {
    QMdiArea *mdiArea = qobject_cast<QMdiArea *>( centralWidget() );
    mdiArea->setViewMode(QMdiArea::SubWindowView);
    m_cascadeViewsAction->setEnabled(true);
    m_tileViewsAction->setEnabled(true);
  }

  /**
   * Closes the detached window and removes it from the m_detachedViews list
   */
  void IpceMainWindow::closeDetachedView() {

    ViewSubWindow *viewWindow = qobject_cast<ViewSubWindow *>( sender() );

    if (!viewWindow) {
      return;
    }

    m_detachedViews.removeAt(m_detachedViews.indexOf(viewWindow));
    viewWindow->close();
  }


  /**
   * Moves the active view from the mdi area to its own independent
   * window. The view, its toolbars, and menu actions, are removed
   * from the main window and placed in an independent
   * QMainWindow. The new window contains the view as well as its
   * toolbars and menu actions. A detached view will not be set as the
   * active view when it is activated.
   */
  void IpceMainWindow::detachActiveView() {
    AbstractProjectItemView *view = m_activeView;

    if (!m_activeView) {
      return;
    }

    QMdiArea *mdiArea = qobject_cast<QMdiArea *>( centralWidget() );
    if (mdiArea) {
      mdiArea->removeSubWindow(view);
      mdiArea->closeActiveSubWindow();
    }

    ViewSubWindow *newWindow = new ViewSubWindow(this, Qt::Window);

    connect( newWindow, SIGNAL( closeWindow() ),
             this, SLOT( closeDetachedView() ) );

    connect( newWindow, SIGNAL( closeWindow() ),
             view, SLOT( deleteLater() ) );

    m_detachedViews.append(newWindow);
    newWindow->setCentralWidget(view);
    newWindow->setWindowTitle( view->windowTitle() );

    if ( !view->permToolBarActions().isEmpty() ) {
      QToolBar *permToolBar = new QToolBar(newWindow);
      foreach ( QAction *action, view->permToolBarActions() ) {
        permToolBar->addAction(action);
      }
      newWindow->addToolBar(permToolBar);
    }

    if ( !view->activeToolBarActions().isEmpty() ) {
      QToolBar *activeToolBar = new QToolBar(newWindow);
      foreach ( QAction *action, view->activeToolBarActions() ) {
        activeToolBar->addAction(action);
      }
      newWindow->addToolBar(activeToolBar);
    }

    if ( !view->toolPadActions().isEmpty() ) {
      QToolBar *toolPad = new QToolBar(newWindow);
      foreach ( QAction *action, view->toolPadActions() ) {
        toolPad->addAction(action);
      }
      newWindow->addToolBar(Qt::RightToolBarArea, toolPad);
    }

    QMenuBar *menuBar = new QMenuBar(newWindow);
    newWindow->setMenuBar(menuBar);

    if ( !view->fileMenuActions().isEmpty() ) {
      QMenu *fileMenu = new QMenu("&File", newWindow);
      foreach ( QAction *action, view->fileMenuActions() ) {
        fileMenu->addAction(action);
      }
      menuBar->addMenu(fileMenu);
    }

    if ( !view->projectMenuActions().isEmpty() ) {
      QMenu *projectMenu = new QMenu("&Project", newWindow);
      foreach ( QAction *action, view->projectMenuActions() ) {
        projectMenu->addAction(action);
      }
      menuBar->addMenu(projectMenu);
    }

    if ( !view->editMenuActions().isEmpty() ) {
      QMenu *editMenu = new QMenu("&Edit", newWindow);
      foreach ( QAction *action, view->editMenuActions() ) {
        editMenu->addAction(action);
      }
      menuBar->addMenu(editMenu);
    }

    QAction *reattachAction = new QAction("Reattach View", newWindow);
    connect( reattachAction, SIGNAL( triggered() ),
             this, SLOT( reattachView() ) );

    QMenu *viewMenu = new QMenu("&View", newWindow);

    viewMenu->addAction(reattachAction);

    if ( !view->viewMenuActions().isEmpty() ) {
      foreach ( QAction *action, view->viewMenuActions() ) {
        viewMenu->addAction(action);
      }
    }
    menuBar->addMenu(viewMenu);

    if ( !view->settingsMenuActions().isEmpty() ) {
      QMenu *settingsMenu = new QMenu("S&ettings", newWindow);
      foreach ( QAction *action, view->settingsMenuActions() ) {
        settingsMenu->addAction(action);
      }
      menuBar->addMenu(settingsMenu);
    }

    if ( !view->helpMenuActions().isEmpty() ) {
      QMenu *helpMenu = new QMenu("&Help", newWindow);
      foreach ( QAction *action, view->helpMenuActions() ) {
        helpMenu->addAction(action);
      }
      menuBar->addMenu(helpMenu);
    }
    newWindow->show();
  }


  /**
   * Reattaches a detached view. This slot can only be called by a QAction
   * from a QMainWindow that contains the view. The view is added to
   * the main window and the window that previously contained it is
   * deleted.
   */
  void IpceMainWindow::reattachView() {
    QAction *reattachAction = qobject_cast<QAction *>( sender() );
    if (!reattachAction) {
      return;
    }

    QMainWindow *viewWindow = qobject_cast<QMainWindow *>( reattachAction->parent() );
    if (!viewWindow) {
      return;
    }

    AbstractProjectItemView *view = qobject_cast<AbstractProjectItemView *>( viewWindow->centralWidget() );
    if (!view) {
      return;
    }

    view->setParent(this);
    viewWindow->deleteLater();

    addView(view);
  }


/**
 * Raises the warningWidget to the front of the tabs. Connected to warning signal from directory.
 */
  void IpceMainWindow::raiseWarningTab() {
    m_warningsDock->raise();
  }

}
