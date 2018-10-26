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
