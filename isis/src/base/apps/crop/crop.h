#ifndef crop_h
#define crop_h

#include "Cube.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "LineManager.h"
#include "FileName.h"
#include "IException.h"
#include "Projection.h"
#include "AlphaCube.h"
#include "Table.h"
#include "SubArea.h"
#include "UserInterface.h"

namespace Isis {
  extern PvlGroup crop(Cube* cube, UserInterface &ui);
  extern PvlGroup crop(UserInterface &ui);
}

#endif
