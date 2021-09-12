#ifndef campt_h
#define campt_h

#include "Pvl.h"
#include "UserInterface.h"

namespace Isis{
  extern void campt(NaifContextPtr naif, Cube *cube, UserInterface &ui, Pvl *log);
  extern void campt(NaifContextPtr naif, UserInterface &ui, Pvl *log);
}

#endif
