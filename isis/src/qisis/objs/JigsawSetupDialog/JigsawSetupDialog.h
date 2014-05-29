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

  class JigsawSetupDialog : public QDialog
  {
    Q_OBJECT

  public:
    explicit JigsawSetupDialog(Project* project, QWidget *parent = 0);
    ~JigsawSetupDialog();

    Control* selectedControl();
    BundleSettings bundleSettings();

  private slots:
    void on_OutlierRejection_toggled(bool checked);

    void on_SpacecraftCombobox_currentIndexChanged(int index);

    void on_PointingCombobox_currentIndexChanged(int index);

  private:
    Ui::JigsawSetupDialog *ui;
    Project * m_project;
  };
};
#endif // JigsawSetupDialog_h
