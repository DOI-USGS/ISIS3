#pragma once

#include <QString>
#include <QFile>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include "Cube.h"
#include "CubeAttribute.h"
#include "SpecialPixel.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "FileName.h"
#include "IException.h"
#include "Preference.h"
#include "ProcessByLine.h"
#include "LineManager.h"
#include "UserInterface.h"

#include "ShadowCamUtilities.h"
#include "ShadowCamConstants.h"

namespace Isis {
  extern void WriteCube(QString& cubeFileIn, QString* cubeFileOut, UserInterface &ui, bool remvBias, int lines);
}