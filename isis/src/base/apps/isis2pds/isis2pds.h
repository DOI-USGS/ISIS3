#ifndef isis2pds_h
#define isis2pds_h

#include "Cube.h"
#include "UserInterface.h"

namespace Isis{
  extern void isis2pds(Cube *cube, UserInterface &ui, Pvl *log=nullptr);
  extern void isis2pds(UserInterface &ui, Pvl *log=nullptr);
}

#endif
