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
#include "TargetGetInfoWorkOrder.h"

#include <QtDebug>

#include <QFileDialog>
#include <QInputDialog>

#include "Directory.h"
#include "IException.h"
#include "Project.h"
#include "TargetBody.h"
#include "TargetInfoWidget.h"

namespace Isis {

  TargetGetInfoWorkOrder::TargetGetInfoWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("Get Info..."));
  }


  TargetGetInfoWorkOrder::TargetGetInfoWorkOrder(const TargetGetInfoWorkOrder &other) :
      WorkOrder(other) {
  }


  TargetGetInfoWorkOrder::~TargetGetInfoWorkOrder() {
  }


  TargetGetInfoWorkOrder *TargetGetInfoWorkOrder::clone() const {
    return new TargetGetInfoWorkOrder(*this);
  }


  bool TargetGetInfoWorkOrder::isExecutable(TargetBody *targetBody) {
    if (!targetBody)
      return false;

    // if we already have a view for this target, don't redisplay
    QList<TargetInfoWidget *> existingViews = project()->directory()->targetInfoViews();
    for (int i = 0; i < existingViews.size(); i++) {
      if (existingViews.at(i)->objectName() == targetBody->displayProperties()->displayName())
        return false;
    }

    return true;
  }


  bool TargetGetInfoWorkOrder::execute() {
    bool success = WorkOrder::execute();

    if (success) {
      QString targetDisplayName = targetBody()->displayProperties()->displayName();
      QUndoCommand::setText(tr("Get %1 target info").arg(targetDisplayName));

      QStringList internalData;
      internalData.append(targetDisplayName);
      setInternalData(internalData);
    }

    return success;
  }


  bool TargetGetInfoWorkOrder::dependsOn(WorkOrder *other) const {
    // depend on types of ourselves.
    return dynamic_cast<TargetGetInfoWorkOrder *>(other);
  }


  void TargetGetInfoWorkOrder::syncRedo() {
    TargetInfoWidget *targetInfoWidget =
        project()->directory()->addTargetInfoView(targetBody());


    if (!targetInfoWidget) {
      QString msg = "error displaying target info";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }


  void TargetGetInfoWorkOrder::syncUndo() {
    //delete project()->directory()->cnetEditorViews().last();
  }
}

