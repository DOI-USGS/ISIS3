#ifndef grid_h
#define grid_h

#include "Cube.h"
#include "UserInterface.h"

namespace Isis{
  extern void grid(Cube *icube, UserInterface &ui);
  extern void grid(UserInterface &ui);
}

#endif