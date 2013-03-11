#include "QnetPointTypeFilter.h"

#include <QtGui>
#include <QGroupBox>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "QnetNavTool.h"
#include "SerialNumberList.h"

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
  QnetPointTypeFilter::QnetPointTypeFilter(QnetNavTool *navTool, QWidget *parent) :
      QnetFilter(navTool, parent) {
    m_pointType = NULL;
    m_free = NULL;
    m_constrained = NULL;
    m_fixed = NULL;
    m_ignoreStatus = NULL;
    m_ignored = NULL;
    m_notIgnored = NULL;
    m_editLockStatus = NULL;
    m_editLocked = NULL;
    m_notEditLocked = NULL;

    // Create the components for the filter window
    m_pointType = new QGroupBox("Filter by Point Type(s)");
    m_pointType->setCheckable(true);
    m_free = new QCheckBox("Free");
    m_constrained = new QCheckBox("Constrained");
    m_fixed = new QCheckBox("Fixed");

    m_free->setChecked(true);

    QVBoxLayout *typeLayout = new QVBoxLayout();
    typeLayout->addWidget(m_free);
    typeLayout->addWidget(m_constrained);
    typeLayout->addWidget(m_fixed);
    typeLayout->addStretch(1);
    m_pointType->setLayout(typeLayout);

    m_ignoreStatus = new QGroupBox("Filter by Ignore Status");
    m_ignoreStatus->setCheckable(true);
    m_ignoreStatus->setChecked(false);
    m_ignored = new QRadioButton("Ignored");
    m_notIgnored = new QRadioButton("Not Ignored");

    m_ignored->setChecked(true);

    QVBoxLayout *ignoreLayout = new QVBoxLayout();
    ignoreLayout->addWidget(m_ignored);
    ignoreLayout->addWidget(m_notIgnored);
    m_ignoreStatus->setLayout(ignoreLayout);

    m_editLockStatus = new QGroupBox("Filter by Edit Lock Status");
    m_editLockStatus->setCheckable(true);
    m_editLockStatus->setChecked(false);
    m_editLocked = new QRadioButton("Edit Locked");
    m_notEditLocked = new QRadioButton("Not Edit Locked");

    m_editLocked->setChecked(true);

    QVBoxLayout *lockLayout = new QVBoxLayout();
    lockLayout->addWidget(m_editLocked);
    lockLayout->addWidget(m_notEditLocked);
    m_editLockStatus->setLayout(lockLayout);

    //QLabel *pad = new QLabel();

    // Create the layout and add the components to it


    QVBoxLayout *statusLayout = new QVBoxLayout();
    statusLayout->addWidget(m_ignoreStatus);
    statusLayout->addWidget(m_editLockStatus);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(m_pointType);
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
    if (controlNet() == NULL) {
      QMessageBox::information((QWidget *)parent(),
          "Error", "No points to filter");
      return;
    }

    // Make sure something is selected for filtering
    if (!m_pointType->isChecked() && !m_ignoreStatus->isChecked() &&
        !m_editLockStatus->isChecked()) {
      QMessageBox::information((QWidget *)parent(),"Input Error",
          "You must select something to filter.");
      return;
    }
  
    // if Filter by Measure Type is selected but no Measure Type is checked, throw error
    if ((m_pointType->isChecked()) &&
        !(m_fixed->isChecked() || m_constrained->isChecked() ||
          m_free->isChecked())) {
      QMessageBox::information((QWidget *)parent(), "Input Error",
          "Filter by Point Type is selected. You must choose at least one "
          "Point Type to filter");
      return;
    }

    // Loop through each value of the filtered points list checking the types of
    // each control point Loop in reverse order since removal list of elements
    // affects index number
    for (int i = filteredPoints().size() - 1; i >= 0; i--) {
      ControlPoint &cp = *(*controlNet())[filteredPoints()[i]];
      //  While keep is true, keep testing for next filter option
      bool keep = true;
      if (m_pointType->isChecked()) {
        if (!PointTypeMatched(cp.GetType())) keep = false;
      }

      if (keep && m_ignoreStatus->isChecked()) {

        //  First check all measures in point.  if all measures are ignored,
        //  point is considered ignored.
        bool allMeasuresIgnored = true;
        for (int j = 0; j < cp.GetNumMeasures(); j++) {
          if (!cp[j]->IsIgnored()) {
            allMeasuresIgnored = false;
            break;
          }
        }
        if (m_ignored->isChecked() && !cp.IsIgnored() && !allMeasuresIgnored) {
          keep = false;
        }
        else if (m_notIgnored->isChecked() &&
                 (cp.IsIgnored() || allMeasuresIgnored)) {
          keep = false;
        }
      }

      if (keep && m_editLockStatus->isChecked()) {
        if (m_editLocked->isChecked() && !cp.IsEditLocked()) {
          keep = false;
        }
        else if (m_notEditLocked->isChecked() && cp.IsEditLocked()) {
          keep = false;
        }
      }

      if (!keep) filteredPoints().removeAt(i);
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
    if (m_fixed->isChecked() && pointType == ControlPoint::Fixed) {
      return true;;
    }
    if (m_constrained->isChecked() && pointType == ControlPoint::Constrained) {
      return true;;
    }
    if (m_free->isChecked() && pointType == ControlPoint::Free) {
      return true;;
    }
    return false;
  }


}
