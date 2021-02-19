/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "TileManager.h"

#include "IException.h"

using namespace std;
namespace Isis {

  /**
   * Constructs a TileManager object
   *
   * @param cube  The cube this buffer will be assiciated with.
   *
   * @param bufNumSamples  The number of samples in each tile buffer.
   *                       Defaults to 128
   *
   * @param bufNumLines The number of lines in each tile buffer. Defaults to 128
   *
   */
  TileManager::TileManager(const Isis::Cube &cube,
                           const int &bufNumSamples, const int &bufNumLines) :
    Isis::BufferManager(cube.sampleCount(), cube.lineCount(), cube.bandCount(),
                        bufNumSamples, bufNumLines, 1,
                        cube.pixelType()) {

    p_numSampTiles = (cube.sampleCount() - 1) / bufNumSamples + 1;
    p_numLineTiles = (cube.lineCount() - 1) / bufNumLines + 1;
  }

  /**
   * Sets the current tile as requested
   *
   * @param tile  The tile number within a band. This number starts with the
   *              upper left corner of the cube and proceedes across the samples
   *              then down the lines. The upper left tile of each band is always
   *              tile one (1) in band (n).
   *
   * @param band The band number within the cube. The first band in a cube is
   *             always one (1).
   *
   * @return bool
   *
   * @throws Isis::IException::Programmer - invalid argument value
   */
  bool TileManager::SetTile(const int tile, const int band) {
    if(tile < 1) {
      string message = "Invalid value for argument [tile]";
      throw IException(IException::Programmer, message, _FILEINFO_);
    }

    if(band < 1) {
      string message = "Invalid value for argument [band]";
      throw IException(IException::Programmer, message, _FILEINFO_);
    }

    int map = (band - 1) * (p_numSampTiles * p_numLineTiles) + tile - 1;

    return setpos(map);
  }
} // end namespace isis

