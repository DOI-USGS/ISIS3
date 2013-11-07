#include "QnetPointJigsawErrorFilter.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>

#include "ControlNet.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "QnetNavTool.h"
#include "SerialNumberList.h"
#include "Statistics.h"

namespace Isis {
  /**
  * Contructor for the Point Error filter.  It
  * creates the Error filter window found in the
  * navtool
  *
  * @param parent The parent widget for the point
  *               error filter
  *
  * @internal
  *  @history  2008-08-06 Tracie Sucharski - Added functionality
  *                          of filtering range of errors.
  *  @history  2010-06-02 Jeannie Walldren - Modify default
  *                          settings of checkboxes and line edits
  *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
  *                          to null.
  *
  */
  QnetPointJigsawErrorFilter::QnetPointJigsawErrorFilter(QnetNavTool *navTool,
      QWidget *parent) : QnetFilter(navTool, parent) {
    m_lessThanCB = NULL;
    m_greaterThanCB = NULL;
    m_lessErrorEdit = NULL;
    m_greaterErrorEdit = NULL;

    // Create the components for the filter window
    QLabel *label = new QLabel("Filter bundle-adjustment error");
    m_lessThanCB = new QCheckBox("Less than (undercontrolled)");
    m_lessErrorEdit = new QLineEdit();
    m_greaterThanCB = new QCheckBox("Greater than (overcontrolled)");
    m_greaterErrorEdit = new QLineEdit();
    QLabel *pixels = new QLabel("pixels");
    QLabel *pad = new QLabel();

    m_lessThanCB->setChecked(false);
    m_lessErrorEdit->setEnabled(false);
    m_greaterThanCB->setChecked(true);
    m_greaterErrorEdit->setEnabled(true);

    connect(m_lessThanCB, SIGNAL(clicked()), this, SLOT(clearEdit()));
    connect(m_greaterThanCB, SIGNAL(clicked()), this, SLOT(clearEdit()));

    // Create the layout and add the components to it
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(label, 0, 0, 1, 2);
    gridLayout->addWidget(m_lessThanCB, 1, 0, 1, 2);
    gridLayout->addWidget(m_lessErrorEdit, 2, 0);
    gridLayout->addWidget(pixels, 2, 1);
    gridLayout->addWidget(m_greaterThanCB, 3, 0, 1, 2);
    gridLayout->addWidget(m_greaterErrorEdit, 4, 0);
    gridLayout->addWidget(pixels, 4, 1);
    gridLayout->addWidget(pad, 5, 0);
    gridLayout->setRowStretch(5, 50);
    this->setLayout(gridLayout);
  }

  /**
   * Filters a list of points for points that have less than or greater
   * than the entered bundle adjust error values.  The filtered list will
   * appear in the navtools point list display.
   *
   * @internal
   *   @history  2007-06-05 Tracie Sucharski - Look at ControlPoint::MaximumError
   *                           instead of ControlPoint::AverageError
   *   @history  2008-08-06 Tracie Sucharski - Added functionality of filtering
   *                           range of errors.
   *   @history  2009-01-08 Jeannie Walldren - Modified to remove
   *                           new filter points from the existing
   *                           filtered list. Previously, a new
   *                           filtered list was created from the
   *                           entire control net each time.
   *   @history  2010-07-14 Tracie Sucharski - ControlPoint::MaximumError
   *                           renamed to MaximumResidual.
   *   @history  2011-04-28 Tracie Sucharski - Sort points in decsending order
   *                           of max residual.
   */
  void QnetPointJigsawErrorFilter::filter() {
    // Make sure we have a list of control points to filter
    if (controlNet() == NULL) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "No points to filter");
      return;
    }

    // Make sure the user entered a value to use in the filtering
    double lessNum = -1.;
    if (m_lessThanCB->isChecked() && m_lessErrorEdit->text() == "") {
      QMessageBox::information((QWidget *)parent(),
          "Error", "Error value must be entered");
      return;
    }
    double greaterNum = -1.;
    if (m_greaterThanCB->isChecked() && m_greaterErrorEdit->text() == "") {
      QMessageBox::information((QWidget *)parent(),
          "Error", "Error value must be entered");
      return;
    }

    // Get the user entered filtering value
    lessNum = m_lessErrorEdit->text().toDouble();
    greaterNum = m_greaterErrorEdit->text().toDouble();

    QMultiMap <double, int> pointMap;
    // Loop through each value of the filtered points list comparing the error of its
    // corresponding point with error with the user entered value and remove it from
    // the filtered list if it is outside the filtering range
    // Loop in reverse order since removal list of elements affects index number
    for (int i = filteredPoints().size() - 1; i >= 0; i--) {
      ControlPoint &cp = *(*controlNet())[filteredPoints()[i]];
      double maxResidual = cp.GetStatistic(&ControlMeasure::GetResidualMagnitude).Maximum();
      if (m_lessThanCB->isChecked() && m_greaterThanCB->isChecked()) {
        if (maxResidual < lessNum && maxResidual > greaterNum) {
          pointMap.insert(maxResidual, filteredPoints()[i]);
          continue;
        }
        else
          filteredPoints().removeAt(i);
      }
      else if (m_lessThanCB->isChecked()) {
        if (maxResidual < lessNum) {
          pointMap.insert(maxResidual, filteredPoints()[i]);
          continue;
        }
        else
          filteredPoints().removeAt(i);
      }
      else if (m_greaterThanCB->isChecked()) {
        if (maxResidual > greaterNum) {
          pointMap.insert(maxResidual, filteredPoints()[i]);
          continue;
        }
        else
          filteredPoints().removeAt(i);
      }
    }

    int filteredIndex = 0;
    QMultiMap<double, int>::const_iterator i = pointMap.constEnd();
    while (i != pointMap.constBegin()) {
      --i;
      filteredPoints()[filteredIndex] = i.value();
      filteredIndex++;
    }
    // Tell the navtool that a list has been filtered and it needs to update
    emit filteredListModified();
    return;
  }


  /**
   * Clears and disables the corresponding line edit if the "less
   * than" or "greater than" checkBox is "unchecked".
   *
   * @internal
   *   @history 2008-08-06 Tracie Sucharski - New method for
   *                         added functionality filtering range
   *                         of errors.
   *   @history 2010-06-02 Jeannie Walldren - Disable the line
   *            edit so the user can not enter a value unless the
   *            corresponding box is checked.
   */
  void QnetPointJigsawErrorFilter::clearEdit() {

    if (m_lessThanCB->isChecked()) {
      m_lessErrorEdit->setEnabled(true);
    }
    else {
      m_lessErrorEdit->clear();
      m_lessErrorEdit->setEnabled(false);
    }
    if (m_greaterThanCB->isChecked()) {
      m_greaterErrorEdit->setEnabled(true);
    }
    else {
      m_greaterErrorEdit->clear();
      m_greaterErrorEdit->setEnabled(false);
    }
  }
}
