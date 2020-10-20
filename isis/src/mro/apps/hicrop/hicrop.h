#ifndef hicrop_h 
#define hicrop_h

#include "Cube.h"
#include "UserInterface.h"

namespace Isis{
  extern void hicrop(UserInterface &ui, Pvl *log=nullptr);
  extern void hicrop(Cube *cube, UserInterface &ui, Pvl *log=nullptr);
}

#endif