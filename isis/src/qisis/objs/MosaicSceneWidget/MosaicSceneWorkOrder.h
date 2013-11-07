#ifndef MosaicSceneWorkOrder_H
#define MosaicSceneWorkOrder_H

#include "WorkOrder.h"

namespace Isis {
  class MosaicSceneWidget;

  /**
   * @brief Work order associated with a MosaicSceneWidget
   *
   * This class is designed to provide common functionality for interactions with a
   *   MosaicSceneWidget.
   *
   * Internal data is in the format of:
   *   Scene Widget Name
   *   Stored Z Position 1 (OPTIONAL)
   *   Stored Z Position 2 (OPTIONAL)
   *   Stored Z Position 3 (OPTIONAL)
   *   Stored Z Position ... (OPTIONAL)
   *
   * @author 2012-10-18 Stuart Sides and Steven Lambright
   *
   * @internal
   */
  class MosaicSceneWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      MosaicSceneWorkOrder(QString actionText, MosaicSceneWidget *scene, Project *project);
      MosaicSceneWorkOrder(Project *project);
      MosaicSceneWorkOrder(const MosaicSceneWorkOrder &other);
      ~MosaicSceneWorkOrder();

    protected:
      MosaicSceneWidget *scene();

      void restoreZPositions(bool zValuesMightBeInUse);
      void storeZPositions(QList<double> zPositions);

    private:
      MosaicSceneWorkOrder &operator=(const MosaicSceneWorkOrder &rhs);
  };
}

#endif
