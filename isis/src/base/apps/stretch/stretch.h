#ifndef stretch_app_h
#define stretch_app_h

#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "UserInterface.h"

namespace Isis {
  extern void stretch(UserInterface &ui, Pvl *log=nullptr);

  extern void stretch(Cube *inCube, UserInterface &ui, Pvl *log=nullptr);
}

#endif
