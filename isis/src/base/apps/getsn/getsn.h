#ifndef getsn_h
#define getsn_h

#include "UserInterface.h"
#include "Pvl.h"
#include "Cube.h"

namespace Isis {
  extern void getsn( Cube *cube, UserInterface &ui, Pvl *log ); 
  extern void getsn( UserInterface &ui, Pvl *log );
}

#endif
