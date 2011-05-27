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
#include "Filename.h"
#include "MosaicFileListWidget.h"
#include "MosaicSceneWidget.h"
#include "ProgressBar.h"
#include "TextFile.h"

// using namespace Qisis;

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
  MosaicController::MosaicController(QStatusBar *status, QSettings &settings) {
    m_mutex = new QMutex;
    m_watcher = NULL;
    m_projectPvl = NULL;

    p_progress = new ProgressBar;
    p_progress->setVisible(false);

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
   * Saves the graphics view as a png, jpg, of tif file.
   */
  void MosaicController::exportView() {
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
    QPixmap pm = QPixmap::grabWidget(p_scene->getScene()->views().last());

    std::string formatString = format.toStdString();
    if(!pm.save(output, formatString.c_str())) {
      QMessageBox::information(p_scene, "Error", "Unable to save " + output);
    }
  }


  /**
   * Add actions that are export-related to the menu
   */
  void MosaicController::addExportActions(QMenu &fileMenu) {
    QAction *saveList = new QAction(this);
    saveList->setText("Save Cube List...");
    connect(saveList, SIGNAL(activated()), this, SLOT(saveList()));

    QAction *exportView = new QAction(this);
    exportView->setText("Export View...");
    connect(exportView, SIGNAL(activated()), this, SLOT(exportView()));

    fileMenu.addAction(saveList);
    fileMenu.addAction(exportView);
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

    projFile.Write(projFileName.toStdString());
  }


  QList<QAction *> MosaicController::getSettingsActions() {
    QList<QAction *> settingsActs;

    QAction *defaultFill = new QAction("Default Footprints &Filled", this);
    defaultFill->setCheckable(true);
    defaultFill->setChecked(m_openFilled);
    connect(defaultFill, SIGNAL(toggled(bool)),
            this, SLOT(defaultFillChanged(bool)));
    settingsActs.append(defaultFill);

    QAction *setAlpha = new QAction("Set Default &Alpha", this);
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
    settings.endGroup();
  }


  /**
   * Create a functor for converting from filename to CubeDisplayProperties
   *
   * This method is always called from the GUI thread.
   */
  MosaicController::FilenameToDisplayFunctor::FilenameToDisplayFunctor(
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
  CubeDisplayProperties *MosaicController::FilenameToDisplayFunctor::operator()(
      const QString &filename) {
    try {
      CubeDisplayProperties *prop = new CubeDisplayProperties(
          QString(Filename(filename.toStdString()).Expanded().c_str()),
          m_mutex);
      prop->setShowFill(m_openFilled);
      
      QColor newColor = prop->getValue(
          CubeDisplayProperties::Color).value<QColor>();
      newColor.setAlpha(m_defaultAlpha);
 
      prop->setColor(newColor);
      prop->moveToThread(m_targetThread);
      return prop;
    }
    catch(iException &e) {
      e.Report(false);
      e.Clear();
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
          (QString)projectCube["Filename"][0],
          m_mutex);
      prop->fromPvl(projectCube);
      prop->moveToThread(m_targetThread);
      return prop;
    }
    catch(iException &e) {
      e.Report(false);
      e.Clear();
      return NULL;
    }
  }


  /**
   * Handle opening cubes by filename. This class constructs and owns the
   *   actual Cube objects.
   */
  void MosaicController::openCubes(QStringList cubeNames) {
    if(cubeNames.size()) {
      if(m_watcher) {
        m_watcher->waitForFinished();
        delete m_watcher;
        m_watcher = NULL;
      }

      p_progress->setText("Opening cubes");

      QFuture< CubeDisplayProperties * > displays = QtConcurrent::mapped(
          cubeNames,
          FilenameToDisplayFunctor(m_mutex, QThread::currentThread(),
            m_openFilled, m_defaultAlpha));

      if(m_maxThreads > 1)
        QThreadPool::globalInstance()->setMaxThreadCount(m_maxThreads - 1);
      else
        QThreadPool::globalInstance()->setMaxThreadCount(
            QThread::idealThreadCount());
      
      m_watcher = new QFutureWatcher< CubeDisplayProperties * >;
      connect(m_watcher, SIGNAL(resultReadyAt(int)),
              this, SLOT(cubeDisplayReady(int)));
      connect(m_watcher, SIGNAL(finished()),
              this, SLOT(loadFinished()));
      connect(m_watcher, SIGNAL(progressValueChanged(int)),
              this, SLOT(updateProgress(int)));
      m_watcher->setFuture(displays);
      p_progress->setRange(m_watcher->progressMinimum(),
          m_watcher->progressMaximum());
      p_progress->setValue(0);
      p_progress->setVisible(true);
    }
  }


  void MosaicController::cubeDisplayReady(int index) {
    if(m_watcher) {
      CubeDisplayProperties *cubeDisplay = m_watcher->resultAt(index);

      if(cubeDisplay) {
        p_unannouncedCubes.append(cubeDisplay);
        p_cubes.append(cubeDisplay);
        connect(cubeDisplay, SIGNAL(destroyed(QObject *)),
                this, SLOT(cubeClosed(QObject *)));

        flushCubes();
      }
    }
  }

  void MosaicController::loadFinished() {
    flushCubes(true);

    if(m_projectPvl && m_projectPvl->HasObject("MosaicFileList"))
      p_fileList->fromPvl(m_projectPvl->FindObject("MosaicFileList"));

    if(m_projectPvl && m_projectPvl->HasObject("MosaicScene"))
      p_scene->fromPvl(m_projectPvl->FindObject("MosaicScene"));

    if(m_projectPvl) {
      delete m_projectPvl;
      m_projectPvl = NULL;
    }

    p_progress->setVisible(false);
  }

  void MosaicController::updateProgress(int newVal) {
    p_progress->setValue(newVal);
  }

  void MosaicController::changeDefaultAlpha() {
    bool ok = false;
    int alpha = QInputDialog::getInt(NULL, "Alpha Value",
        "Set the default transparency value",
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
      if(m_watcher) {
        m_watcher->waitForFinished();
        delete m_watcher;
        m_watcher = NULL;
      }

      m_projectPvl = new Pvl(filename.toStdString());
      PvlObject &cubes(m_projectPvl->FindObject("Cubes"));

      p_progress->setText("Opening cubes");

      if(m_maxThreads > 1)
        QThreadPool::globalInstance()->setMaxThreadCount(m_maxThreads - 1);
      else
        QThreadPool::globalInstance()->setMaxThreadCount(
            QThread::idealThreadCount());

      QFuture< CubeDisplayProperties * > displays = QtConcurrent::mapped(
          cubes.BeginObject(), cubes.EndObject(),
          ProjectToDisplayFunctor(m_mutex, QThread::currentThread()));

      m_watcher = new QFutureWatcher< CubeDisplayProperties * >;
      connect(m_watcher, SIGNAL(resultReadyAt(int)),
              this, SLOT(cubeDisplayReady(int)));
      connect(m_watcher, SIGNAL(finished()),
              this, SLOT(loadFinished()));
      connect(m_watcher, SIGNAL(progressValueChanged(int)),
              this, SLOT(updateProgress(int)));
      m_watcher->setFuture(displays);
      p_progress->setRange(m_watcher->progressMinimum(),
          m_watcher->progressMaximum());
      p_progress->setValue(0);
      p_progress->setVisible(true);
    }
    catch(iException &e) {
      p_progress->setVisible(false);
      flushCubes(true);
      throw iException::Message(iException::Io, "Input project file does is not"
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

    TextFile file(output.toStdString(), "output");

    CubeDisplayProperties *cube;
    foreach(cube, p_cubes) {
      file.PutLine( cube->fileName().toStdString() );
    }
  }


  void MosaicController::flushCubes(bool force) {
    // We really can't have all of the cubes in memory before
    //   the OS stops letting us open more files. Throttle at 1k.
    if(force || p_unannouncedCubes.size() >= 500) {
      if(p_unannouncedCubes.size() > 0) {
        // The concurrent threads will open too many files if we let them keep
        //   going. Pause them while the GUI starts working.
        if(m_watcher) {
          m_watcher->pause();
        }

        // Assume cameras are being used in other parts of code since it's
        //   unknown
        QMutexLocker lock(m_mutex);
        emit cubesAdded(p_unannouncedCubes);

        CubeDisplayProperties *openCube;
        foreach(openCube, p_unannouncedCubes) {
          openCube->closeCube();
        }

        p_unannouncedCubes.clear();

        if(m_watcher) {
          m_watcher->resume();
        }
      }
    }
  }
}

