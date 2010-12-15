#include <QGridLayout>
#include <QMessageBox>
#include "QnetPointTypeFilter.h"
#include "QnetNavTool.h"
#include "ControlNet.h"
#include "SerialNumberList.h"
#include "qnet.h"

using namespace Qisis::Qnet;

namespace Qisis {
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
  QnetPointTypeFilter::QnetPointTypeFilter (QWidget *parent) : QnetFilter(parent) {
    p_ground = NULL;
    p_ignore = NULL;
    p_held = NULL;

    // Create the components for the filter window
    p_ground = new QRadioButton("Ground");
    p_ground->setChecked(false);
    p_ignore = new QRadioButton("Ignored");
    p_held = new QRadioButton("Held");
    QLabel *pad = new QLabel();
    
    // Create the layout and add the components to it
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(p_ground,0,0,1,4);
    gridLayout->addWidget(p_ignore,1,0,1,4);
    gridLayout->addWidget(p_held,2,0,1,4);
    gridLayout->addWidget(pad,3,0);
    gridLayout->setRowStretch(3,50);
    this->setLayout(gridLayout);
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
   *                          Isis::ControlPoint::Ground in case
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
                               "Error","No points to filter");
      return;
    }

    // Loop through each value of the filtered points list checking 
    // the types of each control point 
    // Loop in reverse order since removal list of elements affects index number
    for (int i = g_filteredPoints.size()-1; i >= 0; i--) {
      Isis::ControlPoint cp = (*g_controlNetwork)[g_filteredPoints[i]];
      if (p_ground->isChecked()) {
        if (cp.Type() == Isis::ControlPoint::Ground) {
          continue;
        } 
        else {
          g_filteredPoints.removeAt(i);
        }
      }
      else if (p_ignore->isChecked()) {
        if (cp.Ignore()) {
          continue;
        } 
        else {
          for (int j = 0; j < cp.Size(); j++) {
            if (!cp[j].Ignore()) {
              // if any measure is not ignored, remove from filtered list
              g_filteredPoints.removeAt(i);
              break;
            }
          }
          // if all measures have Ignore=True, treat point as ignored 
          // and keep in filtered list
          continue;
        }
      }
    }

    // Tell the navtool that a list has been filtered and it needs to update
    emit filteredListModified();
    return;
  }
}
