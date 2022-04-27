#ifndef lronacpho_h
#define lronacpho_h

#include "Cube.h"
#include "lronacpho.h"
#include "UserInterface.h"

namespace Isis{
  extern void lronacpho(Cube *iCube, UserInterface &ui, Pvl *log=nullptr);
  extern void lronacpho(UserInterface &ui, Pvl *log=nullptr);
}

#endif