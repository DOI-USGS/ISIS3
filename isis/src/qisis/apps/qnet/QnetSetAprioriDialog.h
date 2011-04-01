#ifndef QnetSetAprioriDialog_h
#define QnetSetAprioriDialog_h

#include <QDialog>

#include "ui_QnetSetAprioriDialog.h"

class QnetSetAprioriDialog : public QDialog, public Ui::QnetSetAprioriDialog {
    Q_OBJECT

  public:
    QnetSetAprioriDialog(QWidget *parent = 0);

};

#endif
