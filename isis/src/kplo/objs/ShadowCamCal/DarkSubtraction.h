#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <QString>
#include <QFile>
#include <string.h>
#include "Cube.h"
#include "CubeAttribute.h"
#include "SpecialPixel.h"
#include "FileName.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "ProcessByLine.h"

#include "ShadowCamUtilities.h"
#include "ShadowCamConstants.h"

namespace Isis {
  extern void DarkSubtraction(QString slope_filename, QString intrcpt_filename, PvlGroup instrument, QString tmpCubeFileIn, QString tmpCubeFileOut, int lines);
}