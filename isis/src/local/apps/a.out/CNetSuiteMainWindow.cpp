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
#include "CNetSuiteMainWindow.h"

#include <QtGui>

#include "Directory.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "MosaicSceneWidget.h"
#include "ProgressWidget.h"
#include "Project.h"


namespace Isis {
  /**
   * Construct the main window. This will create a Directory, the menus, and the dock areas.
   *
   * @param parent The Qt-relationship parent widget (usually NULL in this case)
   */
  CNetSuiteMainWindow::CNetSuiteMainWindow(QWidget *parent) :
      QMainWindow(parent) {
    m_maxThreadCount = -1;

    QWidget *centralWidget = new QWidget;
//  centralWidget->hide();
//  QTabWidget *centralWidget = new QTabWidget;
//  centralWidget->setTabsClosable(true);
//  centralWidget->setMovable(true);


//  connect(centralWidget, SIGNAL(tabCloseRequested(int)),
//          this, SLOT(removeCentralWidgetTab(int)));

    setCentralWidget(centralWidget);
    setDockNestingEnabled(true);

    try {
      m_directory = new Directory(this);
      connect(m_directory,
              SIGNAL(newWidgetAvailable(QWidget *, Qt::DockWidgetArea, Qt::Orientation)),
              this,
              SLOT(addDock(QWidget *, Qt::DockWidgetArea, Qt::Orientation)));
    }
    catch (IException &e) {
      throw IException(e, IException::Programmer,
          "Could not create Directory.", _FILEINFO_);
    }

    createMenus();

    QDockWidget *projectDock = new QDockWidget("Project", this, Qt::SubWindow);
    projectDock->setObjectName("projectDock");
    projectDock->setFeatures(QDockWidget::DockWidgetMovable |
                              QDockWidget::DockWidgetFloatable);
    projectDock->setWidget(m_directory->projectTreeWidget());
    addDockWidget(Qt::LeftDockWidgetArea, projectDock);

    QDockWidget *warningsDock = new QDockWidget("Warnings", this, Qt::SubWindow);
    warningsDock->setObjectName("warningsDock");
    warningsDock->setFeatures(QDockWidget::DockWidgetClosable |
                         QDockWidget::DockWidgetMovable |
                         QDockWidget::DockWidgetFloatable);
    warningsDock->setWhatsThis(tr("This shows notices and warnings from all operations "
                          "on the current project."));
    m_directory->setWarningContainer(warningsDock);
    addDockWidget(Qt::BottomDockWidgetArea, warningsDock);

    QDockWidget *historyDock = new QDockWidget("History", this, Qt::SubWindow);
    historyDock->setObjectName("historyDock");
    historyDock->setFeatures(QDockWidget::DockWidgetClosable |
                         QDockWidget::DockWidgetMovable |
                         QDockWidget::DockWidgetFloatable);
    historyDock->setWhatsThis(tr("This shows all operations performed on the current project."));
    addDockWidget(Qt::BottomDockWidgetArea, historyDock);
    m_directory->setHistoryContainer(historyDock);
    tabifyDockWidget(warningsDock, historyDock);

    QDockWidget *progressDock = new QDockWidget("Progress", this, Qt::SubWindow);
    progressDock->setObjectName("progressDock");
    progressDock->setFeatures(QDockWidget::DockWidgetClosable |
                         QDockWidget::DockWidgetMovable |
                         QDockWidget::DockWidgetFloatable);
    //m_directory->setProgressContainer(progressDock);
    addDockWidget(Qt::BottomDockWidgetArea, progressDock);
    tabifyDockWidget(historyDock, progressDock);

    warningsDock->raise();

    readSettings();

    statusBar()->showMessage("Ready");
    statusBar()->addWidget(m_directory->project()->progress());

    foreach (QProgressBar *progressBar, m_directory->progressBars()) {
      statusBar()->addWidget(progressBar);
    }
  }


  void CNetSuiteMainWindow::addDock(QWidget *newWidgetForDock, Qt::DockWidgetArea area,
                                    Qt::Orientation orientation) {

    QDockWidget *dock = new QDockWidget(newWidgetForDock->windowTitle());
    dock->setWidget(newWidgetForDock);
    dock->setObjectName(newWidgetForDock->objectName());

    // This needs to eventually be a work order...
    dock->setAttribute(Qt::WA_DeleteOnClose);

    connect(newWidgetForDock, SIGNAL(destroyed(QObject *)),
            dock, SLOT(deleteLater()));

    addDockWidget(area, dock, orientation);
  }


  /**
   * Cleans up the directory.
   */
  CNetSuiteMainWindow::~CNetSuiteMainWindow() {
    delete m_directory;
  }


  /**
   * This method takes the max thread count setting and asks QtConcurrent to respect it.
   */
  void CNetSuiteMainWindow::applyMaxThreadCount() {
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
   * Create the main menus. This will ask the directory to populate the menu.
   */
  void CNetSuiteMainWindow::createMenus() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->setObjectName("fileMenu");

    QMenu *projectMenu = menuBar()->addMenu(tr("&Project"));
    projectMenu->setObjectName("projectMenu");

    m_directory->populateMainMenu(menuBar());

    QAction *exitAction = fileMenu->addAction(tr("E&xit"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));


    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->setObjectName("editMenu");

    QAction *undoAction = m_directory->undoAction();
    undoAction->setShortcut(Qt::Key_Z | Qt::CTRL);
    editMenu->addAction(undoAction);

    QAction *redoAction = m_directory->redoAction();
    redoAction->setShortcut(Qt::Key_Z | Qt::CTRL | Qt::SHIFT);
    editMenu->addAction(redoAction);

    QMenu *settingsMenu = menuBar()->addMenu("&Settings");
    settingsMenu->setObjectName("settingsMenu");
    settingsMenu->addActions(m_directory->project()->userPreferenceActions());

    QAction *threadLimitAction = new QAction("Set Thread &Limit", this);
    connect(threadLimitAction, SIGNAL(triggered()),
            this, SLOT(configureThreadLimit()));
    settingsMenu->addAction(threadLimitAction);

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->setObjectName("helpMenu");

    QAction *activateWhatsThisAct = new QAction("&What's This", this);
    activateWhatsThisAct->setShortcut(Qt::SHIFT | Qt::Key_F1);
    activateWhatsThisAct->setIcon(
        QPixmap(FileName("$base/icons/contexthelp.png").expanded()));
    activateWhatsThisAct->setToolTip("Activate What's This and click on parts "
        "this program to see more information about them");
    connect(activateWhatsThisAct, SIGNAL(activated()),
            this, SLOT(enterWhatsThisMode()));

    helpMenu->addAction(activateWhatsThisAct);
  }


  /**
   * Write the window positioning and state information out to a config file. This allows us to
   *   restore the settings when we create another main window (the next time this program is run).
   *
   * The config file used is $HOME/.Isis/$APPNAME/$APPNAME.config
   */
  void CNetSuiteMainWindow::writeSettings() {
    QString appName = QApplication::applicationName();
    QSettings settings(
        FileName("$HOME/.Isis/" + appName + "/" + appName + ".config")
          .expanded(),
        QSettings::NativeFormat);

    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    settings.setValue("maxThreadCount", m_maxThreadCount);
  }


  /**
   * Read the window positioning and state information from the config file.
   *
   * The config file read is $HOME/.Isis/$APPNAME/$APPNAME.config
   */
  void CNetSuiteMainWindow::readSettings() {
    QString appName = QApplication::applicationName();
    QSettings settings(
        FileName("$HOME/.Isis/" + appName + "/" + appName + ".config")
          .expanded(),
        QSettings::NativeFormat);

    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    m_maxThreadCount = settings.value("maxThreadCount", m_maxThreadCount).toInt();
    applyMaxThreadCount();
  }


  /**
   * Handle the close event by writing the window positioning and state information before
   *   forwarding the event to the QMainWindow.
   */
  void CNetSuiteMainWindow::closeEvent(QCloseEvent *event) {
    writeSettings();
    QMainWindow::closeEvent(event);
  }


  /**
   * Ask te user how many threads to use in this program. This includes the GUI thread.
   */
  void CNetSuiteMainWindow::configureThreadLimit() {
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
   * Activate the What's This? cursor. This is useful for the What's This? action in the help menu.
   */
  void CNetSuiteMainWindow::enterWhatsThisMode() {
    QWhatsThis::enterWhatsThisMode();
  }


  /**
   * Close the tab at index. This requires that the central widget is a tab widget.
   */
  void CNetSuiteMainWindow::removeCentralWidgetTab(int index) {
    QTabWidget *centralTabWidget = qobject_cast<QTabWidget *>(centralWidget());

    if (centralTabWidget) {
      delete centralTabWidget->widget(index);
      centralTabWidget->removeTab(index);
    }
  }
}
