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
#include "JigsawWorkOrder.h"

#include <QtDebug>

#include <QDockWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "Control.h"
#include "Directory.h"
#include "JigsawDialog.h"
#include "Project.h"

namespace Isis {

  JigsawWorkOrder::JigsawWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("&Bundle Adjustment..."));
    QUndoCommand::setText("&Bundle Adjustment...");
  }


  JigsawWorkOrder::JigsawWorkOrder(const JigsawWorkOrder &other) :
      WorkOrder(other) {
  }


  JigsawWorkOrder::~JigsawWorkOrder() {
  }


  JigsawWorkOrder *JigsawWorkOrder::clone() const {
    return new JigsawWorkOrder(*this);
  }


  bool JigsawWorkOrder::isExecutable() {
    return (project()->controls().size() > 0 && project()->images().size() > 0);
  }


  bool JigsawWorkOrder::execute() {
    bool success = WorkOrder::execute();
/*
    if (success) {
        JigsawDialog* bundledlg = new JigsawDialog(project());
        bundledlg->setAttribute(Qt::WA_DeleteOnClose);
        bundledlg->show();
//      QUndoCommand::setText(tr("&Bundle Adjustment")
//          .arg(controlList().first()->displayProperties()->displayName()));
    }
*/
    if (success) {
//      QDockWidget* dock = new QDockWidget();
//      dock->setMinimumWidth(525);
//      dock->setMinimumHeight(325);
//      dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
      JigsawDialog* bundledlg = new JigsawDialog(project());
      bundledlg->setAttribute(Qt::WA_DeleteOnClose);
      bundledlg->show();
//      dock->setWidget(bundledlg);
//      dock->show();
    }

    return success;
  }


  bool JigsawWorkOrder::dependsOn(WorkOrder *other) const {
    // depend on types of ourselves.
    return dynamic_cast<JigsawWorkOrder *>(other);
  }


  void JigsawWorkOrder::syncRedo() {
//    TargetInfoWidget *targetInfoWidget =
//        project()->directory()->addTargetInfoView(targetBody());


//    if (!targetInfoWidget) {
//      QString msg = "error displaying target info";
//      throw IException(IException::Programmer, msg, _FILEINFO_);
//    }
  }


  void JigsawWorkOrder::syncUndo() {
    //delete project()->directory()->cnetEditorViews().last();
  }
}

