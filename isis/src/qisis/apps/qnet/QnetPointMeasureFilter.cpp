#include <QGridLayout>
#include <QMessageBox>
#include "QnetPointMeasureFilter.h"
#include "QnetNavTool.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "SerialNumberList.h"

#include "qnet.h"

using namespace std;
using namespace Qisis::Qnet;

namespace Qisis {
  /**
   * Contructor for the Point Measure filter.  It creates the
   * Measure filter window found in the navtool
   * 
   * @param parent The parent widget for the point measure
   *               filter
   * @internal 
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null.
   *  
   */
  QnetPointMeasureFilter::QnetPointMeasureFilter (QWidget *parent) : QnetFilter(parent) {
    p_measureType = NULL;
    p_unmeasured = NULL;
    p_manual = NULL;
    p_estimated = NULL;
    p_automatic = NULL; 
    p_validatedManual = NULL; 
    p_validatedAutomatic = NULL;
    p_ignoreStatus = NULL;
    p_ignored = NULL;
    p_notIgnored = NULL; 

    // Create the components for the filter window
    p_measureType = new QCheckBox("Filter by Measure Type(s)");
    p_unmeasured = new QCheckBox("Unmeasured");
    p_manual = new QCheckBox("Manual");
    p_estimated = new QCheckBox("Estimated");
    p_automatic = new QCheckBox("Auto-Registered");
    p_validatedManual = new QCheckBox("Manual/Validated");
    p_validatedAutomatic = new QCheckBox("Auto-Registered/Validated");

    p_measureType->setChecked(false);
    p_unmeasured->setEnabled(false);
    p_manual->setEnabled(false);
    p_estimated->setEnabled(false);
    p_automatic->setEnabled(false);
    p_validatedManual->setEnabled(false);
    p_validatedAutomatic->setEnabled(false);
    connect(p_measureType,SIGNAL(toggled(bool)),this,SLOT(enableTypeFilter()));

    p_ignoreStatus = new QCheckBox("Filter by Ignore Status");
    p_ignored = new QRadioButton("Ignored");
    p_notIgnored = new QRadioButton("Not Ignored");

    p_ignoreStatus->setChecked(false);
    p_ignored->setChecked(true);
    p_ignored->setEnabled(false);
    p_notIgnored->setEnabled(false);
    connect(p_ignoreStatus,SIGNAL(toggled(bool)),this,SLOT(enableIgnoreFilter()));

    QLabel *pad = new QLabel();


    // Create the layout and add the components to it
    QVBoxLayout *ignorelayout = new QVBoxLayout();
    ignorelayout->addWidget(p_ignoreStatus);
    ignorelayout->addWidget(p_ignored);
    ignorelayout->addWidget(p_notIgnored);
    ignorelayout->addWidget(pad);
    ignorelayout->addWidget(pad);
    ignorelayout->addWidget(pad);

    QVBoxLayout *typelayout = new QVBoxLayout();
    typelayout->addWidget(p_measureType);
    typelayout->addWidget(p_unmeasured);
    typelayout->addWidget(p_manual);
    typelayout->addWidget(p_estimated);
    typelayout->addWidget(p_automatic);
    typelayout->addWidget(p_validatedManual);
    typelayout->addWidget(p_validatedAutomatic);
    typelayout->addWidget(pad);
    typelayout->addWidget(pad);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addLayout(typelayout);
    layout->addLayout(ignorelayout);
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
   */
  void QnetPointMeasureFilter::filter() {
    // Make sure there is a control net loaded to filter
    if (g_controlNetwork == NULL) {
      QMessageBox::information((QWidget *)parent(),
                               "Error","No points to filter");
      return;
    }

    // Make sure they selected at least one type to filter for
    if (!(p_measureType->isChecked()) && !(p_ignoreStatus->isChecked())) {
      QMessageBox::information((QWidget *)parent(),"Error",
                                "You must select at least one measure property to filter");
      return;
    }
    // if Filter by Measure Type is selected but no Measure Type is checked, throw error 
    if ((p_measureType->isChecked()) && 
        !(p_unmeasured->isChecked() || 
          p_manual->isChecked() ||
          p_estimated->isChecked() ||
          p_automatic->isChecked() ||
          p_validatedAutomatic->isChecked() ||
          p_validatedManual->isChecked())) {
      QMessageBox::information((QWidget *)parent(),"Error",
                                "Filter by Measure Type is selected. You must choose at least one Measure Type to filter");
      return;
    }


    // Loop through each value of the filtered points list 
    // checking the types of each control measure for each 
    // of the control points.  If no measures match, we remove
    // it from the filtered list
    // Loop in reverse order since removal list of elements affects index number
    for (int i = g_filteredPoints.size()-1; i >= 0; i--) {
      Isis::ControlPoint cp = (*g_controlNetwork)[g_filteredPoints[i]];
      int numMeasNotMatching = 0;
      for (int j=0; j<cp.Size(); j++) {
        if (p_ignoreStatus->isChecked()) {
          // if the point contains a measure whose ignore status matches
          // the user's selection, check whether the measure type matches
          if (p_ignored->isChecked() && cp[j].Ignore()) { 
            if (p_measureType->isChecked()) {
              if (MeasureTypeMatched(cp[j].Type())){
                // keep this point in the list and go on to the next point
                break;
              }
            }
            else {
              // keep this point in the list and go on to the next point
              break;
            }
          }
          else if (p_notIgnored->isChecked() && !cp[j].Ignore()) {
            if (p_measureType->isChecked()) {
              // keep this point in the list and go on to the next point
              if (MeasureTypeMatched(cp[j].Type())){
                break;
              }
            }
            else {
              // keep this point in the list and go on to the next point
              break;
            }
          }
        }
        else { // ignore status not selected, only filter by measure type
          if (MeasureTypeMatched(cp[j].Type())){
            // keep this point in the list and go on to the next point
            break;
          }
        } 
        // if this measure doesn't match any of the checked values, increment
        numMeasNotMatching++;
      }
      
      int numMeasures = cp.Size();

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
   */
  bool QnetPointMeasureFilter::MeasureTypeMatched(int cmType){
    if (p_unmeasured->isChecked() && cmType == Isis::ControlMeasure::Unmeasured) {
      return true;;
    }
    if (p_manual->isChecked() && cmType == Isis::ControlMeasure::Manual) {
      return true;;
    }
    if (p_estimated->isChecked() && cmType == Isis::ControlMeasure::Estimated) {
      return true;;
    }
    if (p_automatic->isChecked() && cmType == Isis::ControlMeasure::Automatic) {
      return true;;
    }
    if (p_validatedManual->isChecked() && cmType == Isis::ControlMeasure::ValidatedManual) {
      return true;;
    }
    if (p_validatedAutomatic->isChecked() && cmType == Isis::ControlMeasure::ValidatedAutomatic) {
      return true;;
    }
    return false;
  }

  /**
   * @brief Enables ignore status filter when corresponding 
   *        checkbox is selected.
   *  
   * This slot is connected to the toggle signal of the "Filter by 
   * Ignore Status" checkbox.  When the box is checked, the radio 
   * buttons are enabled. 
   * 
   * 
   * @internal 
   *   @history 2010-06-02 Jeannie Walldren - Original version 
   */
  void QnetPointMeasureFilter::enableIgnoreFilter() {
    if (p_ignoreStatus->isChecked()) {
      p_ignored->setEnabled(true);
      p_notIgnored->setEnabled(true);
    }
    else{
      p_ignored->setEnabled(false);
      p_notIgnored->setEnabled(false);
    }

  }

  /**
   * @brief Enables measure type filter when corresponding 
   *        checkbox is selected.
   *  
   * This slot is connected to the toggle signal of the "Filter by
   * Measure Type" checkbox.  When the box is checked, the 
   * checkboxes below are enabled. 
   * 
   * @internal 
   *   @history 2010-06-02 Jeannie Walldren - Original version 
   */
  void QnetPointMeasureFilter::enableTypeFilter() {
    if (p_measureType->isChecked()) {
      p_unmeasured->setEnabled(true);
      p_manual->setEnabled(true);
      p_estimated->setEnabled(true);
      p_automatic->setEnabled(true);
      p_validatedManual->setEnabled(true);
      p_validatedAutomatic->setEnabled(true);
    }
    else{
      p_unmeasured->setEnabled(false);
      p_manual->setEnabled(false);
      p_estimated->setEnabled(false);
      p_automatic->setEnabled(false);
      p_validatedManual->setEnabled(false);
      p_validatedAutomatic->setEnabled(false);
      p_unmeasured->setChecked(false);
      p_manual->setChecked(false);
      p_estimated->setChecked(false);
      p_automatic->setChecked(false);
      p_validatedManual->setChecked(false);
      p_validatedAutomatic->setChecked(false);
    }

  }

}
