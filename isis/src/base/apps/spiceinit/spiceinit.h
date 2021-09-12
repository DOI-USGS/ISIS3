#ifndef spiceinit_h
#define spiceinit_h

#include <vector>

#include <QString>

#include "Cube.h"
#include "Pvl.h"
#include "UserInterface.h"

namespace Isis {
  extern void spiceinit(NaifContextPtr naif, Cube *cube, UserInterface &ui, Pvl *log=nullptr);
  extern void spiceinit(NaifContextPtr naif, UserInterface &ui,
                        Pvl *log = nullptr);
}

#endif
