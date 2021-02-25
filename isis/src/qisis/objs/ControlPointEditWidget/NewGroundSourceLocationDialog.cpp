/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "NewGroundSourceLocationDialog.h"

#include <iostream>

#include <QButtonGroup>
#include <QLayout>
#include <QPushButton>

using namespace std;

namespace Isis {

  /**
   * Dialog to determine new ground source location
   *
   * @param title      - Dialog Title
   * @param filterList - Dialog Filter list
   * @param dir        - Current Directory
   * @param parent     - Parent widget
   */
  NewGroundSourceLocationDialog::NewGroundSourceLocationDialog(QString title, QString &directory,
                                                               QWidget *parent) :
                                 QFileDialog(parent, title, directory) {

    setFileMode(QFileDialog::Directory);
//  setOptions(QFileDialog::ShowDirsOnly);
//  // This returns a list of all the buttons in QFileDialog
//  QList<QPushButton *> allButtons = this->findChildren<QPushButton *>();
//  // Edit the first (Open) button title.
//  allButtons[0]->setText("&Save");
//
//  /// Edit the second (Cancel) button title.
//  allButtons[1]->setText("Cancel");

    QVBoxLayout *vertBoxLayout = new QVBoxLayout;
//  hBoxLayout->setAlignment(Qt::AlignLeft);
//  hBoxLayout->setSpacing(25);
//  hBoxLayout->setContentsMargins (25, 11, 25, 11 );

    // Button Group
//  QButtonGroup *optionsButtonGroup = new QButtonGroup();

    m_changeAllGround = new QCheckBox("Change location of all subsequent ground points loaded",
                                      this);
//  m_changeAllGround->setWhatsThis("Save the Image As Viewed with Full Resoultion");

    m_changeControlNet = new QCheckBox("Change location of ground source in control net."
                                           "  Note:  If above box is checked, all locations will"
                                           " be changed.", this);

//  optionsButtonGroup->addButton(m_changeAllGround);
//  optionsButtonGroup->addButton(m_changeControlNet);

    vertBoxLayout->addWidget(m_changeAllGround);
    vertBoxLayout->addWidget(m_changeControlNet);

    //  Get parent's layout (FileDialog), then add new buttons
    QLayout *dialogLayout = layout();
    dialogLayout->addItem(vertBoxLayout);
//  setLayout(dialogLayout);
  }


  /**
   * Indicates whether all subsequent ground source files should be found in new source directory.
   *
   */
  bool NewGroundSourceLocationDialog::changeAllGroundSourceLocation() {

    return m_changeAllGround->isChecked();
  }


  /**
   * Indicates whether the control network should be changed to reflect new ground source location.
   *
   */
  bool NewGroundSourceLocationDialog::changeControlNet() {

    return m_changeControlNet->isChecked();
  }
}
