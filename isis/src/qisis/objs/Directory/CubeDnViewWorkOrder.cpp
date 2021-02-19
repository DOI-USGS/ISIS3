/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CubeDnViewWorkOrder.h"

#include <QtDebug>

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "CubeDnView.h"
#include "Directory.h"
#include "ImageList.h"
#include "MdiCubeViewport.h"
#include "Project.h"
#include "ProjectItem.h"
#include "ProjectItemModel.h"
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
    m_isUndoable = false;
    QAction::setText(tr("Display &Images..."));
    m_isSavedToHistory = false;
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
    if (!images)
      return false;
    return (images->count() > 0 && images->count() < 50);
  }

  /**
   * This check is used by Directory::supportedActions(DataType data).
   *
   * @param images ShapeList we are checking
   *
   * @return @b bool True if the number of shapes is greater than 0 and less than 20.
   */
  bool CubeDnViewWorkOrder::isExecutable(ShapeList *shapes) {
    if (!shapes)
      return false;
    return (shapes->count() > 0 && shapes->count() < 20);
  }



  /**
   * @brief This method asks the user what view they want to see their cube list in. The user
   * can select an existing vew or they can create a new view. The user's choice is then saved using
   * setInternalData().
   *
   * @see WorkOrder::setupExecution()
   *
   * @return @b bool True if WorkOrder::setupExecution() returns true.
   */
  bool CubeDnViewWorkOrder::setupExecution() {
    bool success = WorkOrder::setupExecution();


    if (success) {
      QStringList viewOptions;

      QList<CubeDnView *> existingViews = project()->directory()->cubeDnViews();
      int viewToUse = -1;

      if (existingViews.count()) {
        for (int i = 0; i < existingViews.count(); i++) {
          viewOptions.append(existingViews[i]->windowTitle());
        }
      }

      viewOptions.append(tr("New Cube DN View"));

      if (viewOptions.count() > 1) {
        QString selected = QInputDialog::getItem(NULL, tr("View to see cubes in"),
            tr("Which view would you like your\nimage's DN data to be put into?"),
            viewOptions, viewOptions.count() - 1, false, &success);

        viewToUse = viewOptions.indexOf(selected);
      }
      else {
        viewToUse = viewOptions.count() - 1;
      }

      bool newView = false;
      if (viewToUse == viewOptions.count() - 1) {
        newView = true;
        QUndoCommand::setText(tr("View image DN data of list in new cube DN view"));
      }
      else if (viewToUse != -1) {
        QUndoCommand::setText(tr("View image DN data in cube DN view [%1]")
            .arg(existingViews[viewToUse]->windowTitle()));
      }

      QStringList internalData;
      internalData.append(QString::number(viewToUse));
      internalData.append(newView? "new view" : "existing view");
      setInternalData(internalData);
    }

    return success;
  }


  /**
   * @brief This method adds a new CubeDnView to the project's directory and then adds
   * currentItem() to that.
   */
  void CubeDnViewWorkOrder::execute() {
    QList<ProjectItem *> selectedItems = project()->directory()->model()->selectedItems();

    int viewToUse = internalData().first().toInt();

    CubeDnView *view = NULL;
    if (viewToUse == project()->directory()->cubeDnViews().count()) {
      view = project()->directory()->addCubeDnView();
    }
    else {
      view = project()->directory()->cubeDnViews()[viewToUse];
    }

    view->addItems(selectedItems);
    project()->setClean(false);
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
}
