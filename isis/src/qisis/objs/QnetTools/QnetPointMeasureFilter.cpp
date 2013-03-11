#include "QnetPointMeasureFilter.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QRadioButton>

#include "QnetNavTool.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "SerialNumberList.h"

using namespace std;

namespace Isis {
  /**
   * Contructor for the Point Measure filter.  It creates the
   * Measure filter window found in the navtool
   *
   * @param parent The parent widget for the point measure
   *               filter
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null.
   *   @history 2010-07-16 Tracie Sucharski - Implemented binary
   *            control networks.
   *
   */
  QnetPointMeasureFilter::QnetPointMeasureFilter(QnetNavTool *navTool, QWidget *parent) :
      QnetFilter(navTool, parent) {
    m_measureType = NULL;
    m_candidate = NULL;
    m_manual = NULL;
    m_registeredPixel = NULL;
    m_registeredSubPixel = NULL;
    m_ignoreStatus = NULL;
    m_ignored = NULL;
    m_notIgnored = NULL;
    m_editLockStatus = NULL;
    m_editLocked = NULL;
    m_notEditLocked = NULL;

    // Create the components for the filter window
    m_measureType = new QGroupBox("Filter by Measure Type(s)");
    m_measureType->setCheckable(true);
    m_measureType->setChecked(false);
    m_candidate = new QCheckBox("Candidate");
    m_manual = new QCheckBox("Manual");
    m_registeredPixel = new QCheckBox("RegisteredPixel");
    m_registeredSubPixel = new QCheckBox("RegisteredSubPixel");

    m_ignoreStatus = new QGroupBox("Filter by Ignore Status");
    m_ignoreStatus->setCheckable(true);
    m_ignoreStatus->setChecked(false);
    m_ignored = new QRadioButton("Ignored");
    m_notIgnored = new QRadioButton("Not Ignored");

    m_editLockStatus = new QGroupBox("Filter by Edit Lock Status");
    m_editLockStatus->setCheckable(true);
    m_editLockStatus->setChecked(false);
    m_editLocked = new QRadioButton("Edit Locked");
    m_notEditLocked = new QRadioButton("Not Edit Locked");

    QVBoxLayout *typeLayout = new QVBoxLayout();
    typeLayout->addWidget(m_candidate);
    typeLayout->addWidget(m_manual);
    typeLayout->addWidget(m_registeredPixel);
    typeLayout->addWidget(m_registeredSubPixel);
    typeLayout->addStretch(1);
    m_measureType->setLayout(typeLayout);

    QVBoxLayout *ignoreLayout = new QVBoxLayout();
    ignoreLayout->addWidget(m_ignored);
    ignoreLayout->addWidget(m_notIgnored);
    m_ignoreStatus->setLayout(ignoreLayout);

    QVBoxLayout *lockLayout = new QVBoxLayout();
    lockLayout->addWidget(m_editLocked);
    lockLayout->addWidget(m_notEditLocked);
    m_editLockStatus->setLayout(lockLayout);

    QVBoxLayout *statusLayout = new QVBoxLayout();
    statusLayout->addWidget(m_ignoreStatus);
    statusLayout->addWidget(m_editLockStatus);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(m_measureType);
    layout->addLayout(statusLayout);

    this->setLayout(layout);
  }



  /**
   * Filters a list of points for points that have at least one measure
   * of the selected type(s). The filtered list will appear in the
   * navtools point list display.
   *
   * @internal
   *   @history 2009-01-08 Jeannie Walldren - Modified to remove
   *                          new filter points from the existing
   *                          filtered list. Previously, a new
   *                          filtered list was created from the
   *                          entire control net each time.
   *   @history 2010-06-02 Jeannie Walldren - Added functionality
   *                          to filter by ignore status in
   *                          addition to measure type
   *   @history 2010-07-16 Tracie Sucharski - Implemented binary
   *                          control networks.
   */
  void QnetPointMeasureFilter::filter() {
    // Make sure there is a control net loaded to filter
    if (controlNet() == NULL) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "No points to filter");
      return;
    }

    // Make sure they selected at least one type to filter for
    if (!m_measureType->isChecked() && !m_ignoreStatus->isChecked() &&
        !m_editLockStatus->isChecked()) {
      QMessageBox::information((QWidget *)parent(), "Input Erro",
          "You must select at least one measure property to filter");
      return;
    }
    // if Filter by Measure Type is selected but no Measure Type is checked, throw error
    if ((m_measureType->isChecked()) &&
        !(m_candidate->isChecked() || m_manual->isChecked() ||
          m_registeredPixel->isChecked() || m_registeredSubPixel->isChecked())) {
      QMessageBox::information((QWidget *)parent(), "Input Error",
          "Filter by Measure Type is selected. You must choose at least one "
          "Measure Type to filter");
      return;
    }


    // Loop through each value of the filtered points list
    // checking the types of each control measure for each
    // of the control points.  If no measures match, we remove
    // it from the filtered list
    // Loop in reverse order since removal list of elements affects index number
    for (int i = filteredPoints().size() - 1; i >= 0; i--) {
      ControlPoint &cp = *(*controlNet())[filteredPoints()[i]];
      int numMeasNotMatching = 0;
      for (int j = 0; j < cp.GetNumMeasures(); j++) {
        //  While keep is true, keep testing for next filter option
        bool keep = true;
        if (m_measureType->isChecked() && !MeasureTypeMatched(cp[j]->GetType())) {
          keep = false; 
        }
//     ????? Not sure why this code was here, was introduced for binary, but
//              it does not work.  TODO:   GET RID OF???
//        //  Is this a reference measure
//        bool reference = cp.IsReferenceExplicit() &&
//           ((QString(cp[j]->GetCubeSerialNumber()) == cp.GetReferenceSN()));
//        if (!MeasureTypeMatched(cp[j]->GetType()) && !reference) keep = false;

        if (keep && m_ignoreStatus->isChecked()) {
          if (m_ignored->isChecked() && !cp[j]->IsIgnored()) keep = false;
          if (m_notIgnored->isChecked() && cp[j]->IsIgnored()) keep = false;
        }

        if (keep && m_editLockStatus->isChecked()) {
          if (m_editLocked->isChecked() && !cp[j]->IsEditLocked()) keep = false;
          if (m_notEditLocked->isChecked()&& cp[j]->IsEditLocked()) keep = false;
        }

        // if this measure doesn't match any of the checked values, increment
        if (!keep) numMeasNotMatching++;
      }

      int numMeasures = cp.GetNumMeasures();

      // if no measures match the checked values,
      // remove this point from the filter list
      if (numMeasNotMatching == numMeasures) {
        filteredPoints().removeAt(i);
      }
    }

    // Tell the navtool that a list has been filtered and it needs to update
    emit filteredListModified();
    return;
  }

  /**
   * @brief Returns whether the measure type passed in matches a
   *        type selected by the user.
   *
   * This method is called by the filter() method to checks
   * whether a particular measure type matches any of the
   * checkboxes selected by the user.
   *
   * @param cmType ControlMeasure type to compare with user
   *               selection.
   * @return <b>bool</b> True if the measure type matches a type
   *         selected by the user.
   * @history 2010-07-16 Tracie Sucharski - Implemented binary
   *                         control networks.
   */
  bool QnetPointMeasureFilter::MeasureTypeMatched(int cmType) {
    if (m_candidate->isChecked() && cmType == ControlMeasure::Candidate) {
      return true;;
    }
    if (m_manual->isChecked() && cmType == ControlMeasure::Manual) {
      return true;;
    }
    if (m_registeredPixel->isChecked() && cmType == ControlMeasure::RegisteredPixel) {
      return true;;
    }
    if (m_registeredSubPixel->isChecked() && cmType == ControlMeasure::RegisteredSubPixel) {
      return true;;
    }
    return false;
  }

}
