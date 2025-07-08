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

      // If there's helper buttons, create a vertical box layout
      // and add it next to the radio buttons
      if((item == 0) && (p_ui->HelpersSize(group, param) != 0)) {
        // Create Vertical layout box
        QVBoxLayout *vlo = new QVBoxLayout;
        grid->addLayout(vlo, 0, 3);

        // Get helpers and add to vertical layout
        QWidget *helper = AddHelpers(p_buttonGroup);
        vlo->addWidget(helper, 0, Qt::AlignTop);

        RememberWidget(helper);
      }
      // Create radio button & add to vertical layout
      QRadioButton *rb = new QRadioButton(btext);
      rb->setObjectName(ui.ParamListValue(group, param, item));
      lo->addWidget(rb);
      p_buttonGroup->addButton(rb);
      RememberWidget(rb);
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

