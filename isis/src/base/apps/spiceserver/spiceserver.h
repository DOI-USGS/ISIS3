#ifndef spiceserver_h
#define spiceserver_h

#include "Pvl.h"
#include "UserInterface.h"
#include "NaifContext.h"

namespace Isis{
  extern void spiceserver(NaifContextPtr naif, UserInterface &ui, Pvl *log = nullptr);
}

#endif
