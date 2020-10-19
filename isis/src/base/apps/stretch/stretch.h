#ifndef stretch_app_h
#define stretch_app_h

#include "TextFile.h"
#include "Statistics.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "Stretch.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "UserInterface.h"

namespace Isis {
  extern void stretch(UserInterface &ui, Pvl *log=nullptr);

  extern void stretch(Cube *inCube, UserInterface &ui, Pvl *log=nullptr);
}

#endif
