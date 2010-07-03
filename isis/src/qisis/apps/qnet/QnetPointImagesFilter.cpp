#include <QGridLayout>
#include <QMessageBox>
#include "QnetPointImagesFilter.h"
#include "QnetNavTool.h"
#include "ControlNet.h"
#include "SerialNumberList.h"

#include "qnet.h"

using namespace Qisis::Qnet;

namespace Qisis {
  /**
   * Contructor for the Point Images filter.
   * It creates the Images filter window found
   * in the navtool
   *
   * @param parent The parent widget for the
   *               point points filter
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null.
   *
   */
  QnetPointImagesFilter::QnetPointImagesFilter(QWidget *parent) : QnetFilter(parent) {
    p_lessThanRB = NULL;
    p_greaterThanRB = NULL;
    p_imageEdit = NULL;
    // Create the components for the main window
    QLabel *label = new QLabel("Filter by number of images in each point");
    p_lessThanRB = new QRadioButton("Less than");
    p_greaterThanRB = new QRadioButton("Greater than");
    p_imageEdit = new QLineEdit();
    QLabel *units = new QLabel("images");
    p_lessThanRB->setChecked(true);
    QLabel *pad = new QLabel();

    // Create the layout and add the components to it
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(label, 0, 0, 1, 2);
    gridLayout->addWidget(p_lessThanRB, 1, 0, 1, 2);
    gridLayout->addWidget(p_greaterThanRB, 2, 0, 1, 2);
    gridLayout->addWidget(p_imageEdit, 3, 0);
    gridLayout->addWidget(units, 3, 1);
    gridLayout->addWidget(pad, 4, 0);
    gridLayout->setRowStretch(4, 50);
    this->setLayout(gridLayout);
  }

  /**
   * Filters a list of points for points that have less than or greater
   * than the entered number of images.  The filtered list will appear in
   * the navtools point list display.
   *
   * @internal
   *   @history 2009-01-08 Jeannie Walldren - Modified filter()
   *                          method to remove new filter points
   *                          from the existing filtered list.
   *                          Previously, a new filtered list was
   *                          created from the entire control net
   *                          each time.
   */
  void QnetPointImagesFilter::filter() {
    // Make sure we have points to filter
    if(g_controlNetwork == NULL) {
      QMessageBox::information((QWidget *)parent(),
                               "Error", "No points to filter");
      return;
    }

    // Make sure the user has entered a value for the filtering
    int num = -1;
    if(p_imageEdit->text() == "") {
      QMessageBox::information((QWidget *)parent(),
                               "Error", "Image filter value must be entered");
      return;
    }

    // Get the user entered filter value
    num = p_imageEdit->text().toInt();

    // Loop through each value of the filtered points list the user
    // entered value with the number of measures in the point and
    // remove it from the list if it is outside the filtering value range
    // Loop in reverse order since removal list of elements affects index number
    for(int i = g_filteredPoints.size() - 1; i >= 0; i--) {
      Isis::ControlPoint cp = (*g_controlNetwork)[g_filteredPoints[i]];
      if(p_lessThanRB->isChecked()) {
        if(cp.Size() < num) continue;
        else g_filteredPoints.removeAt(i);
      }
      else if(p_greaterThanRB->isChecked()) {
        if(cp.Size() > num) continue;
        else g_filteredPoints.removeAt(i);
      }
    }
    // Tell the navtool that a list has been filtered and it needs to update
    emit filteredListModified();
    return;
  }
}
