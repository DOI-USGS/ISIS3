#include <QtGui>

#include "QnetDeletePointDialog.h"

#include "qnet.h"

using namespace Qisis::Qnet;

QnetDeletePointDialog::QnetDeletePointDialog(QWidget *parent) : QDialog(parent) {

  setupUi(this);
  connect(okButton, SIGNAL(clicked()),this, SLOT(accept()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

}

