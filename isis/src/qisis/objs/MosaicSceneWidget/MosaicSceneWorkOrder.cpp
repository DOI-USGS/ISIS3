#include "MosaicSceneWorkOrder.h"

#include <QProgressDialog>

#include "Directory.h"
#include "Footprint2DView.h"
#include "MosaicSceneWidget.h"

namespace Isis {

  MosaicSceneWorkOrder::MosaicSceneWorkOrder(QString actionText, MosaicSceneWidget *scene,
      Project *project) : WorkOrder(project) {

    QAction::setText(actionText);
    QUndoCommand::setText(tr("%1 on [%2]").arg(actionText).arg(scene->windowTitle()));

    QStringList data;
    data.append(scene->windowTitle());
    setInternalData(data);
  }


  MosaicSceneWorkOrder::MosaicSceneWorkOrder(Project *project) :
      WorkOrder(project) {
  }


  MosaicSceneWorkOrder::MosaicSceneWorkOrder(const MosaicSceneWorkOrder &other) :
      WorkOrder(other) {
  }


  /**
   * Destructor
   */
  MosaicSceneWorkOrder::~MosaicSceneWorkOrder() {
  }


  /**
   * Returns the MosaicSceneWidget corresponding to this work order's interal data
   * (the MosaicSceneWidget's window title).
   * 
   * @return @b MosaicSceneWidget* Returns a pointer to the MosaicSceneWidget with a window title
   *                               that matches this work order's internal data's window title.
   * 
   * @see MosaicSceneWorkOrder::MosaicSceneWorkOrder()
   */
  MosaicSceneWidget *MosaicSceneWorkOrder::scene() {

    MosaicSceneWidget *result = NULL;

    foreach (Footprint2DView *scene, directory()->footprint2DViews()) {
      if (internalData().first() == scene->mosaicSceneWidget()->windowTitle()) {
        result = scene->mosaicSceneWidget();
      }
    }
    return result;
  }


  /**
   *
   * @see MosaicSceneWidget::moveZ for documentation on zValuesMightBeInUse
   */
  void MosaicSceneWorkOrder::restoreZPositions(bool zValuesMightBeInUse) {
    QStringList zPositions = internalData().mid(1);

    MosaicSceneWidget *sceneWidget = scene();

    QProgressDialog progress(tr("Restoring Z Values"), "", 0, imageList()->count());
    progress.setCancelButton(NULL);
    for (int i = imageList()->count() - 1; i >= 0; i--) {
      int originalZ = qRound(zPositions[i].toDouble());
      sceneWidget->moveZ(imageList()->at(i), originalZ, zValuesMightBeInUse);
      progress.setValue(progress.value() + 1);
    }
  }


  void MosaicSceneWorkOrder::storeZPositions(QList<double> zPositions) {
    QStringList data;
    data.append(internalData().first());

    foreach (double zPosition, zPositions) {
      data.append(QString::number(qRound(zPosition)));
    }

    setInternalData(data);
  }
}
