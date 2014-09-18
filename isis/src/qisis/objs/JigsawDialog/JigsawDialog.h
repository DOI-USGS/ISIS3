#ifndef JigsawDialog_h
#define JigsawDialog_h

#include <QDialog>
  
namespace Ui {
  class JigsawDialog;
}

class QString;

namespace Isis {
  class BundleResults;
  class BundleSettings;
  class Control;
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
   */
  class JigsawDialog : public QDialog {
    Q_OBJECT

  public:
    explicit JigsawDialog(Project *project, QWidget *parent = 0);
    ~JigsawDialog();

  public slots:
    void outputBundleStatus(QString status);
    void bundleFinished(BundleResults *bundleResults);
    
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
#endif 
