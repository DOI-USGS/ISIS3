#ifndef JigsawSetupDialog_h
#define JigsawSetupDialog_h

#include <QDialog>

namespace Ui {
  class JigsawSetupDialog;
}

namespace Isis {
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
   */
  class Project;
  class Control;
  class BundleSettings;

  class JigsawSetupDialog : public QDialog {
    Q_OBJECT

  public:
//    explicit JigsawSetupDialog(Project* project, QWidget *parent = 0);
    explicit JigsawSetupDialog(Project* project, bool useLastSettings = true,
                               bool readOnly = false, QWidget *parent = 0);
    ~JigsawSetupDialog();

    Control *selectedControl();// TODO: return const references ???
    QString *selectedControlName();// TODO: return const references ???
    BundleSettings *bundleSettings();// TODO: return const references ???

  private slots:

    //void on_radiusCheckBox_toggled(bool checked);
    //void on_outlierRejectionCheckBox_toggled(bool checked);

    void on_positionComboBox_currentIndexChanged(int index);
    void on_pointingComboBox_currentIndexChanged(int index);

    void on_maximumLikelihoodModel1ComboBox_currentIndexChanged(int index);
    void on_maximumLikelihoodModel2ComboBox_currentIndexChanged(int index);
    void on_maximumLikelihoodModel3ComboBox_currentIndexChanged(int index);

  private:
    void makeReadOnly()    ;
    void fillFromSettings(BundleSettings* settings);

  private:
    Ui::JigsawSetupDialog *m_ui;
    Project * m_project;
  };
};
#endif // JigsawSetupDialog_h
