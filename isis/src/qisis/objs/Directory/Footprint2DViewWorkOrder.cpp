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
#include "Footprint2DViewWorkOrder.h"

#include <QtDebug>

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "Directory.h"
//#include "Footprint2DView.h"
#include "MosaicSceneItem.h"
#include "MosaicSceneWidget.h"
#include "Project.h"
//#include "ProjectItem.h"
//#include "ProjectItemModel.h"

namespace Isis {
  
  /** 
   * Constructor. This method sets the text of the project to View Footprints.
   * 
   * @param project Current project
   */
  Footprint2DViewWorkOrder::Footprint2DViewWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("View &Footprints..."));
  }

  /**
   * This method has not been implemented.
   */
  Footprint2DViewWorkOrder::Footprint2DViewWorkOrder(const Footprint2DViewWorkOrder &other) :
      WorkOrder(other) {
  }

  /**
   * Destructor
   */
  Footprint2DViewWorkOrder::~Footprint2DViewWorkOrder() {
  }

  /**
   * This method clones the current Footprint2DViewWorkOrder and returns it.
   * 
   * @return @b Footprint2DViewWorkOrder that was cloned
   */
  Footprint2DViewWorkOrder *Footprint2DViewWorkOrder::clone() const {

    return new Footprint2DViewWorkOrder(*this);
  }
  
  /**
   * This method returns true if one of an image in ImageList images isFootprintable.
   * 
   * @param images ImageList of images
   * 
   * @return @b bool True if one of the images in ImagesList images isFootprintable
   */
  bool Footprint2DViewWorkOrder::isExecutable(ImageList *images) {
    bool result = false;

    foreach (Image *image, *images) {
      result = result || image->isFootprintable();
    }

    return result;
  }

  /**
   * This method calls WorkOrder's execute.
   * 
   * @return @b bool True if WorkOrder::execute() returns true.
   */
  bool Footprint2DViewWorkOrder::execute() {
    bool success = WorkOrder::execute();
    return success;
  }

  /**
   * This method returns whether or not other depends on a Footprint2DViewWorkOrder.
   * 
   * @param other WorkOrder that we are checking for dependencies
   * 
   * @return @b bool True if other depends on a Footprint2DViewWorkOrder
   */
  bool Footprint2DViewWorkOrder::dependsOn(WorkOrder *other) const {
    // depend on types of ourselves.
    return dynamic_cast<Footprint2DViewWorkOrder *>(other);
  }

  /**
   * This methods adds the current item to Footprint2DView.
   * 
   */
  void Footprint2DViewWorkOrder::syncRedo() {
    /*  //tjw
    ProjectItem *currentItem = project()->directory()->model()->currentItem();
    Footprint2DView *view = project()->directory()->addFootprint2DView();
    view->addItem( currentItem );
   */
  }
}

