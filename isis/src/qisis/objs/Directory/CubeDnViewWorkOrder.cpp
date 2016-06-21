/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
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
#include "CubeDnViewWorkOrder.h"

#include <QtDebug>

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "CubeDnView.h"
#include "Directory.h"
#include "MdiCubeViewport.h"
#include "Project.h"
#include "Workspace.h"

namespace Isis {

  /**
   * This method sets the text of the work order.
   * 
   * @param project The Project that we are going to work with.
   * 
   */
  CubeDnViewWorkOrder::CubeDnViewWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("View Raw &Cubes..."));
  }

  /**
   * As of 06/06/2016 this method is not implemented.
   */
  CubeDnViewWorkOrder::CubeDnViewWorkOrder(const CubeDnViewWorkOrder &other) :
      WorkOrder(other) {
  }

  /**
   * Destructor
   */
  CubeDnViewWorkOrder::~CubeDnViewWorkOrder() {
  }

  /**
   * This method clones the CubeDnViewWorkOrder
   * 
   * @return @b CubeDnViewWorkOrder Returns a clone of the CubeDnViewWorkOrder
   */
  CubeDnViewWorkOrder *CubeDnViewWorkOrder::clone() const {
    return new CubeDnViewWorkOrder(*this);
  }

  /**
   * This check is used by Directory::supportedActions(DataType data).
   *
   * @param images ImageList we are checking
   * 
   * @return @b bool True if the number of images is greater than 0 and less than 50.
   */
  bool CubeDnViewWorkOrder::isExecutable(ImageList *images) {
    return (images->count() > 0 && images->count() < 50);
  }

  /**
   * If WorkOrder::execute() returns true, then this method returns true.
   * 
   * @return @b bool True if WorkOrder::execute() returns true.
   */
  bool CubeDnViewWorkOrder::execute() {
    bool success = WorkOrder::execute();
    return success;
  }

  /**
   * This method returns true if other depends on a CubeDnViewWorkOrder
   * 
   * @param order the WorkOrder we want to check for dependancies
   * 
   * @return @b bool True if WorkOrder depends on a CubeDnViewWorkOrder
   * 
   */
  bool CubeDnViewWorkOrder::dependsOn(WorkOrder *other) const {
    // depend on types of ourselves.
    return dynamic_cast<CubeDnViewWorkOrder *>(other);
  }

  /**
   * This method adds a new CubeDnView to the project's directory and then adds currentItem() to
   * that. 
   */
  void CubeDnViewWorkOrder::syncRedo() {
    ProjectItem *currentItem = project()->directory()->model()->currentItem();
    CubeDnView *view = project()->directory()->addCubeDnView();
    view->addItem( currentItem );
  }
}

