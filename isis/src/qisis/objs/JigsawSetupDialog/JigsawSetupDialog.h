#ifndef JigsawSetupDialog_h
#define JigsawSetupDialog_h

#include <QDialog>

namespace Ui {
  class JigsawSetupDialog;
}

namespace Isis {
  class Project;
  class Control;
  class BundleSettings;

  class JigsawSetupDialog : public QDialog {
    Q_OBJECT

  public:
    explicit JigsawSetupDialog(Project* project, QWidget *parent = 0);
    ~JigsawSetupDialog();

    Control *selectedControl();
    BundleSettings *bundleSettings();

  private slots:

    //void on_radiusCheckBox_toggled(bool checked);
    //void on_outlierRejectionCheckBox_toggled(bool checked);

    void on_positionComboBox_currentIndexChanged(int index);
    void on_pointingComboBox_currentIndexChanged(int index);

    void on_maximumLikelihoodModel1ComboBox_currentIndexChanged(int index);
    void on_maximumLikelihoodModel2ComboBox_currentIndexChanged(int index);
    void on_maximumLikelihoodModel3ComboBox_currentIndexChanged(int index);

  private:
    Ui::JigsawSetupDialog *m_ui;
    Project * m_project;
  };
};
#endif // JigsawSetupDialog_h
