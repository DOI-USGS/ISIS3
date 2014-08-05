#include "JigsawDialog.h"

#include <QDebug>

#include "JigsawSetupDialog.h"
#include "ui_JigsawDialog.h"

#include "BundleAdjust.h"
#include "BundleResults.h"
#include "BundleSettings.h"
#include "Control.h"
#include "iTime.h"
#include "Process.h"
#include "Project.h"

namespace Isis {

  JigsawDialog::JigsawDialog(Project *project, QWidget *parent) : 
                      QDialog(parent), m_ui(new Ui::JigsawDialog) {
    m_ui->setupUi(this);

    m_project = project;
    m_selectedControl = NULL;
    m_bundleSettings = NULL;

    QList<BundleResults *> bundleResults = m_project->bundleResults();
    if (bundleResults.size() <= 0) {
      m_ui->useLastSettings->setEnabled(false);
    }

    setWindowFlags(Qt::WindowStaysOnTopHint);
  }

  JigsawDialog::~JigsawDialog() {
    if (m_ui) {
      delete m_ui;
    }
    m_ui = NULL;
  }



  void JigsawDialog::on_JigsawSetupButton_pressed() {

    JigsawSetupDialog setupdlg(m_project, true, false, this);

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

    QList<BundleResults *> bundleResults = m_project->bundleResults();
    if (m_ui->useLastSettings->isChecked() && bundleResults.size() > 0) {
       BundleSettings *lastBundleSettings = (bundleResults.last())->bundleSettings();

       if (lastBundleSettings) {
         m_bundleSettings = lastBundleSettings;
       }
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


    BundleAdjust bundleAdjustment(*m_bundleSettings, *m_selectedControl, snlist, false);

    connect( &bundleAdjustment, SIGNAL( statusUpdate(QString) ),
             this, SLOT( outputBundleStatus(QString) ) );
    connect( &bundleAdjustment, SIGNAL( bundleConvergence(bool) ),
             this, SLOT( updateConvergenceStatus(bool) ) );
    
    bundleAdjustment.solveCholesky();

//     if (m_ui->convergenceStatusLabel->text() != "Bundle converged, camera pointing updated") {
//       bundleConvergence(false);
//     }
    
//#if 0
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    m_ui->useLastSettings->setEnabled(true);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//#endif
  }


  
  /**
   * Update the label or text edit area with the most recent status update by appending to
   * list and refreshing.
   *
   * @param status Current status of bundle.
   */
  void JigsawDialog::outputBundleStatus(QString status) {
    QString update = "\n" + status;
    m_ui->statusUpdatesLabel->setText( m_ui->statusUpdatesLabel->text().append(update) );
  }



  /**
   * This slot will be called if the bundle converges.
   *
   * maybe give the dialog a convergence status label that will say
   *   "Bundle has not converged. Camera pointing not updated."
   * until the bundle is done.
   */
  void JigsawDialog::updateConvergenceStatus(bool converged) {
    if (converged) {
      // create BundleResults object
      // BundleResults results = bundleAdjustment.solveCholesky();
      // should be returned by BundleAdjustment?
      QString fname = m_selectedControl->fileName();
      BundleResults *dummyBundleResults = new BundleResults(*m_bundleSettings, fname);
      dummyBundleResults->setRunTime(Isis::iTime::CurrentLocalTime().toAscii().data());
      m_project->addBundleResults(dummyBundleResults);

      // create output control net
  //     bundleAdjustment.controlNet()->Write("ONET.net");
  //
  //       for (int i = 0; i < bundleAdjustment.images(); i++) {
  //         Process p;
  //         CubeAttributeInput inAtt;
  //         Cube *c = p.SetInputCube(bundleAdjustment.fileName(i), inAtt, ReadWrite);
  //         //check for existing polygon, if exists delete it
  //         if (c->label()->hasObject("Polygon")) {
  //           c->label()->deleteObject("Polygon");
  //         }
  //
  //         // check for CameraStatistics Table, if exists, delete
  //         for (int iobj = 0; iobj < c->label()->objects(); iobj++) {
  //           PvlObject obj = c->label()->object(iobj);
  //           if (obj.name() != "Table") continue;
  //           if (obj["Name"][0] != QString("CameraStatistics")) continue;
  //           c->label()->deleteObject(iobj);
  //           break;
  //         }
  //
  //         //  Get Kernel group and add or replace LastModifiedInstrumentPointing
  //         //  keyword.
  //         Table cmatrix = bundleAdjustment.cMatrix(i);
  //         QString jigComment = "Jigged = " + Isis::iTime::CurrentLocalTime();
  //         cmatrix.Label().addComment(jigComment);
  //         Table spvector = bundleAdjustment.spVector(i);
  //         spvector.Label().addComment(jigComment);
  //         c->write(cmatrix);
  //         c->write(spvector);
  //         p.WriteHistory(*c);
  //       }

  //       m_ui->convergenceStatusLabel->setText("Bundle converged, camera pointing updated");
    }
    else {
//       m_ui->convergenceStatusLabel->setText("Bundle did not converge, camera pointing NOT updated");
    }

  }
}

