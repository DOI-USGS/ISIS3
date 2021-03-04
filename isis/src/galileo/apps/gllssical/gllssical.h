#ifndef gllssical_h // Change this to your app name in all lower case suffixed with _h (e.g. campt_h, cam2map_h etc.)
#define gllssical_h

#include "Cube.h"
#include "UserInterface.h"

namespace Isis{
  extern void gllssical(Cube *cube, UserInterface &ui, Pvl *log=nullptr);
  extern void gllssical(UserInterface &ui, Pvl *log=nullptr);
}

#endif