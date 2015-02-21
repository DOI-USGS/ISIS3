#include "JigsawDialog.h"

#include <QDebug>
#include <QThread>

#include "JigsawSetupDialog.h"
#include "ui_JigsawDialog.h"

#include "BundleAdjust.h"
#include "BundleSolutionInfo.h"
#include "BundleSettings.h"
#include "JigsawSetupDialog.h"
#include "Control.h"
#include "iTime.h"
#include "Process.h"
#include "Project.h"
#include "ui_JigsawDialog.h"

namespace Isis {

  JigsawDialog::JigsawDialog(Project *project, QWidget *parent) : 
                      QDialog(parent), m_ui(new Ui::JigsawDialog) {
    m_ui->setupUi(this);

    m_project = project;
    m_selectedControl = NULL;
    m_bundleSettings = NULL;

    QList<BundleSolutionInfo *> bundleSolutionInfo = m_project->bundleSolutionInfo();
    if (bundleSolutionInfo.size() <= 0) {
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

    QList<BundleSolutionInfo *> bundleSolutionInfo = m_project->bundleSolutionInfo();
    if (m_ui->useLastSettings->isChecked() && bundleSolutionInfo.size() > 0) {
       BundleSettings *lastBundleSettings = (bundleSolutionInfo.last())->bundleSettings();

       if (lastBundleSettings) {
         m_bundleSettings = lastBundleSettings;
       }
    }

    // Non threaded *************************************************************
    SerialNumberList snlist;

    QList<ImageList *> imagelists = m_project->images();

    for (int i = 0; i < imagelists.size(); i++) {
      ImageList* list = imagelists.at(i);
      for (int j = 0; j < list->size(); j++) {
        Image* image = list->at(j);
        snlist.Add(image->fileName());
      }
    }

    BundleAdjust bundleAdjustment(*m_bundleSettings, *m_selectedControl, snlist, false);

    connect( &bundleAdjustment, SIGNAL( statusUpdate(QString) ),
             this, SLOT( outputBundleStatus(QString) ) );

    BundleSolutionInfo br = bundleAdjustment.solveCholeskyBR();

    bundleFinished(&br);
    // **************************************************************************
    
    // Threaded *****************************************************************
//     QThread *bundleThread = new QThread;
// 
//     // Takes too long to copy/recreate the serial number list
//     BundleAdjust *ba = new BundleAdjust(*m_bundleSettings,
//                                         *m_selectedControl,
//                                         m_project->images(),
//                                         false);
//     ba->moveToThread(bundleThread);
// 
//     connect( ba, SIGNAL( statusUpdate(QString) ),
//              this, SLOT( outputBundleStatus(QString) ) );
// 
//     connect( bundleThread, SIGNAL( started() ),
//              ba, SLOT( solveCholesky() ) );
// 
//     connect( ba, SIGNAL( resultsReady(BundleSolutionInfo *) ),
//              this, SLOT( bundleFinished(BundleSolutionInfo *) ) );
// 
//     connect( bundleThread, SIGNAL( finished() ),
//              bundleThread, SLOT( deleteLater() ) );
// 
//     bundleThread->start();
    // **************************************************************************

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
   * This method will be called when the bundle is complete. This method will only be used when the
   * bundle is threaded. It can be used when the bundle is not threaded but we don't need it
   * because we have solveCholeskyBR().
   *
   *
   *
   * @param bundleSolutionInfo The results of the bundle run.
   */
  void JigsawDialog::bundleFinished(BundleSolutionInfo *bundleSolutionInfo) {
    if ( bundleSolutionInfo->bundleResults()->converged() ) {
      bundleSolutionInfo->setRunTime( Isis::iTime::CurrentLocalTime().toAscii().data() );
      m_project->addBundleSolutionInfo( new BundleSolutionInfo(*bundleSolutionInfo) );

      //TODO: move correlation matrix to correct position in project directory
      
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
    //This bundle was bad so we should delete all remenants.
    
    //TODO: delete correlation matrix cov file...
    //TODO: delete bundle results object

//       m_ui->convergenceStatusLabel->setText("Bundle did not converge, camera pointing NOT updated");
  }

  // TODO: Give user the option to keep or throw away the bundle. Even if the bundle converged it
  //       coulde still be worthless.
  }  
}

