#ifndef findfeatures_h // Change this to your app name in all lower case suffixed with _h (e.g. campt_h, cam2map_h etc.)
#define findfeatures_h

#include "UserInterface.h"
#include "Pvl.h"

namespace Isis{
  extern void findfeatures(UserInterface &ui, Pvl *log=nullptr);
}

#endif
