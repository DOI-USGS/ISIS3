#ifndef shadow_h
#define shadow_h

#include "Pvl.h"
#include "UserInterface.h"

namespace Isis{
  extern void shadow(UserInterface &ui, Pvl *log=nullptr);

  extern void shadow(Cube *demCube, UserInterface &ui, Pvl *log=nullptr);
}

#endif
