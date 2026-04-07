#ifndef shadowcamcal_h
#define shadowcamcal_h

#include <QString>

#include "Cube.h"
#include "PvlGroup.h"
#include "UserInterface.h"

namespace Isis {

  /**
   * @brief Performs ShadowCam calibration
   *
   * Applies a series of corrections to the ShadowCam data, including bias pixel removal, bias average subtraction,
   * gain correction, dark subtraction, flat field correction, and radiance correction. The results are written to a
   * new calibrated cube.
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
   * Applies a series of corrections to the ShadowCam data, including bias pixel removal, bias average subtraction,
   * gain correction, dark subtraction, flat field correction, and radiance correction. The results are written to a
   * new calibrated cube.
   *
   * @param inCube The input cube to be calibrated.
   * @param ui The user interface for input parameters.
   *
   * @internal
   *    @history 2026-03-30 Cordell Michaud - Added this new overload for better testability and refactored.
   */
  extern void shadowcamcal(Cube *inCube, UserInterface &ui);

  namespace ShadowCam {
    /**
     * @brief Write the input cube to the output cube, removing bias pixel columns if specified.
     *
     * @param cubeFileIn The input cube file path.
     * @param cubeFileOut The output cube file path.
     * @param ui The user interface for input parameters.
     * @param removeBias Whether to remove bias pixels.
     * @param lines The number of lines in the cube.
     *
     * @internal
     *    @history 2026-03-30 Cordell Michaud - Moved this function to shadowcamcal and refactored it.
     */
    void WriteCube(const QString &cubeFileIn, const QString &cubeFileOut, UserInterface &ui, bool removeBias,
      int lines);

    /**
     * @brief Subtracts per-channel bias pixel averages from each sample.
     *
     * @param useMedian Whether to use the median as an average instead of the mean.
     * @param tempCubeFileIn The input cube file path.
     * @param tempCubeFileOut The output cube file path.
     * @param lines The number of lines in the cube.
     *
     * @internal
     *    @history 2026-03-30 Cordell Michaud - Renamed this function from BiasPixelSubtraction, moved it to
     *                                          shadowcamcal, and refactored it.
     */
    void SubtractBiasPixels(bool useMedian, const QString &tempCubeFileIn, const QString &tempCubeFileOut, int lines);
  }
}

#endif