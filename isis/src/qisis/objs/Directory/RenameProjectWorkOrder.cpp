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
  RenameProjectWorkOrder::RenameProjectWorkOrder(QString newName, Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("&Rename Project..."));
    QUndoCommand::setText(tr("Rename Project"));

    QStringList internalData;
    internalData.append(project->name());
    internalData.append(newName);
    setInternalData(internalData);
  }


  RenameProjectWorkOrder::RenameProjectWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("&Rename Project..."));
    QUndoCommand::setText(tr("Rename Project"));
  }


  RenameProjectWorkOrder::RenameProjectWorkOrder(const RenameProjectWorkOrder &other) :
      WorkOrder(other) {
  }


  RenameProjectWorkOrder::~RenameProjectWorkOrder() {
  }


  RenameProjectWorkOrder *RenameProjectWorkOrder::clone() const {
    return new RenameProjectWorkOrder(*this);
  }


  bool RenameProjectWorkOrder::isExecutable(Context context) {
    return (context == ProjectContext);
  }


  bool RenameProjectWorkOrder::execute() {
    bool success = WorkOrder::execute();

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

    QUndoCommand::setText(tr("Rename Project To [%1]").arg(internalData()[1]));

    return success && (internalData()[1] != project()->name());
  }


  /**
   * Check the validity of the given (proposed) project name. Returns true if it's acceptable, false
   *   otherwise.
   */
  bool RenameProjectWorkOrder::isNameValid(QString nameToCheck) {
    return !nameToCheck.isEmpty();
  }


  bool RenameProjectWorkOrder::dependsOn(WorkOrder *other) const {
    // depend on types of ourselves only.
    return dynamic_cast<RenameProjectWorkOrder *>(other);
  }


  void RenameProjectWorkOrder::syncRedo() {
    project()->setName(internalData()[1]);
  }


  void RenameProjectWorkOrder::syncUndo() {
    project()->setName(internalData()[0]);
  }
}

