#ifndef mapmos_h
#define mapmos_h

#include "UserInterface.h"
#include "Pvl.h"
#include "Cube.h"

namespace Isis {
  extern void mapmos( UserInterface &ui, Pvl *log );
  extern void mapmos( Cube *inCube, UserInterface &ui, Pvl *log );
}

#endif
