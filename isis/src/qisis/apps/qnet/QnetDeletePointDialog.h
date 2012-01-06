#ifndef QnetDeletePointDialog_h
#define QnetDeletePointDialog_h

#include <QDialog>

#include "ui_QnetDeletePointDialog.h"

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class QnetDeletePointDialog : public QDialog, public Ui::QnetDeletePointDialog {
    Q_OBJECT

  public:
    QnetDeletePointDialog(QWidget *parent = 0);

};

#endif
