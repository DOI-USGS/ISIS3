#ifndef MoveToTopSceneWorkOrder_H
#define MoveToTopSceneWorkOrder_H

#include "MosaicSceneWorkOrder.h"

namespace Isis {
  class MosaicSceneWidget;

  /**
   * @brief Move images on top of all other images in a mosaic scene
   * This workorder is synchronous and undoable
   *
   * This shows up as "Bring to Front" to the user.
   *
   * @author 2012-10-04 Stuart Sides and Steven Lambright
   *
   * @internal
   *   @history 2017-04-16 J Bonn - Updated to new workorder design #4764.
   */
  class MoveToTopSceneWorkOrder : public MosaicSceneWorkOrder {
      Q_OBJECT
    public:
      MoveToTopSceneWorkOrder(MosaicSceneWidget *scene, Project *project);
      MoveToTopSceneWorkOrder(Project *project);
      MoveToTopSceneWorkOrder(const MoveToTopSceneWorkOrder &other);
      ~MoveToTopSceneWorkOrder();

      virtual MoveToTopSceneWorkOrder *clone() const;

      void execute();
      void undoExecution();

    private:
      MoveToTopSceneWorkOrder &operator=(const MoveToTopSceneWorkOrder &rhs);
  };
}

#endif
