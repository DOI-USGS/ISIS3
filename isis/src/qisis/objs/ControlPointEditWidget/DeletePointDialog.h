#ifndef DeletePointDialog_h
#define DeletePointDialog_h

#include <QDialog>

#include "ui_DeletePointDialog.h"

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class DeletePointDialog : public QDialog, public Ui::DeletePointDialog {
    Q_OBJECT

  public:
    DeletePointDialog(QWidget *parent = 0);

};

#endif
