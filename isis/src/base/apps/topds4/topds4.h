#ifndef topds4_h
#define topds4_h

#include "Cube.h"
#include "Pvl.h"
#include "UserInterface.h"

namespace Isis {
  extern void topds4(Cube* cube, UserInterface &ui, Pvl *log=nullptr);
  extern void topds4(UserInterface &ui, Pvl *log=nullptr);
}

#endif
