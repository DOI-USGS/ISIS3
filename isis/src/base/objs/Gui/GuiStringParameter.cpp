#include <QHBoxLayout>

#include "GuiStringParameter.h"
#include "UserInterface.h"

namespace Isis {

  GuiStringParameter::GuiStringParameter(QGridLayout *grid, UserInterface &ui,
                                     int group, int param) :
      GuiParameter(grid, ui, group, param) {

    p_lineEdit = new QLineEdit;
    connect(p_lineEdit,SIGNAL(textChanged(const QString &)),this,SIGNAL(ValueChanged()));
    grid->addWidget(p_lineEdit,param,2);

    if (p_ui->HelpersSize(group,param) != 0) {
      grid->addWidget(AddHelpers(p_lineEdit),param,3);
    }

    RememberWidget(p_lineEdit);

    p_type = StringWidget;
  }


  GuiStringParameter::~GuiStringParameter() {}

  void GuiStringParameter::Set (iString newValue) {
    p_lineEdit->setText ((iString)newValue);
  }


  iString GuiStringParameter::Value () {
    return p_lineEdit->text().toStdString();
  }


}

