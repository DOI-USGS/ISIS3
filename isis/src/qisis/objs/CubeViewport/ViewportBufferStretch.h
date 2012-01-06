#ifndef VieportBufferStretch_h
#define VieportBufferStretch_h

#include "ViewportBufferAction.h"

class QRect;

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class ViewportBufferStretch : public ViewportBufferAction {
    public:
      ViewportBufferStretch();
      ~ViewportBufferStretch();

      virtual ViewportBufferAction::ActionType getActionType() {
        return ViewportBufferAction::stretch;
      };
  };
}

#endif
