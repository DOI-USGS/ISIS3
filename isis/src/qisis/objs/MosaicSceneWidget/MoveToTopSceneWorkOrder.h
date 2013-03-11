#ifndef MoveToTopSceneWorkOrder_H
#define MoveToTopSceneWorkOrder_H

#include "MosaicSceneWorkOrder.h"

namespace Isis {
  class MosaicSceneWidget;

  /**
   * @brief Move images on top of all other images in a mosaic scene
   *
   * This shows up as "Bring to Front" to the user.
   *
   * @author 2012-10-04 Stuart Sides and Steven Lambright
   *
   * @internal
   */
  class MoveToTopSceneWorkOrder : public MosaicSceneWorkOrder {
      Q_OBJECT
    public:
      MoveToTopSceneWorkOrder(MosaicSceneWidget *scene, Project *project);
      MoveToTopSceneWorkOrder(Project *project);
      MoveToTopSceneWorkOrder(const MoveToTopSceneWorkOrder &other);
      ~MoveToTopSceneWorkOrder();

      virtual MoveToTopSceneWorkOrder *clone() const;

      void syncRedo();
      void syncUndo();

    private:
      MoveToTopSceneWorkOrder &operator=(const MoveToTopSceneWorkOrder &rhs);
  };
}

#endif
