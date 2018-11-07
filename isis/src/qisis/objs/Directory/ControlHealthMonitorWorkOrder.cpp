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
#include "ControlHealthMonitorWorkOrder.h"

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
   * @brief Creates a WorkOrder that will display the Control Net Health Monitor interface.
   * @param project The Project that this work order should be interacting with.
   */
  ControlHealthMonitorWorkOrder::ControlHealthMonitorWorkOrder(Project *project) :
      WorkOrder(project) {

    // This workorder is not undoable
    m_isUndoable = false;

    QAction::setText(tr("View Control Net Health Monitor") );
    QUndoCommand::setText(tr("View Control Net Health Monitor"));
  }


  /**
   * @brief Copies the 'other' WorkOrder instance into this new instance.
   * @param other The WorkOrder being copied.
   */
  ControlHealthMonitorWorkOrder::ControlHealthMonitorWorkOrder(const ControlHealthMonitorWorkOrder &other) :
      WorkOrder(other) {
  }


  /**
   * @brief The Destructor.
   */
  ControlHealthMonitorWorkOrder::~ControlHealthMonitorWorkOrder() {
  }


  /**
   * @brief Returns a copy of this ControlHealthMonitorWorkOrder instance.
   * @return @b (ControlHealthMonitorWorkOrder *) A pointer to a copy of this WorkOrder.
   */
  ControlHealthMonitorWorkOrder *ControlHealthMonitorWorkOrder::clone() const {
    return new ControlHealthMonitorWorkOrder(*this);
  }


  /**
   * @brief Determines if we can execute the health monitor.
   *        We can execute if there's an active control.
   *
   * @param controls (ControlList *) The ControlList chosen from the project tree.
   * @return  @b bool True if  we can set as active, False otherwise.
   */
  bool ControlHealthMonitorWorkOrder::isExecutable(ControlList *controls) {
    return !!controls;
  }

  /**
   * @brief Make sure an active ImageList has been chosen.
   *
   * @return @b bool True if project has an active Control Network, False otherwise.
   */
  bool ControlHealthMonitorWorkOrder::setupExecution() {

    bool success = WorkOrder::setupExecution();
    if (success) {
      if (!isExecutable(controlList())) {

        QMessageBox::critical(NULL, tr("Unable to load Control Net Health Monitor."),
                              tr("You must first set an active control in order to view the health monitor."));
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
  void ControlHealthMonitorWorkOrder::execute() {
    try {
      project()->directory()->addControlHealthMonitorView();
    }
    catch (IException &e) {
      m_status = WorkOrderFinished;
      QMessageBox::critical(NULL, tr("Error"), tr(e.what()));
    }
  }
}
