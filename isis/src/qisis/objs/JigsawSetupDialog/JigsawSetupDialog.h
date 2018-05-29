#ifndef JigsawSetupDialog_h
#define JigsawSetupDialog_h

#include <QDialog>

#include "BundleSettings.h"

namespace Ui {
  class JigsawSetupDialog;
}

namespace Isis {
  class Project;
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
   *   @history 2017-04-25 Ian Humphrey - Added public loadSettings() to allow JigsawDialog to
   *                           load its current settings into the setup dialog. Fixes #4817.
   *   @history 2017-04-27 Ian Humphrey - Added selectControl() to allow JigsawDialog to
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

    void on_radiusCheckBox_toggled(bool checked);
    //void on_outlierRejectionCheckBox_toggled(bool checked);

    // general tab
    void on_positionComboBox_currentIndexChanged(int index);
    void on_pointingComboBox_currentIndexChanged(int index);
    void on_controlNetworkComboBox_currentTextChanged(const QString &arg1);

    // maximum liklihood tab
    void on_maximumLikelihoodModel1ComboBox_currentIndexChanged(int index);
    void on_maximumLikelihoodModel2ComboBox_currentIndexChanged(int index);
    void on_maximumLikelihoodModel3ComboBox_currentIndexChanged(int index);

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
    void on_spkSolveDegreeSpinBox_2_valueChanged(int arg1);
    void on_rightAscensionLineEdit_textChanged(const QString &arg1);
    void on_declinationLineEdit_textChanged(const QString &arg1);
    void on_rightAscensionVelocityLineEdit_textChanged(const QString &arg1);
    void on_declinationVelocityLineEdit_textChanged(const QString &arg1);
    void on_spinRateLineEdit_textChanged(const QString &arg1);
    void on_primeMeridianOffsetLineEdit_textChanged(const QString &arg1);

    public slots:
    void slotTextChanged(const QString &text);
    void checkIsValid();


    private:
    void makeReadOnly()    ;
    void fillFromSettings(const BundleSettingsQsp settings);
    void showTargetParametersGroupBox();
    void hideTargetParametersGroupBox();

  private:
    Ui::JigsawSetupDialog *m_ui;
    Project *m_project;
  };
};
#endif // JigsawSetupDialog_h
