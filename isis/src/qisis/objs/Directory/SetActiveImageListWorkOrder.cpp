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
#include "SetActiveImageListWorkOrder.h"

#include <QMessageBox>
#include <QtDebug>

#include "Directory.h"
#include "IException.h"
#include "ImageList.h"
#include "Project.h"

namespace Isis {

/**
   * @brief Creates a not undable WorkOrder that will set the active ImageList in the project.
   * @param project  The Project that this work order should be interacting with.
   */
  SetActiveImageListWorkOrder::SetActiveImageListWorkOrder(Project *project) :
      WorkOrder(project) {
    // This work order is not undoable
    m_isUndoable = false;

    QAction::setText(tr("Set Active Image List") );
    QUndoCommand::setText(tr("Set Active Image List"));
  }


  /**
   * @brief Copies the 'other' WorkOrder instance into this new instance.
   * @param other The WorkOrder being copied.
   */
  SetActiveImageListWorkOrder::SetActiveImageListWorkOrder(const SetActiveImageListWorkOrder &other) :
      WorkOrder(other) {
  }


  /**
   * @brief The Destructor.
   */
  SetActiveImageListWorkOrder::~SetActiveImageListWorkOrder() {
  }


  /**
   * @brief Returns a copy of this SetActiveImageListWorkOrder instance.
   * @return @b (SetActiveImageListWorkOrder *) A pointer to a copy of this WorkOrder.
   */
  SetActiveImageListWorkOrder *SetActiveImageListWorkOrder::clone() const {
    return new SetActiveImageListWorkOrder(*this);
  }


  /**
   * @brief Determines if we can set this imageList as active.
   *
   * @param ImageList *imageList
   * @return  @b bool True if  we can set as active, False otherwise.
   */
  bool SetActiveImageListWorkOrder::isExecutable(ImageList *imageList) {
    if (imageList->name() == "") {
      return false;
    }
    if (project()->activeImageList()) {
      if (project()->activeImageList()->name() == imageList->name()) {
        return false;
      }
    }
    return true;
  }


  /**
   * @brief Simply calls the parent WorkOrder::setupExecution(). There is nothing specific
   * that this work order needs to set up before execution. This was separated from execute() as
   * part of the WorkOrder redesign.
   *
   * @see WorkOrder::setupExecution()
   *
   * @return bool Returns true if successful (i.e. if the parent call succeeds).
   */
  bool SetActiveImageListWorkOrder::setupExecution() {
    return WorkOrder::setupExecution();
  }


  /**
   * @brief Executes this work order.
   *
   * Sets the active image list for the project.
   */
  void SetActiveImageListWorkOrder::execute() {
    try {
      project()->setActiveImageList(imageList()->name());
    }
    catch (IException e) {
      m_status = WorkOrderFinished;
      QMessageBox::critical(NULL, tr("Error"), tr(e.what()));
    }
  }
}
