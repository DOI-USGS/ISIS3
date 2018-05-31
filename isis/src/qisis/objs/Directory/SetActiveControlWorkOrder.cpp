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
#include "Directory.h"
#include "IException.h"
#include "Project.h"
#include "ProjectItem.h"
#include "ProjectItemModel.h"

namespace Isis {

/**
   * @brief Creates a WorkOrder that will set the active Control in the project.
   * @param project The Project that this work order should be interacting with.
   */
  SetActiveControlWorkOrder::SetActiveControlWorkOrder(Project *project) :
      WorkOrder(project) {

    // This workorder is not undoable
    m_isUndoable = false;

    QAction::setText(tr("Set Active Control Network") );
    QUndoCommand::setText(tr("Set Active Control Network"));
  }


  /**
   * @brief Copies the 'other' WorkOrder instance into this new instance.
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
   * @param controls (ControlList *) The ControlList chosen from the project tree.
   * @return  @b bool True if  we can set as active, False otherwise.
   */
  bool SetActiveControlWorkOrder::isExecutable(ControlList *controls) {

    // Return false if more than 1 control was selected or if selected is already active
    if (controls) {
      if (controls->size() != 1 || project()->activeControl() == controls->at(0)) {
        return false;
      }
      return true;
    }
    return false;
  }


  /**
   * @brief Make sure an active ImageList has been chosen.
   *
   * @return @b bool True if project has an active ImageList, False otherwise.
   */
  bool SetActiveControlWorkOrder::setupExecution() {

    bool success = WorkOrder::setupExecution();
    if (success) {
      if (!project()->activeImageList()) {

        QMessageBox::critical(NULL, tr("Unable to set active control."),
                              tr("You must first choose an active Image List before setting "
                              "the active control net."));
        success = false;
      }
      //  So far, so good, set the command text
      else {
        QUndoCommand::setText(tr("Set Active Control Network to [%1]").arg(
                                 controlList()->at(0)->displayProperties()->displayName()));
      }
    }

    return success;
  }


  /**
   * @brief  Set the active control net for the project.  This allows any views to operate on
   *               the same control net.  The active image list must be set before the active
   *               control net is chosen.  If not, a critical message dialog is displayed and we
   *               return false.
   *
   */
  void SetActiveControlWorkOrder::execute() {
    try {
      project()->setActiveControl(controlList()->at(0)->displayProperties()->displayName());
    }
    catch (IException e) {
      m_status = WorkOrderFinished;
      QMessageBox::critical(NULL, tr("Error"), tr(e.what()));
    }
  }
}
