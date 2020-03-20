#ifndef campt_h
#define campt_h

#include "Pvl.h"
#include "UserInterface.h"

namespace Isis{
  extern void campt(Cube *cube, UserInterface &ui, Pvl *log);
  extern void campt(UserInterface &ui, Pvl *log);
}

#endif
