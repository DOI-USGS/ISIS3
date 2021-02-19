/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QHBoxLayout>
#include <QWidget>
#include "UserInterface.h"

#include "GuiBooleanParameter.h"


namespace Isis {

  GuiBooleanParameter::GuiBooleanParameter(QGridLayout *grid, UserInterface &ui,
      int group, int param) :
    GuiParameter(grid, ui, group, param) {

    p_checkBox = new QCheckBox((QString)ui.ParamBrief(group, param));

    grid->addWidget(p_checkBox, param, 2);

    RememberWidget(p_checkBox);
    connect(p_checkBox, SIGNAL(toggled(bool)), this, SIGNAL(ValueChanged()));

    if(p_ui->HelpersSize(group, param) != 0) {
      grid->addWidget(AddHelpers(p_checkBox), param, 3);
    }

    p_type = BooleanWidget;
  }


  GuiBooleanParameter::~GuiBooleanParameter() {}


  void GuiBooleanParameter::Set(QString newValue) {
    p_checkBox->setChecked(p_ui->StringToBool(newValue));
    emit ValueChanged();
  }


  QString GuiBooleanParameter::Value() {
    return p_checkBox->isChecked() ? "YES" : "NO";
  }

  std::vector<QString> GuiBooleanParameter::Exclusions() {
    std::vector<QString> list;

    // Exclude exclusions or inclusions
    if(Value() == "YES") {
      for(int i = 0; i < p_ui->ParamExcludeSize(p_group, p_param); i++) {
        QString s = p_ui->ParamExclude(p_group, p_param, i);
        list.push_back(s);
      }
    }
    else {
      for(int i = 0; i < p_ui->ParamIncludeSize(p_group, p_param); i++) {
        QString s = p_ui->ParamInclude(p_group, p_param, i);
        list.push_back(s);
      }
    }

    return list;
  }

  //! Return if the parameter value is different from the default value
  bool GuiBooleanParameter::IsModified() {
    if(!IsEnabled()) return false;
    QString value;
    if(p_ui->ParamDefault(p_group, p_param).size() > 0) {
      value = p_ui->ParamDefault(p_group, p_param).toUpper();
    }
    else {
      value = "NO";
    }

    if(value == "0") value = "NO";
    if(value == "FALSE") value = "NO";
    if(value == "N") value = "NO";
    if(value == "OFF") value = "NO";
    if(value == "1") value = "YES";
    if(value == "TRUE") value = "YES";
    if(value == "Y") value = "YES";
    if(value == "ON") value = "YES";

    if(Value() == value) return false;
    return true;
  }
}

