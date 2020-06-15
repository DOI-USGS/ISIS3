#ifndef cnetwinnow_h
#define cnetwinnow_h

#include "ControlNet.h"
#include "ControlMeasure.h"
#include "UserInterface.h"
#include "Progress.h"

namespace Isis {
  extern void cnetwinnow(UserInterface &ui, Progress *progress = 0);

  extern void cnetwinnow(ControlNet &net, SerialNumberList &serialNumList, UserInterface &ui, Progress *progress = 0);
}

#endif
