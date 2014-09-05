#ifndef DeleteControlPointDialog_h
#define DeleteControlPointDialog_h

#include <QDialog>

#include "ui_DeleteControlPointDialog.h"

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
// TODO: Replace QnetDeleteControlPointDialog with this class??
class DeleteControlPointDialog : public QDialog, public Ui::DeleteControlPointDialog {
    Q_OBJECT

  public:
    DeleteControlPointDialog(QWidget *parent = 0);

};

#endif
