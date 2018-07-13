#ifndef JigsawSetupDialog_h
#define JigsawSetupDialog_h

#include <QDialog>

#include "BundleSettings.h"

namespace Ui {
  class JigsawSetupDialog;
}

class QComboBox;
class QItemSelection;
class QTableWidget;
class QTableWidgetItem;

namespace Isis {
  class BundleObservationSolveSettings;
  class Project;
  class ProjectItem;
  class Control;

  /**
   * @brief
   *
   * @ingroup ControlNetworks
   *
   * @author 2014-04-21 Ken Edmundson
   *
   * @internal
   *   @history 2014-04-21 Ken Edmundson - Original version.
   *   @history 2014-07-16 Jeannie Backer - Use MaximumLikelihoodWFunctions
   *                           static method to convert string to Model enum.
   *   @history 2014-07-23 Jeannie Backer - Added selectedControlName(). Commented out degree combo
   *                           box enable calls.
   *   @history 2015-02-20 Jeannie Backer - Replaced BundleResults references with
   *                           BundleSolutionInfo and BundleStatistics references with BundleResults
   *                           due to class name changes.
   *   @history 2015-09-03 Jeannie Backer - Updated to be more compliant with ISIS coding standards.
   *                           Changed condition of if-statements for QLineEdit::setText for the
   *                           global a priori sigma variables from less than zero to !IsNullPixel
   *                           for consistency with bundle classes.
   *   @history 2016-08-18 Jeannie Backer - Removed all references to deprecated solve methods
   *                           SpeckialK and OldSparse. References #4162.
   *   @history 2016-08-25 Adam Paquette - Updated documentation. Fixes #4299.
   *   @history 2017-04-25 Ian Humphrey - Added public loadSettings() to allow JigsawRunWidget to
   *                           load its current settings into the setup dialog. Fixes #4817.
   *   @history 2017-04-27 Ian Humphrey - Added selectControl() to allow JigsawRunWidget to
   *                           properly tell the setup dialog which control to select in its
   *                           combo box. References #4817.
   *   @history 2017-05-16 Tracie Sucharski - Comment qDebug statements.
   *   @history 2017-05-23 Tracie Sucharski - Added a QGroupBox for the target parameters so that
   *                           the box can be disabled/enabled rather than each parameter
   *                           separately.
   *   @history 2017-06-07 Ian Humphrey - Modified fillFromSettings() so when the weightings are
   *                           loaded into the text line edits they are set to modified, which
   *                           allows for proper restoring of user defined weightings.
   *   @history 2017-08-14 Summer Stapleton - Updated icons/images to properly licensed or open 
   *                           source images. Fixes #5105.
   *   @history 2018-03-19 Ken Edmundson - Added bundle output control network file name. Added
   *                           method on_controlNetworkComboBox_currentTextChanged to update the
   *                           output control network filename when the input control network
   *                           selected filename changes. E.g. if input control net name is
   *                           fred.net, the output filename QLineEdit is automatically changed to
   *                           fred-out.net. The user can always manually change the output control
   *                           net name to anything they choose.
   *   @history 2018-06-01 Christopher Combs - Added support for ui changes, exclusive options and 
   *                           input validators.
   *   @history 2018-06-21 Ian Humphrey - Added on_applySettingsPushButtonClicked() to listen for when
   *                           the apply button is clicked on the observation solve settings tab.
   *                           References #497.
   *   @history 2018-06-21 Tyler Wilson - Added support in the Bundle Observations Solve Settings
   *                           (BOSS) tab for displaying user-selected images from the main Project
   *                           treeview.  All changes were made in the
   *                           createObservationSolveSettingsTreeView() function.  References #497.
   *   @history 2018-06-25 Ian Humphrey - Implemented the position and pointing a priori sigma
   *                           tables in the observation solve settings tab. References #497.
   *   @history 2018-06-26 Tyler Wilson - Added the function 
   *                           updateBundleObservationSolveSettings(BundleObservationSolveSettings &) 
   *                           which grabs BOSS settings from the JSD BOSS tab for selected images 
   *                           in the BOSS QTreeView and saves them in a BOSS object.
   *   @history 2018-06-26 Tyler Wilson - Added support in
   *                           updateBundleObservationSolveSettings(BundleObservationSolveSettings &)
   *                           for the user to set an arbitrary number of position/pointing Apriori
   *                           Sigma values beyond position/velocity/acceleration.  References #497.
   *   @history 2018-06-27 Ian Humphrey - Added validateSigmaTables() that checks if there are any
   *                           invalid a priori sigma values whenever an a priori sigma values changes.
   *                           If any value is invalid, the OK and Apply Settings buttons are disabled
   *                           until all the a priori sigma values are valid again. References #497.
   *   @history 2018-06-28 Christopher Combs - Implemented pseudocode in on_applySettings... method.
   *                           Added selected images to default BOSS object. References #497.
   *   @history 2018-07-01 Ian Humphrey - Added updateSolveSettingsSigmaTables() to try to
   *                           generalize how the sigma tables are updated. Solve options have
   *                           their respective solve degree and degree combo boxes set to 2 unless
   *                           the solve option is ALL. References #497.
   */
  class JigsawSetupDialog : public QDialog {
    Q_OBJECT

  public:
//    explicit JigsawSetupDialog(Project* project, QWidget *parent = 0);
    explicit JigsawSetupDialog(Project* project,
                               bool useLastSettings = true,
                               bool readOnly = false,
                               QWidget *parent = 0);
    ~JigsawSetupDialog();

    Control *selectedControl();                                 // TODO: return const references ???
    QString selectedControlName();                              // TODO: return const references ???
    QString outputControlName();                                // TODO: return const references ???
    BundleSettingsQsp bundleSettings();                         // TODO: return const references ???

    void loadSettings(const BundleSettingsQsp settings);
    void selectControl(const QString &controlName);

  private slots:

    void on_pointRadiusSigmaCheckBox_toggled(bool checked);
    //void on_projectItemSelectionChanged(const QList<ProjectItem *> selectedItems);
    //void on_outlierRejectionCheckBox_toggled(bool checked);

    // general tab
    // void on_positionComboBox_currentIndexChanged(int index);
    // void on_pointingComboBox_currentIndexChanged(int index);
    void on_inputControlNetCombo_currentTextChanged(const QString &arg1);

    // general tab - line edit validators 
    void on_pointLatitudeSigmaLineEdit_textChanged(const QString &arg1);
    void on_pointLongitudeSigmaLineEdit_textChanged(const QString &arg1);
    void on_pointRadiusSigmaLineEdit_textChanged(const QString &arg1);

    void on_outlierRejectionMultiplierLineEdit_textChanged(const QString &arg1);
    void on_maximumLikelihoodModel1QuantileLineEdit_textChanged(const QString &arg1);
    void on_maximumLikelihoodModel2QuantileLineEdit_textChanged(const QString &arg1);
    void on_maximumLikelihoodModel3QuantileLineEdit_textChanged(const QString &arg1);

    void on_sigma0ThresholdLineEdit_textChanged(const QString &arg1);
    void on_maximumIterationsLineEdit_textChanged(const QString &arg1);

    // general tab - outlier rejection exclusivity lock/unlocks
    void on_maximumLikelihoodModel1ComboBox_currentIndexChanged(int index);
    void on_maximumLikelihoodModel2ComboBox_currentIndexChanged(int index);
    void on_maximumLikelihoodModel3ComboBox_currentIndexChanged(int index);
    void on_outlierRejectionCheckBox_stateChanged(int arg1);

    // target body tab
    void on_poleRaCheckBox_stateChanged(int arg1);
    void on_poleRaVelocityCheckBox_stateChanged(int arg1);
    void on_poleDecCheckBox_stateChanged(int arg1);
    void on_poleDecVelocityCheckBox_stateChanged(int arg1);
    void on_spinRateCheckBox_stateChanged(int arg1);
    void on_primeMeridianOffsetCheckBox_stateChanged(int arg1);
    void on_radiiButtonGroupClicked(int arg1);
    void on_aRadiusLineEdit_textChanged(const QString &arg1);
    void on_targetBodyComboBox_currentIndexChanged(int index);
    void on_spkSolveDegreeSpinBox_valueChanged(int arg1);
    void on_ckSolveDegreeSpinBox_valueChanged(int arg1);
    void on_rightAscensionLineEdit_textChanged(const QString &arg1);
    void on_declinationLineEdit_textChanged(const QString &arg1);
    void on_rightAscensionVelocityLineEdit_textChanged(const QString &arg1);
    void on_declinationVelocityLineEdit_textChanged(const QString &arg1);
    void on_spinRateLineEdit_textChanged(const QString &arg1);
    void on_primeMeridianOffsetLineEdit_textChanged(const QString &arg1);
    
    void on_applySettingsPushButton_clicked();

    void on_positionComboBox_currentIndexChanged(const QString &arg1);
    void on_pointingComboBox_currentIndexChanged(const QString &arg1);

    void updateSolveSettingsSigmaTables(const QComboBox *solveOptionComboBox, 
                                        QTableWidget *table);
    void validateSigmaValue(QTableWidgetItem *);
    void validateSigmaTables();
    

    public slots:
    void slotTextChanged(const QString &text);
    void checkIsValid();
    void treeViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);



    private:
    void makeReadOnly();
    void fillFromSettings(const BundleSettingsQsp settings);
    void showTargetParametersGroupBox();
    void hideTargetParametersGroupBox();
    void updateBundleObservationSolveSettings(BundleObservationSolveSettings &boss);

    void createObservationSolveSettingsTreeView();

  private:
    Ui::JigsawSetupDialog *m_ui;
    Project *m_project;
    BundleSettingsQsp m_bundleSettings; /**< The BundleSettings Object created by this dialog */
  };
};
#endif // JigsawSetupDialog_h
