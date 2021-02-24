#ifndef ctxcal_h 
#define ctxcal_h

#include "Cube.h"
#include "UserInterface.h"

namespace Isis{
  extern void ctxcal(Cube *cube, UserInterface &ui);
  extern void ctxcal(UserInterface &ui);
}

#endif