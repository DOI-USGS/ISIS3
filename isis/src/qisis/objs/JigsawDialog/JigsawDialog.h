#ifndef JigsawDialog_h
#define JigsawDialog_h

#include <QDialog>
#include <QDir>
#include <QPointer>
#include <QWidget>

#include "BundleSettings.h"
#include "IException.h"

namespace Ui {
  class JigsawDialog;
}

class QString;

namespace Isis {
  class BundleAdjust;
  class BundleSolutionInfo;
  class Control;
  class Cube;
  class Directory;
  class FileName;
  class Project;

  /**
   * This dialog allows the user to select the bundle adjust parameters, run the bundle, and view
   * the results.
   *
   * @author 2014-??-?? Ken Edmundson
   *
   * @internal
   *   @history 2014-09-18 Kimberly Oyama - Added code to thread the bundle run. It is currently
   *                           commented out but it works.
   *   @history 2015-02-20 Jeannie Backer - Replaced BundleResults references with
   *                           BundleSolutionInfo and BundleStatistics references with BundleResults
   *                           due to class name changes.
   *   @history 2015-09-03 Jeannie Backer - Modified to create JigsawSetupDialog object using the
   *                           value for the useLastSettings checkbox. When the Run button is
   *                           clicked, the run time will now be used to create a uniquely named
   *                           directory to contain the output files for the bundle solution.
   *   @history 2017-04-17 Ian Humphrey - Added second constructor that can be used when the
   *                           JigsawWorkOrder initially creates a setup dialog so it can pass
   *                           information to this dialog. Added init() delegate method for
   *                           constructors to use to reduce code duplication. Modified
   *                           notifyThreadFinished to update the Run button. References #4748.
   *   @history 2017-04-18 Ian Humphrey - Added members for an Accept, Reject, and Close button
   *                           to determine when to save a successful bundle adjust's results
   *                           to the project (or when to discard the results). Added placeholder
   *                           private slots for accepting and rejecting the results. Removed
   *                           default OK and Cancel buttons from UI file. Fixes #4781.
   *   @history 2017-04-18 Tracie Sucharski - Write the updated control net to the runtime
   *                           folder under results folder.  Fixes #4783.
   *   @history 2017-04-25 Ian Humphrey - Updated the setup clicked slot to load the current
   *                           settings into the setup dialog if we are not using the last
   *                           (most recent) settings in the project. Fixes #4817.
   *   @history 2017-04-26 Ian Humphrey - Added updateScrollBar() and clearDialog() to reduce
   *                           code duplication. Modified the run clicked slot to clear the
   *                           dialog display anytime that a bundle adjust is re-ran. Fixes #4808.
   *   @history 2017-04-27 Ian Humphrey - Modified to track the last used control net to properly
   *                           update the jigsaw setup dialog's cnet combo box. References #4817.
   *   @history 2017-05-04 Ian Humphrey & Makayla Shepherd - Updated acceptBundleResults()
   *                           to concurrently save the bundled images (ecub's) to the project.
   *                           Fixes #4804, #4837.
   *   @history 2017-05-04 Ian Humphrey & Makayla Shepherd - Removed connection handling deleting the
   *                           bundle adjust later. This prevents a segfault from occuring when the
   *                           bundle adjust was accessed in the acceptBundleResults() slot, Since
   *                           its memory may have been deleted by then. Now manually managing the
   *                           memory for m_bundleAdjust. Fixes #4849.
   *   @history 2017-05-15 Tracie Sucharski - Commented out code in acceptBundleResults which was
   *                           not being used and causing compile warnings.  Add creation of
   *                           BundleSolutionInfo folder to the acceptBundleResults method.
   */
  class JigsawDialog : public QDialog {
    Q_OBJECT

  public:
    explicit JigsawDialog(Project *project, QWidget *parent = 0);
    explicit JigsawDialog(Project *project,
                          BundleSettingsQsp bundleSettings,
                          Control *selectedControl,
                          QWidget *parent = 0);

    ~JigsawDialog();

  public slots:
    void outputBundleStatus(QString status);
    void errorString(QString error);
    void reportException(QString exception);
    void updateIterationSigma0(int iteration, double sigma0);
    void bundleFinished(BundleSolutionInfo *bundleSolutionInfo);
    void notifyThreadFinished();

  protected:
    void init();
    BundleAdjust *m_bundleAdjust;
    Project *m_project;
    Control *m_selectedControl;
    QString m_selectedControlName;
    BundleSettingsQsp m_bundleSettings;

  private:
    bool m_bRunning; /**< Indicates whether or not the bundle adjust is running. */
    QPushButton *m_accept; /**< Dialog's accept button that is used to save the bundle results. */
    QPushButton *m_close; /**< Dialog's close button that is used to close the dialog. */
    QPushButton *m_reject; /**< Dialog's reject button that is used to discard the results. */

    /**
     * Functor used to copy images to a specified destination directory. This is used by
     * a QtConcurrent::mapped call in acceptBundleResults().
     *
     * @author 2017-05-04 Ian Humphrey
     *
     * @internal
     */
    class CopyImageToResultsFunctor :
        public std::unary_function<const FileName &, Cube *> {
      public:
        CopyImageToResultsFunctor(const QDir &destination);
        ~CopyImageToResultsFunctor();
        Cube *operator()(const FileName &image);
      private:
        CopyImageToResultsFunctor &operator=(const CopyImageToResultsFunctor &other);
        QDir m_destinationFolder; /**< Directory to copy the image to. */
    };

  private slots:
    void on_JigsawSetupButton_pressed();
    void on_JigsawRunButton_clicked();
    void acceptBundleResults();
    void rejectBundleResults();
    void clearDialog();
    void updateScrollBar();

  private:
    /** Captures the most recent results of a bundle. JigsawDialog owns this pointer. */
    BundleSolutionInfo *m_bundleSolutionInfo;
    /** Reference to self's UI generated with QtDesigner. */
    Ui::JigsawDialog *m_ui;
  };
};
#endif
