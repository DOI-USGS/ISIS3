#ifndef grid_h
#define grid_h

#include "Cube.h"
#include "Pvl.h"
#include "UserInterface.h"

namespace Isis{
  extern void grid(Cube *icube, UserInterface &ui, Pvl *log);
  extern void grid(UserInterface &ui, Pvl *log);
}

#endif