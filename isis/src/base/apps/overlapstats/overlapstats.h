#ifndef overlapstats_h
#define overlapstats_h

#include "UserInterface.h"
#include "Pvl.h"
#include "Cube.h"

namespace Isis {
  extern void overlapstats( UserInterface &ui, Pvl *log );
  extern QString FormatString( double input, int head, int tail );
}

#endif
