#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <memory>
#include <string.h>
#include "Cube.h"
#include "CubeAttribute.h"
#include "LineManager.h"
#include "SpecialPixel.h"
#include "ProcessByLine.h"
#include "ShadowCamUtilities.h"
#include "ShadowCamConstants.h"

namespace Isis {
  extern void BiasPixelSubtraction(bool use_median, QString tmpCubeFileIn, QString tmpCubeFileOut, int lines);
}
