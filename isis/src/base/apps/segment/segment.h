#ifndef segment_h
#define segment_h

#include "Cube.h"
#include "UserInterface.h"

namespace Isis {
  extern void segment(Cube *cube, UserInterface &ui);
  extern void segment(UserInterface &ui);
}

#endif
