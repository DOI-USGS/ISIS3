#include <QGridLayout>
#include <QMessageBox>
#include "QnetCubePointsFilter.h"
#include "QnetNavTool.h"
#include "ControlNet.h"
#include "SerialNumberList.h"

#include "qnet.h"

using namespace Qisis::Qnet;

namespace Qisis {
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
  QnetCubePointsFilter::QnetCubePointsFilter (QWidget *parent) : QnetFilter(parent) {
    p_lessThanRB = NULL; 
    p_greaterThanRB = NULL;
    p_pointEdit = NULL;     

    // Create the components for the filter window
    QLabel *label = new QLabel("Filter by number of points in cube");
    p_lessThanRB = new QRadioButton("Less than (undercontrolled)");
    p_greaterThanRB = new QRadioButton("Greater than (overcontrolled)");
    p_pointEdit = new QLineEdit();
    QLabel *units = new QLabel ("points");
    p_lessThanRB->setChecked(true);
    QLabel *pad = new QLabel();

    // Create the layout and add the components to it
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(label,0,0,1,2);
    gridLayout->addWidget(p_lessThanRB,1,0,1,2);
    gridLayout->addWidget(p_greaterThanRB,2,0,1,2);
    gridLayout->addWidget(p_pointEdit,3,0);
    gridLayout->addWidget(units,3,1);
    gridLayout->addWidget(pad,4,0);
    gridLayout->setRowStretch(4,50);
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
    if (g_serialNumberList == NULL) {
      QMessageBox::information((QWidget *)parent(),
                               "Error","No cubes to filter");
      return;
    }

    // Make sure the user has entered a value for filtering
    int num = -1;
    if (p_pointEdit->text() == "") {
      QMessageBox::information((QWidget *)parent(),
                               "Error","Point value must be entered");
      return;
    }

    // Get the value the user entered for filtering
    num = p_pointEdit->text().toInt();

    // Loop through each image in the list
    // Loop in reverse order since removal of list elements affects index number
    for (int i = g_filteredImages.size()-1; i >= 0; i--) {

      // Set up a counter parameter
      int count = 0;

      // Loop through each control point in the network
      for (int c=0; c<g_controlNetwork->Size(); c++) {
        Isis::ControlPoint cp = (*g_controlNetwork)[c];

        // Check to see if it has a measure on the current image
        for (int cm=0; cm<cp.Size(); cm++) {
          // Increment the count if it does
          if ((cp[cm].CubeSerialNumber()) == ((*g_serialNumberList).SerialNumber(g_filteredImages[i]))){ 
            count++;
            break;
          }
        }
      }

      //  If the count is greater than the number the user entered and the 
      // greater than option was selected, add the image to the filtered list
      if (p_greaterThanRB->isChecked()) {
        if (count > num) continue;
        else g_filteredImages.removeAt(i);
      }
      // If the count is less than the number the user entered and the less than
      // option was selected, add the image to the filtered list
      else if (p_lessThanRB->isChecked()) {
        if (count < num) continue;
        else g_filteredImages.removeAt(i);
      }
    }
    // Tell the navtool a list has been filtered and it needs to update
    emit filteredListModified();
    return;
  }
}
