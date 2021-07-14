#ifndef DeleteControlPointDialog_h
#define DeleteControlPointDialog_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QDialog>

#include "ui_DeleteControlPointDialog.h"

/**
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class DeleteControlPointDialog : public QDialog, public Ui::DeleteControlPointDialog {
    Q_OBJECT

  public:
    DeleteControlPointDialog(QWidget *parent = 0);

};

#endif
