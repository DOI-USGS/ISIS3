#include <QtGui>

#include "QnetSetAprioriDialog.h"

QnetSetAprioriDialog::QnetSetAprioriDialog(QWidget *parent) : QDialog(parent) {

  setupUi(this);
  connect(setAprioriButton, SIGNAL(clicked()), this, SIGNAL(setApriori()));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
//  connect(closeButton, SIGNAL(clicked()), this, SIGNAL(aprioriDialogClosed()));

}

void QnetSetAprioriDialog::clearLineEdits() {
  aprioriLatEdit->setText("");
  aprioriLonEdit->setText("");
  aprioriRadiusEdit->setText("");
  latSigmaEdit->setText("");
  lonSigmaEdit->setText("");
  radiusSigmaEdit->setText("");
}
