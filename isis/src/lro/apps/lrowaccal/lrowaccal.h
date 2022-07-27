#ifndef lrowaccal_h
#define lrowaccal_h

#include "Cube.h"
#include "UserInterface.h"


namespace Isis {
  extern void lrowaccal(UserInterface &ui);
  extern void lrowaccal(Cube *icube, UserInterface &ui);
}

#endif