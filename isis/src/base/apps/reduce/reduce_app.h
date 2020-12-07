#ifndef reduceapp_h // Change this to your app name in all lower case suffixed with _h (e.g. campt_h, cam2map_h etc.)
#define reduceapp_h

#include "UserInterface.h"

namespace Isis{
  extern void reduce(UserInterface &ui, Pvl *log=nullptr);
}

#endif
