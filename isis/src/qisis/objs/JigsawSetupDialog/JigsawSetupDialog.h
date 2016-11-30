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
   */

  class JigsawSetupDialog : public QDialog {
    Q_OBJECT

  public:
//    explicit JigsawSetupDialog(Project* project, QWidget *parent = 0);
    explicit JigsawSetupDialog(Project* project, bool useLastSettings = true,
                               bool readOnly = false, QWidget *parent = 0);
    ~JigsawSetupDialog();

    Control *selectedControl();// TODO: return const references ???
    QString *selectedControlName();// TODO: return const references ???
    BundleSettingsQsp bundleSettings();// TODO: return const references ???

  private slots:

    void on_radiusCheckBox_toggled(bool checked);
    //void on_outlierRejectionCheckBox_toggled(bool checked);

    // general tab
    void on_positionComboBox_currentIndexChanged(int index);
    void on_pointingComboBox_currentIndexChanged(int index);

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
    void fillFromSettings(BundleSettingsQsp settings);
    void showTargetAngleWidgets();
    void hideTargetAngleWidgets();

  private:
    Ui::JigsawSetupDialog *m_ui;
    Project *m_project;
  };
};
#endif // JigsawSetupDialog_h
