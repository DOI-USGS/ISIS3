#include "MatchToolNewPointDialog.h"

#include <QtGui>
#include <algorithm>

#include "ControlNet.h"
#include "iString.h"
#include "SerialNumberList.h"


namespace Isis {
  // initialize static variable
  QString MatchToolNewPointDialog::lastPtIdValue = "";

  /**
   * MatchToolNewPointDialog constructor
   * @param parent The parent widget for the
   *               cube points filter
   *
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Set lastPointIdValue
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null.
   *   @history 2011-03-08 Tracie Sucharski - If there is a saved ID
   *                          and there is no point in the network with that
   *                          id, do not disable "ok" button.  This allows
   *                          user to use the same id from a previous point
   *                          creation if the point was never saved.
   *
   */
  MatchToolNewPointDialog::MatchToolNewPointDialog(const ControlNet &cnet, QWidget *parent) : QDialog(parent) {

    ptIdValue = NULL;
    fileList = NULL;
    m_ptIdLabel = NULL;
    m_doneButton = NULL;

    m_ptIdLabel = new QLabel("Point ID:");
    ptIdValue = new QLineEdit;
    m_ptIdLabel->setBuddy(ptIdValue);
    ptIdValue->setText(lastPtIdValue);
    ptIdValue->selectAll();
    connect(ptIdValue, SIGNAL(textChanged(const QString &)),
            this, SLOT(enableDoneButton(const QString &)));

    QLabel *listLabel = new QLabel("Displayed Cubes / Selected measures:  \n"
                                   "Left click on the cube viewport to select approximate measure "
                                   "location.\nCubes will be highlighted below as you select "
                                   "measure locations.");

    fileList = new QListWidget;
    fileList->setSelectionMode(QAbstractItemView::NoSelection);

    //  Create Done & Cancel buttons
    m_doneButton = new QPushButton("Done selecting measures");
    m_doneButton->setToolTip("All measures have been selected.  Load the new point into the "
                             "control point editor for refinement.");
    m_doneButton->setWhatsThis("You have left-clicked on all cube viewports you want to create "
                               "as a control measure.  The new point will be loaded into the "
                               "control point editor for refinement.");
    //  If the last point id used was never saved to network, do not set ok
    //  button to faslse
    if (lastPtIdValue.isEmpty() || cnet.ContainsPoint(lastPtIdValue)) {
      m_doneButton->setEnabled(false);
    }
    QPushButton *cancelButton = new QPushButton("Cancel");
    cancelButton->setToolTip("Cancel without creating a new point.");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_doneButton);
    buttonLayout->addWidget(cancelButton);

    connect(m_doneButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_doneButton, SIGNAL(clicked()), this, SIGNAL(measuresFinished()));

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(cancelButton, SIGNAL(clicked()), this, SIGNAL(newPointCanceled()));

    QHBoxLayout *ptIdLayout = new QHBoxLayout;
    ptIdLayout->addWidget(m_ptIdLabel);
    ptIdLayout->addWidget(ptIdValue);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(ptIdLayout);
    vLayout->addWidget(listLabel);
    vLayout->addWidget(fileList);
    vLayout->addLayout(buttonLayout);

    setLayout(vLayout);
    setWindowTitle("Create New ControlPoint");
    show();

  }


  /**
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Removed "std::"
   *            since "using namespace std"
   *   @history 2010-10-29 Tracie Sucharski - Changed std::vector<std::string>
   *            to QSringList
   */
  void MatchToolNewPointDialog::setFiles(QStringList pointFiles) {

    fileList->addItems(pointFiles);

  }



  void MatchToolNewPointDialog::highlightFile(QString file) {

    QList<QListWidgetItem *> found = fileList->findItems(file, Qt::MatchFixedString);
    if (!found.isEmpty()) {
      fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
      found.at(0)->setSelected(true);
      fileList->setSelectionMode(QAbstractItemView::NoSelection);
    }
  }



  /**
   *
   * @param text
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Set lastPointIdValue
   *            to the m_ptIdValue
   */
  void MatchToolNewPointDialog::enableDoneButton(const QString &text) {
    MatchToolNewPointDialog::lastPtIdValue = ptIdValue->text();
    m_doneButton->setEnabled(!text.isEmpty());
  }


}
