#ifndef ProfileDialog_h
#define ProfileDialog_h

#include <QDialog>

#include "ui_ProfileDialog.h"

/**
 * @author 2012-02-06 Tracie Sucharski
 *
 * @internal
 */
class ProfileDialog : public QDialog, public Ui::ProfileDialog {
    Q_OBJECT

  public:
    ProfileDialog(QWidget *parent = 0);

  signals:
    void createStart();
    void createEnd();

  private slots:
    void createStartSelected();
    void createEndSelected();
    void help();
   
  private:
    bool m_startCreated;
    bool m_endCreated;


};

#endif
