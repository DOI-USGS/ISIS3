#include "JigsawDialog.h"

#include <QDebug>
#include <QScrollBar>
#include <QThread>

#include "JigsawSetupDialog.h"
//#include "ui_JigsawDialog.h"

#include "BundleAdjust.h"
#include "BundleSolutionInfo.h"
#include "Directory.h"
#include "IException.h"
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

    m_bundleAdjust = NULL;
    m_project = project;
    m_selectedControl = NULL;

    m_bRunning = false;

    QList<BundleSolutionInfo *> bundleSolutionInfo = m_project->bundleSolutionInfo();
    if (bundleSolutionInfo.size() <= 0) {
      m_ui->useLastSettings->setEnabled(false);
    }

    m_ui->iterationLcdNumber->setDigitCount(3);

    m_ui->sigma0LcdNumber->setMode(QLCDNumber::Dec);
    m_ui->sigma0LcdNumber->setDigitCount(5);

    setWindowFlags(Qt::WindowStaysOnTopHint);
  }


  JigsawDialog::~JigsawDialog() {
    if (m_ui) {
      delete m_ui;
    }
    m_ui = NULL;
  }


  void JigsawDialog::on_JigsawSetupButton_pressed() {

    // Each time the SetUp button is pressed, create JigsawSetupDialog object with
    // project,
    // useLastSettings = matches check box
    // readOnly = false,
    // parent = this
    JigsawSetupDialog setupdlg(m_project, 
                               m_ui->useLastSettings->isChecked(), 
                               false, 
                               this);

    // how to set up default settings object if last is not used or doesnt exist ???
    // if (m_bundleSettings != NULL && setupdlg.useLastSettings()) {
    // }
    // if (m_bundleSettings == NULL) {
    //   m_bundleSettings = setupdlg.bundleSettings();
    // }

    if (setupdlg.exec() == QDialog::Accepted) {
      m_selectedControlName = setupdlg.selectedControlName();
      m_selectedControl = setupdlg.selectedControl();
      m_bundleSettings = setupdlg.bundleSettings();
    }
  }


  void JigsawDialog::on_JigsawRunButton_clicked() {

    if (!m_bRunning) {
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
         BundleSettingsQsp lastBundleSettings = (bundleSolutionInfo.last())->bundleSettings();

         if (lastBundleSettings) {
           m_bundleSettings = lastBundleSettings;
         }
      }

    // Non threaded *************************************************************
//    SerialNumberList snlist;

//    QList<ImageList *> imagelists = m_project->images();

//    for (int i = 0; i < imagelists.size( ); i++) {
//      ImageList* list = imagelists.at(i);
//      for (int j = 0; j < list->size(); j++) {
//        Image* image = list->at(j);
//        snlist.Add(image->fileName());
//      }
//    }

//    BundleAdjust bundleAdjustment(m_bundleSettings, *m_selectedControl, snlist, false);

//    connect( &bundleAdjustment, SIGNAL( statusUpdate(QString) ),
//             this, SLOT( outputBundleStatus(QString) ) );

//    // clear dialog output window
//    m_ui->statusUpdatesLabel->clear();

//    BundleSolutionInfo br = bundleAdjustment.solveCholeskyBR();

//    bundleFinished(&br);
    // **************************************************************************

    // Threaded *****************************************************************
       QThread *bundleThread = new QThread;

       // Takes too long to copy/recreate the serial number list
//       BundleAdjust *ba = new BundleAdjust(m_bundleSettings,
//                                           *m_selectedControl,
//                                           m_project->images(),
//                                           false);

    // Use the run time to create a uniquely named directory in the results
    // directory. This run time directory will contain the output files for
    // this run (along with correlation matrix and serialized bundle
    // information files).

// KLE - test commenting out 2016-05-30
//    QString runTime = Isis::iTime::CurrentLocalTime().toLatin1().data();
//    QDir cwd = m_project->addBundleSolutionInfoFolder(runTime);
//    QString path = cwd.absolutePath() + "/";
//    m_bundleSettings->setOutputFilePrefix(path);
       
//     m_bundleAdjust = new BundleAdjust(m_bundleSettings,
//                                            *m_selectedControl,
//                                            m_project->images(),
//                                            false);
    m_bundleAdjust = new BundleAdjust(m_bundleSettings, *m_selectedControl, m_project->images(),
                                      false);

       m_bundleAdjust->moveToThread(bundleThread);

       connect( m_bundleAdjust, SIGNAL( statusUpdate(QString) ),
                this, SLOT( outputBundleStatus(QString) ) );

       connect( m_bundleAdjust, SIGNAL( error(QString) ),
                this, SLOT( errorString(QString) ) );

       connect( m_bundleAdjust, SIGNAL( bundleException(QString) ),
                this, SLOT( reportException(QString) ) );

       connect( m_bundleAdjust, SIGNAL( iterationUpdate(int, double) ),
                this, SLOT( updateIterationSigma0(int, double) ) );

       connect( bundleThread, SIGNAL( started() ),
                m_bundleAdjust, SLOT( solveCholesky() ) );

       connect( m_bundleAdjust, SIGNAL( resultsReady(BundleSolutionInfo *) ),
                this, SLOT( bundleFinished(BundleSolutionInfo *) ) );

       connect( bundleThread, SIGNAL( finished() ),
                bundleThread, SLOT( deleteLater() ) );

       // ken testing
       connect( bundleThread, SIGNAL( finished() ),
                this, SLOT( notifyThreadFinished() ) );

       connect( m_bundleAdjust, SIGNAL( finished() ),
                bundleThread, SLOT( quit() ) );

       connect( m_bundleAdjust, SIGNAL( finished() ),
                m_bundleAdjust, SLOT( deleteLater() ) );
       // ken testing

       bundleThread->start();

       // change "Run" button text to "Abort" (or maybe pause)
       m_bRunning = true;
       m_ui->JigsawRunButton->setText("&Abort");
       update();
    }
    else {
      // if bundle is running then we want to abort
      if (m_bRunning) {
        m_bundleAdjust->abortBundle();
        m_bRunning = false;
        m_ui->JigsawRunButton->setText("&Aborting...");
        update();
      }
    }

    // ********************************************** ****************************
//#if 0
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    m_ui->useLastSettings->setEnabled(true);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//#endif
  }


  /**
   * Update the label or text edit area with the most recent status update by appending to list and
   * refreshing.
   *
   * @param status Current status of bundle.
   */
  void JigsawDialog::outputBundleStatus(QString status) {
    QString updateStr = "\n" + status;

    m_ui->statusUpdatesLabel->setText( m_ui->statusUpdatesLabel->text().append(updateStr) );

    m_ui->statusUpdateScrollArea->verticalScrollBar()->setSliderPosition(
          m_ui->statusUpdateScrollArea->verticalScrollBar()->maximum());

    update();
  }


  /**
   * Update the label or text edit area with the error message by appending to list and refreshing.
   *
   * @param error Error status of bundle.
   */
  void JigsawDialog::errorString(QString error) {
    QString errorStr = "\n" + error;
    m_ui->statusUpdatesLabel->setText( m_ui->statusUpdatesLabel->text().append(errorStr) );

    m_ui->statusUpdateScrollArea->verticalScrollBar()->setSliderPosition(
          m_ui->statusUpdateScrollArea->verticalScrollBar()->maximum());

    update();
  }


  /**
   * Update the label or text edit area with the error message by appending to list and refreshing.
   *
   * @param error Error status of bundle.
   */
  void JigsawDialog::reportException(QString exception) {
    QString exceptionStr = "\n" + exception;
    m_ui->statusUpdatesLabel->setText( m_ui->statusUpdatesLabel->text().append(exceptionStr) );

    m_ui->statusUpdateScrollArea->verticalScrollBar()->setSliderPosition(
          m_ui->statusUpdateScrollArea->verticalScrollBar()->maximum());

    update();
  }


  /**
   * Update the label or text edit area with the error message by appending to list and refreshing.
   *
   * @param error Error status of bundle.
   */
  void JigsawDialog::updateIterationSigma0(int iteration, double sigma0) {
    m_ui->iterationLcdNumber->display(iteration);
    m_ui->sigma0LcdNumber->display(sigma0);

    update();
  }


  void JigsawDialog::notifyThreadFinished() {
    QString str = "\nThread Finished signal received";
    m_ui->statusUpdatesLabel->setText( m_ui->statusUpdatesLabel->text().append(str) );

    // set Run button text back to "Run"
    m_ui->JigsawRunButton->setText("&Run...");

    update();

    m_ui->statusUpdateScrollArea->verticalScrollBar()->setSliderPosition(
          m_ui->statusUpdateScrollArea->verticalScrollBar()->maximum());
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
    if ( bundleSolutionInfo->bundleResults().converged() ) {
      bundleSolutionInfo->setRunTime( Isis::iTime::CurrentLocalTime().toLatin1().data() );
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
  //       could still be worthless.
  }  
}

  /**
   * Run bundle in a thread or abort currently running bundle thread.
   *
   * 2015-08-24 Notes added: Ken Edmundson
   *
   * If a bundle is NOT currently running, we ...
   *   1) create a QThread object (bundleThread)
   *   2) create a new pointer to a BundleAdjust object (m_bundleAdjust)
   *   3) move the BundleAdjust object to the QThread
   *   4) connect signals & slots to ...
   *     a) emit status and error updates from the BundleAdjust object to this dialog
   *     b) mark the QThread and BundleAdjust objects to "deleteLater"
   *   5) start the QThread which invokes the BundleAdjust::solveCholesky() method
   *   6) set this dialogs member m_bRunning to "true"
   *   7) change the text on the "Run" button to "Abort"
   *
   * If a bundle IS currently running, we ...
   *   1) set BundleAdjust::m_abort member variable to "true"
   *   2) set this dialogs member m_bRunning to "false"
   *   3) change the text on the "Run" button to "Aborting..."
   *
   * We still need to ...
   *   1) on bundle abort, change "Run" button text back to "Run"
   *   2) on bundle success, don't immediately put BundleSolutionInfo out to project, but give the
   *      user the opportunity to "Save Results" or "Discard Results"
   *   3) probably get rid of the big text output scroll area and replace with individual boxes for
   *      pertinent statistics like
   *      a) iteration number
   *      b) sigma0
   *      c) what else?
   *   4) get rid of the serial number list ...
   *      a) I think this should be obsolete now as the Image class contains a serial number member
   *      b) the Image class will probably also need an observation number member
   *      c) regarding the "held list", I think that the ability to set an individual observation's
   *         solve settings eliminates the need for this list. Note that in the past, if an image
   *         was in the held list, all 3D points associated with measures on the image were held
   *         also. That still doesn't make sense to me (maybe it will someday?)
   *
   * For a good, brief tutorial on the proper use of QThreads, see ...
   *
   *   "https://mayaposch.wordpress.com/2011/11/01/how-to-really-truly-use-qthreads-the-full-
   *    explanation/"
   *
   */
/*
  void JigsawDialog::on_JigsawRunButton_clicked() {

    // Non threaded *************************************************************
    SerialNumberList snlist;

    QList<ImageList *> imagelists = m_project->images();

    for (int i = 0; i < imagelists.size( ); i++) {
      ImageList* list = imagelists.at(i);
      for (int j = 0; j < list->size(); j++) {
        Image* image = list->at(j);
        snlist.Add(image->fileName());
      }
    }

    BundleAdjust bundleAdjustment(m_bundleSettings, *m_selectedControl, snlist, false);

    connect( &bundleAdjustment, SIGNAL( statusUpdate(QString) ),
             this, SLOT( outputBundleStatus(QString) ) );

    // clear dialog output window
    m_ui->statusUpdatesLabel->clear();

    BundleSolutionInfo br = bundleAdjustment.solveCholeskyBR();

    bundleFinished(&br);
    // **************************************************************************
    
    // ********************************************** ****************************
//#if 0
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    m_ui->useLastSettings->setEnabled(true);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//#endif
  }
*/
