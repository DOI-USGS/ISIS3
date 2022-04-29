#ifndef lronaccal_h
#define lronaccal_h

#include "Cube.h"
#include "lronaccal.h"
#include "UserInterface.h"

namespace Isis{
  extern void lronaccal(UserInterface &ui);
  extern void lronaccal(Cube *iCube, UserInterface &ui);
}

#endif