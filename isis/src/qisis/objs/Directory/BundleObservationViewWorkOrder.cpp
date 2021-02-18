/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "BundleObservationViewWorkOrder.h"

#include <QtDebug>

#include "Directory.h"
#include "BundleObservationView.h"
#include "Project.h"
#include "ProjectItemModel.h"

namespace Isis {

  /**
   * Creates a work order to view BundleObservation. This WorkOrder is not undoable and runs
   * synchronously.
   *
   * @param Project *project Pointer to the project this work order belongs to.
   */
  BundleObservationViewWorkOrder::BundleObservationViewWorkOrder(Project *project) :
      WorkOrder(project) {
    m_isUndoable = false;
    m_isSavedToHistory = false;
    QAction::setText(tr("&View..."));
    QUndoCommand::setText(tr("View..."));
  }


  /**
   * @brief Copy constructor, creates a copy of the other BundleObservationViewWorkOrder.
   *
   * @param BundleObservationViewWorkOrder &other The other work order to copy state from.
   */
  BundleObservationViewWorkOrder::BundleObservationViewWorkOrder(const BundleObservationViewWorkOrder &other) :
      WorkOrder(other) {
    m_isUndoable = other.m_isUndoable;
  }


  /**
   * @brief Destructor to clean up any memory that this work order allocates.
   */
   BundleObservationViewWorkOrder::~BundleObservationViewWorkOrder() {
  }


  /**
   * @brief This method clones the current BundleObservationViewWorkOrder and returns it.
   *
   * @return @b BundleObservationViewWorkOrder that was cloned
   */
  BundleObservationViewWorkOrder *BundleObservationViewWorkOrder::clone() const {

    return new BundleObservationViewWorkOrder(*this);
  }


  /**
   * @brief
   *              False if none of the images has a footprint. This is used by
   *              Directory::supportedActions(DataType data) to determine what actions are appended
   *              to context menus.
   *
   * @param images ImageList of images
   *
   * @return @b bool True if one of the images in ImagesList images isFootprintable
   */
  bool BundleObservationViewWorkOrder::isExecutable(FileItemQsp fileItem) {
    bool result = false;

    if (fileItem) {
      result = true;
    }
    return result;
  }


  /**
   * @brief Setup this WorkOrder for execution.
   *
   * @return @b bool True if WorkOrder::execute() returns true and BundleObservationView can be
   *                 displayed, otherwise return False.
   */
  bool BundleObservationViewWorkOrder::setupExecution() {

    bool success = WorkOrder::setupExecution();
    return success;
  }


  /**
   * @brief This adds a new BundleObservationView to the project.
   *
   * @todo  When BundleObservation is serialized, get the BundleObservation and pass
   *        into Directory::addBundleObservationView.
   *
   */
  void BundleObservationViewWorkOrder::execute() {
//    ProjectItem * selectedItem = project()->directory()->model()->selectedItems();
    project()->directory()->addBundleObservationView(fileItem());
    project()->setClean(false);
  }

}
