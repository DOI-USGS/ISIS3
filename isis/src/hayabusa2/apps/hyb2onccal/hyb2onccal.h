#ifndef hyb2onccal_h
#define hyb2onccal_h

#include "Cube.h"
#include "Pvl.h"
#include "UserInterface.h"

namespace Isis {
  extern void hyb2onccal(Cube *icube, UserInterface &ui, Pvl *log);
  extern void hyb2onccal(UserInterface &ui, Pvl *log);
}

#endif //hyb2onccal_h

