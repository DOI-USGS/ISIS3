#include "SaveAsDialog.h"

#include <iostream>

#include <QButtonGroup>
#include <QLayout>
#include <QFileDialog>
#include <QPushButton>

using namespace std;

namespace Isis {

  /**
   * Constructor - Displays FileDialog with different save options
   *
   * @author Sharmila Prasad (5/11/2011)
   *
   * @param pTitle      - Dialog Title
   * @param pFilterList - Dialog Filter list
   * @param pDir        - Current Directory
   * @param pParent     - Parent widget
   */
  SaveAsDialog::SaveAsDialog(QString pTitle, QStringList &pFilterList, QDir &pDir, QWidget *pParent) :
    FileDialog(pTitle, pFilterList, pDir, pParent), p_dir(pDir)
    {

    this->setFileMode(QFileDialog::AnyFile);
    // This returns a list of all the buttons in QFileDialog
    QList<QPushButton *> allPButtons = this->findChildren<QPushButton *>();

    // Edit the first (Open) button title.
    this->setLabelText(QFileDialog::Accept, "Save");
    /// Edit the second (Cancel) button title.
    this->setLabelText(QFileDialog::Reject, "Cancel");

    p_saveAsType = FullImage;
    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->setAlignment(Qt::AlignLeft);
    hBoxLayout->setSpacing(25);
    hBoxLayout->setContentsMargins (25, 11, 25, 11 );

    // Button Group
    QButtonGroup *exportOptionsGrp = new QButtonGroup();
    exportOptionsGrp->setExclusive(true); // only one option can be clicked

    p_fullImage    = new QRadioButton("Export Entire &Image", this);
    connect(p_fullImage, SIGNAL(clicked(bool)), this, SLOT(setFullImage(bool)));
    exportOptionsGrp->addButton(p_fullImage);
    p_fullImage->setWhatsThis("Make a duplicate of the original image.");

    p_exportAsIs    = new QRadioButton("Export Viewport &As Is", this);
    connect(p_exportAsIs, SIGNAL(clicked(bool)), this, SLOT(setAsIs(bool)));
    exportOptionsGrp->addButton(p_exportAsIs);
    p_exportAsIs->setWhatsThis("Save the viewport as it is currently being viewed.");

    p_exportFullRes = new QRadioButton("Export Viewport at Full &Res", this);
    connect(p_exportFullRes, SIGNAL(clicked(bool)), this, SLOT(setFullResolution(bool)));
    exportOptionsGrp->addButton(p_exportFullRes);
    p_exportFullRes->setWhatsThis("Save the viewport but at the full resolution of the original image.");

    hBoxLayout->addWidget(p_exportAsIs);
    hBoxLayout->addWidget(p_exportFullRes);

    p_fullImage->setEnabled(true);
    p_exportAsIs->setEnabled(true);
    p_exportFullRes->setEnabled(true);

    p_fullImage->setChecked(true);

    QLayout *dialogLayout = layout();
    dialogLayout->addWidget(p_fullImage);
    dialogLayout->addItem(hBoxLayout);
    dialogLayout->setAlignment(hBoxLayout, Qt::AlignLeft );
    setLayout(dialogLayout);

  }

  /**
   * Get user chosen save type
   *
   * @author Sharmila Prasad (5/11/2011)
   *
   * @return int - Return user chosen save type
   */
  int SaveAsDialog::getSaveAsType()
  {
    if (p_exportAsIs->isChecked())
      return ExportAsIs;
    if(p_exportFullRes->isChecked())
      return ExportFullRes;
    return FullImage;
  }

  /**
   * Check FullImage radio button and if checked set the
   * saveAsType to FullImage
   *
   * @author Sharmila Prasad (5/11/2011)
   *
   * @param pbChecked - Button Checked(true/false)
   */
  void SaveAsDialog::setFullImage(bool pbChecked)
  {
    if(pbChecked) {
      p_saveAsType = FullImage;
    }
  }

  /**
   * Check ExportAsIs radio button and if checked set the
   * saveAsType to ExportAsIs
   *
   * @author Sharmila Prasad (5/11/2011)
   *
   * @param pbChecked - Button Checked(true/false)
   */
  void SaveAsDialog::setAsIs(bool pbChecked)
  {
    if(pbChecked) {
      p_saveAsType = ExportAsIs;
    }
  }

  /**
   * Check ExportFullRes radio button and if checked set the
   * saveAsType to ExportFullRes
   *
   * @author Sharmila Prasad (5/11/2011)
   *
   * @param pbChecked - Button Checked(true/false)
   */
  void SaveAsDialog::setFullResolution(bool pbChecked)
  {
    if(pbChecked) {
      p_saveAsType = ExportFullRes;
    }
  }
}
