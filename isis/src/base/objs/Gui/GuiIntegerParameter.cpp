#include <QHBoxLayout>
#include <QIntValidator>

#include "GuiIntegerParameter.h"
#include "UserInterface.h"

namespace Isis {

  GuiIntegerParameter::GuiIntegerParameter(QGridLayout *grid, UserInterface &ui,
                                     int group, int param) :
      GuiParameter(grid, ui, group, param) {

    p_lineEdit = new QLineEdit;
    p_lineEdit->setValidator(new QIntValidator(p_lineEdit));
    connect(p_lineEdit,SIGNAL(textChanged(const QString &)),this,SIGNAL(ValueChanged()));
    grid->addWidget(p_lineEdit,param,2);

    if (p_ui->HelpersSize(group,param) != 0) {
      grid->addWidget(AddHelpers(p_lineEdit),param,3);
    }

    RememberWidget(p_lineEdit);
    p_type = IntegerWidget;
  }


  GuiIntegerParameter::~GuiIntegerParameter() {}

  void GuiIntegerParameter::Set (iString newValue) {
    p_lineEdit->setText (newValue);
  }


  iString GuiIntegerParameter::Value () {
    return p_lineEdit->text().toStdString();
  }

}

