#ifndef MoveDownOneSceneWorkOrder_H
#define MoveDownOneSceneWorkOrder_H

#include "MosaicSceneWorkOrder.h"

namespace Isis {
  class MosaicSceneWidget;

  /**
   * @brief Move images, one by one, below the immediately-below intersecting image in a scene
   * This workorder is synchronous and undoable
   *
   * This shows up as "Send Backward" to the user.
   *
   * @author 2012-10-04 Stuart Sides and Steven Lambright
   *
   * @internal
   *   @history 2017-04-16 J Bonn - Updated to new workorder design #4764.
   */
  class MoveDownOneSceneWorkOrder : public MosaicSceneWorkOrder {
      Q_OBJECT
    public:
      MoveDownOneSceneWorkOrder(MosaicSceneWidget *scene, Project *project);
      MoveDownOneSceneWorkOrder(Project *project);
      MoveDownOneSceneWorkOrder(const MoveDownOneSceneWorkOrder &other);
      ~MoveDownOneSceneWorkOrder();

      virtual MoveDownOneSceneWorkOrder *clone() const;

      void execute();
      void undoExecution();

    private:
      MoveDownOneSceneWorkOrder &operator=(const MoveDownOneSceneWorkOrder &rhs);
  };
}

#endif
