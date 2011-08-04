#include "QnetPointTypeFilter.h"

#include <QtGui>
#include <QGroupBox>

#include "ControlMeasure.h"
#include "QnetNavTool.h"
#include "ControlNet.h"
#include "SerialNumberList.h"

#include "qnet.h"

using namespace Isis::Qnet;

namespace Isis {
  /**
   * Contructor for the Point Type filter.  It creates the
   * Type filter window found in the navtool
   *
   * @param parent The parent widget for the point type
   *               filter
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null.
   *   @history 2010-07-16 Tracie Sucharski - Implemented binary
   *                           control networks.
   *   @history 2010-10-05 Tracie Sucharski - Remove multiple ground types, back
   *                           to simply tie or ground.
   *
   */
  QnetPointTypeFilter::QnetPointTypeFilter(QWidget *parent) : QnetFilter(parent) {
    p_pointType = NULL;
    p_free = NULL;
    p_constrained = NULL;
    p_fixed = NULL;
    p_ignoreStatus = NULL;
    p_ignored = NULL;
    p_notIgnored = NULL;
    p_editLockStatus = NULL;
    p_editLocked = NULL;
    p_notEditLocked = NULL;

    // Create the components for the filter window
    p_pointType = new QGroupBox("Filter by Point Type(s)");
    p_pointType->setCheckable(true);
    p_free = new QCheckBox("Free");
    p_constrained = new QCheckBox("Constrained");
    p_fixed = new QCheckBox("Fixed");

    p_free->setChecked(true);

    QVBoxLayout *typeLayout = new QVBoxLayout();
    typeLayout->addWidget(p_free);
    typeLayout->addWidget(p_constrained);
    typeLayout->addWidget(p_fixed);
    typeLayout->addStretch(1);
    p_pointType->setLayout(typeLayout);

    p_ignoreStatus = new QGroupBox("Filter by Ignore Status");
    p_ignoreStatus->setCheckable(true);
    p_ignoreStatus->setChecked(false);
    p_ignored = new QRadioButton("Ignored");
    p_notIgnored = new QRadioButton("Not Ignored");

    p_ignored->setChecked(true);

    QVBoxLayout *ignoreLayout = new QVBoxLayout();
    ignoreLayout->addWidget(p_ignored);
    ignoreLayout->addWidget(p_notIgnored);
    p_ignoreStatus->setLayout(ignoreLayout);

    p_editLockStatus = new QGroupBox("Filter by Edit Lock Status");
    p_editLockStatus->setCheckable(true);
    p_editLockStatus->setChecked(false);
    p_editLocked = new QRadioButton("Edit Locked");
    p_notEditLocked = new QRadioButton("Not Edit Locked");

    p_editLocked->setChecked(true);

    QVBoxLayout *lockLayout = new QVBoxLayout();
    lockLayout->addWidget(p_editLocked);
    lockLayout->addWidget(p_notEditLocked);
    p_editLockStatus->setLayout(lockLayout);

    //QLabel *pad = new QLabel();

    // Create the layout and add the components to it


    QVBoxLayout *statusLayout = new QVBoxLayout();
    statusLayout->addWidget(p_ignoreStatus);
    statusLayout->addWidget(p_editLockStatus);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(p_pointType);
    layout->addLayout(statusLayout);

    this->setLayout(layout);
  }

  /**
   * Filters a list of points and keeps points that have the
   * selected property or in the given range. Note: If a point
   * does not have Ignore=True but all measures of the point have
   * Ignore=True, this point will be considered "Ignored".  The
   * filtered list will appear in the navtools point list display.
   *
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Fixed comment.
   *   @history 2009-01-08 Jeannie Walldren - Modified to remove
   *                          new filter points from the existing
   *                          filtered list. Previously, a new
   *                          filtered list was created from the
   *                          entire control net each time.
   *   @history 2010-06-02 Jeannie Walldren - Modified to treat
   *                          a point as ignored if all of its
   *                          measures have Ignore=True
   *   @history 2010-06-03 Jeannie Walldren - Replaced "0" with
   *                          ControlPoint::Ground in case
   *                          enumeration changes.
   *   @history 2010-07-16 Tracie Sucharski - Implemented binary
   *                          control networks.
   *   @history 2010-10-05 Tracie Sucharski - Remove multiple ground types, back
   *                           to simply tie or ground.  Also help points no
   *                           longer an option.
   */
  void QnetPointTypeFilter::filter() {
    // Make sure there is a control net loaded
    if (g_controlNetwork == NULL) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "No points to filter");
      return;
    }

    // Make sure something is selected for filtering
    if (!p_pointType->isChecked() && !p_ignoreStatus->isChecked() &&
        !p_editLockStatus->isChecked()) {
      QMessageBox::information((QWidget *)parent(),"Input Error",
          "You must select something to filter.");
      return;
    }
  
    // if Filter by Measure Type is selected but no Measure Type is checked, throw error
    if ((p_pointType->isChecked()) &&
        !(p_fixed->isChecked() || p_constrained->isChecked() ||
          p_free->isChecked())) {
      QMessageBox::information((QWidget *)parent(), "Input Error",
          "Filter by Point Type is selected. You must choose at least one "
          "Point Type to filter");
      return;
    }

    // Loop through each value of the filtered points list checking the types of
    // each control point Loop in reverse order since removal list of elements
    // affects index number
    for (int i = g_filteredPoints.size() - 1; i >= 0; i--) {
      ControlPoint &cp = *(*g_controlNetwork)[g_filteredPoints[i]];
      //  While keep is true, keep testing for next filter option
      bool keep = true;
      if (p_pointType->isChecked()) {
        if (!PointTypeMatched(cp.GetType())) keep = false;
      }

      if (keep && p_ignoreStatus->isChecked()) {

        //  First check all measures in point.  if all measures are ignored,
        //  point is considered ignored.
        bool allMeasuresIgnored = true;
        for (int j = 0; j < cp.GetNumMeasures(); j++) {
          if (!cp[j]->IsIgnored()) {
            allMeasuresIgnored = false;
            break;
          }
        }
        if (p_ignored->isChecked() && !cp.IsIgnored() && !allMeasuresIgnored) {
          keep = false;
        }
        else if (p_notIgnored->isChecked() &&
                 (cp.IsIgnored() || allMeasuresIgnored)) {
          keep = false;
        }
      }

      if (keep && p_editLockStatus->isChecked()) {
        if (p_editLocked->isChecked() && !cp.IsEditLocked()) {
          keep = false;
        }
        else if (p_notEditLocked->isChecked() && cp.IsEditLocked()) {
          keep = false;
        }
      }

      if (!keep) g_filteredPoints.removeAt(i);
    }

    // Tell the navtool that a list has been filtered and it needs to update
    emit filteredListModified();
    return;
  }



  /**
   * @brief Returns whether the point type passed in matches a type selected by 
   *        the user.
   *
   * This method is called by the filter() method to checks
   * whether a particular point type matches any of the checkboxes selected by 
   * the user. 
   *
   * @param pointType ControlPoint type to compare with user selection. 
   * @return <b>bool</b> True if the point type matches a type selected by the 
   *         user.
   */
  bool QnetPointTypeFilter::PointTypeMatched(int pointType) {
    if (p_fixed->isChecked() && pointType == ControlPoint::Fixed) {
      return true;;
    }
    if (p_constrained->isChecked() && pointType == ControlPoint::Constrained) {
      return true;;
    }
    if (p_free->isChecked() && pointType == ControlPoint::Free) {
      return true;;
    }
    return false;
  }


}
