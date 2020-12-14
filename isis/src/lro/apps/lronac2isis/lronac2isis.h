#ifndef lronac2isis_h // Change this to your app name in all lower case suffixed with _h (e.g. campt_h, cam2map_h etc.)
#define lronac2isis_h

#include "Cube.h"
#include "UserInterface.h"

namespace Isis{
  extern void lronac2isis(Cube *cube, UserInterface &ui);
  extern void lronac2isis(UserInterface &ui);
}

#endif