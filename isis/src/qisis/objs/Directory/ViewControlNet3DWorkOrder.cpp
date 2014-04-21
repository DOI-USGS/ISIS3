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
#include "ViewControlNet3DWorkOrder.h"

#include <QtDebug>

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "Control.h"
#include "Directory.h"
#include "Project.h"

namespace Isis {

  ViewControlNet3DWorkOrder::ViewControlNet3DWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("&View ControlNet 3D..."));
  }


  ViewControlNet3DWorkOrder::ViewControlNet3DWorkOrder(const ViewControlNet3DWorkOrder &other) :
      WorkOrder(other) {
  }


  ViewControlNet3DWorkOrder::~ViewControlNet3DWorkOrder() {
  }


  ViewControlNet3DWorkOrder *ViewControlNet3DWorkOrder::clone() const {
    return new ViewControlNet3DWorkOrder(*this);
  }


  bool ViewControlNet3DWorkOrder::isExecutable(QList<Control *> controls) {
    return (controls.count() == 1);
  }


  bool ViewControlNet3DWorkOrder::execute() {
    bool success = WorkOrder::execute();

    if (success) {
//        JigsawDialog* bundledlg = new JigsawDialog(project());
//        bundledlg->setAttribute(Qt::WA_DeleteOnClose);
//        bundledlg->show();
//      QUndoCommand::setText(tr("&Bundle Adjustment")
//          .arg(controlList().first()->displayProperties()->displayName()));
    }

    return success;
  }


  bool ViewControlNet3DWorkOrder::dependsOn(WorkOrder *other) const {
    // depend on types of ourselves.
    return dynamic_cast<ViewControlNet3DWorkOrder *>(other);
  }


  void ViewControlNet3DWorkOrder::syncRedo() {
    //project()->directory()->addCnetEditorView(controlList().first());
  }


  void ViewControlNet3DWorkOrder::syncUndo() {
    //delete project()->directory()->cnetEditorViews().last();
  }
}

