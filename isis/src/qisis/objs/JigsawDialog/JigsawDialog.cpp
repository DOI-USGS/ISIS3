#include "JigsawDialog.h"

#include <QDebug>

#include "BundleAdjust.h"
#include "BundleResults.h"
#include "BundleSettings.h"
#include "Control.h"
#include "iTime.h"
#include "JigsawSetupDialog.h"
#include "ui_JigsawDialog.h"
#include "Process.h"
#include "Project.h"

namespace Isis {

  JigsawDialog::JigsawDialog(Project *project, QWidget *parent) : 
                      QDialog(parent), m_ui(new Ui::JigsawDialog) {
    m_ui->setupUi(this);

    m_project = project;
    m_selectedControl = NULL;
    m_bundleSettings = NULL;

    setWindowFlags(Qt::WindowStaysOnTopHint);
  }

  JigsawDialog::~JigsawDialog() {
    if (m_ui) {
      delete m_ui;
    }
    m_ui = NULL;
  }



  void JigsawDialog::on_JigsawSetupButton_pressed() {

    JigsawSetupDialog setupdlg(m_project, this);

    // if (m_bundleSettings != NULL && setupdlg.useLastSettings()) {
    // }

    if (setupdlg.exec() == QDialog::Accepted) {
      m_selectedControlName = setupdlg.selectedControlName();
      m_selectedControl = setupdlg.selectedControl();
      m_bundleSettings = setupdlg.bundleSettings();
    }
  }



  void JigsawDialog::on_JigsawRunButton_clicked() {

    // ??? warning dialogs ???
    if (m_selectedControl == NULL) {
    }

    if (m_project->images().size() == 0) {
    }

    // assume defaults or send warning???
    if (m_bundleSettings == NULL) {
    }

    SerialNumberList snlist;

    QList<ImageList *> imagelists = m_project->images();

    for (int i = 0; i < imagelists.size(); i++) {
      ImageList* list = imagelists.at(i);
      for (int j = 0; j < list->size(); j++) {
        Image* image = list->at(j);
        snlist.Add(image->fileName());
        qDebug() << image->fileName();
      }
    }

//    ControlNet *cnet = m_selectedControl->controlNet();

//    BundleAdjust bundleAdjustment(*m_bundleSettings, *cnet, snlist, false);
    BundleAdjust bundleAdjustment(*m_bundleSettings, *m_selectedControlName, snlist, false);
    // run bundle (thread with BundleThread::QThread) - pump output to modeless dialog
    // BundleResults results = bundleAdjustment.solveCholesky();
    bundleAdjustment.solveCholesky();

//     if ( !bundleAdjustment.isConverged() ) {
//        qDebug() << "Status = Bundle did not converge, camera pointing NOT updated";
//      }
//      else {
//        bundleAdjustment.controlNet()->Write("ONET.net");
//        for (int i = 0; i < bundleAdjustment.images(); i++) {
//          Process p;
//          CubeAttributeInput inAtt;
//          Cube *c = p.SetInputCube(bundleAdjustment.fileName(i), inAtt, ReadWrite);
//          //check for existing polygon, if exists delete it
//          if (c->label()->hasObject("Polygon")) {
//            c->label()->deleteObject("Polygon");
//          }
//
//          // check for CameraStatistics Table, if exists, delete
//          for (int iobj = 0; iobj < c->label()->objects(); iobj++) {
//            PvlObject obj = c->label()->object(iobj);
//            if (obj.name() != "Table") continue;
//            if (obj["Name"][0] != QString("CameraStatistics")) continue;
//            c->label()->deleteObject(iobj);
//            break;
//          }
//
//          //  Get Kernel group and add or replace LastModifiedInstrumentPointing
//          //  keyword.
//          Table cmatrix = bundleAdjustment.cMatrix(i);
//          QString jigComment = "Jigged = " + Isis::iTime::CurrentLocalTime();
//          cmatrix.Label().addComment(jigComment);
//          Table spvector = bundleAdjustment.spVector(i);
//          spvector.Label().addComment(jigComment);
//          c->write(cmatrix);
//          c->write(spvector);
//          p.WriteHistory(*c);
//        }
//        qDebug() << "Status = Camera pointing updated";
//      }

#if 0
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // create dummy bundle results and add to project
    BundleSettings *settings = new BundleSettings();
    QString fname = m_selectedControl->fileName();
//    BundleResults *bundleResults = new BundleResults(*settings,*cnet, fname);
    BundleResults *bundleResults = new BundleResults(*settings, fname);
    bundleResults->setRunTime(Isis::iTime::CurrentLocalTime().toAscii().data());
    m_project->addBundleResults(bundleResults);


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif
  }
}

