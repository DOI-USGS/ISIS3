#include "MosaicController.h"

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QFuture>
#include <QFutureWatcher>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QProgressBar>
#include <QSettings>
#include <QtConcurrentMap>

#include "ControlNet.h"
#include "Cube.h"
#include "CubeDisplayProperties.h"
#include "FileName.h"
#include "IException.h"
#include "MosaicFileListWidget.h"
#include "MosaicSceneWidget.h"
#include "ProgressBar.h"
#include "Pvl.h"
#include "PvlObject.h"
#include "TextFile.h"


namespace Isis {

  /**
   * MosaicWidget constructor.
   * MosaicWidget is derived from QSplitter, the left side of the
   * splitter is a QTreeWidget and the right side of the splitter
   * is a QGraphicsView.
   *
   *
   * @param parent
   */
  MosaicController::MosaicController(QStatusBar *status, QSettings &settings) :
      m_cubesLeftToOpen(new QStringList),
      m_projectCubesLeftToOpen(new QList<PvlObject>) {
    m_mutex = new QMutex;
    m_watcher = NULL;
    m_projectPvl = NULL;

    p_progress = new ProgressBar;
    p_progress->setVisible(false);
    p_progress->setValue(0);

    p_fileList = new MosaicFileListWidget(settings);
    p_scene = new MosaicSceneWidget(status);
    p_worldScene = new MosaicSceneWidget(status);

    connect(this, SIGNAL(cubesAdded(QList<CubeDisplayProperties *>)),
            p_scene, SLOT(addCubes(QList<CubeDisplayProperties *>)));

    connect(this, SIGNAL(cubesAdded(QList<CubeDisplayProperties *>)),
            p_worldScene, SLOT(addCubes(QList<CubeDisplayProperties *>)));

    connect(this, SIGNAL(cubesAdded(QList<CubeDisplayProperties *>)),
            p_fileList, SLOT(addCubes(QList<CubeDisplayProperties *>)));

    connect(p_scene, SIGNAL(projectionChanged(Projection *)),
            p_worldScene, SLOT(setProjection(Projection *)));

    connect(p_scene, SIGNAL(visibleRectChanged(QRectF)),
            p_worldScene, SLOT(setOutlineRect(QRectF)));

    settings.beginGroup("MosaicController");
    m_openFilled = settings.value("openFilled", true).toBool();
    m_defaultAlpha = settings.value("defaultAlpha", 60).toInt();
    m_maxThreads = settings.value("maxThreads", 0).toInt();
    m_maxOpenCubes = settings.value("maxOpenCubes", 750).toInt();
    settings.endGroup();
  }


  /**
   * Free the allocated memory by this object
   */
  MosaicController::~MosaicController() {
    if(m_watcher) {
      m_watcher->waitForFinished();
      delete m_watcher;
      m_watcher = NULL;
    }

    p_fileList->deleteLater();
    p_scene->deleteLater();
    p_worldScene->deleteLater();
    p_progress->deleteLater();
  }


  /**
   * Add actions that are export-related to the menu
   */
  void MosaicController::addExportActions(QMenu &fileMenu) {
    QList<QAction *> exportActions = p_scene->getExportActions();

    foreach (QAction * exportAct, exportActions) {
      fileMenu.addAction(exportAct);
    }

    exportActions = p_fileList->getExportActions();

    foreach (QAction * exportAct, exportActions) {
      fileMenu.addAction(exportAct);
    }
  }


  QProgressBar *MosaicController::getProgress() {
    return p_progress;
  }


  void MosaicController::saveProject(QString projFileName) {
    Pvl projFile;

    PvlObject cubeProps("Cubes");

    CubeDisplayProperties *cube;
    foreach(cube, p_cubes) {
      cubeProps += cube->toPvl();
    }

    projFile += cubeProps;
    projFile += p_fileList->toPvl();
    projFile += p_scene->toPvl();

    projFile.write(projFileName);
  }


  QList<QAction *> MosaicController::getSettingsActions() {
    QList<QAction *> settingsActs;

    QAction *defaultFill = new QAction("Default Footprints &Filled", this);
    defaultFill->setCheckable(true);
    defaultFill->setChecked(m_openFilled);
    connect(defaultFill, SIGNAL(toggled(bool)),
            this, SLOT(defaultFillChanged(bool)));
    settingsActs.append(defaultFill);

    QAction *setSmallOpenCubeCount = new QAction("&Safe File Open", this);
    setSmallOpenCubeCount->setCheckable(true);
    setSmallOpenCubeCount->setChecked(m_maxOpenCubes < 750);
    setSmallOpenCubeCount->setWhatsThis("This lowers the number of "
        "simulataneously open files drastically in order to stay under the "
        "operating system limit. Only use this if you are having trouble "
        "loading large numbers of images.");
    connect(setSmallOpenCubeCount, SIGNAL(toggled(bool)),
            this, SLOT(setSmallNumberOfOpenCubes(bool)));
    settingsActs.append(setSmallOpenCubeCount);

    QAction *setFileListCols = new QAction("Set Current File List &Columns as Default", this);
    setFileListCols->setWhatsThis(tr("Use the currently visible columns in the file list as the "
          "default when no project has been opened"));
    connect(setFileListCols, SIGNAL(triggered(bool)),
            this, SLOT(setDefaultFileListCols()));
    settingsActs.append(setFileListCols);

    QAction *setAlpha = new QAction("Set Default &Transparency", this);
    connect(setAlpha, SIGNAL(triggered(bool)),
            this, SLOT(changeDefaultAlpha()));
    settingsActs.append(setAlpha);

    QAction *setThreads = new QAction("Set &Thread Limit", this);
    connect(setThreads, SIGNAL(triggered(bool)),
            this, SLOT(changeMaxThreads()));
    settingsActs.append(setThreads);

    return settingsActs;
  }


  void MosaicController::saveSettings(QSettings &settings) {
    settings.beginGroup("MosaicController");
    settings.setValue("openFilled", m_openFilled);
    settings.setValue("defaultAlpha", m_defaultAlpha);
    settings.setValue("maxThreads", m_maxThreads);
    settings.setValue("maxOpenCubes", m_maxOpenCubes);
    settings.endGroup();
  }


  /**
   * Create a functor for converting from filename to CubeDisplayProperties
   *
   * This method is always called from the GUI thread.
   */
  MosaicController::FileNameToDisplayFunctor::FileNameToDisplayFunctor(
      QMutex *cameraMutex, QThread *targetThread, bool openFilled,
      int defaultAlpha) {
    m_mutex = cameraMutex;
    m_targetThread = targetThread;
    m_openFilled = openFilled;
    m_defaultAlpha = defaultAlpha;
  }


  /**
   * Read the QString filename and make a cubedisplayproperties
   *   from it. Set the default values. This is what we're doing in another
   *   thread, so make sure the QObject ends up in the correct thread.
   *
   * This method is never called from the GUI thread.
   */
  CubeDisplayProperties *MosaicController::FileNameToDisplayFunctor::operator()(
      const QString &filename) {
    try {
      CubeDisplayProperties *prop = new CubeDisplayProperties(
          QString(FileName(filename).expanded()),
          m_mutex);
      prop->setShowFill(m_openFilled);

      QColor newColor = prop->getValue(
          CubeDisplayProperties::Color).value<QColor>();
      newColor.setAlpha(m_defaultAlpha);

      prop->setColor(newColor);
      prop->moveToThread(m_targetThread);
      return prop;
    }
    catch(IException &e) {
      e.print();
      return NULL;
    }
  }


  /**
   * Create a functor for converting from project to CubeDisplayProperties
   *
   * This method is always called from the GUI thread.
   */
  MosaicController::ProjectToDisplayFunctor::ProjectToDisplayFunctor(
      QMutex *cameraMutex, QThread *targetThread) {
    m_mutex = cameraMutex;
    m_targetThread = targetThread;
  }


  /**
   * Read the pvlObject from the project file and make a cubedisplayproperties
   *   from it. This is what we're doing in another thread, so make sure the
   *   QObject ends up in the correct thread.
   *
   * This method is never called from the GUI thread.
   */
  CubeDisplayProperties *MosaicController::ProjectToDisplayFunctor::operator()(
      const PvlObject &projectCube) {
    try {
      CubeDisplayProperties *prop = new CubeDisplayProperties(
          (QString)projectCube["FileName"][0],
          m_mutex);
      prop->fromPvl(projectCube);
      prop->moveToThread(m_targetThread);
      return prop;
    }
    catch(IException &e) {
      e.print();
      return NULL;
    }
  }


  /**
   * Handle opening cubes by filename. This class constructs and owns the
   *   actual Cube objects.
   */
  void MosaicController::openCubes(QStringList cubeNames) {
    if (!cubeNames.size() && m_cubesLeftToOpen->size()) {
      cubeNames = m_cubesLeftToOpen->mid(0, m_maxOpenCubes);
      *m_cubesLeftToOpen = m_cubesLeftToOpen->mid(m_maxOpenCubes);
    }

    if(cubeNames.size()) {
      if(m_watcher && !m_watcher->isFinished()) {
        m_cubesLeftToOpen->append(cubeNames);
        p_progress->setRange(0, p_progress->maximum() + cubeNames.size());
      }
      else {
        p_progress->setText("Opening cubes");

        if (cubeNames.size() > m_maxOpenCubes) {
          m_cubesLeftToOpen->append(cubeNames.mid(m_maxOpenCubes));
          cubeNames = cubeNames.mid(0, m_maxOpenCubes);
        }

        QFuture< CubeDisplayProperties * > displays = QtConcurrent::mapped(
            cubeNames,
            FileNameToDisplayFunctor(m_mutex, QThread::currentThread(),
              m_openFilled, m_defaultAlpha));

        if(m_maxThreads > 1)
          QThreadPool::globalInstance()->setMaxThreadCount(m_maxThreads - 1);
        else
          QThreadPool::globalInstance()->setMaxThreadCount(
              QThread::idealThreadCount());

        delete m_watcher;
        m_watcher = NULL;

        m_watcher = new QFutureWatcher< CubeDisplayProperties * >;
        connect(m_watcher, SIGNAL(resultReadyAt(int)),
                this, SLOT(cubeDisplayReady(int)));
        connect(m_watcher, SIGNAL(finished()),
                this, SLOT(loadFinished()));
        m_watcher->setFuture(displays);
        if (!p_progress->isVisible())
          p_progress->setRange(0, cubeNames.size() + m_cubesLeftToOpen->size());
        p_progress->setVisible(true);
      }
    }
  }


  void MosaicController::openProjectCubes(QList<PvlObject> projectCubes) {
    if (!projectCubes.size() && m_projectCubesLeftToOpen->size()) {
      projectCubes = m_projectCubesLeftToOpen->mid(0, m_maxOpenCubes);
      *m_projectCubesLeftToOpen = m_projectCubesLeftToOpen->mid(m_maxOpenCubes);
    }

    if(projectCubes.size()) {
      if(m_watcher && !m_watcher->isFinished()) {
        m_projectCubesLeftToOpen->append(projectCubes);
        p_progress->setRange(0, p_progress->maximum() + projectCubes.size());
      }
      else {
        p_progress->setText("Opening cubes");

        if (projectCubes.size() > m_maxOpenCubes) {
          m_projectCubesLeftToOpen->append(projectCubes.mid(m_maxOpenCubes));
          projectCubes = projectCubes.mid(0, m_maxOpenCubes);
        }

        QFuture< CubeDisplayProperties * > displays = QtConcurrent::mapped(
            projectCubes,
            ProjectToDisplayFunctor(m_mutex, QThread::currentThread()));

        if(m_maxThreads > 1)
          QThreadPool::globalInstance()->setMaxThreadCount(m_maxThreads - 1);
        else
          QThreadPool::globalInstance()->setMaxThreadCount(
              QThread::idealThreadCount());

        delete m_watcher;
        m_watcher = NULL;

        m_watcher = new QFutureWatcher< CubeDisplayProperties * >;
        connect(m_watcher, SIGNAL(resultReadyAt(int)),
                this, SLOT(cubeDisplayReady(int)));
        connect(m_watcher, SIGNAL(finished()),
                this, SLOT(loadFinished()));
        m_watcher->setFuture(displays);
        if (!p_progress->isVisible())
          p_progress->setRange(0,
              projectCubes.size() + m_projectCubesLeftToOpen->size());
        p_progress->setVisible(true);
      }
    }
  }


  void MosaicController::cubeDisplayReady(int index) {
    if(m_watcher) {
      CubeDisplayProperties *cubeDisplay = m_watcher->resultAt(index);
      p_progress->setValue(p_progress->value() + 1);

      if(cubeDisplay) {
        p_unannouncedCubes.append(cubeDisplay);
        p_cubes.append(cubeDisplay);
        connect(cubeDisplay, SIGNAL(destroyed(QObject *)),
                this, SLOT(cubeClosed(QObject *)));
      }
    }
  }

  void MosaicController::loadFinished() {
    flushCubes();

    if (m_projectCubesLeftToOpen->size()) {
      QList<PvlObject> empty;
      openProjectCubes(empty);
    }
    else if (m_cubesLeftToOpen->size()) {
      openCubes(QStringList());
    }
    else {
      if(m_projectPvl && m_projectPvl->hasObject("MosaicFileList"))
        p_fileList->fromPvl(m_projectPvl->findObject("MosaicFileList"));

      if(m_projectPvl && m_projectPvl->hasObject("MosaicScene"))
        p_scene->fromPvl(m_projectPvl->findObject("MosaicScene"));

      if(m_projectPvl) {
        delete m_projectPvl;
        m_projectPvl = NULL;
      }

      p_progress->setVisible(false);
      p_progress->setValue(0);
    }
  }


  void MosaicController::changeDefaultAlpha() {
    bool ok = false;
    int alpha = QInputDialog::getInt(NULL, "Transparency Value",
        "Set the default transparency value\n"
        "Values are 0 (invisible) to 255 (solid)",
        m_defaultAlpha, 0, 255, 1, &ok);

    if(ok) {
      m_defaultAlpha = alpha;
    }
  }


  void MosaicController::changeMaxThreads() {
    bool ok = false;

    QStringList options;

    int current = 0;
    options << "Use all available";

    for(int i = 1; i < 24; i++) {
      QString option = QString("Use ") + QString::number(i + 1) + " threads";

      options << option;
      if(m_maxThreads == i + 1)
        current = i;
    }

    QString res = QInputDialog::getItem(NULL, "Concurrency",
        "Set the number of threads to use",
        options, current, false, &ok);

    if(ok) {
      m_maxThreads = options.indexOf(res) + 1;
    }
  }


  /**
   * Open cube being deleted
   */
  void MosaicController::cubeClosed(QObject * cubeDisplayObj) {
    CubeDisplayProperties *cubeDisplay =
        (CubeDisplayProperties *)cubeDisplayObj;

    QList<CubeDisplayProperties *>::iterator foundElement;
    foundElement = qFind(p_cubes.begin(), p_cubes.end(), cubeDisplay);
    p_cubes.erase(foundElement);

    if(p_cubes.empty())
      emit allCubesClosed();
  }


  void MosaicController::defaultFillChanged(bool fill) {
    m_openFilled = fill;
  }


  void MosaicController::readProject(QString filename) {
    try {
      m_projectPvl = new Pvl(filename);
      PvlObject &cubes(m_projectPvl->findObject("Cubes"));

      QList<PvlObject> cubesList;

      for (int i = 0; i < cubes.objects(); i++) {
        cubesList.append(cubes.object(i));
      }

      if(m_projectPvl && m_projectPvl->hasObject("MosaicScene"))
        p_scene->preloadFromPvl(m_projectPvl->findObject("MosaicScene"));

      openProjectCubes(cubesList);
    }
    catch(IException &e) {
      p_progress->setVisible(false);
      flushCubes();
      throw IException(e, IException::Unknown, "Input project file does is not"
          " an up to date qmos project", _FILEINFO_);
    }
  }


  void MosaicController::saveList() {
    QString output =
        QFileDialog::getSaveFileName((QWidget *)parent(),
          "Choose output file",
          QDir::currentPath() + "/files.lis",
          QString("List File (*.lis);;Text File (*.txt);;All Files (*.*)"));
    if(output.isEmpty()) return;

    TextFile file(output, "output");

    CubeDisplayProperties *cube;
    foreach(cube, p_cubes) {
      file.PutLine( cube->fileName() );
    }
  }
  
  
  void MosaicController::setDefaultFileListCols() {
    if (p_fileList) {
      p_fileList->setDefaultFileListCols();
    }
  }


  void MosaicController::setSmallNumberOfOpenCubes(bool useSmallNumber) {
    if (useSmallNumber)
      m_maxOpenCubes = 20;
    else
      m_maxOpenCubes = 750;
  }


  /**
   * This method takes the opened cube files, announces them to the GUI widgets,
   *   and then closes the Cube files so that more can be opened later.
   */
  void MosaicController::flushCubes() {
    // We really can't have all of the cubes in memory before
    //   the OS stops letting us open more files.
    if(p_unannouncedCubes.size() > 0) {
      // Assume cameras are being used in other parts of code since it's
      //   unknown
      QMutexLocker lock(m_mutex);
      emit cubesAdded(p_unannouncedCubes);

      CubeDisplayProperties *openCube;
      foreach(openCube, p_unannouncedCubes) {
        openCube->closeCube();
      }

      p_unannouncedCubes.clear();
    }
  }
}
