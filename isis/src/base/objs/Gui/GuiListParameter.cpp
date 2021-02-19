/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QVBoxLayout>
#include <QRadioButton>
#include <QAbstractButton>
#include <QList>

#include "UserInterface.h"

#include "GuiListParameter.h"

namespace Isis {

  GuiListParameter::GuiListParameter(QGridLayout *grid, UserInterface &ui,
                                     int group, int param) :
    GuiParameter(grid, ui, group, param) {

    // Reset the default alignment of the label
    p_label->setAlignment(Qt::AlignRight | Qt::AlignTop);


    // Create a vertical box layout for the radio buttons and add it to
    // the grid layout
    QVBoxLayout *lo = new QVBoxLayout;
    grid->addLayout(lo, param, 2);

    // Create a button group so these buttons don't react to other buttons
    // with the same parent
    p_buttonGroup = new QButtonGroup();

    // Create a button for each list item and add each to a button group and
    // to the layout
    for(int item = 0; item < ui.ParamListSize(group, param); item++) {
      QString btext = ui.ParamListBrief(group, param, item);
      btext += " (";
      btext += ui.ParamListValue(group, param, item);
      btext += ")";

      // If there's helper buttons, they need to be added with the 1st value in
      // the list
      if((item == 0) && (p_ui->HelpersSize(group, param) != 0)) {
        // Create Horizontal layout box
        QHBoxLayout *hlo = new QHBoxLayout;
        lo->addLayout(hlo);

        // Create radio button & add to horizontal layout
        QRadioButton *rb = new QRadioButton(btext);
        hlo->addWidget(rb);
        p_buttonGroup->addButton(rb);

        // Get helpers and add to horizontal layout
        QWidget *helper = AddHelpers(p_buttonGroup);
        hlo->addWidget(helper);

        RememberWidget(rb);
        RememberWidget(helper);
      }
      else {
        QRadioButton *rb = new QRadioButton(btext);
        lo->addWidget(rb);
        p_buttonGroup->addButton(rb);
        RememberWidget(rb);
      }
    }
    connect(p_buttonGroup, SIGNAL(buttonClicked(QAbstractButton *)),
            this, SIGNAL(ValueChanged()));

    p_type = ListWidget;
  }


  GuiListParameter::~GuiListParameter() {
    delete p_buttonGroup;
  }


  void GuiListParameter::Set(QString newValue) {
    QString value = newValue.toUpper();

    int foundAtButton = -1;
    for(int i = 0; i < p_ui->ParamListSize(p_group, p_param); i++) {
      QString option = p_ui->ParamListValue(p_group, p_param, i).toUpper();
      //if(option.compare(0, value.size(), value) == 0) foundAtButton = i;
      if(option == value) foundAtButton = i;
    }

    if(foundAtButton != -1) {
      p_buttonGroup->buttons()[foundAtButton]->setChecked(true);
    }

    emit ValueChanged();
  }


  QString GuiListParameter::Value() {
    if(p_buttonGroup->checkedButton() == 0) {
      return "";
    }

    return p_ui->ParamListValue(p_group, p_param,
                                p_buttonGroup->buttons().indexOf(p_buttonGroup->checkedButton()));
  }

  std::vector<QString> GuiListParameter::Exclusions() {
    std::vector<QString> list;

    if(p_buttonGroup->checkedButton() == 0) return list;
    int index = p_buttonGroup->buttons().indexOf(p_buttonGroup->checkedButton());

    for(int i = 0; i < p_ui->ParamListExcludeSize(p_group, p_param, index); i++) {
      QString s = p_ui->ParamListExclude(p_group, p_param, index, i);
      list.push_back(s);
    }

    return list;
  }
}

