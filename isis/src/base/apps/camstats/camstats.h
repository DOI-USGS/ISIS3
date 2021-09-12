#ifndef camstats_h
#define camstats_h

#include "Pvl.h"
#include "UserInterface.h"

namespace Isis{
  extern void camstats(NaifContextPtr naif, UserInterface &ui, Pvl *log);
  extern void camstats(NaifContextPtr naif, Cube *icube, UserInterface &ui, Pvl *log);
}

#endif
