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
#include "SaveProjectWorkOrder.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QtConcurrentMap>

#include "Cube.h"
#include "CubeAttribute.h"
#include "FileName.h"
#include "Project.h"

namespace Isis {

  /**
   * Constructor
   *
   * Creates a work order for saving the state of the project.
   *
   * @param Project *project Pointer to the project to save.
   */
  SaveProjectWorkOrder::SaveProjectWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("&Save Project"));
    QUndoCommand::setText(tr("Save Project"));

    setCreatesCleanState(true);
  }


  /**
   * Copy constrcutor
   *
   * Creates a SaveProjectWorkOrder from another one.
   *
   * @param const SaveProjectWorkOrder &order The work order to copy from.
   */
  SaveProjectWorkOrder::SaveProjectWorkOrder(const SaveProjectWorkOrder &other) :
      WorkOrder(other) {
  }


  /**
   * Destructor
   */
  SaveProjectWorkOrder::~SaveProjectWorkOrder() {
  }


  /**
   * Clones an existing SaveProjectWorkder and gives back a newly allocated copy of the work order.
   *
   * @return SaveProjectWorkOrder* Returns a newly allocated clone of this work order.
   */
  SaveProjectWorkOrder *SaveProjectWorkOrder::clone() const {
    return new SaveProjectWorkOrder(*this);
  }


  /**
   * Sets up the work order.
   *
   * This will check to see if the Project::save() was completed (i.e. checks if the file
   * dialog created for a temp project wasn't cancelled by the user). If the Project::save()
   * was not cancelled, then the project is set to a clean state and this returns true. Otherwise
   * if the Project::save() is cancelled, the project is still not clean and false is returned
   * indicated the setup failed.
   *
   * @see Project::save()
   *
   * @return bool Returns if the setup was successful or not. See description for more details.
   */
  bool SaveProjectWorkOrder::setupExecution() {
    bool success = WorkOrder::setupExecution();

    if (success) {
      // Check to save if the save dialog (for a temp project) completed 
      // (i.e. it was not cancelled)
      success = project()->save();
      if (success) {
        project()->setClean(true);
      }
    }

    return success;
  }
}
