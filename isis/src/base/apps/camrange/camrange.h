#ifndef camrange_h
#define camrange_h

#include "UserInterface.h"
#include "Pvl.h"
#include "Cube.h"

namespace Isis {
  extern void camrange( UserInterface &ui, Pvl *log );
  extern void camrange( Cube *cube, UserInterface &ui, Pvl *log );
}

#endif
