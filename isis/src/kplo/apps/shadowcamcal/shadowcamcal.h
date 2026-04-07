#ifndef shadowcamcal_h
#define shadowcamcal_h

#include <QString>

#include "Cube.h"
#include "UserInterface.h"

namespace Isis {

  /**
   * @brief Performs ShadowCam calibration
   *
   * This function applies a series of corrections to the ShadowCam data, including bias pixel
   * removal, bias average subtraction, gain correction, dark subtraction, flat field correction,
   * and radiance correction. The results are written to a new calibrated cube.
   *
   * @param ui The user interface for input parameters.
   *
   * @internal
   *    @history 2026-03-30 Cordell Michaud - Removed unused log parameter and refactored.
   */
  extern void shadowcamcal(UserInterface &ui);

  /**
   * @brief Performs ShadowCam calibration
   *
   * This function applies a series of corrections to the ShadowCam data, including bias pixel
   * removal, bias average subtraction, gain correction, dark subtraction, flat field correction,
   * and radiance correction. The results are written to a new calibrated cube.
   *
   * @param inCube The input cube to be calibrated.
   * @param ui The user interface for input parameters.
   *
   * @internal
   *    @history 2026-03-30 Cordell Michaud - Added this new overload for better testability and refactored.
   */
  extern void shadowcamcal(Cube *inCube, UserInterface &ui);

  namespace ShadowCam {
    void WriteCube(const QString &cubeFileIn, const QString &cubeFileOut, UserInterface &ui, bool removeBias, int lines);
  }
}

#endif