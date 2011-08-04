#include <QGridLayout>
#include <QMessageBox>
#include "QnetPointMeasureFilter.h"
#include "QnetNavTool.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "SerialNumberList.h"

#include "qnet.h"

using namespace std;
using namespace Isis::Qnet;

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
  QnetPointMeasureFilter::QnetPointMeasureFilter(QWidget *parent) : QnetFilter(parent) {
    p_measureType = NULL;
    p_candidate = NULL;
    p_manual = NULL;
    p_registeredPixel = NULL;
    p_registeredSubPixel = NULL;
    p_ignoreStatus = NULL;
    p_ignored = NULL;
    p_notIgnored = NULL;
    p_editLockStatus = NULL;
    p_editLocked = NULL;
    p_notEditLocked = NULL;

    // Create the components for the filter window
    p_measureType = new QGroupBox("Filter by Measure Type(s)");
    p_measureType->setCheckable(true);
    p_measureType->setChecked(false);
    p_candidate = new QCheckBox("Candidate");
    p_manual = new QCheckBox("Manual");
    p_registeredPixel = new QCheckBox("RegisteredPixel");
    p_registeredSubPixel = new QCheckBox("RegisteredSubPixel");

    p_ignoreStatus = new QGroupBox("Filter by Ignore Status");
    p_ignoreStatus->setCheckable(true);
    p_ignoreStatus->setChecked(false);
    p_ignored = new QRadioButton("Ignored");
    p_notIgnored = new QRadioButton("Not Ignored");

    p_editLockStatus = new QGroupBox("Filter by Edit Lock Status");
    p_editLockStatus->setCheckable(true);
    p_editLockStatus->setChecked(false);
    p_editLocked = new QRadioButton("Edit Locked");
    p_notEditLocked = new QRadioButton("Not Edit Locked");

    QVBoxLayout *typeLayout = new QVBoxLayout();
    typeLayout->addWidget(p_candidate);
    typeLayout->addWidget(p_manual);
    typeLayout->addWidget(p_registeredPixel);
    typeLayout->addWidget(p_registeredSubPixel);
    typeLayout->addStretch(1);
    p_measureType->setLayout(typeLayout);

    QVBoxLayout *ignoreLayout = new QVBoxLayout();
    ignoreLayout->addWidget(p_ignored);
    ignoreLayout->addWidget(p_notIgnored);
    p_ignoreStatus->setLayout(ignoreLayout);

    QVBoxLayout *lockLayout = new QVBoxLayout();
    lockLayout->addWidget(p_editLocked);
    lockLayout->addWidget(p_notEditLocked);
    p_editLockStatus->setLayout(lockLayout);

    QVBoxLayout *statusLayout = new QVBoxLayout();
    statusLayout->addWidget(p_ignoreStatus);
    statusLayout->addWidget(p_editLockStatus);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(p_measureType);
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
    if (g_controlNetwork == NULL) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "No points to filter");
      return;
    }

    // Make sure they selected at least one type to filter for
    if (!p_measureType->isChecked() && !p_ignoreStatus->isChecked() &&
        !p_editLockStatus->isChecked()) {
      QMessageBox::information((QWidget *)parent(), "Input Erro",
          "You must select at least one measure property to filter");
      return;
    }
    // if Filter by Measure Type is selected but no Measure Type is checked, throw error
    if ((p_measureType->isChecked()) &&
        !(p_candidate->isChecked() || p_manual->isChecked() ||
          p_registeredPixel->isChecked() || p_registeredSubPixel->isChecked())) {
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
    for (int i = g_filteredPoints.size() - 1; i >= 0; i--) {
      ControlPoint &cp = *(*g_controlNetwork)[g_filteredPoints[i]];
      int numMeasNotMatching = 0;
      for (int j = 0; j < cp.GetNumMeasures(); j++) {
        //  While keep is true, keep testing for next filter option
        bool keep = true;
        if (p_measureType->isChecked() && !MeasureTypeMatched(cp[j]->GetType())) {
          keep = false; 
        }
//     ????? Not sure why this code was here, was introduced for binary, but
//              it does not work.  TODO:   GET RID OF???
//        //  Is this a reference measure
//        bool reference = cp.IsReferenceExplicit() &&
//           ((QString(cp[j]->GetCubeSerialNumber()) == cp.GetReferenceSN()));
//        if (!MeasureTypeMatched(cp[j]->GetType()) && !reference) keep = false;

        if (keep && p_ignoreStatus->isChecked()) {
          if (p_ignored->isChecked() && !cp[j]->IsIgnored()) keep = false;
          if (p_notIgnored->isChecked() && cp[j]->IsIgnored()) keep = false;
        }

        if (keep && p_editLockStatus->isChecked()) {
          if (p_editLocked->isChecked() && !cp[j]->IsEditLocked()) keep = false;
          if (p_notEditLocked->isChecked()&& cp[j]->IsEditLocked()) keep = false;
        }

        // if this measure doesn't match any of the checked values, increment
        if (!keep) numMeasNotMatching++;
      }

      int numMeasures = cp.GetNumMeasures();

      // if no measures match the checked values,
      // remove this point from the filter list
      if (numMeasNotMatching == numMeasures) {
        g_filteredPoints.removeAt(i);
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
    if (p_candidate->isChecked() && cmType == ControlMeasure::Candidate) {
      return true;;
    }
    if (p_manual->isChecked() && cmType == ControlMeasure::Manual) {
      return true;;
    }
    if (p_registeredPixel->isChecked() && cmType == ControlMeasure::RegisteredPixel) {
      return true;;
    }
    if (p_registeredSubPixel->isChecked() && cmType == ControlMeasure::RegisteredSubPixel) {
      return true;;
    }
    return false;
  }

}
