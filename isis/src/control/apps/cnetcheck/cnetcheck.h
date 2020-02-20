#ifndef cnetcheck_h
#define cnetcheck_h

#include <QString>

#include "UserInterface.h"
#include "ControlNet.h"
#include "Pvl.h"
#include "FileList.h"

namespace Isis {
  extern QString cnetcheck(UserInterface &ui, Pvl *log=NULL);
  extern QString cnetcheck(ControlNet &innet, FileList &inlist, UserInterface &ui, Pvl *log=NULL);
}

#endif
