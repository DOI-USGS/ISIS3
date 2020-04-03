#ifndef enlarge_app_h
#define enlarge_app_h


#include "Cube.h"
#include "Pvl.h"
#include "UserInterface.h"

namespace Isis{
  extern void enlarge(Cube *cube, UserInterface &ui, Pvl *log);
  extern void enlarge(UserInterface &ui, Pvl *log);
}

#endif
