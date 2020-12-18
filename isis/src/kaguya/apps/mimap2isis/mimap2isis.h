#ifndef mimap2isis_h
#define mimap2isis_h

#include "Pvl.h"
#include "UserInterface.h"

namespace Isis{
  extern void mimap2isis(UserInterface &ui, Pvl *log=nullptr);
}

#endif
