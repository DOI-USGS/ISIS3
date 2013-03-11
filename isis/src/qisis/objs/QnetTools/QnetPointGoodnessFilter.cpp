#include "QnetPointGoodnessFilter.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>

#include "QnetNavTool.h"

#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "SerialNumberList.h"
#include "SpecialPixel.h"

using namespace std;

namespace Isis {
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
  QnetPointGoodnessFilter::QnetPointGoodnessFilter (QnetNavTool *navTool,
      QWidget *parent) : QnetFilter(navTool, parent) {
    m_lessThanCB = NULL; 
    m_greaterThanCB = NULL; 
    m_maxValueEdit = NULL;
    m_minValueEdit = NULL;

    // Create the components for the filter window
    m_lessThanCB = new QCheckBox("Less than ");
    m_maxValueEdit = new QLineEdit();
    m_greaterThanCB = new QCheckBox("Greater than ");
    m_minValueEdit = new QLineEdit();
    QLabel *pad = new QLabel;

    m_lessThanCB->setChecked(false);
    m_maxValueEdit->setEnabled(false);
    m_greaterThanCB->setChecked(false);
    m_minValueEdit->setEnabled(false);

    connect(m_lessThanCB,SIGNAL(clicked()),this,SLOT(clearEdit()));
    connect(m_greaterThanCB,SIGNAL(clicked()),this,SLOT(clearEdit()));

    // Create the layout and add the components to it
    QGridLayout *gridLayout = new QGridLayout();
    //gridLayout->addWidget(label,0,0,1,2);
    gridLayout->addWidget(m_lessThanCB,1,0,1,2);
    gridLayout->addWidget(m_maxValueEdit,2,0);
    gridLayout->addWidget(m_greaterThanCB,3,0,1,2);
    gridLayout->addWidget(m_minValueEdit,4,0);
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
    if (controlNet() == NULL) {
      QMessageBox::information((QWidget *)parent(),
                               "Error","No points to filter");
      return;
    }

    // Make sure the user entered a value to use in the filtering
    if (m_lessThanCB->isChecked() && m_maxValueEdit->text() == "") {
      QMessageBox::information((QWidget *)parent(),
                               "Error","Maximum Goodness of Fit value must be entered");
      return;
    }
    if (m_greaterThanCB->isChecked() && m_minValueEdit->text() == "") {
      QMessageBox::information((QWidget *)parent(),
                               "Error","Minimum Goodness of Fit value must be entered");
      return;
    }

    // Get the user entered filtering value
    double maxValue = m_maxValueEdit->text().toDouble();
    double minValue = m_minValueEdit->text().toDouble();

    // Loop through each value of the filtered points list 
    // Loop in reverse order since removal list of elements affects index number
    for (int i = filteredPoints().size()-1; i >= 0; i--) {
      ControlPoint &cp = *(*controlNet())[filteredPoints()[i]];
      int numMeasOutsideRange = 0;
      // Loop through each measure of the point at this index of the control net
      for (int j = 0; j < cp.GetNumMeasures(); j++) {

        double goodnessOfFit = cp[j]->GetLogData(
                    ControlMeasureLogData::GoodnessOfFit).GetNumericalValue();
        if (goodnessOfFit == Null) {
          numMeasOutsideRange++;
        }
        else if (m_lessThanCB->isChecked() && m_greaterThanCB->isChecked()) {
          if (goodnessOfFit < maxValue && goodnessOfFit > minValue) break;
          else numMeasOutsideRange++;
        }
        else if (m_lessThanCB->isChecked()) { 
          if (goodnessOfFit < maxValue) break;
          else numMeasOutsideRange++;
        }
        else if (m_greaterThanCB->isChecked()) {
          if (goodnessOfFit > minValue) break;
          else numMeasOutsideRange++;
        }
      }
      // if no measures are within the range, remove from filter list
      if (cp.GetNumMeasures() == numMeasOutsideRange) {
        filteredPoints().removeAt(i);
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

    if (m_lessThanCB->isChecked()) {
      m_maxValueEdit->setEnabled(true);
    }
    else {
      m_maxValueEdit->clear();
      m_maxValueEdit->setEnabled(false);
    }
    if (m_greaterThanCB->isChecked()) {
      m_minValueEdit->setEnabled(true);
    }
    else {
      m_minValueEdit->clear();
      m_minValueEdit->setEnabled(false);
    }
  }
}
