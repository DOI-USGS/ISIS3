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
#include "CloseProjectWorkOrder.h"

#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QtConcurrentMap>

#include "Project.h"

namespace Isis {

  /**
   * This method sets the text of the work order to Close Project and 
   * sets setCreatesCleanState to true
   * 
   * @param project The Project that we are going to open
   */
  CloseProjectWorkOrder::CloseProjectWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("&Close Project"));
    QUndoCommand::setText(tr("Close Project"));

    setCreatesCleanState(true);
  }

  /**
   * As of 06/06/2016 this method is not implemented.
   */
  CloseProjectWorkOrder::CloseProjectWorkOrder(const CloseProjectWorkOrder &other) :
      WorkOrder(other) {
  }

  /**
   * Destructor
   */
  CloseProjectWorkOrder::~CloseProjectWorkOrder() {

  }

  /**
   * This method clones the CloseProjectWorkOrder
   * 
   * @return @b CloseProjectWorkOrder Returns a clone of the CloseProjectWorkOrder
   */
  CloseProjectWorkOrder *CloseProjectWorkOrder::clone() const {
    return new CloseProjectWorkOrder(*this);
  }

  /**
   * This method will always return true
   * 
   * @return @b bool True always
   */
  bool CloseProjectWorkOrder::isExecutable() {
//  return (project());
    return true;
  }

  /**
   * If WorkOrder::execute() returns true, then this method returns true.
   * 
   * @return @b bool True if WorkOrder::execute() returns true.
   */
  bool CloseProjectWorkOrder::setupExecution() {
    bool success = WorkOrder::setupExecution();

//    // If more than this work order is in the history, don't allow this operation
//    if (success && project()->workOrderHistory().count()) {
//      QMessageBox::critical(NULL, tr("Unable To Open a Project"),
//                            tr("If you have modified your current project, you cannot open a new"
//                               " project because this is not yet implemented"));
//      success = false;
//    }
//    else if (success) {
//      QString projectName = QFileDialog::getExistingDirectory(qobject_cast<QWidget *>(parent()),
//                                                              tr("Open Project"));

//      if (!projectName.isEmpty()) {
//        project()->open(projectName);
//      }
//      else {
//        success = false;
//      }
//    }

    return success;
  }
}
