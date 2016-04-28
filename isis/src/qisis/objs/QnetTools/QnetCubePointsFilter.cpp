#include "QnetCubePointsFilter.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QRadioButton>
#include <QtWidgets>

#include "QnetNavTool.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "SerialNumberList.h"

namespace Isis {
  /**
   * Contructor for the Cube Points filter.  It
   * creates the Points filter window found in
   * the navtool
   *
   * @param parent The parent widget for the
   *               cube points filter
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null.
   *
   */
  QnetCubePointsFilter::QnetCubePointsFilter(QnetNavTool *navTool, QWidget *parent) :
      QnetFilter(navTool, parent) {
    m_lessThanRB = NULL;
    m_greaterThanRB = NULL;
    m_pointEdit = NULL;

    // Create the components for the filter window
    QLabel *label = new QLabel("Filter by number of points in cube");
    m_lessThanRB = new QRadioButton("Less than (undercontrolled)");
    m_greaterThanRB = new QRadioButton("Greater than (overcontrolled)");
    m_pointEdit = new QLineEdit();
    QLabel *units = new QLabel("points");
    m_lessThanRB->setChecked(true);
    QLabel *pad = new QLabel();

    // Create the layout and add the components to it
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(label, 0, 0, 1, 2);
    gridLayout->addWidget(m_lessThanRB, 1, 0, 1, 2);
    gridLayout->addWidget(m_greaterThanRB, 2, 0, 1, 2);
    gridLayout->addWidget(m_pointEdit, 3, 0);
    gridLayout->addWidget(units, 3, 1);
    gridLayout->addWidget(pad, 4, 0);
    gridLayout->setRowStretch(4, 50);
    this->setLayout(gridLayout);
  }

  /**
   * Filters a list of images for images that have more or less
   * than the user entered number of points. The filtered list
   * will appear in the navtools cube list display.
   *
   * @internal
   *   @history 2009-01-08 Jeannie Walldren - Modified to create
   *                          new filtered list from images in the
   *                          existing filtered list. Previously,
   *                          a new filtered list was created from
   *                          the entire serial number list each
   *                          time.
   */
  void QnetCubePointsFilter::filter() {
    // Make sure we have a list of images to filter
    if (serialNumberList() == NULL) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "No cubes to filter");
      return;
    }

    // Make sure the user has entered a value for filtering
    int num = -1;
    if (m_pointEdit->text() == "") {
      QMessageBox::information((QWidget *)parent(),
          "Error", "Point value must be entered");
      return;
    }

    // Get the value the user entered for filtering
    num = m_pointEdit->text().toInt();

    // Loop through each image in the list
    // Loop in reverse order since removal of list elements affects index number
    for (int i = filteredImages().size() - 1; i >= 0; i--) {

      // Set up a counter parameter
      int count = 0;

      // Loop through each control point in the network
      for (int c = 0; c < controlNet()->GetNumPoints(); c++) {
        ControlPoint &cp = *(*controlNet())[c];

        // Check to see if it has a measure on the current image
        for (int cm = 0; cm < cp.GetNumMeasures(); cm++) {
          // Increment the count if it does
          if ((cp[cm]->GetCubeSerialNumber()) == ((*serialNumberList()).serialNumber(filteredImages()[i]))) {
            count++;
            break;
          }
        }
      }

      //  If the count is greater than the number the user entered and the
      // greater than option was selected, add the image to the filtered list
      if (m_greaterThanRB->isChecked()) {
        if (count > num)
          continue;
        else
          filteredImages().removeAt(i);
      }
      // If the count is less than the number the user entered and the less than
      // option was selected, add the image to the filtered list
      else if (m_lessThanRB->isChecked()) {
        if (count < num)
          continue;
        else
          filteredImages().removeAt(i);
      }
    }
    // Tell the navtool a list has been filtered and it needs to update
    emit filteredListModified();
    return;
  }
}
