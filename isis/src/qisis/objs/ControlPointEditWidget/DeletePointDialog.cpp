#include <QtGui>

#include "DeletePointDialog.h"

DeletePointDialog::DeletePointDialog(QWidget *parent) : QDialog(parent) {

  setupUi(this);
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

