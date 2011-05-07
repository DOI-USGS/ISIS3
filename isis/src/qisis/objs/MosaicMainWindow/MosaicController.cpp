#include "MosaicController.h"

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QMessageBox>
#include <QProgressBar>
#include <QSettings>

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

    if(loadUnfilled != QMessageBox::Cancel) {
      p_progress->setText("Opening cubes");
      p_progress->setRange(0, cubeNames.size() - 1);
      p_progress->setValue(0);
      p_progress->setVisible(true);

      for(int cubePos = 0; cubePos < cubeNames.size(); cubePos ++) {
        p_progress->setValue(cubePos);

        QString cubeName = cubeNames[cubePos];
        try {
          if(cubeName != "") {
            CubeDisplayProperties *cubeDisplay = new CubeDisplayProperties(
                QString(Filename(cubeName.toStdString()).Expanded().c_str()));

            if(loadUnfilled == QMessageBox::Yes)
              cubeDisplay->setShowFill(false);

            p_unannouncedCubes.append(cubeDisplay);
            p_cubes.append(cubeDisplay);

            connect(cubeDisplay, SIGNAL(destroyed(QObject *)),
                    this, SLOT(cubeClosed(QObject *)));

            flushCubes();
          }
        }
        catch(iException &e) {
          e.Report(false);
          e.Clear();
        }
      }

      flushCubes(true);

      p_progress->setVisible(false);
    }

//     if(p_cubes.empty())
//       emit allCubesClosed();
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
      Pvl projFile(filename.toStdString());
      PvlObject cubes(projFile.FindObject("Cubes"));

      p_progress->setText("Opening cubes");
      p_progress->setRange(0, cubes.Objects() - 1);
      p_progress->setValue(0);
      p_progress->setVisible(true);

      for(int cubePos = 0; cubePos < cubes.Objects(); cubePos ++) {
        try {
          p_progress->setValue(cubePos);
          CubeDisplayProperties *cubeDisplay = new CubeDisplayProperties(
              cubes.Object(cubePos));

          p_unannouncedCubes.append(cubeDisplay);
          p_cubes.append(cubeDisplay);

          connect(cubeDisplay, SIGNAL(destroyed(QObject *)),
                  this, SLOT(cubeClosed(QObject *)));

          flushCubes();
        }
        catch(iException &e) {
          e.Report(false);
          e.Clear();
        }
      }

      flushCubes(true);

      p_fileList->fromPvl(projFile.FindObject("MosaicFileList"));
      p_scene->fromPvl(projFile.FindObject("MosaicScene"));
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

