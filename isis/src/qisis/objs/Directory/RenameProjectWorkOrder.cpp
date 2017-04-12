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
#include "RenameProjectWorkOrder.h"

#include <QtDebug>

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "Directory.h"
#include "MosaicSceneItem.h"
#include "MosaicSceneWidget.h"
#include "Project.h"

namespace Isis {

  /** 
   * Creates a work order to rename the project using the given new project name. This constructor 
   * is only called if user double-clicks the project name on the project tree.  This WorkOrder is 
   * undoable and runs synchronously. 
   * 
   * @param QString newName New project name 
   * @param Project *project Pointer to the project this work order belongs to.
   */
  RenameProjectWorkOrder::RenameProjectWorkOrder(QString newName, Project *project) :
      WorkOrder(project) {

    m_isUndoable = true;

    QAction::setText(tr("&Rename Project..."));
    QUndoCommand::setText(tr("Rename Project"));

    
    QStringList internalData;
    internalData.append(project->name());
    internalData.append(newName);
    setInternalData(internalData);
  }


  /** 
   * Creates a work order to rename the project. This WorkOrder is undoable and runs synchronously.
   * 
   * @param Project *project Pointer to the project this work order belongs to.
   */
  RenameProjectWorkOrder::RenameProjectWorkOrder(Project *project) :
      WorkOrder(project) {

    m_isUndoable = true;

    QAction::setText(tr("&Rename Project..."));
    QUndoCommand::setText(tr("Rename Project"));
  }


  /**
   * @brief Copy constructor.
   *
   * Creates a copy of the other RenameProjectWorkOrder.
   *
   * @param RenameProjectWorkOrder &other The other work order to copy state from.
   */
  RenameProjectWorkOrder::RenameProjectWorkOrder(const RenameProjectWorkOrder &other) :
      WorkOrder(other) {

    m_isUndoable = other.m_isUndoable;
  }


  /**
   * @brief Destructor.
   *
   * Destructor to clean up any memory that this work order allocates.
   */
  RenameProjectWorkOrder::~RenameProjectWorkOrder() {
  }


  /**
   * @brief This method clones the current RenameProjectWorkOrder and returns it.
   * 
   * @return @b Footprint2DViewWorkOrder that was cloned
   */
  RenameProjectWorkOrder *RenameProjectWorkOrder::clone() const {
    return new RenameProjectWorkOrder(*this);
  }


  /**
   * @brief This method returns true if the user clicked on the project name on the project tree, or 
   *              selected "Rename Project" from the Project menu, otherwise False. This is used by
   *              Directory::supportedActions(DataType data) to determine what actions are appended
   *              to context menus.
   * 
   * @param Context context This indicates whether this WorkOrder is  part of a Project.  Context is 
   *                an enum defined in WorkOrder with two possible values: NoContext,ProjectContext.
   * 
   * @return @b bool True if user selected 
   */
  bool RenameProjectWorkOrder::isExecutable(Context context) {
    return (context == ProjectContext);
  }


  /**
   * @brief Setup this WorkOrder for execution.  Prompt for new project name and make sure it is a 
   *              valid directory location.  This calls WorkOrder::setupExecution().
   * 
   * @return @b bool True if WorkOrder::execute() returns true and footprints can be created, 
   *         otherwise returns False.
   */
  bool RenameProjectWorkOrder::setupExecution() {
    bool success = WorkOrder::setupExecution();

    //  Prompt for new project name.  This will only happen if user initiated by right-clicking on
    //  project name from the project tree or selecting "Rename Project" from the Project menu.
    //  Otherwise, they double-clicked on the project name on the tree and entered a name through
    //  Qt's line edit.
    if (success && internalData().count() == 0) {
      QString newName;

      do {
        newName = QInputDialog::getText(NULL, tr("Enter Project Name"),
            tr("Please enter the new project name"), QLineEdit::Normal,
            project()->name(), &success);

        if (success && !isNameValid(newName)) {
          QMessageBox::critical(NULL, tr("Invalid Project Name"),
                                tr("Project name [%1] is not valid").arg(newName));
        }
      }
      while (success && !isNameValid(newName));

      QStringList internalData;
      internalData.append(project()->name());
      internalData.append(newName);
      setInternalData(internalData);
    }

    QUndoCommand::setText(tr("Rename Project from [%1] to [%2]").
                          arg(internalData()[0]).arg(internalData()[1]));

    return success && (internalData()[1] != project()->name());
  }


  /**
   * Check the validity of the given (proposed) project name. Returns true if it's acceptable, false
   *   otherwise.
   */
  bool RenameProjectWorkOrder::isNameValid(QString nameToCheck) {
    return !nameToCheck.isEmpty();
  }


 /**
   * This WorkOrder is only dependent on another RenameProjectWorkOrder.  It cannot be run if 
   * another RenameProjectWorkOrder is currently being executed. 
   *        
   * @param other  The WorkOrder being checked for dependency.
   * @return @b bool  True if there is the other WorkOrder is another RenameProjectWorkOrder, False
   *         otherwise.
   */
  bool RenameProjectWorkOrder::dependsOn(WorkOrder *other) const {
    // depend on types of ourselves only.
    return dynamic_cast<RenameProjectWorkOrder *>(other);
  }


  /**
   * @brief This will rename the project.
   *  
   */
  void RenameProjectWorkOrder::execute() {
    project()->setName(internalData()[1]);
  }


  /**
   * @brief Changes the project name back to the old name.
   *
   * This will undo this WorkOrder's execute by renaming the project back to the old name.  The old
   * name was saved in the first internal data value.
   *
   * @see WorkOrder::undoExecution()
   */
  void RenameProjectWorkOrder::undoExecution() {
    project()->setName(internalData()[0]);
  }
}

