#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>

#include "Application.h"
#include "Filename.h"
#include "MdiCubeViewport.h"
#include "QnetFileTool.h"
#include "SerialNumber.h"
#include "SerialNumberList.h"
#include "Workspace.h"

#include "qnet.h"
using namespace Qisis::Qnet;
using namespace std;

namespace Qisis {
  /** 
   * Constructor 
   *  
   * @internal 
   *   @history 2008-12-10 Jeannie Walldren - Reworded "What's this?" description
   *                          for saveAs action. Changed "Save As" action text to
   *                          match QnetTool's "Save As" action
   */
  QnetFileTool::QnetFileTool (QWidget *parent) : Qisis::FileTool(parent) {
    openAction()->setToolTip("Open network");
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
   * 
   */
  void QnetFileTool::open () {

    //  If network already opened, prompt for saving
    if (g_serialNumberList != NULL && p_saveNet) {
      //  If control net has been changed , prompt for user to save
      int resp = QMessageBox::warning((QWidget*)parent(),"Qnet",
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
    QString list = QFileDialog::getOpenFileName((QWidget*)parent(),
                                                "Select a list of cubes",
                                                ".",
                                                filter);
    if (list.isEmpty()) return;

    // Find directory and save for use in file dialog for net file
    Isis::Filename file(list.toStdString());
    QString dir = file.absolutePath();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    // Use the list to get serial numbers and polygons
    try {
      if (g_serialNumberList != NULL) {
        delete g_serialNumberList;
        g_serialNumberList = NULL;
      }
      g_serialNumberList = new Isis::SerialNumberList(list.toStdString());

      if (g_controlNetwork != NULL) {
        delete g_controlNetwork;
        g_controlNetwork = NULL;
      }
    }
    catch (Isis::iException &e) {
      QString message = "Error processing cube list.  \n";
      string errors = e.Errors();
      message += errors.c_str();
      e.Clear ();
      QMessageBox::information((QWidget *)parent(),"Error",message);
      QApplication::restoreOverrideCursor();
      return;
    }

    QApplication::restoreOverrideCursor();
    filter = "Control net (*.net *.cnet *.ctl);;";
    filter += "Pvl file (*.pvl);;";
    filter += "Text file (*.txt);;";
    filter += "All (*)";
    cNetFilename = QFileDialog::getOpenFileName((QWidget*)parent(),
                                               "Select a control network",
                                               dir,
                                               filter);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (cNetFilename.isEmpty()) {
      g_controlNetwork = new Isis::ControlNet();
      g_controlNetwork->SetType(Isis::ControlNet::ImageToGround);
      g_controlNetwork->SetUserName(Isis::Application::UserName());
    }
    else {
      try {
        g_controlNetwork = new Isis::ControlNet(cNetFilename.toStdString());
      }
      catch (Isis::iException &e) {
        QString message = "Invalid control network.  \n";
        string errors = e.Errors();
        message += errors.c_str();
        e.Clear ();
        QMessageBox::information((QWidget *)parent(),"Error",message);
        QApplication::restoreOverrideCursor();
        return;
      }
    }

    //  Initialize cameras for control net
    g_controlNetwork->SetImages (*g_serialNumberList);

    serialNumberListUpdated();
    emit controlNetworkUpdated(cNetFilename);
    QApplication::restoreOverrideCursor();
    return;
  }

  /**
   * Exit the program
   */
  void QnetFileTool::exit() {
    //  If control net has been changed , prompt for user to save
    if (p_saveNet) {
      int resp = QMessageBox::warning((QWidget*)parent(),"QnetTool",
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
    QString fn=QFileDialog::getSaveFileName((QWidget*)parent(),
                                            "Choose filename to save under",
                                            ".", filter);
    if ( !fn.isEmpty() ) {
      try {
        g_controlNetwork->Write(fn.toStdString());
      } 
      catch (Isis::iException &e) {
        QString message = "Error saving control network.  \n";
        string errors = e.Errors();
        message += errors.c_str();
        e.Clear ();
        QMessageBox::information((QWidget *)parent(),"Error",message);
        return;
      }
    }
    else {
      QMessageBox::information((QWidget *)parent(),
                               "Error","Saving Aborted");
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
   *   @history  2008-10-08 Tracie Sucharski - Do not display cube if it is
   *                             already displayed, set as active window.
   *   @history 2008-12-10 Jeannie Walldren - Fixed documentation
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::" since "using
   *                          namespace std"
   *  
   */
  void QnetFileTool::loadImage(const QString &serialNumber) {
    if (g_serialNumberList->HasSerialNumber(serialNumber.toStdString())) {
      string tempFilename = g_serialNumberList->Filename(serialNumber.toStdString());
      QString filename = tempFilename.c_str();
      QVector< MdiCubeViewport * > * cvpList = 
      g_vpMainWindow->workspace()->cubeViewportList();
      bool found = false;
      for (int i=0; i<(int)cvpList->size(); i++) {
        string sn = Isis::SerialNumber::Compose(*((*cvpList)[i]->cube()));
        if (sn == serialNumber.toStdString()) {
          g_vpMainWindow->workspace()->setActiveSubWindow((QMdiSubWindow *)(*cvpList)[i]->parentWidget());
          found = true;
          break;
        }
      }
      if (!found) emit fileSelected(filename);
    }
    else {
      // TODO:  Handle error?
    }
  }

  /**
   * Load given point 
   *  
   * @param point Control point to load 
   */
  void QnetFileTool::loadPoint(Isis::ControlPoint *point) {
    for (int i=0; i<point->Size(); i++) {
      string cubeSN = (*point)[i].CubeSerialNumber();
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
