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

    /**
     * @brief Subtracts dark current from the input cube.
     *
     * @param slopeFilename The CSV filename containing the slope coefficients for dark current subtraction.
     * @param interceptFilename The CSV filenames containing the intercept coefficients for dark current subtraction.
     * @param instrumentGroup The instrument group from the cube label.
     * @param cubeFileIn The input cube file path.
     * @param cubeFileOut The output cube file path.
     * @param lines The number of lines in the cube.
     *
     * @internal
     *    @history 2026-03-30 Cordell Michaud - Renamed this function from DarkSubtraction, moved it to shadowcamcal and
     *                                          refactored it.
     */
    void SubtractDark(const QString &slopeFilename, const QString &interceptFilename, const PvlGroup &instrumentGroup,
      const QString &cubeFileIn, const QString &cubeFileOut, int lines);

    /**
     * @brief Applies the flatfield correction to the input cube.
     * 
     * @param flatfieldCoeffFilename  The CSV filename containing the flatfield coefficients for flatfield correction.
     * @param instrumentGroup The instrument group from the cube label.
     * @param cubeFileIn The input cube file path.
     * @param cubeFileOut The output cube file path.
     * @param lines The number of lines in the cube.
     *
     * @internal
     *    @history 2026-03-31 Cordell Michaud - Renamed this function from FlatFieldCorrection, moved it to
     *                                          shadowcamcal, and refactored it.
     */
    void CorrectFlatfield(const QString &flatfieldCoeffFilename, const PvlGroup &instrumentGroup,
      const QString &cubeFileIn, const QString &cubeFileOut, int lines);

    /**
     * @brief Applies the gain correction to the input cube.
     *
     * @param cubeFileIn The input cube file path.
     * @param cubeFileOut The output cube file path.
     * @param gainCoeffFilename The CSV filename containing the gain coefficients for gain correction.
     * @param instrumentGroup The instrument group from the cube label.
     * @param lines The number of lines in the cube.
     *
     * @internal
     *    @history 2026-03-31 Cordell Michaud - Renamed this function from GainCorrection, moved it to shadowcamcal, and
     *                                          refactored it.
     */
    void CorrectGain(const QString &cubeFileIn, const QString &cubeFileOut, const QString &gainCoeffFilename,
      const PvlGroup &instrumentGroup, int lines);

    /**
     * @brief Applies radiance correction to the input cube.
     *
     * @param radianceCoeffFilename The CSV filename containing the radiance coefficients for radiance correction.
     * @param instrumentGroup The instrument group from the cube label.
     * @param cubeFileIn The input cube file path.
     * @param cubeFileOut The output cube file path.
     * @param lines The number of lines in the cube.
     *
     * @internal
     *    @history 2026-03-31 Cordell Michaud - Renamed this function from RadianceCoefficients, moved it to shadowcamcal,
     *                                          and refactored it.
     */
    void CorrectRadiance(const QString &radianceCoeffFilename, const PvlGroup &instrumentGroup, const QString &cubeFileIn,
      const QString &cubeFileOut, int lines);
  }
}

#endif