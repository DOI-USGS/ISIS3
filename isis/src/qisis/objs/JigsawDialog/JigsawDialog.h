#ifndef JigsawDialog_h
#define JigsawDialog_h

#include <QDialog>

#include "JigsawSetupDialog.h"

namespace Ui {
  class JigsawDialog;
}

class QString;

namespace Isis {
  class BundleSettings;
  class Control;
  class Project;

  class JigsawDialog : public QDialog
  {
    Q_OBJECT

  public:
    explicit JigsawDialog(Project *project, QWidget *parent = 0);
    ~JigsawDialog();

  private slots:
    void on_JigsawSetupButton_pressed();
    void on_JigsawRunButton_clicked();

  private:
    Ui::JigsawDialog *m_ui;

  protected:
    Project *m_project;
    Control *m_selectedControl;
    QString *m_selectedControlName;
    BundleSettings *m_bundleSettings;
  };
};
#endif // JigsawDialog_h
