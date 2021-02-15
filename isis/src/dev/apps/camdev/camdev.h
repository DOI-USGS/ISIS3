#ifndef camdev_h 
#define camdev_h

#include "UserInterface.h"
#include "Cube.h"

namespace Isis {
  extern void camdev(Cube *icube, UserInterface &ui);
  extern void camdev(UserInterface &ui);
}

#endif