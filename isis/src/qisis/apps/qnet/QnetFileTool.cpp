#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>

#include "Application.h"
#include "Camera.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "Cube.h"
#include "Filename.h"
#include "MdiCubeViewport.h"
#include "Progress.h"
#include "QnetFileTool.h"
#include "SerialNumber.h"
#include "SerialNumberList.h"
#include "Workspace.h"

#include "qnet.h"
using namespace Isis::Qnet;
using namespace std;

namespace Isis {
  /**
   * Constructor
   *
   * @internal
   *   @history 2008-12-10 Jeannie Walldren - Reworded "What's this?" description
   *                          for saveAs action. Changed "Save As" action text to
   *                          match QnetTool's "Save As" action
   */
  QnetFileTool::QnetFileTool(QWidget *parent) : FileTool(parent) {
    openAction()->setText("Open control network & cube list");
    openAction()->setToolTip("Open control network & cube list");
    QString whatsThis =
      "<b>Function:</b> Open a <i>control network</i> \
       <p><b>Shortcut:</b>  Ctrl+O\n</p>";
    openAction()->setWhatsThis(whatsThis);

    saveAction()->setText("Save Control Network &As...");
    whatsThis =
      "<b>Function:</b> Save the current <i>control network</i> under chosen filename";
    saveAction()->setWhatsThis(whatsThis);
    saveAction()->setEnabled(true);

    p_saveNet = false;

    p_openGround = new QAction(parent);
    p_openGround->setText("Open &Ground Source");
    p_openGround->setStatusTip("Open a ground source for choosing ground points");
    whatsThis =
      "<b>Function:</b> Open and display a ground source for choosing ground points."
      "This can be level1, level2 or dem cube.";
    p_openGround->setWhatsThis(whatsThis);
    p_openGround->setEnabled(false);
    connect (p_openGround,SIGNAL(activated()),this,SIGNAL(newGroundFile()));

    p_openDem = new QAction(parent);
    p_openDem->setText("Open &Radius Source");
    whatsThis =
      "<b>Function:</b> Open a DEM for determining the radius when "
      "choosing ground points.  This is not the file that will be displayed "
      "to be used for visually picking points.  This is strictly used to "
      "determine the radius value.";
    p_openDem->setWhatsThis(whatsThis);
    p_openDem->setEnabled(false);
    connect (p_openDem,SIGNAL(activated()),this,SIGNAL(newDemFile()));

    QMenu *menu = g_vpMainWindow->getMenu(menuName());
    menu->addAction(p_openGround);
    menu->addAction(p_openDem);
    menu->addSeparator();

  }


  QnetFileTool::~QnetFileTool() {
    delete g_serialNumberList;
    g_serialNumberList = NULL;
    delete g_controlNetwork;
    g_controlNetwork = NULL;

  }




  /**
   *  Open a list of cubes
   *
   * @author  2007-05-01 Elizabeth Ribelin
   *
   * @internal
   *  @history  2007-06-07 Tracie Sucharski - Allow new network to be opened,
   *                          prompt to save old network.
   *  @history  2008-11-26 Tracie Sucharski - Remove all polygon/overlap
   *                          references, this functionality will be qmos.
   *  @history 2008-11-26 Jeannie Walldren - Uncommented "emit
   *                         controlNetworkUpdated()" line and added parameter name
   *                         defined in this method.
   *  @history 2008-12-10 Jeannie Walldren - Fixed documentation
   *  @history 2010-07-01 Jeannie Walldren - Added file extension filters for
   *                         input control network
   *  @history 2010-11-09 Tracie Sucharski - "emit" was missing from the signal
   *                         serialNumberListUpdated.
   *  @history 2011-08-08 Tracie Sucharski - If new network, set the Target
   *
   */
  void QnetFileTool::open() {

    //  If network already opened, prompt for saving
    if (g_serialNumberList != NULL && p_saveNet) {
      //  If control net has been changed , prompt for user to save
      int resp = QMessageBox::warning((QWidget *)parent(), "Qnet",
          "The control network files has been modified.\n"
          "Do you want to save your changes?",
          QMessageBox::Yes | QMessageBox::Default,
          QMessageBox::No,
          QMessageBox::Cancel | QMessageBox::Escape);
      if (resp == QMessageBox::Yes) {
        saveAs();
      }
      p_saveNet = false;
    }

    QString filter = "List of cubes (*.lis *.lst *.list);;";
    filter += "Text file (*.txt);;";
    filter += "All (*)";
    QString list = QFileDialog::getOpenFileName((QWidget *)parent(),
        "Select a list of cubes",
        ".",
        filter);
    if (list.isEmpty())
      return;

    // Find directory and save for use in file dialog for net file
    Filename file(list.toStdString());
    QString dir = file.absolutePath();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    // Use the list to get serial numbers and polygons
    try {
      if (g_serialNumberList != NULL) {
        delete g_serialNumberList;
        g_serialNumberList = NULL;
      }
      g_serialNumberList = new SerialNumberList(list.toStdString());

      if (g_controlNetwork != NULL) {
        delete g_controlNetwork;
        g_controlNetwork = NULL;
      }
    }
    catch (iException &e) {
      QString message = "Error processing cube list.  \n";
      string errors = e.Errors();
      message += errors.c_str();
      e.Clear();
      QMessageBox::information((QWidget *)parent(), "Error", message);
      QApplication::restoreOverrideCursor();
      return;
    }

    QApplication::restoreOverrideCursor();
    filter = "Control net (*.net *.cnet *.ctl);;";
    filter += "Pvl file (*.pvl);;";
    filter += "Text file (*.txt);;";
    filter += "All (*)";
    cNetFilename = QFileDialog::getOpenFileName((QWidget *)parent(),
        "Select a control network",
        dir,
        filter);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (cNetFilename.isEmpty()) {
      //  Create new control net
      g_controlNetwork = new ControlNet();
      g_controlNetwork->SetUserName(Application::UserName());
      //  Determine target from first file in cube list
      Cube *cube = new Cube;
      cube->open(g_serialNumberList->Filename(0));
      g_controlNetwork->SetTarget(cube->getCamera()->Target());
      delete cube;
      cube = NULL;
    }
    else {
      try {
        Progress progress;
        g_controlNetwork = new ControlNet(cNetFilename.toStdString(),
                                                &progress);
      }
      catch (iException &e) {
        QString message = "Invalid control network.  \n";
        string errors = e.Errors();
        message += errors.c_str();
        e.Clear();
        QMessageBox::information((QWidget *)parent(), "Error", message);
        QApplication::restoreOverrideCursor();
        return;
      }
    }

    //  Initialize cameras for control net
    try {
      Progress progress;
      g_controlNetwork->SetImages(*g_serialNumberList,&progress);
    }
    catch (iException &e) {
      QString message = "Cannot initialize images in control network.  \n";
      string errors = e.Errors();
      message += errors.c_str();
      e.Clear();
      QMessageBox::information((QWidget *)parent(), "Error", message);
      QApplication::restoreOverrideCursor();
      return;
    }
    
    p_openGround->setEnabled(true);
    p_openDem->setEnabled(true);

    QApplication::restoreOverrideCursor();

    emit serialNumberListUpdated();
    emit controlNetworkUpdated(cNetFilename);
    emit newControlNetwork(g_controlNetwork);
    return;
  }

  /**
   * Exit the program
   */
  void QnetFileTool::exit() {
    //  If control net has been changed , prompt for user to save
    if (p_saveNet) {
      int resp = QMessageBox::warning((QWidget *)parent(), "QnetTool",
          "The control network files has been modified.\n"
          "Do you want to save your changes?",
          QMessageBox::Yes | QMessageBox::Default,
          QMessageBox::No,
          QMessageBox::Cancel | QMessageBox::Escape);
      if (resp == QMessageBox::Yes) {
        saveAs();
      }
      if (resp == QMessageBox::Cancel) {
        return;
      }
    }
    qApp->quit();
  }

  /**
   *  Save control network with given file
   *  @internal
   *  @history 2010-07-01 Jeannie Walldren - Added file extension filters for
   *                         input control network
   *
   */
  void QnetFileTool::saveAs() {
    QString filter = "Control net (*.net *.cnet *.ctl);;";
    filter += "Pvl file (*.pvl);;";
    filter += "Text file (*.txt);;";
    filter += "All (*)";
    QString fn = QFileDialog::getSaveFileName((QWidget *)parent(),
        "Choose filename to save under",
        ".", filter);
    if (!fn.isEmpty()) {
      try {
        g_controlNetwork->Write(fn.toStdString());
      } 
      catch (iException &e) {
        QString message = "Error saving control network.  \n";
        string errors = e.Errors();
        message += errors.c_str();
        e.Clear();
        QMessageBox::information((QWidget *)parent(), "Error", message);
        return;
      }
    }
    else {
      QMessageBox::information((QWidget *)parent(),
          "Error", "Saving Aborted");
    }
    p_saveNet = false;
  }

  /**
   * Load given cube in Workspace
   *
   * @param serialNumber [in]   (QString)   Serial number of cube to display
   *
   * @author  2007-05-01 Elizabeth Ribelin
   *
   * @internal
   *  @history  2008-10-08 Tracie Sucharski - Do not display cube if it is
   *                          already displayed, set as active window.
   *  @history 2008-12-10 Jeannie Walldren - Fixed documentation
   *  @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   *  @history 2010-07-12 Jeannie Walldren - Updated setActiveSubWindow call due
   *                          to change in Workspace class
   *  @history 2011-04-08  Tracie Sucharski - Remove test for sn existing in
   *                          serial number list since the list is where the
   *                          serial number came from.
   */
  void QnetFileTool::loadImage(const QString &serialNumber) {

    string tempFilename = g_serialNumberList->Filename(serialNumber.toStdString());
    QString filename = tempFilename.c_str();
    QVector< MdiCubeViewport * > * cvpList =
      g_vpMainWindow->workspace()->cubeViewportList();
    bool found = false;
    for (int i = 0; i < (int)cvpList->size(); i++) {
      string sn = SerialNumber::Compose(*((*cvpList)[i]->cube()));
      if (sn == serialNumber.toStdString()) {
        g_vpMainWindow->workspace()->
        setActiveSubWindow((QMdiSubWindow *)(*cvpList)[i]->parentWidget()->parent());
        found = true;
        break;
      }
    }
    if (!found)
      emit fileSelected(filename);
  }

  /**
   * Load images for the given point 
   *
   * @param point Control point to load
   *  
   * @internal 
   *   @history 2010-12-10 Tracie Sucharski - Renamed slot loadPoint to
   *                          loadPointImages.
   */
  void QnetFileTool::loadPointImages(ControlPoint *point) {
    for (int i = 0; i < point->GetNumMeasures(); i++) {
      string cubeSN = (*point)[i]->GetCubeSerialNumber();
      loadImage(cubeSN.c_str());
    }
  }

  /**
   *  Sets save net flag to true.
   */
  void QnetFileTool::setSaveNet() {
    p_saveNet = true;
  }


}
