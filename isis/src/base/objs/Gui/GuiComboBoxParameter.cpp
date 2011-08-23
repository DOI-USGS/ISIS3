#include <QVBoxLayout>
#include <QComboBox>
#include <QList>

#include "UserInterface.h"

#include "GuiParameter.h"
#include "GuiComboBoxParameter.h"

namespace Isis {

  GuiComboBoxParameter::GuiComboBoxParameter(QGridLayout *grid, UserInterface &ui,
                                     int group, int param) :
    GuiParameter(grid, ui, group, param) {

    // Reset the default alignment of the label
    p_label->setAlignment(Qt::AlignRight | Qt::AlignTop);

    // Create a vertical box layout for the combo box and add it to
    // the grid layout
    QVBoxLayout *lo = new QVBoxLayout;
    grid->addLayout(lo, param, 2);

    // Create a combo box 
    p_combo = new QComboBox();

    // Create a menu item in the combo box for each list item and add it
    // to the layout
    for(int item = 0; item < ui.ParamListSize(group, param); item++) {
      iString btext = ui.ParamListBrief(group, param, item);
      btext += " (";
      btext += ui.ParamListValue(group, param, item);
      btext += ")";
      p_combo->insertItem(item, (iString)btext);
    }
    lo->addWidget(p_combo);
    connect(p_combo, SIGNAL(activated(int)),
            this, SIGNAL(ValueChanged()));

    p_type = ComboWidget;
    RememberWidget(p_combo);
  }


  GuiComboBoxParameter::~GuiComboBoxParameter() {
    delete p_combo;
  }


  void GuiComboBoxParameter::Set(iString newValue) {
    iString value = newValue;
    value.UpCase();

    int foundAtButton = -1;
    for(int i = 0; i < p_ui->ParamListSize(p_group, p_param); i++) {
      iString option = p_ui->ParamListValue(p_group, p_param, i);
      option.UpCase();
      if(option.compare(0, value.size(), value) == 0) foundAtButton = i;
    }

    if(foundAtButton != -1) {
      p_combo->setCurrentIndex(foundAtButton);
    }

    emit ValueChanged();
  }


  iString GuiComboBoxParameter::Value() {
    return (iString)p_ui->ParamListValue(p_group, p_param,
                                         p_combo->currentIndex());
  }

  std::vector<std::string> GuiComboBoxParameter::Exclusions() {
    std::vector<std::string> list;

    int index = p_combo->currentIndex();

    for(int i = 0; i < p_ui->ParamListExcludeSize(p_group, p_param, index); i++) {
      std::string s = p_ui->ParamListExclude(p_group, p_param, index, i);
      list.push_back(s);
    }

    return list;
  }

  void GuiComboBoxParameter::setOption (int option) {
    std::cout << "Combo box option: " << option << std::endl;
  }
}
