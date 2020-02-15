#ifndef spiceinit_h
#define spiceinit_h

#include <vector>

#include <QString>

#include "Cube.h"
#include "Pvl.h"
#include "UserInterface.h"

namespace Isis {
  extern void spiceinit(Cube *cube, UserInterface &ui, Pvl *log=nullptr);
  extern void spiceinit(UserInterface &ui,
                        Pvl *log = nullptr);
}

#endif
