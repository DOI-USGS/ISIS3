/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CloseProjectWorkOrder.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QtConcurrentMap>

#include "SaveProjectWorkOrder.h"
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
    m_isSavedToHistory = false;
    m_isUndoable = false;
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
    if (success && !project()->isClean()) {
      QMessageBox *box = new QMessageBox(QMessageBox::NoIcon,
              QString("Current Project Has Unsaved Changes"),
              QString("Would you like to save your current project?"),
              QMessageBox::NoButton, qobject_cast<QWidget *>(parent()), Qt::Dialog);
      QPushButton *save = box->addButton("Save", QMessageBox::AcceptRole);
      QPushButton *dontsave = box->addButton("Don't Save", QMessageBox::RejectRole);
      QPushButton *cancel = box->addButton("Cancel", QMessageBox::NoRole);
      box->exec();

      if (box->clickedButton() == (QAbstractButton*)cancel) {
        success = false;
      }

      else if (box->clickedButton() == (QAbstractButton*)dontsave) {
        success = true;
      }

      else if (box->clickedButton() == (QAbstractButton*)save) {
        SaveProjectWorkOrder *workOrder = new SaveProjectWorkOrder(project());
        project()->addToProject(workOrder);
      }
    }
    return success;
  }

 /**
  * @description Clear the current Project
  */
  void CloseProjectWorkOrder::execute() {
    project()->clear();
    project()->setClean(true);
  }
}
