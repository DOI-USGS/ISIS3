#ifndef cnetpvl2bin_h
#define cnetpvl2bin_h

#include "UserInterface.h"
#include "ControlNet.h"
#include "Progress.h"

namespace Isis {
  extern void cnetpvl2bin(UserInterface &ui, Progress *progress=0);
  extern void cnetpvl2bin(ControlNet &net, UserInterface &ui, Progress *progress=0);
}

#endif
