#include <QHBoxLayout>
#include <QWidget>
#include "UserInterface.h"

#include "GuiBooleanParameter.h"


namespace Isis {

  GuiBooleanParameter::GuiBooleanParameter(QGridLayout *grid, UserInterface &ui,
                                           int group, int param) :
      GuiParameter(grid, ui, group, param) {

    p_checkBox = new QCheckBox((iString)ui.ParamBrief(group, param));

    grid->addWidget(p_checkBox,param,2);

    RememberWidget(p_checkBox);
    connect(p_checkBox,SIGNAL(toggled(bool)),this,SIGNAL(ValueChanged()));

    if (p_ui->HelpersSize(group,param) != 0) {
      grid->addWidget(AddHelpers(p_checkBox),param,3);
    }
  
    p_type = BooleanWidget;
  }


  GuiBooleanParameter::~GuiBooleanParameter() {}


  void GuiBooleanParameter::Set (iString newValue) {
    p_checkBox->setChecked(p_ui->StringToBool(newValue));
    emit ValueChanged();
  }


  iString GuiBooleanParameter::Value () {
    return p_checkBox->isChecked() ? "YES" : "NO";
  }

  std::vector<std::string> GuiBooleanParameter::Exclusions() {
    std::vector<std::string> list;

    // Exclude exclusions or inclusions
    if (Value() == "YES") {
      for (int i=0; i<p_ui->ParamExcludeSize(p_group,p_param); i++) {
        std::string s = p_ui->ParamExclude(p_group,p_param,i);
        list.push_back(s);
      }
    }
    else {
      for (int i=0; i<p_ui->ParamIncludeSize(p_group,p_param); i++) {
        std::string s = p_ui->ParamInclude(p_group,p_param,i);
        list.push_back(s);
      }
    }

    return list;
  }

  //! Return if the parameter value is different from the default value
  bool GuiBooleanParameter::IsModified() {
    if (!IsEnabled()) return false;
    iString value;
    if (p_ui->ParamDefault (p_group, p_param).size() > 0) {
      value = p_ui->ParamDefault(p_group,p_param);
    }
    else {
      value = "NO";
    }

    value.UpCase();
    if (value == "0") value = "NO";
    if (value == "FALSE") value = "NO";
    if (value == "N") value = "NO";
    if (value == "OFF") value = "NO";
    if (value == "1") value = "YES";
    if (value == "TRUE") value = "YES";
    if (value == "Y") value = "YES";
    if (value == "ON") value = "YES";

    if (Value() == value) return false;
    return true;
  }
}

