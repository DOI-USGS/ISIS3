#ifndef gaussstretch_h
#define gaussstretch_h

#include "Cube.h"
#include "UserInterface.h"

namespace Isis {
    extern void gaussstretch(Cube *icube, UserInterface &ui);
    extern void gaussstretch(UserInterface &ui);
}

#endif
