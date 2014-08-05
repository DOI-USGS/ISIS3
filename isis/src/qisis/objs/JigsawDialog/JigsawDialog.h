#ifndef JigsawDialog_h
#define JigsawDialog_h

#include <QDialog>

// #include "JigsawSetupDialog.h"
// #include "ui_JigsawDialog.h"

namespace Ui {
  class JigsawDialog;
}

class QString;

namespace Isis {
  class BundleSettings;
  class Control;
  class Project;

  class JigsawDialog : public QDialog {
    Q_OBJECT

  public:
    explicit JigsawDialog(Project *project, QWidget *parent = 0);
    ~JigsawDialog();

  public slots:
    void outputBundleStatus(QString status);
    void updateConvergenceStatus(bool converged);
    
  protected:
    Project *m_project;
    Control *m_selectedControl;
    QString *m_selectedControlName;
    BundleSettings *m_bundleSettings;
    
  private slots:
    void on_JigsawSetupButton_pressed();
    void on_JigsawRunButton_clicked();

  private:
    Ui::JigsawDialog *m_ui;
  };
};
#endif // JigsawDialog_h
