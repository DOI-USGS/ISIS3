/**
 * @file
 * $Revision: 1.2 $
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
#include <QLabel>
#include <QGroupBox>
#include <QDoubleValidator>

#include "CubeAttribute.h"
#include "GuiOutputAttribute.h"

namespace Isis {
  //! Convenience access to dialog
  int GuiOutputAttribute::GetAttributes (const std::string &defaultAttribute,
                                        std::string &newAttribute,
                                        const std::string &title,
                                        bool allowProp,
                                        QWidget *parent) {
    // Construct dialog if necessary
    static GuiOutputAttribute *p_dialog = 0;
    if (p_dialog == 0) {
      p_dialog = new GuiOutputAttribute (parent);
    }
    p_dialog->setWindowTitle((iString)title);
    p_dialog->SetPropagation(allowProp);

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
  GuiOutputAttribute::GuiOutputAttribute (QWidget *parent) : QDialog (parent) {
    // Create the pixel type group
    p_propagate = new QRadioButton("&Propagate");
    p_propagate->setToolTip("Propagate pixel type from input cube");
    p_unsignedByte = new QRadioButton("&Unsigned Byte");
    p_unsignedByte->setToolTip("Unsigned 8-bit pixels");
    p_signedWord = new QRadioButton("&Signed Word");
    p_signedWord->setToolTip("Signed 16-bit pixels");
    p_real = new QRadioButton("&Real");
    p_real->setToolTip("Floating point 32-bit pixels");

    QButtonGroup *buttonGroup = new QButtonGroup();
    buttonGroup->addButton(p_propagate);
    buttonGroup->addButton(p_unsignedByte);
    buttonGroup->addButton(p_signedWord);
    buttonGroup->addButton(p_real);
    buttonGroup->setExclusive(true);

    p_minEdit = new QLineEdit ();
    p_maxEdit = new QLineEdit ();
    QLabel *minLabel = new QLabel("Minimum");
    QLabel *maxLabel = new QLabel("Maximum");
    connect (p_propagate,SIGNAL(toggled(bool)),p_minEdit,SLOT(setDisabled(bool)));
    connect (p_propagate,SIGNAL(toggled(bool)),p_maxEdit,SLOT(setDisabled(bool)));
    connect (p_unsignedByte,SIGNAL(toggled(bool)),p_minEdit,SLOT(setEnabled(bool)));
    connect (p_unsignedByte,SIGNAL(toggled(bool)),p_maxEdit,SLOT(setEnabled(bool)));
    connect (p_signedWord,SIGNAL(toggled(bool)),p_minEdit,SLOT(setEnabled(bool)));
    connect (p_signedWord,SIGNAL(toggled(bool)),p_maxEdit,SLOT(setEnabled(bool)));
    connect (p_real,SIGNAL(toggled(bool)),p_minEdit,SLOT(setDisabled(bool)));
    connect (p_real,SIGNAL(toggled(bool)),p_maxEdit,SLOT(setDisabled(bool)));
    p_minEdit->setValidator(new QDoubleValidator(p_minEdit));
    p_maxEdit->setValidator(new QDoubleValidator(p_maxEdit));

    QGridLayout *gridLayout = new QGridLayout ();
    gridLayout->addWidget(p_propagate,0,0);
    gridLayout->addWidget(p_unsignedByte,1,0);
    gridLayout->addWidget(p_signedWord,2,0);
    gridLayout->addWidget(p_real,3,0);
    gridLayout->addWidget(minLabel,0,1);
    gridLayout->addWidget(p_minEdit,1,1);
    gridLayout->addWidget(maxLabel,2,1);
    gridLayout->addWidget(p_maxEdit,3,1);

    QGroupBox *pixelTypeBox = new QGroupBox("Pixel Type");
    pixelTypeBox->setLayout(gridLayout);

    // Create detached/attached stuff
    p_attached = new QRadioButton("&Attached");
    p_attached->setToolTip("Save labels and image data in one file");
    p_detached = new QRadioButton("&Detached");
    p_detached->setToolTip("Save labels and image data in separate files");
    p_attached->setChecked(true);

    buttonGroup = new QButtonGroup();
    buttonGroup->addButton(p_attached);
    buttonGroup->addButton(p_detached);
    buttonGroup->setExclusive(true);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(p_attached);
    layout->addWidget(p_detached);

    QGroupBox *labelFormatBox = new QGroupBox("Label Format");
    labelFormatBox->setLayout(layout);

    // Create cube format stuff
    p_tiled = new QRadioButton("&Tiled");
    p_tiled->setToolTip("Save image data in tiled format");
    p_bsq = new QRadioButton("&BSQ");
    p_bsq->setToolTip("Save image data in band sequential format");

    buttonGroup = new QButtonGroup();
    buttonGroup->addButton(p_tiled);
    buttonGroup->addButton(p_bsq);
    buttonGroup->setExclusive(true);

    layout = new QVBoxLayout();
    layout->addWidget(p_tiled);
    layout->addWidget(p_bsq);

    QGroupBox *cubeFormatBox = new QGroupBox("Cube Format");
    cubeFormatBox->setLayout(layout);

    // Create cube format stuff
    p_lsb = new QRadioButton("&LSB");
    p_lsb->setToolTip("Save image data in little endian format");
    p_msb = new QRadioButton("&MSB");
    p_msb->setToolTip("Save image data in big endian format");

    buttonGroup = new QButtonGroup();
    buttonGroup->addButton(p_lsb);
    buttonGroup->addButton(p_msb);
    buttonGroup->setExclusive(true);

    layout = new QVBoxLayout();
    layout->addWidget(p_lsb);
    layout->addWidget(p_msb);

    QGroupBox *byteOrderBox = new QGroupBox("Byte Order");
    byteOrderBox->setLayout(layout);

    // Create the action buttons
    QPushButton *okButton = new QPushButton ("Ok");
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    QPushButton* cancelButton = new QPushButton ("Cancel");
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    // Put the buttons in a horizontal orientation
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->addWidget(okButton);
    actionLayout->addWidget(cancelButton);

    // Put the grid layout and action layout on the dialog
    QVBoxLayout *dialogLayout = new QVBoxLayout(this);
    dialogLayout->addWidget(pixelTypeBox);
    dialogLayout->addWidget(labelFormatBox);
    dialogLayout->addWidget(cubeFormatBox);
    dialogLayout->addWidget(byteOrderBox);
    dialogLayout->addLayout(actionLayout);
  }


  // Destructor
  GuiOutputAttribute::~GuiOutputAttribute () {}


  // Return the attributes in the dialog
  std::string GuiOutputAttribute::GetAttributes () {
    std::string att;
    if (p_lsb->isChecked()) att += "+lsb";
    if (p_msb->isChecked()) att += "+msb";

    if (p_tiled->isChecked()) att += "+tiled";
    if (p_bsq->isChecked()) att += "+bsq";

    if (p_attached->isChecked()) att += "+attached";
    if (p_detached->isChecked()) att += "+detached";

    if (p_real->isChecked()) att += "+real";
    if (p_unsignedByte->isChecked()) att += "+8bit";
    if (p_signedWord->isChecked()) att += "+16bit";

    if (p_unsignedByte->isChecked() || p_signedWord->isChecked()) {
      if ((p_minEdit->text() != "") && (p_maxEdit->text() != "")) {
        att += "+";
        att += p_minEdit->text().toStdString();
        att += ":";
        att += p_maxEdit->text().toStdString();
      }
    }

    Isis::CubeAttributeOutput catt(att);
    std::string s;
    catt.Write(s);
    return s;
  }

  // Set the attributes in the dialog
  void GuiOutputAttribute::SetAttributes (const std::string &value) {
    Isis::CubeAttributeOutput att(value);
    if (att.FileFormat() == Isis::Tile) {
      p_tiled->setChecked(true);
    }
    else {
      p_bsq->setChecked(true);
    }

    if (att.ByteOrder() == Isis::Lsb) {
      p_lsb->setChecked(true);
    }
    else {
      p_msb->setChecked(true);
    }

    if (att.AttachedLabel()) {
      p_attached->setChecked(true);
    }
    else {
      p_detached->setChecked(true);
    }

    if (att.PropagatePixelType()) {
      p_propagate->setChecked(true);
    }
    else if (att.PixelType() == Isis::UnsignedByte) {
      p_unsignedByte->setChecked(true);
    }
    else if (att.PixelType() == Isis::SignedWord) {
      p_signedWord->setChecked(true);
    }
    else {
      p_real->setChecked(true);
    }

    if (!att.PropagateMinimumMaximum()) {
      p_minEdit->setText(QString::number(att.Minimum()));
      p_maxEdit->setText(QString::number(att.Maximum()));
    }
  }

  //! Do we allow propagation
  void GuiOutputAttribute::SetPropagation(bool enabled) {
    p_propagationEnabled = enabled;
    p_propagate->setEnabled(enabled);
  }
}

