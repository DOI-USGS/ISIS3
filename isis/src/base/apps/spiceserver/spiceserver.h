#ifndef spiceserver_h
#define spiceserver_h

#include "Pvl.h"
#include "UserInterface.h"

namespace Isis{
  extern void spiceserver(UserInterface &ui, Pvl *log = nullptr);
}

#endif
