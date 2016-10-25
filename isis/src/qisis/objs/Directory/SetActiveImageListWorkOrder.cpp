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

#include <QtDebug>

#include "Directory.h"
#include "IException.h"
#include "ImageList.h"
#include "Project.h"

namespace Isis {

/**
   * @brief Creates a WorkOrder that will set the active ImageList in the project.
   * @param project  The Project that this work order should be interacting with.
   */
  SetActiveImageListWorkOrder::SetActiveImageListWorkOrder(Project *project) :
      WorkOrder(project) {

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
    if (project()->activeImageList()) {
      if (project()->activeImageList()->name() == imageList->name()) {
        return false;
      }
    }
    return true;
  }


  /**
   * @brief Attempt to set imageList as active ImageList.
   * @return @b bool True if successful, False otherwise.
   */
  bool SetActiveImageListWorkOrder::execute() {
    bool success = WorkOrder::execute();

    if (success) {
      project()->setActiveImageList(imageList());
    }

    return success;
  }
}

