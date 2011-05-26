#include "MosaicController.h"

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QFuture>
#include <QFutureWatcher>
#include <QGraphicsScene>
#include <QGraphicsView>
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
    p_scene2 = new MosaicSceneWidget(status);

    connect(this, SIGNAL(cubesAdded(QList<CubeDisplayProperties *>)),
            p_scene, SLOT(addCubes(QList<CubeDisplayProperties *>)));

    connect(this, SIGNAL(cubesAdded(QList<CubeDisplayProperties *>)),
            p_scene2, SLOT(addCubes(QList<CubeDisplayProperties *>)));

    connect(this, SIGNAL(cubesAdded(QList<CubeDisplayProperties *>)),
            p_fileList, SLOT(addCubes(QList<CubeDisplayProperties *>)));

    connect(p_scene, SIGNAL(projectionChanged(Projection *)),
            p_scene2, SLOT(setProjection(Projection *)));

    connect(p_scene, SIGNAL(visibleRectChanged(QRectF)),
            p_scene2, SLOT(setOutlineRect(QRectF)));
  }


  /**
   * Free the allocated memory by this object
   */
  MosaicController::~MosaicController() {
    p_fileList->deleteLater();
    p_scene->deleteLater();
    p_scene2->deleteLater();
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


  MosaicController::FilenameToDisplayFunctor::FilenameToDisplayFunctor(
      QMutex *cameraMutex, QThread *targetThread) {
    m_mutex = cameraMutex;
    m_targetThread = targetThread;
  }


  CubeDisplayProperties *MosaicController::FilenameToDisplayFunctor::operator()(
      const QString &filename) {
    try {
      CubeDisplayProperties *prop = new CubeDisplayProperties(
          QString(Filename(filename.toStdString()).Expanded().c_str()),
          m_mutex);
      prop->moveToThread(m_targetThread);
      return prop;
    }
    catch(iException &e) {
      e.Report(false);
      e.Clear();
      return NULL;
    }
  }


  MosaicController::ProjectToDisplayFunctor::ProjectToDisplayFunctor(
      QMutex *cameraMutex, QThread *targetThread) {
    m_mutex = cameraMutex;
    m_targetThread = targetThread;
  }


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
    QString cubeName;

    QMessageBox::StandardButton loadUnfilled = QMessageBox::No;

    if(cubeNames.size() > 500) {
      loadUnfilled = QMessageBox::question(
          (QWidget *)parent(), "Large Amount of Files",
          "You appear to be opening a large amount of files. Would you like to "
          "load them without the fill in order to improve performance?",
          QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
          QMessageBox::Yes);
    }

    if(loadUnfilled != QMessageBox::Cancel && cubeNames.size()) {
      if(m_watcher) {
        m_watcher->waitForFinished();
        delete m_watcher;
        m_watcher = NULL;
      }

      p_progress->setText("Opening cubes");

      QFuture< CubeDisplayProperties * > displays = QtConcurrent::mapped(
          cubeNames,
          FilenameToDisplayFunctor(m_mutex, QThread::currentThread()));

      m_watcher = new QFutureWatcher< CubeDisplayProperties * >;
      m_watcher->setPendingResultsLimit(50);
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
    static int i = 0;
    i++;
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

    i --;
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

      QFuture< CubeDisplayProperties * > displays = QtConcurrent::mapped(
          cubes.BeginObject(), cubes.EndObject(),
          ProjectToDisplayFunctor(m_mutex, QThread::currentThread()));

      m_watcher = new QFutureWatcher< CubeDisplayProperties * >;
      // The max open cubes is PendingResults + flush count = 1050 currently
      m_watcher->setPendingResultsLimit(50);
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

    p_progress->setVisible(false);
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
    if(force || p_unannouncedCubes.size() >= 1000) {
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
}

