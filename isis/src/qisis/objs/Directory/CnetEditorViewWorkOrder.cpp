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
