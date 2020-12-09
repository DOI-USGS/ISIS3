#ifndef pointreg_h
#define pointreg_h

#include "ControlNet.h"
#include "UserInterface.h"
#include "Pvl.h"

namespace Isis{
  extern void pointreg(ControlNet inNet, UserInterface &ui, Pvl *appLog);
  extern void pointreg(UserInterface &ui, Pvl *appLog);
}

#endif
