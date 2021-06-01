#ifndef cnetstats_h
#define cnetstats_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QString>

#include "ControlNet.h"
#include "Pvl.h"
#include "UserInterface.h"

namespace Isis{
  extern void cnetstats(ControlNet &innet, QString &serialNumFile, UserInterface &ui, Pvl *log=NULL);
  extern void cnetstats(UserInterface &ui, Pvl *log=NULL);
}

#endif
