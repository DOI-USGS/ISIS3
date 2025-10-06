#pragma once

#include <cmath>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <QString>
#include <QFile>
#include <QTemporaryDir>
#include <QStandardPaths>
#include <QFileInfo>
#include <sstream>
#include "IString.h"
#include <stdexcept>

#include "ShadowCamUtilities.h"
#include "ShadowCamConstants.h"
#include "BiasPixelSubtraction.h"
#include "GainCorrection.h"
#include "RadianceCoefficients.h"
#include "FlatFieldCorrection.h"
#include "DarkSubtraction.h"
#include "WriteCube.h"
#include "Cube.h"
#include "Progress.h"
#include "FileName.h"
#include "PvlKeyword.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "LineManager.h"
#include "FileName.h"
#include "IException.h"
#include "UserInterface.h"
#include "Preference.h"

namespace Isis {

  /**
   * @brief Main function to perform ShadowCam calibration
   * 
   * This function applies a series of corrections to the ShadowCam data, including bias pixel
   * removal, bias average subtraction, gain correction, dark subtraction, flat field correction,
   * and radiance correction. The results are written to a new calibrated cube.
   *
   * @param ui Reference to the user interface for input parameters.
   * @param log Optional log to record progress and errors.
   */
  extern void shadowcamcal(UserInterface &ui, Pvl *log = nullptr);

}
