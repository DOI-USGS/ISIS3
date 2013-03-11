#ifndef MoveToBottomSceneWorkOrder_H
#define MoveToBottomSceneWorkOrder_H

#include "MosaicSceneWorkOrder.h"

namespace Isis {
  class MosaicSceneWidget;

  /**
   * @brief Move images below all other images in a mosaic scene
   *
   * This shows up as "Send to Back" to the user.
   *
   * @author 2012-10-18 Stuart Sides and Steven Lambright
   *
   * @internal
   */
  class MoveToBottomSceneWorkOrder : public MosaicSceneWorkOrder {
      Q_OBJECT
    public:
      MoveToBottomSceneWorkOrder(MosaicSceneWidget *scene, Project *project);
      MoveToBottomSceneWorkOrder(Project *project);
      MoveToBottomSceneWorkOrder(const MoveToBottomSceneWorkOrder &other);
      ~MoveToBottomSceneWorkOrder();

      virtual MoveToBottomSceneWorkOrder *clone() const;

      void syncRedo();
      void syncUndo();

    private:
      MoveToBottomSceneWorkOrder &operator=(const MoveToBottomSceneWorkOrder &rhs);
  };
}

#endif
