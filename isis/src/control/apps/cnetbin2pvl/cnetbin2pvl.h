#ifndef cnetbin2pvl_h
#define cnetbin2pvl_h

#include "UserInterface.h"
#include "ControlNet.h"
#include "Progress.h"

namespace Isis {
  extern void cnetbin2pvl(UserInterface &ui, Progress *progress=0);
  extern void cnetbin2pvl(ControlNet &net, UserInterface &ui, Progress *progress=0);
}

#endif
