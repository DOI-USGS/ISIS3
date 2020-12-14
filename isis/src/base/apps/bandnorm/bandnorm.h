#ifndef std2isis_h 
#define std2isis_h

#include "UserInterface.h"
#include "Cube.h"

namespace Isis {
  extern void bandnorm(Cube *icube, UserInterface &ui);
  extern void bandnorm(UserInterface &ui);
}

#endif