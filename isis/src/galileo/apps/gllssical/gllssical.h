#ifndef gllssical_h 
#define gllssical_h

#include "Cube.h"
#include "UserInterface.h"

namespace Isis{
  extern void gllssical(Cube *cube, UserInterface &ui, Pvl *log=nullptr);
  extern void gllssical(UserInterface &ui, Pvl *log=nullptr);
}

#endif