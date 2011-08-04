#ifndef VieportBufferStretch_h
#define VieportBufferStretch_h

#include "ViewportBufferAction.h"

class QRect;

namespace Isis {
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
