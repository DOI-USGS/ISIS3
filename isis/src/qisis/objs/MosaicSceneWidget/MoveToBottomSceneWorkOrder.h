#ifndef MoveToBottomSceneWorkOrder_H
#define MoveToBottomSceneWorkOrder_H

#include "MosaicSceneWorkOrder.h"

namespace Isis {
  class MosaicSceneWidget;

  /**
   * @brief Move images below all other images in a mosaic scene
   * This workorder is synchronous and undoable
   *
   * This shows up as "Send to Back" to the user.
   *
   * @author 2012-10-18 Stuart Sides and Steven Lambright
   *
   * @internal
   *   @history 2017-04-16 J Bonn - Updated to new workorder design #4764.
   */
  class MoveToBottomSceneWorkOrder : public MosaicSceneWorkOrder {
      Q_OBJECT
    public:
      MoveToBottomSceneWorkOrder(MosaicSceneWidget *scene, Project *project);
      MoveToBottomSceneWorkOrder(Project *project);
      MoveToBottomSceneWorkOrder(const MoveToBottomSceneWorkOrder &other);
      ~MoveToBottomSceneWorkOrder();

      virtual MoveToBottomSceneWorkOrder *clone() const;

      void execute();
      void undoExecution();

    private:
      MoveToBottomSceneWorkOrder &operator=(const MoveToBottomSceneWorkOrder &rhs);
  };
}

#endif
