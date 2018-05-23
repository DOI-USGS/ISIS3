#include "JigsawDialog.h"

#include <QtConcurrent>
#include <QDebug>
#include <QDir>
#include <QFuture>
#include <QScrollBar>
#include <QThread>

#include "JigsawSetupDialog.h"

#include "BundleAdjust.h"
#include "BundleSolutionInfo.h"
#include "Cube.h"
#include "Directory.h"
#include "FileName.h"
#include "IException.h"
#include "Image.h"
#include "ImageList.h"
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
                             QString outputControlFileName,
                             QWidget *parent) : QDialog(parent), m_ui(new Ui::JigsawDialog) {
    m_project = project;
    m_bundleSettings = bundleSettings;
    m_selectedControl = selectedControl;
    m_selectedControlName = FileName(selectedControl->fileName()).name();
    m_outputControlName = outputControlFileName;
    init();
  }


  /**
   * @brief Constructor delegate.
   *
   * Delegate method that helps the constructors. This is used to reduce repeated code.
   */
  void JigsawDialog::init() {
    m_ui->setupUi(this);

    // Note: The buttons are added to the UI setup from the JigsawDialog.ui file.
    // These could have been added to the UI file itself (as XML).

    // Three buttons: Accept, Reject, Close. Initially only close is enabled.
    // Close is only disabled when a bundle is running.
    // After a bundle is successfully run, reject and accept are enabled.
    // If aborting a bundle, only close will be enabled.
    m_accept = new QPushButton(tr("&Accept"));
    m_reject = new QPushButton(tr("&Reject"));
    m_close = new QPushButton(tr("&Close"));
    m_accept->setEnabled(false);
    m_reject->setEnabled(false);
    m_close->setEnabled(true);

    // Add tool tips to the buttons
    m_accept->setToolTip(tr("Accept the bundle results and save them to the project."));
    m_reject->setToolTip(tr("Reject and discard the bundle results. This resets the dialog."));
    m_close->setToolTip(tr("Close this dialog."));

    // Add the buttons to the QDialogButtonBox defined in the UI file.
    // Note that according to the Qt doc'n for QDialogButtonBox, addButton() causes the
    // dialog box to take ownership of the QPushButton's, so we don't manually manage their memory.
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
//    m_bundleSolutionInfo = NULL;

    m_bRunning = false;

    QList<BundleSolutionInfo *> bundleSolutionInfo = m_project->bundleSolutionInfo();
    if (bundleSolutionInfo.size() <= 0) {
      m_ui->useLastSettings->setEnabled(false);
    }

    m_ui->iterationLcdNumber->setDigitCount(3);

    m_ui->sigma0LcdNumber->setMode(QLCDNumber::Dec);
    m_ui->sigma0LcdNumber->setDigitCount(5);

    QString lastSettingsToolTip("Use the settings from the most recently accepted bundle adjust.");
    QString lastSettingsWhat("When checked, the settings from the most recently accepted bundle "
                             "adjust (i.e. the most recent bundle results in the project) will be "
                             "used for running the next bundle adjust when \"Run\" is clicked.");
    m_ui->useLastSettings->setToolTip(lastSettingsToolTip);
    m_ui->useLastSettings->setWhatsThis(lastSettingsWhat);

    setWindowFlags(Qt::WindowStaysOnTopHint);
  }


  /**
   * Destructor.
   */
  JigsawDialog::~JigsawDialog() {
//    if (m_bundleSolutionInfo) {
//      delete m_bundleSolutionInfo;
//    }
    if (m_bundleAdjust) {
      delete m_bundleAdjust;
      m_bundleAdjust = NULL;
    }
    if (m_ui) {
      delete m_ui;
    }
//    m_bundleSolutionInfo = NULL;
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

    // We want to use the current settings if not use the most recently accepted bundle settings.
    // This allows user to click "Setup", make changes, "OK", then click "Setup", and those changes
    // are present in the setup dialog.
    if (m_bundleSettings && !m_ui->useLastSettings->isChecked()) {
      setupdlg.loadSettings(m_bundleSettings);
      // We also tell the setup dialog what the last selected control is.
      setupdlg.selectControl(m_selectedControlName);
    }

    if (setupdlg.exec() == QDialog::Accepted) {
      m_selectedControlName = setupdlg.selectedControlName();
      m_outputControlName = setupdlg.outputControlName();
      m_selectedControl = setupdlg.selectedControl();
      m_bundleSettings = setupdlg.bundleSettings();
      // The settings have been modified, might be misleading to keep this check after setup.
      m_ui->useLastSettings->setChecked(false);
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

         // Grab the control name that was used in that bundle adjustment.
         m_selectedControlName
             = FileName(bundleSolutionInfo.last()->inputControlNetFileName()).name();
      }

      // Clear the dialog displays.
      clearDialog();

      QThread *bundleThread = new QThread;

      // Make sure to clean up any previously run bundle adjusts.
      if (m_bundleAdjust) {
        delete m_bundleAdjust;
        m_bundleAdjust = NULL;
      }

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

      bundleThread->start();

      // change "Run" button text to "Abort" (or maybe pause)
      m_bRunning = true;
      m_close->setEnabled(false);
      m_ui->JigsawRunButton->setText("&Abort");
      update();
    }
    else {
      // Make sure to abort the bundle if it is currently running.
      m_bundleAdjust->abortBundle();
      m_bRunning = false;
      m_ui->JigsawRunButton->setText("&Aborting...");
      update();
    }
  }


  /**
   * Constructs a image copier functor for copying images used in the bundle adjustment to the
   * bundle solution info results (when the bundle is accepted).
   */
  JigsawDialog::CopyImageToResultsFunctor::CopyImageToResultsFunctor(const QDir &destination) {
    m_destinationFolder = destination;
  }


  /**
   * Destructor.
   */
  JigsawDialog::CopyImageToResultsFunctor::~CopyImageToResultsFunctor() {
    m_destinationFolder = QDir();
  }


  /**
   * @brief Callable operator that copies an image to the bundle solution info results.
   *
   * This makes the functor callable - this will copy the passed FileName and return a pointer
   * to the newly copied external cube.
   *
   * @param const FileName &image File name of the image to create an external copy of.
   *
   * @return Cube* Returns a pointer to the external cube copy. Returns NULL if an error
   *               occurs.
   */
  Cube *JigsawDialog::CopyImageToResultsFunctor::operator()(const FileName &image) {
    try {
      Cube *result = NULL;

      // Get the destination folder and create that path.
      FileName destination(QFileInfo(m_destinationFolder, image.name()).absoluteFilePath());
      m_destinationFolder.mkpath(destination.path());

      // The input FileName will be referencing an imported ecub file.
      // Need to get the .cub file (via Cube::externalCubeFileName) to copy.
      // This method returns whatever value is set for the ^DnFile keyword... will not contain
      // a path if the .ecub and .cub are in the same directory.
      Cube importCube(image, "r");
      FileName dnCubeFileName;
      // The .ecub's ^DnFile is cubeFileName.cub (.ecub sitting next to .cub)
      if (importCube.externalCubeFileName().path() == ".") {

        QDir relative(m_destinationFolder.absolutePath());
        dnCubeFileName = FileName(QDir(image.path()).canonicalPath() + "/" + importCube.externalCubeFileName().name());
        QString s = relative.relativeFilePath(dnCubeFileName.toString());
        // Locate the DnFile cube by using the input image's (ecub) path and the DnFile name (cub)
        //dnCubeFileName = FileName(image.path() + "/" + importCube.externalCubeFileName().name());
        // WHY DO WE NEED TO USE QDIR canonical path? why is the image.path relative and not abs?
        //dnCubeFileName = FileName(s);
        dnCubeFileName = FileName(QDir(image.path()).canonicalPath() + "/" +
                                  importCube.externalCubeFileName().name());
        Cube dnCube(dnCubeFileName, "r");


        result = dnCube.copy(destination, CubeAttributeOutput("+External"));
        result->relocateDnData(s);
      }
      // The .ecub's ^DnFile is an absolute path (.ecub potentially not located next to .cub)
      else {
        dnCubeFileName = importCube.externalCubeFileName();
        Cube dnCube(dnCubeFileName, "r");
        result = dnCube.copy(destination, CubeAttributeOutput("+External"));
      }
      return result;
    }
    // Error tracking should be more robust, see ImportImagesWorkOrder.
    catch (IException &e) {
      std::cout << "\nerror: " << e.what();
      return NULL;
    }
  }


  /**
   * Accepts the bundle results and saves them to the project. The "Accept" and "Reject" buttons
   * will be disabled.
   */
  void JigsawDialog::acceptBundleResults() {
    m_accept->setEnabled(false);
    m_reject->setEnabled(false);

    // create bundle results folder
    QString runTime = m_bundleSolutionInfo->runTime();
    QDir bundleDir = m_project->addBundleSolutionInfoFolder(runTime); //???
                                                           // save solution information to a file

    m_bundleSolutionInfo->bundleSettings()->setOutputFilePrefix(bundleDir.absolutePath() + "/");

    //  Write csv files
    m_bundleSolutionInfo->outputResiduals();
    m_bundleSolutionInfo->outputImagesCSV();
    m_bundleSolutionInfo->outputPointsCSV();

    //  Write text summary file
    m_bundleSolutionInfo->outputText();

    // create output control net file name
    FileName outputControlName;
    if (!m_outputControlName.isEmpty()) {
      outputControlName
          = FileName(m_project->bundleSolutionInfoRoot() + "/" + runTime + "/" +
                     m_outputControlName);
    }
    else {
      outputControlName
          = FileName(m_project->bundleSolutionInfoRoot() + "/" + runTime + "/Out-" + runTime + "-" +
                     FileName(m_bundleSolutionInfo->inputControlNetFileName()).name());
    }

    // Write output control net with correct path to results folder + runtime
    m_bundleSolutionInfo->bundleResults().outputControlNet()->Write(outputControlName.toString());

    // create Control with output control net and add to m_bundleSolutionInfo
    m_bundleSolutionInfo->setOutputControl(new Control(m_project, outputControlName.expanded()));

    // Iterate through all of the image lists (the "imports" in the project).
    QList<ImageList *> imageLists = m_bundleSolutionInfo->imageList();
    foreach (ImageList *imageList, imageLists) {
      // Keep track of the file names of the images that were used in the bundle.
      QStringList imagesToCopy;

      // Now, we iterate through each image in the current image list ("import"), and we determine
      // the location of the image and where to copy it to (as an ecub).
      foreach (Image *image, *imageList) {
        FileName original(image->fileName());
        // Update our list of tracked file names for the images we are going to copy.
        imagesToCopy.append(original.expanded());
      }
      // Concurrently copy the bundled images as ecub's to the bundle solution info results.
      CopyImageToResultsFunctor copyImage(m_project->bundleSolutionInfoRoot() + "/" +
                                          m_bundleSolutionInfo->runTime() + "/images/" +
                                          imageList->name());
      QFuture<Cube *> copiedCubes = QtConcurrent::mapped(imagesToCopy, copyImage);

      // Prepare for our adjusted images (ecubs)
      ImageList *adjustedImages = new ImageList(imageList->name(), imageList->path());

      // Update the adjusted images' labels
      for (int i = 0; i < imagesToCopy.size(); i++) {
        Cube *ecub = copiedCubes.resultAt(i);
        if (ecub) {
          Process propagateHistory;
          propagateHistory.SetInputCube(ecub);

          // check for existing polygon, if exists delete it
          if (ecub->label()->hasObject("Polygon")) {
            ecub->label()->deleteObject("Polygon");
          }

          // check for CameraStatistics Table, if exists, delete
          for (int iobj = 0; iobj < ecub->label()->objects(); iobj++) {
            PvlObject obj = ecub->label()->object(iobj);
            if (obj.name() != "Table") continue;
            if (obj["Name"][0] != QString("CameraStatistics")) continue;
            ecub->label()->deleteObject(iobj);
            break;
          }

          // Timestamp and propagate the instrument pointing table and instrument position table
          QString bundleTimestamp = "Jigged = " + m_bundleSolutionInfo->runTime();
          Table cMatrix = m_bundleAdjust->cMatrix(i);
          Table spVector = m_bundleAdjust->spVector(i);
          cMatrix.Label().addComment(bundleTimestamp);
          spVector.Label().addComment(bundleTimestamp);
          ecub->write(cMatrix);
          ecub->write(spVector);
          // The ecub is now adjusted, add this to our list of adjusted images
          Image *newImage = new Image(ecub);
          adjustedImages->append(newImage);
          newImage->closeCube();
        }
      }
      // Tell the BundleSolutionInfo what the adjusted images are
      m_bundleSolutionInfo->addAdjustedImages(adjustedImages);
    }

    // Tell the project about the BundleSolutionInfo
//    m_project->addBundleSolutionInfo( new BundleSolutionInfo(*m_bundleSolutionInfo) );
    m_project->addBundleSolutionInfo(m_bundleSolutionInfo);

    // Make sure that when we add our results, we let the use last settings box be checkable.
    m_ui->useLastSettings->setEnabled(true);
    //  Once the bundle has been accepted, re-enable the close button
    m_close->setEnabled(true);

  //       m_ui->convergenceStatusLabel->setText("Bundle converged, camera pointing updated");
    //This bundle was bad so we should delete all remenants.

    //TODO: delete correlation matrix cov file...
    //TODO: delete bundle results object

//       m_ui->convergenceStatusLabel->setText("Bundle did not converge, camera pointing NOT updated");
    m_project->setClean(false);
  }


  /**
   * Rejects the bundle results and discards them. The "Accept" and "Reject" buttons will be
   * disabled.
   */
  void JigsawDialog::rejectBundleResults() {
    // TODO should there be a prompt to user (are you sure?) -- Annoying?
    // TODO Add tooltip/what'sthis for the buttons!!!! (CTR)
    // Disable the "Accept" and "Reject" buttons, enable the "Close" button
    m_accept->setEnabled(false);
    m_reject->setEnabled(false);
    m_close->setEnabled(true);

    // Clear the dialog so the lcd's are 0 and the status text is cleared.
    clearDialog();
    QString statusText("Bundle Rejected.\n\n");
    m_ui->statusUpdatesLabel->setText(statusText);

    // Cleanup the results (bundle solution info)
    // How does this affect m_bundleSettings or m_bundleAdjustment?
    // How does this affect using the last (most recent) settings for the run?
//    delete m_bundleSolutionInfo;
//    m_bundleSolutionInfo = NULL;
  }


  /**
   * Resets the dialog's status widgets to their default state. This will clear the status text,
   * reset the lcd displays to 0, and update the scroll on the scroll bar. This does NOT affect
   * the state of the buttons.
   */
  void JigsawDialog::clearDialog() {
    m_ui->iterationLcdNumber->display(0);
    m_ui->sigma0LcdNumber->display(0);
    m_ui->statusUpdatesLabel->clear();
    updateScrollBar();
  }


  /**
   * Updates the scroll bar to position to its maximum setting (the bottom).
   */
  void JigsawDialog::updateScrollBar() {
    m_ui->statusUpdateScrollArea->verticalScrollBar()->setSliderPosition(
        m_ui->statusUpdateScrollArea->verticalScrollBar()->maximum());
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

    updateScrollBar();

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

    updateScrollBar();

    update();

    //  Re-enable the close button
    m_close->setEnabled(true);
  }


  /**
   * Update the label or text edit area with the error message by appending to list and refreshing.
   *
   * @param error Error status of bundle.
   */
  void JigsawDialog::reportException(QString exception) {
    QString exceptionStr = "\n" + exception;
    m_ui->statusUpdatesLabel->setText( m_ui->statusUpdatesLabel->text().append(exceptionStr) );

    updateScrollBar();

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

    updateScrollBar();

    update();
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

    bundleSolutionInfo->setRunTime( Isis::iTime::CurrentLocalTime().toLatin1().data() );
    m_bundleSolutionInfo = bundleSolutionInfo;

    // Since results are available, the user can accept (save) or reject(discard) the results.
    m_accept->setEnabled(true);
    m_reject->setEnabled(true);
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
