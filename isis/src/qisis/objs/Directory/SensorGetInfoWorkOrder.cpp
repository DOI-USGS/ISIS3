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
#include "SensorGetInfoWorkOrder.h"

#include <QtDebug>

#include <QFileDialog>
#include <QInputDialog>

#include "Directory.h"
#include "IException.h"
#include "Project.h"
#include "GuiCamera.h"
#include "SensorInfoWidget.h"

namespace Isis {

  SensorGetInfoWorkOrder::SensorGetInfoWorkOrder(Project *project) :
      WorkOrder(project) {
    QAction::setText(tr("Get Info..."));
  }


  SensorGetInfoWorkOrder::SensorGetInfoWorkOrder(const SensorGetInfoWorkOrder &other) :
      WorkOrder(other) {
  }


  SensorGetInfoWorkOrder::~SensorGetInfoWorkOrder() {
  }


  SensorGetInfoWorkOrder *SensorGetInfoWorkOrder::clone() const {
    return new SensorGetInfoWorkOrder(*this);
  }


  bool SensorGetInfoWorkOrder::isExecutable(GuiCamera *guiCamera) {
    if (!guiCamera)
      return false;

    // if we already have a view for this camera, don't redisplay
    QList<SensorInfoWidget *> existingViews = project()->directory()->sensorInfoViews();
    for (int i = 0; i < existingViews.size(); i++) {
      if (existingViews.at(i)->objectName() == guiCamera->displayProperties()->displayName())
        return false;
    }

    return true;
  }


  bool SensorGetInfoWorkOrder::execute() {
    bool success = WorkOrder::execute();

    if (success) {
      QString sensorDisplayName = guiCamera()->displayProperties()->displayName();
      QUndoCommand::setText(tr("Get %1 sensor info").arg(sensorDisplayName));

      QStringList internalData;
      internalData.append(sensorDisplayName);
      setInternalData(internalData);
    }

    return success;
  }


  bool SensorGetInfoWorkOrder::dependsOn(WorkOrder *other) const {
    // depend on types of ourselves.
    return dynamic_cast<SensorGetInfoWorkOrder *>(other);
  }


  void SensorGetInfoWorkOrder::syncRedo() {
    SensorInfoWidget *sensorInfoWidget =
        project()->directory()->addSensorInfoView(guiCamera());


    if (!sensorInfoWidget) {
      QString msg = "error displaying sensor info";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }


  void SensorGetInfoWorkOrder::syncUndo() {
    //delete project()->directory()->cnetEditorViews().last();
  }
}

