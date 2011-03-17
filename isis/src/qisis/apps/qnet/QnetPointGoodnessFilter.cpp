#include <QGridLayout>
#include <QMessageBox>

#include "QnetPointGoodnessFilter.h"
#include "QnetNavTool.h"

#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"
#include "ControlNet.h"
#include "SerialNumberList.h"
#include "SpecialPixel.h"
#include "qnet.h"

using namespace Isis;
using namespace std;
using namespace Qisis::Qnet;

namespace Qisis {
  /**
   * Contructor for the Point Goodness of Fit filter.  It creates 
   * the Goodness of Fit filter window found in the navtool 
   * 
   * @param parent The parent widget for the point type
   *               filter
   * @internal 
   *  @history  2010-06-02 Jeannie Walldren - Modify default
   *                          settings of checkboxes and line edits
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null in constructor.
   *  
   */
  QnetPointGoodnessFilter::QnetPointGoodnessFilter (QWidget *parent) : QnetFilter(parent) {
    p_lessThanCB = NULL; 
    p_greaterThanCB = NULL; 
    p_maxValueEdit = NULL;
    p_minValueEdit = NULL;

    // Create the components for the filter window
    p_lessThanCB = new QCheckBox("Less than ");
    p_maxValueEdit = new QLineEdit();
    p_greaterThanCB = new QCheckBox("Greater than ");
    p_minValueEdit = new QLineEdit();
    QLabel *pad = new QLabel();

    p_lessThanCB->setChecked(false);
    p_maxValueEdit->setEnabled(false);
    p_greaterThanCB->setChecked(false);
    p_minValueEdit->setEnabled(false);

    connect(p_lessThanCB,SIGNAL(clicked()),this,SLOT(clearEdit()));
    connect(p_greaterThanCB,SIGNAL(clicked()),this,SLOT(clearEdit()));

    // Create the layout and add the components to it
    QGridLayout *gridLayout = new QGridLayout();
    //gridLayout->addWidget(label,0,0,1,2);
    gridLayout->addWidget(p_lessThanCB,1,0,1,2);
    gridLayout->addWidget(p_maxValueEdit,2,0);
    gridLayout->addWidget(p_greaterThanCB,3,0,1,2);
    gridLayout->addWidget(p_minValueEdit,4,0);
    gridLayout->addWidget(pad,5,0);
    gridLayout->setRowStretch(5,50);
    this->setLayout(gridLayout);
  }

  /**
   * @brief Method overwrites parent method.
   * This method keeps all points that contain at least one
   * measure whose Goodness of Fit is within the range specified 
   * by the user. 
   *  
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Original Version
   *   @history 2009-01-08 Jeannie Walldren - Modified to remove
   *                          new filter points from the existing
   *                          filtered list. Previously, a new
   *                          filtered list was created from the
   *                          entire control net each time.
   */
  void QnetPointGoodnessFilter::filter() {
    // Make sure there is a control net loaded
    if (g_controlNetwork == NULL) {
      QMessageBox::information((QWidget *)parent(),
                               "Error","No points to filter");
      return;
    }

    // Make sure the user entered a value to use in the filtering
    if (p_lessThanCB->isChecked() && p_maxValueEdit->text() == "") {
      QMessageBox::information((QWidget *)parent(),
                               "Error","Maximum Goodness of Fit value must be entered");
      return;
    }
    if (p_greaterThanCB->isChecked() && p_minValueEdit->text() == "") {
      QMessageBox::information((QWidget *)parent(),
                               "Error","Minimum Goodness of Fit value must be entered");
      return;
    }

    // Get the user entered filtering value
    double maxValue = p_maxValueEdit->text().toDouble();
    double minValue = p_minValueEdit->text().toDouble();

    // Loop through each value of the filtered points list 
    // Loop in reverse order since removal list of elements affects index number
    for (int i = g_filteredPoints.size()-1; i >= 0; i--) {
      ControlPoint &cp = *(*g_controlNetwork)[g_filteredPoints[i]];
      int numMeasOutsideRange = 0;
      // Loop through each measure of the point at this index of the control net
      for (int j = 0; j < cp.GetNumMeasures(); j++) {

        double goodnessOfFit = cp[j]->GetLogData(
                    ControlMeasureLogData::GoodnessOfFit).GetNumericalValue();
        if (goodnessOfFit == Isis::Null) {
          numMeasOutsideRange++;
        }
        else if (p_lessThanCB->isChecked() && p_greaterThanCB->isChecked()) {
          if (goodnessOfFit < maxValue && goodnessOfFit > minValue) break;
          else numMeasOutsideRange++;
        }
        else if (p_lessThanCB->isChecked()) { 
          if (goodnessOfFit < maxValue) break;
          else numMeasOutsideRange++;
        }
        else if (p_greaterThanCB->isChecked()) {
          if (goodnessOfFit > minValue) break;
          else numMeasOutsideRange++;
        }
      }
      // if no measures are within the range, remove from filter list
      if (cp.GetNumMeasures() == numMeasOutsideRange) {
        g_filteredPoints.removeAt(i);
      }
    }

    // Tell the navtool that a list has been filtered and it needs to update
    emit filteredListModified();
    return;
  }
  /**
   * Clears and disables the corresponding line edit if the "less 
   * than" or "greater than" checkBox is "unchecked".  Method 
   * overrides parent method. 
   *  
   * @internal 
   *   @history 2010-06-02 Jeannie Walldren - Disable the line
   *            edit so the user can not enter a value unless the
   *            corresponding box is checked.
   *  
   */
  void QnetPointGoodnessFilter::clearEdit() {

    if (p_lessThanCB->isChecked()) {
      p_maxValueEdit->setEnabled(true);
    }
    else {
      p_maxValueEdit->clear();
      p_maxValueEdit->setEnabled(false);
    }
    if (p_greaterThanCB->isChecked()) {
      p_minValueEdit->setEnabled(true);
    }
    else {
      p_minValueEdit->clear();
      p_minValueEdit->setEnabled(false);
    }
  }
}
