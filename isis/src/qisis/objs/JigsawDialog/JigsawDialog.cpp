#include "JigsawDialog.h"

#include <QDebug>
#include <QScrollBar>
#include <QThread>

#include "JigsawSetupDialog.h"

#include "BundleAdjust.h"
#include "BundleSolutionInfo.h"
#include "Directory.h"
#include "FileName.h"
#include "IException.h"
#include "JigsawSetupDialog.h"
#include "Control.h"
#include "iTime.h"
#include "Process.h"
#include "Project.h"
#include "ui_JigsawDialog.h"

namespace Isis {

  /**
   * @brief Constructor.
   *
   * Creates a dialog for running a jigsaw (bundle adjustment) and changing the solve settings.
   *
   * @param Project *project Pointer to the project this dialog belongs to.
   * @param QWidget *parent Pointer to parent widget.
   */
  JigsawDialog::JigsawDialog(Project *project, QWidget *parent) :
      QDialog(parent), m_ui(new Ui::JigsawDialog) {
    m_project = project;
    m_selectedControl = NULL;
    init();
  }


  /**
   * @brief Constructor that takes bundle settings and a selected control.
   *
   * Creates a dialog after the jigsaw solve settings have been set up and a control has been
   * selected.
   *
   * @param Project *project Pointer to the project this dialog belongs to.
   * @param BundleSettingsQsp bundleSettings Settings to give to this dialog to use for a jigsaw.
   * @param Control *selectedControl Pointer to the selected control to adjust.
   * @param QWidget *parent Pointer to the parent widget.
   */
  JigsawDialog::JigsawDialog(Project *project,
                             BundleSettingsQsp bundleSettings,
                             Control *selectedControl,
                             QWidget *parent) : QDialog(parent), m_ui(new Ui::JigsawDialog) {

    m_project = project;
    m_bundleSettings = bundleSettings;
    m_selectedControl = selectedControl;
    init();
  }


  /**
   * @brief Constructor delegate.
   *
   * Delegate method that helps the constructors. This is used to reduce repeated code.
   */
  void JigsawDialog::init() {
    m_ui->setupUi(this);

    // Three buttons: Accept, Reject, Close. Initially only close is enabled.
    // After a bundle is successfully run, reject and accept are enabled.
    // If aborting a bundle, only close will be enabled.
    m_accept = new QPushButton(tr("&Accept"));
    m_reject = new QPushButton(tr("&Reject"));
    m_close = new QPushButton(tr("&Close"));
    m_accept->setEnabled(false);
    m_reject->setEnabled(false);
    m_close->setEnabled(true);

    // Add the buttons to the QDialogButtonBox defined in the UI file.
    m_ui->buttonBox->addButton(m_accept, QDialogButtonBox::ActionRole);
    m_ui->buttonBox->addButton(m_reject, QDialogButtonBox::ActionRole);
    m_ui->buttonBox->addButton(m_close, QDialogButtonBox::AcceptRole);

    // Accept will handle saving the results.
    connect(m_accept, SIGNAL(clicked(bool)),
           this, SLOT(acceptBundleResults()));
    // Reject will handle discarding the results.
    connect(m_reject, SIGNAL(clicked(bool)),
           this, SLOT(rejectBundleResults()));

    m_bundleAdjust = NULL;

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


  /**
   * Destructor.
   */
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
    // Once a bundle is run, the previous results cannot be accepted or rejected.
    m_accept->setEnabled(false);
    m_reject->setEnabled(false);

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

//    qDebug()<<"JigsawDialog::on_jigsawRun  #imagelists = "<<m_project->images().count();
//    qDebug()<<"JigsawDialog::on_jigsawRun  images = "<<m_project->images();

      m_bundleAdjust = new BundleAdjust(m_bundleSettings, *m_selectedControl, m_project->images(),
                                        false);

      m_bundleAdjust->moveToThread(bundleThread);

      // Track the status updates bundle adjust gives and update the dialog.
      connect( m_bundleAdjust, SIGNAL( statusUpdate(QString) ),
               this, SLOT( outputBundleStatus(QString) ) );

      // Track any errors that may occur during the bundle adjust and update the dialog.
      connect( m_bundleAdjust, SIGNAL( error(QString) ),
               this, SLOT( errorString(QString) ) );

      // Update the iteration dialog element when the bundle updates its iteration count.
      connect( m_bundleAdjust, SIGNAL( iterationUpdate(int, double) ),
               this, SLOT( updateIterationSigma0(int, double) ) );

      // When we start the bundle thread, run the bundle adjustment.
      connect( bundleThread, SIGNAL( started() ),
               m_bundleAdjust, SLOT( solveCholesky() ) );

      // When the bundle adjust says results are ready, we can allow the dialog to update the
      // project as necessary.
      connect( m_bundleAdjust, SIGNAL( resultsReady(BundleSolutionInfo *) ),
               this, SLOT( bundleFinished(BundleSolutionInfo *) ) );

      // Schedule the bundle thread for deletion when it finishes.
      connect( bundleThread, SIGNAL( finished() ),
               bundleThread, SLOT( deleteLater() ) );

      // ken testing
      // Notify the dialog that the bundle thread is finished, and update the gui elements.
      connect( bundleThread, SIGNAL( finished() ),
               this, SLOT( notifyThreadFinished() ) );

      // Tell the thread to quit (stop) when the bundle adjust finishes (successfully or not)
      connect( m_bundleAdjust, SIGNAL( finished() ),
               bundleThread, SLOT( quit() ) );

      // Schedule the bundle adjustment for deletion.
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

    m_ui->useLastSettings->setEnabled(true);
  }


  /**
   * Accepts the bundle results and saves them to the project. The "Accept" and "Reject" buttons
   * will be disabled.
   */
  void JigsawDialog::acceptBundleResults() {
    qDebug() << "\n*** Accepting the results...\n";

    m_accept->setEnabled(false);
    m_reject->setEnabled(false);

    m_bundleSolutionInfo->setRunTime( Isis::iTime::CurrentLocalTime().toLatin1().data() );
    m_project->addBundleSolutionInfo( new BundleSolutionInfo(*m_bundleSolutionInfo) );

    // create output control net
    // Write the new jigged control net with correct path to results folder + runtime
    FileName jiggedControlName(m_project->bundleSolutionInfoRoot() + "/" +
                               m_bundleSolutionInfo->runTime() + "/" +
                               FileName(m_bundleSolutionInfo->controlNetworkFileName()).name());
    m_bundleSolutionInfo->bundleResults().outputControlNet()->Write(jiggedControlName.toString());


      //TODO: move correlation matrix to correct position in project directory

  //
  //       for (int i = 0; i < bundleAdjustment.images(); i++) {
  //         Process p;
  //         CubeAttributeInput inAtt;
  //  4-18-2017 TLS  bundleAdjustment.fileName is an .ecub.  code encompassed by my comment
  //                 is test code.

//      Cube *c = 
//    Cube *c = p.SetInputCube(bundleAdjustment.fileName(i), inAtt, ReadWrite);







  //  4-18-2017 TLS  bundleAdjustment.fileName is an .ecub.  code encompassed by my comment
  //                 is test code.
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
    //This bundle was bad so we should delete all remenants.

    //TODO: delete correlation matrix cov file...
    //TODO: delete bundle results object

//       m_ui->convergenceStatusLabel->setText("Bundle did not converge, camera pointing NOT updated");

  }


  /**
   * Rejects the bundle results and discards them. The "Accept" and "Reject" buttons will be
   * disabled.
   */
  void JigsawDialog::rejectBundleResults() {
    qDebug() << "\n*** Rejecting the results...\n";
    m_accept->setEnabled(false);
    m_reject->setEnabled(false);

    m_bundleSolutionInfo = NULL;
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


  /**
   * @brief Notifies the dialog that the bundle thread has finished.
   *
   * This slot is used to notify the dialog that the bundle has finished. The bundle thread
   * finishes when the bundle adjust finishes (either successfully or unsuccessfully, or if the
   * user aborts the run).
   */
  void JigsawDialog::notifyThreadFinished() {
    //QString str = "\nThread Finished signal received";
    //m_ui->statusUpdatesLabel->setText( m_ui->statusUpdatesLabel->text().append(str) );

    // set Run button text back to "Run"
    m_ui->JigsawRunButton->setText("&Run");
    // Since this slot is invoked when the thread finishes, the bundle adjustment is no longer
    // running.
    m_bRunning = false;
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
    // Since results are available, the user can accept (save) or reject(discard) the results.
    m_accept->setEnabled(true);
    m_reject->setEnabled(true);
    // m_close->setEnabled(false);

    m_bundleSolutionInfo = bundleSolutionInfo;
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
