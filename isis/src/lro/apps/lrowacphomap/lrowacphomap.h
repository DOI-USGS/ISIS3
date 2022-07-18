#ifndef lrowacphomap_h
#define lrowacphomap_h

#include "Cube.h"
#include "Pvl.h"
#include "UserInterface.h"


namespace Isis {
    extern Pvl lrowacphomap(UserInterface &ui);
    extern Pvl lrowacphomap(Cube *icube, UserInterface &ui);
}

#endif