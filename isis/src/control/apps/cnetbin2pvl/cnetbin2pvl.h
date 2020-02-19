#ifndef cnetbin2pvl_h
#define cnetbin2pvl_h

#include "UserInterface.h"
#include "ControlNet.h"
#include "Progress.h"

namespace Isis {
  extern void cnetbin2pvl(UserInterface &ui);
  extern void cnetbin2pvl(ControlNet &net, Progress &progress, UserInterface &ui);
}

#endif
