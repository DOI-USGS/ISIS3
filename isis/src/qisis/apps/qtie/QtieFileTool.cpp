#include "QtieFileTool.h"

#include <QApplication>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QPushButton>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QGridLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPainter>

#include "Application.h"
#include "Camera.h"
#include "ControlNet.h"
#include "Cube.h"
#include "Filename.h"
#include "MainWindow.h"
#include "MdiCubeViewport.h"
#include "SerialNumberList.h"
#include "Workspace.h"




namespace Isis {
  // Constructor
  QtieFileTool::QtieFileTool(QWidget *parent) : FileTool(parent) {
    openAction()->setToolTip("Open images");
    QString whatsThis =
      "<b>Function:</b> Open a <i>images</i> \
       <p><b>Shortcut:</b>  Ctrl+O\n</p>";
    openAction()->setWhatsThis(whatsThis);

    saveAction()->setEnabled(false);
  }

  /**
   *  Open base image and image to be adjusted
   *
   * @author 2007-08-13 Tracie Sucharski
   *
   * @internal
   * @history 2009-06-10  Tracie Sucharski - Added error checking and looping
   *                            when opening files.  Allow new files to be
   *                            opened.
   * @history 2010-05-11  Tracie Sucharski - Added ability to read in control
   *                            network.
   * @history 2010-05-13  Tracie Sucharski - Use match cube directory to determine
   *                            default directory for cnet.
   *
   */
  void QtieFileTool::open() {


    //  If we've already opened files, clear before starting over
    if (cubeViewportList()->size() > 0) {
      closeAll();
      emit newFiles();
    }


    QString baseFile, matchFile;
    QString dir;
    QString filter = "Isis Cubes (*.cub);;";
    filter += "Detached labels (*.lbl);;";
    filter += "All (*)";

    Cube *baseCube = new Cube;
    while (!baseCube->isOpen()) {
      baseFile = QFileDialog::getOpenFileName((QWidget *)parent(),
                                              "Select basemap cube (projected)",
                                              ".", filter);
      if (baseFile.isEmpty()) {
        delete baseCube;
        return;
      }

      // Find directory and save for use in file dialog for match cube
      Filename fname(baseFile.toStdString());
      dir = fname.absolutePath();

      //  Make sure base is projected
      try {
        baseCube->open(baseFile.toStdString());
        try {
          baseCube->getProjection();
        }
        catch (iException &e) {
          baseCube->close();
          QString message = "Base must be projected";
          QMessageBox::critical((QWidget *)parent(), "Error", message);
          baseCube->close();
        }
      }
      catch (iException &e) {
        QString message = "Unable to open base cube";
        QMessageBox::critical((QWidget *)parent(), "Error", message);
      }
    }
    baseCube->close();


    Cube *matchCube = new Cube;

    while (!matchCube->isOpen()) {
      // Get match cube
      matchFile = QFileDialog::getOpenFileName((QWidget *)parent(),
                  "Select cube to tie to base",
                  dir, filter);
      if (matchFile.isEmpty()) {
        delete baseCube;
        delete matchCube;
        (*(cubeViewportList()))[0]->close();
        return;
      }

      //Make sure match is not projected
      try {
        matchCube->open(matchFile.toStdString());
        try {

          if (matchCube->hasGroup("Mapping")) {
            QString message = "The match cube cannot be a projected cube.";
            QMessageBox::critical((QWidget *)parent(), "Error", message);
            matchCube->close();
            continue;
          }
        }
        catch (iException &e) {
          QString message = "Error reading match cube labels.";
          QMessageBox::critical((QWidget *)parent(), "Error", message);
          matchCube->close();
        }
      }
      catch (iException &e) {
        QString message = "Unable to open match cube";
        QMessageBox::critical((QWidget *)parent(), "Error", message);
      }
    }
    // Find target
    iString target = matchCube->getCamera()->Target();
    matchCube->close();

    // Find directory and save for use in file dialog for match cube
    Filename fname(matchFile.toStdString());
    dir = fname.absolutePath();

    //Open control net
    filter = "Control net (*.net);;";
    filter += "Text file (*.txt);;";
    filter += "All (*)";
    QString cnetFile = QFileDialog::getOpenFileName((QWidget *)parent(),
                       "Select a control network",
                       dir, filter);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ControlNet *cnet;
    if (cnetFile.isEmpty()) {
      cnet = new ControlNet();
      cnet->SetNetworkId("Qtie");
      cnet->SetUserName(Application::UserName());
      //  Set control net target
      cnet->SetTarget(target);
    }
    else {
      try {
        cnet = new ControlNet(cnetFile.toStdString());
      }
      catch (iException &e) {
        QString message = "Invalid control network.  \n";
        std::string errors = e.Errors();
        message += errors.c_str();
        e.Clear();
        QMessageBox::information((QWidget *)parent(), "Error", message);
        QApplication::restoreOverrideCursor();
        return;
      }
    }

    QApplication::restoreOverrideCursor();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    emit fileSelected(baseFile);
    baseCube = (*(cubeViewportList()))[0]->cube();
    QApplication::restoreOverrideCursor();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    emit fileSelected(matchFile);
    matchCube = (*(cubeViewportList()))[1]->cube();
    emit cubesOpened(baseCube, matchCube, cnet);
    QApplication::restoreOverrideCursor();

    return;
  }





#if 0
  // Exit the program
  ............................................

  void QtieFileTool::exit() {
    qApp->quit();
  }
#endif
}
