#ifndef cnetstats_h
#define cnetstats_h

#include <QString>

#include "ControlNet.h"
#include "Pvl.h"
#include "UserInterface.h"

namespace Isis{
  extern void cnetstats(ControlNet &innet, QString &serialNumFile, UserInterface &ui, Pvl *log=NULL);
  extern void cnetstats(UserInterface &ui, Pvl *log=NULL);
}

#endif
