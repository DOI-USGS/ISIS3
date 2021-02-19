/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CnetEditorViewWorkOrder.h"

#include <QtDebug>

#include <QAction>
#include <QUndoCommand>

#include "CnetEditorView.h"
#include "Control.h"
#include "ControlList.h"
#include "Directory.h"
#include "Project.h"

namespace Isis {

  CnetEditorViewWorkOrder::CnetEditorViewWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("View &Network..."));
    m_isSavedToHistory = false;
    m_isUndoable = false;
  }


  CnetEditorViewWorkOrder::CnetEditorViewWorkOrder(const CnetEditorViewWorkOrder &other) :
      WorkOrder(other) {
  }


  CnetEditorViewWorkOrder::~CnetEditorViewWorkOrder() {
  }


  CnetEditorViewWorkOrder *CnetEditorViewWorkOrder::clone() const {
    return new CnetEditorViewWorkOrder(*this);
  }


  bool CnetEditorViewWorkOrder::isExecutable(ControlList * controls) {


    if (controls) {
      return (controls->count() >= 1);
    }
    return false;
  }


  bool CnetEditorViewWorkOrder::setupExecution() {
    bool success = WorkOrder::setupExecution();

    if (success) {
      QUndoCommand::setText(tr("View control network [%1] in new cnet editor view")
          .arg(controlList()->first()->displayProperties()->displayName()));
    }

    return success;
  }


  bool CnetEditorViewWorkOrder::dependsOn(WorkOrder *other) const {
    // depend on types of ourselves.
    return dynamic_cast<CnetEditorViewWorkOrder *>(other);
  }


  void CnetEditorViewWorkOrder::execute() {

    for (int i = 0; i < controlList()->size(); i++) {
      project()->directory()->addCnetEditorView(controlList()->at(i));
    }
    project()->setClean(false);
  }

  void CnetEditorViewWorkOrder::undoExecution() {
    delete project()->directory()->cnetEditorViews().last();
  }
}
