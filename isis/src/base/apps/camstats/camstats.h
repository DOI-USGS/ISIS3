#ifndef camstats_h
#define camstats_h

#include "Pvl.h"
#include "UserInterface.h"

namespace Isis{
  extern void camstats(UserInterface &ui, Pvl *log);
  extern void camstats(Cube *icube, UserInterface &ui, Pvl *log);
}

#endif
