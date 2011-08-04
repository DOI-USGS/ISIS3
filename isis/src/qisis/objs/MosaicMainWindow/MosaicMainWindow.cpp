#include "MosaicMainWindow.h"

#include <QDockWidget>
#include <QMenu>
#include <QSettings>

#include "Camera.h"
#include "FileDialog.h"
#include "MosaicController.h"
#include "MosaicFileListWidget.h"
#include "MosaicSceneWidget.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "TextFile.h"
#include "ToolPad.h"


namespace Isis {
  MosaicMainWindow::MosaicMainWindow(QString title, QWidget *parent) :
      MainWindow(title, parent),
      p_settings(Filename("$HOME/.Isis/qmos/qmos.config").Expanded().c_str(),
                 QSettings::NativeFormat) {
    p_filename = "";
    p_fileMenu = NULL;
    p_settingsMenu = NULL;
    p_viewMenu = NULL;

    m_controllerVisible = false;

    setWindowTitle(title);

    p_permToolbar = new QToolBar("Standard Tools", this);
    p_permToolbar->setObjectName("Standard Tools");
    addToolBar(p_permToolbar);

    p_activeToolbar = new QToolBar("Active Tool", this);
    p_activeToolbar->setObjectName("Active Tool");
    addToolBar(p_activeToolbar);

    QStatusBar *sbar = statusBar();
    sbar->showMessage("Ready");

    p_toolpad = new ToolPad("Tool Pad", this);
    p_toolpad->setObjectName("Tool Pad");
    // default to the right hand side for qview-like behavior... we might
    //   want to do something different here
    addToolBar(Qt::RightToolBarArea, p_toolpad);

    p_progressBar = new QProgressBar(parent);
    p_progressBar->setOrientation(Qt::Horizontal);
    sbar->addWidget(p_progressBar);
    setupMenus();

    p_fileListDock = new QDockWidget("File List", this, Qt::SubWindow);
    p_fileListDock->setObjectName("FileListDock");
    p_fileListDock->setFeatures(QDockWidget::DockWidgetFloatable |
                                QDockWidget::DockWidgetMovable |
                                QDockWidget::DockWidgetClosable);

    p_mosaicPreviewDock = new QDockWidget("Mosaic World View",
                                          this, Qt::SubWindow);
    p_mosaicPreviewDock->setObjectName("MosaicPreviewDock");
    p_mosaicPreviewDock->setFeatures(QDockWidget::DockWidgetFloatable |
                                     QDockWidget::DockWidgetMovable |
                                     QDockWidget::DockWidgetClosable);

    addDockWidget(Qt::LeftDockWidgetArea, p_fileListDock);
    addDockWidget(Qt::LeftDockWidgetArea, p_mosaicPreviewDock);

    readSettings();

    setCentralWidget(new QWidget());
    centralWidget()->setLayout(new QHBoxLayout());

    p_mosaicController = NULL;
    createController();
    installEventFilter(this);
  }


  /**
   * This event filter is installed in the constructor.
   * @param o
   * @param e
   *
   * @return bool
   */
  bool MosaicMainWindow::eventFilter(QObject *o, QEvent *e) {
    switch(e->type()) {
      case QEvent::Close:
        saveSettings2();

      default:
        return false;
    }
  }


  /**
   * Sets up the menus on the menu bar for the qmos window.
   *
   * This should be the job of the widgets this application uses... inheritance
   *   off of something that says it has menu options probably. This should
   *   probably have open project, save project, close project, and exit.
   *   Projects need to be an accumulated file also, if we want them.
   */
  void MosaicMainWindow::setupMenus() {
    // Create the file menu
    p_fileMenu = menuBar()->addMenu("&File");

    iString iconDir = Filename("$base/icons").Expanded();

    QAction *open = new QAction(this);
    open->setText("Open Cube...");
    open->setIcon(QPixmap(QString::fromStdString(iconDir.c_str()) + "/fileopen.png"));
    connect(open, SIGNAL(activated()), this, SLOT(open()));

    QAction *openList = new QAction(this);
    openList->setText("Open Cube List...");
    openList->setIcon(QPixmap(QString::fromStdString(iconDir.c_str()) + "/mActionHelpContents.png"));
    connect(openList, SIGNAL(activated()), this, SLOT(openList()));

    QAction *saveProject = new QAction(this);
    saveProject->setText("Save Project");
    saveProject->setShortcut(Qt::CTRL + Qt::Key_S);
    saveProject->setIcon(QPixmap(QString::fromStdString(iconDir.c_str()) + "/mActionFileSave.png"));
    p_actionsRequiringOpen.append(saveProject);
    connect(saveProject, SIGNAL(activated()), this, SLOT(saveProject()));

    QAction *saveProjectAs = new QAction(this);
    saveProjectAs->setText("Save Project As...");
    saveProjectAs->setIcon(QPixmap(QString::fromStdString(iconDir.c_str()) + "/mActionFileSaveAs.png"));
    p_actionsRequiringOpen.append(saveProjectAs);
    connect(saveProjectAs, SIGNAL(activated()), this, SLOT(saveProjectAs()));

    QAction *loadProject = new QAction(this);
    loadProject->setText("Load Project...");
    loadProject->setIcon(QPixmap(QString::fromStdString(iconDir.c_str()) + "/mActionExportMapServer.png"));
    p_actionsRequiringClosed.append(loadProject);
    connect(loadProject, SIGNAL(activated()), this, SLOT(loadProject()));
    

    QAction *closeProject = new QAction(this);
    closeProject->setText("Close Project");
    p_actionsRequiringOpen.append(closeProject);
    connect(closeProject, SIGNAL(activated()), this, SLOT(closeMosaic()));

    QAction *exit = new QAction(this);
    exit->setText("Exit");
    connect(exit, SIGNAL(activated()), this, SLOT(close()));

    QAction *actionRequiringOpen = NULL;
    foreach(actionRequiringOpen, p_actionsRequiringOpen) {
      actionRequiringOpen->setEnabled(false);
    }

    QAction *actionRequiringClosed = NULL;
    foreach(actionRequiringClosed, p_actionsRequiringClosed) {
      actionRequiringClosed->setEnabled(true);
    }

    p_fileMenu->addAction(open);
    p_fileMenu->addAction(openList);
    p_fileMenu->addSeparator();
    p_fileMenu->addAction(loadProject);
    p_fileMenu->addAction(saveProject);
    p_fileMenu->addAction(saveProjectAs);
    p_fileMenu->addAction(closeProject);
    p_fileMenu->addSeparator();
    p_exportMenu = p_fileMenu->addMenu("&Export");
    p_fileMenu->addAction(exit);

    permanentToolBar()->addAction(loadProject);
    permanentToolBar()->addAction(saveProject);
    permanentToolBar()->addAction(saveProjectAs);
    permanentToolBar()->addSeparator();
    permanentToolBar()->addAction(open);
    permanentToolBar()->addAction(openList);
    permanentToolBar()->addSeparator();

    p_viewMenu = menuBar()->addMenu("&View");
    p_settingsMenu = menuBar()->addMenu("&Settings");

    updateMenuVisibility();
  }


  /**
   * Calles MosaicWidget's open method which opens a cube file and
   * displays the footprint in the graphics view.
   *
   */
  void MosaicMainWindow::open() {
    QStringList filterList;
    filterList.append("Isis cubes (*.cub)");
    filterList.append("All Files (*)");

    QDir directory = p_lastOpenedFile.dir();

    QStringList selected = QFileDialog::getOpenFileNames(this, "Open Cubes",
        directory.path(), filterList.join(";;"));

    if(!selected.empty()) {
      openFiles(selected);
    }
  }


  void MosaicMainWindow::updateMenuVisibility() {
    QMenuBar *rootMenu = menuBar();

    QAction *rootAction = NULL;
    foreach(rootAction, rootMenu->actions()) {
      QMenu *rootMenu = rootAction->menu();

      if(rootMenu) {
        rootAction->setVisible(updateMenuVisibility(rootAction->menu()));
      }
    }
  }


  void MosaicMainWindow::createController() {
    if(p_mosaicController == NULL) {
      p_mosaicController = new MosaicController(statusBar(), p_settings);
      
      
      QList<QAction *> settingsActs = p_mosaicController->getSettingsActions();
      
      QAction *settingsAct;
      foreach(settingsAct, settingsActs) {
        connect(settingsAct, SIGNAL(destroyed(QObject *)),
                this, SLOT(updateMenuVisibility()));

        p_settingsMenu->addAction(settingsAct);
      }

      updateMenuVisibility();
    }
  }


  void MosaicMainWindow::displayController() {
    createController();
    
    if(p_mosaicController && !m_controllerVisible) {
      m_controllerVisible = true;
      p_mosaicController->addExportActions(*p_exportMenu);

      p_fileListDock->setWidget(p_mosaicController->getMosaicFileList());
      p_mosaicPreviewDock->setWidget(p_mosaicController->getMosaicWorldScene());

      centralWidget()->layout()->addWidget(
          p_mosaicController->getMosaicScene());

      QAction *actionRequiringOpen = NULL;
      foreach(actionRequiringOpen, p_actionsRequiringOpen) {
        actionRequiringOpen->setEnabled(true);
      }

      QAction *actionRequiringClosed = NULL;
      foreach(actionRequiringClosed, p_actionsRequiringClosed) {
        actionRequiringClosed->setEnabled(false);
      }

      p_mosaicController->getMosaicScene()->addTo(p_toolpad);
      p_mosaicController->getMosaicScene()->addToPermanent(p_permToolbar);
      p_mosaicController->getMosaicScene()->addTo(p_activeToolbar);

      statusBar()->addWidget(p_mosaicController->getProgress());
      statusBar()->addWidget(
          p_mosaicController->getMosaicScene()->getProgress());
      statusBar()->addWidget(
          p_mosaicController->getMosaicWorldScene()->getProgress());
      statusBar()->addWidget(
          p_mosaicController->getMosaicFileList()->getProgress());

      QList<QAction *> viewActs =
          p_mosaicController->getMosaicFileList()->getViewActions();

      QAction *viewAct;
      foreach(viewAct, viewActs) {
        connect(viewAct, SIGNAL(destroyed(QObject *)),
                this, SLOT(updateMenuVisibility()));

        p_viewMenu->addAction(viewAct);
      }

      updateMenuVisibility();
    }
  }


  bool MosaicMainWindow::updateMenuVisibility(QMenu *menu) {
    bool anythingVisible = false;

    if(menu) {
      QList<QAction *> actions = menu->actions();

      // Recursively search the menu for other menus to show or hide and handle
      //   every internal being invisible
      QAction *menuAction = NULL;
      foreach(menuAction, menu->actions()) {
        bool thisVisible = true;

        if(menuAction->menu() != NULL) {
          thisVisible = updateMenuVisibility(menuAction->menu());
        }
        else {
          thisVisible = menuAction->isVisible();
        }

        if(thisVisible)
          anythingVisible = true;
      }

      menu->menuAction()->setVisible(anythingVisible);
    }

    return anythingVisible;
  }


  /**
   * Opens a list of cube files instead of one at a time.
   *
   */
  void MosaicMainWindow::openList() {
    // Set up the list of filters that are default with this dialog.
    QStringList filterList;
    filterList.append("List Files (*.lis)");
    filterList.append("Text Files (*.txt)");
    filterList.append("All files (*)");

    QDir directory = p_lastOpenedFile.dir();

    QString selected = QFileDialog::getOpenFileName(this, "Open Cube List",
        directory.path(), filterList.join(";;"));

    if(selected != "") {
      TextFile fileList((iString) selected);

      QStringList filesInList;
      iString line;

      while(fileList.GetLine(line)) {
        filesInList.append(line);
      }

      if(filesInList.empty()) {
        iString msg = "No files were found inside the file list";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
      }

      openFiles(filesInList);
    }
  }


  /**
   * This overriden method is called from the constructor so that
   * when the Mosaicmainwindow is created, it know's it's size
   * and location and the tool bar location.
   *
   */
  void MosaicMainWindow::readSettings() {
    // Call the base class function to read the size and location
    MainWindow::readSettings();

    p_settings.beginGroup("MosaicMainWindow");
    QByteArray state = p_settings.value("state", QByteArray("0")).toByteArray();
    restoreState(state);

    // load window position and size
    QPoint pos = p_settings.value("pos", QPoint(100, 100)).toPoint();
    QSize size = p_settings.value("size", QSize(750,
        600)).toSize();
    resize(size);
    move(pos);

    p_settings.endGroup();
  }


  void MosaicMainWindow::openFiles(QStringList cubeNames) {
    // Create a mosaic widget if we don't have one
    if(!cubeNames.empty())
      displayController();

    if(p_mosaicController)
      p_mosaicController->openCubes(cubeNames);
  }


  /**
   * This overriden method is called when the MosaicMainWindow
   * is closed or hidden to write the size and location settings
   * (and tool bar location) to a config file in the user's home
   * directory.
   *
   */
  void MosaicMainWindow::saveSettings2() {
    // Now write the settings that are specific to this window.
    p_settings.beginGroup("MosaicMainWindow");
    p_settings.setValue("state", saveState());

    p_settings.setValue("pos", pos());
    p_settings.setValue("size", size());

    p_settings.endGroup();
    closeMosaic();
  }


  /**
   * Allows the user to save a project file.
   */
  void MosaicMainWindow::saveProjectAs() {
    if(p_mosaicController) {
      QString fn =  QFileDialog::getSaveFileName(this, "Save Project",
                    QDir::currentPath() + "/untitled.mos",
                    "Mosaic (*.mos)");
      if(fn.isEmpty()) return;

      p_mosaicController->saveProject(fn);
      p_filename = fn;
    }
  }


  /**
   * Called from the file menu to save a project file.
   *
   */
  void MosaicMainWindow::saveProject() {
    if(p_filename == "") {
      saveProjectAs();
    }
    else {
      p_mosaicController->saveProject(p_filename);
    }
  }


  /**
   * Allows users to select a project which is then read in and
   * displayed in the qmos window.
   *
   */
  void MosaicMainWindow::loadProject() {
    if(!m_controllerVisible) {
      QString fn =  QFileDialog::getOpenFileName(this, "Load Project",
                    QDir::currentPath(),
                    "Mosaic (*.mos)");
      loadProject(fn);
    }
  }


  void MosaicMainWindow::loadProject(QString fn) {
    if(!fn.isEmpty()) {
      displayController();

      if(p_mosaicController)
        p_mosaicController->readProject(fn);

      p_filename = fn;
    }
  }


  void MosaicMainWindow::closeMosaic() {
    if(p_mosaicController) {
      QAction *actionRequiringOpen = NULL;
      foreach(actionRequiringOpen, p_actionsRequiringOpen) {
        actionRequiringOpen->setEnabled(false);
      }

      QAction *actionRequiringClosed = NULL;
      foreach(actionRequiringClosed, p_actionsRequiringClosed) {
        actionRequiringClosed->setEnabled(true);
      }

      p_mosaicController->saveSettings(p_settings);
      delete p_mosaicController;
      p_mosaicController = NULL;

      p_filename = "";
      m_controllerVisible = false;
    }

    // Create a non-visible controller... so we have things like the settings
    //   menu
    createController();
  }
}

