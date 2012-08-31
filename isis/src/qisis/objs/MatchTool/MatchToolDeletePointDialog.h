#ifndef MatchToolDeletePointDialog_h
#define MatchToolDeletePointDialog_h

#include <QDialog>

#include "ui_MatchToolDeletePointDialog.h"

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class MatchToolDeletePointDialog : public QDialog, public Ui::MatchToolDeletePointDialog {
    Q_OBJECT

  public:
    MatchToolDeletePointDialog(QWidget *parent = 0);

};

#endif
