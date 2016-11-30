/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include "SetActiveControlWorkOrder.h"

#include <QMessageBox>
#include <QtDebug>

#include "Control.h"
#include "ControlNet.h"
#include "Directory.h"
#include "IException.h"
#include "Project.h"

namespace Isis {

/**
   * @brief Creates a WorkOrder that will set the active Control in the project.
   * @param project  The Project that this work order should be interacting with.
   */
  SetActiveControlWorkOrder::SetActiveControlWorkOrder(Project *project) :
      WorkOrder(project) {

    QAction::setText(tr("Set Active Control Network") );
    QUndoCommand::setText(tr("Set Active Control Network"));
  }


  /**
   * @brief Copies the 'other' WorkOrdeer instance into this new instance.
   * @param other The WorkOrder being copied.
   */
  SetActiveControlWorkOrder::SetActiveControlWorkOrder(const SetActiveControlWorkOrder &other) :
      WorkOrder(other) {
  }


  /**
   * @brief The Destructor.
   */
  SetActiveControlWorkOrder::~SetActiveControlWorkOrder() {
  }


  /**
   * @brief Returns a copy of this SetActiveControlWorkOrder instance.
   * @return @b (SetActiveControlWorkOrder *) A pointer to a copy of this WorkOrder.
   */
  SetActiveControlWorkOrder *SetActiveControlWorkOrder::clone() const {
    return new SetActiveControlWorkOrder(*this);
  }


  /**
   * @brief Determines if we can set this control as active.  
   *
   * @param Control *control
   * @return  @b bool True if  we can set as active, False otherwise.
   */
  bool SetActiveControlWorkOrder::isExecutable(ControlList *controls) {
    if (controls->size() != 1 || project()->activeControl() == controls->at(0)) {
      return false;
    }

    return true;
  }


  /**
   * @brief Attempt to set control as active control.
   * @return @b bool True if successful, False otherwise.
   */
  bool SetActiveControlWorkOrder::execute() {
    bool success = WorkOrder::execute();

    if (success) {

      if (project()->activeImageList()) {
        project()->setActiveControl(controlList()->at(0));
        project()->activeControl()->controlNet()->SetImages(*project()->activeImageList()->serialNumberList());
        success = true;
      }
      else {
        QMessageBox::critical(NULL, tr("Unable to set active control."), 
                              tr("You must first choose an active Image List before setting "
                                 "the active control net."));
        success = false;
      }
//    internalData.append(activeControl()->displayProperties()->displayName());
//    setInternalData(internalData);
    }

    return success;
  }
}

