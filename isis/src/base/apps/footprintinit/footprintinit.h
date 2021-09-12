#include "Cube.h"
#include "Pvl.h"
#include "UserInterface.h"

namespace Isis {
  extern void footprintinit(UserInterface &ui, NaifContextPtr naif, Pvl *log=nullptr);

  extern void footprintinit(NaifContextPtr naif, Cube *cube, UserInterface &ui, Pvl *log=nullptr);
}
