/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/07/11 20:16:52 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <iostream>
#include <string>

#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "CubeAttribute.h"
#include "GuiInputAttribute.h"

namespace Isis {
  //! Convenience access to dialog
  int GuiInputAttribute::GetAttributes (const std::string &defaultAttribute,
                                        std::string &newAttribute,
                                        const std::string &title,
                                        QWidget *parent) {
    // Construct dialog if necessary
    static GuiInputAttribute *p_dialog = 0;
    if (p_dialog == 0) {
      p_dialog = new GuiInputAttribute (parent);
    }
    p_dialog->setWindowTitle(title.c_str());

    // Load default attributes and then get the new ones
    p_dialog->SetAttributes(defaultAttribute);
    if (p_dialog->exec() == QDialog::Accepted) {
      newAttribute = p_dialog->GetAttributes();
      return 1;
    }
    newAttribute = defaultAttribute;
    return 0;
  }

  //! Constuctor
  GuiInputAttribute::GuiInputAttribute (QWidget *parent) : QDialog (parent) {
    // Create the two radio buttons
    QRadioButton *allBands = new QRadioButton("&All Bands");
    allBands->setToolTip("Select all bands from the input cube");

    QRadioButton *listBands = new QRadioButton("&Band List");
    listBands->setToolTip("Select any bands from the input cube");

    // Create the button group for the radio buttons
    p_buttonGroup = new QButtonGroup();
    p_buttonGroup->addButton(allBands);
    p_buttonGroup->addButton(listBands);
    p_buttonGroup->setExclusive(true);

    // Create the text field for list toggle button
    p_lineEdit = new QLineEdit ();
    connect(allBands, SIGNAL(toggled(bool)), p_lineEdit, SLOT(setDisabled(bool)));
    allBands->setChecked(true);

    // Put the buttons and text field in a gridlayout
    QGridLayout *gridLayout = new QGridLayout ();
    gridLayout->addWidget(allBands,0,0);
    gridLayout->addWidget(listBands,1,0);
    gridLayout->addWidget(p_lineEdit, 1, 1);

    // Create the action buttons
    QPushButton *okButton = new QPushButton ("Ok");
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    QPushButton* cancelButton = new QPushButton ("Cancel");
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

#if 0
    // Add them to a button group
    QButtonGroup *actionGroup = new QButtonGroup();
    actionGroup->addButton(okButton);
    actionGroup->addButton(cancelButton);
#endif

    // Put the buttons in a horizontal orientation
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->addWidget(okButton);
    actionLayout->addWidget(cancelButton);

    // Put the grid layout and action layout on the dialog
    QVBoxLayout *dialogLayout = new QVBoxLayout(this);
    dialogLayout->addLayout(gridLayout);
    dialogLayout->addLayout(actionLayout);
  }


  // Destructor
  GuiInputAttribute::~GuiInputAttribute () {}


  // Return the attributes in the dialog
  std::string GuiInputAttribute::GetAttributes () {
    if (p_lineEdit->isEnabled()) {
      Isis::iString s = p_lineEdit->text().toStdString();
      s.Remove(" ");
      s.Trim("+");
      if (s == "") return s;
      return (std::string)"+" + s;
    }
    else {
      return std::string("");
    }
  }

  // Set the attributes in the dialog
  void GuiInputAttribute::SetAttributes (const std::string &value) {
    Isis::CubeAttributeInput att(value);
    std::vector<std::string> bands = att.Bands();
    if (bands.size() == 0) {
      p_buttonGroup->buttons()[0]->setChecked(true);
      p_lineEdit->setText("");
    }
    else {
      p_buttonGroup->buttons()[1]->setChecked(true);
      p_lineEdit->setText((iString)att.BandsStr());
    }
  }
}

