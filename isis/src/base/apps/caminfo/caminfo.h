#ifndef caminfo_h
#define caminfo_h

#include "UserInterface.h"

namespace Isis{
    void caminfo(UserInterface &ui);

    void caminfo(Cube *incube, UserInterface &ui);
}

#endif
