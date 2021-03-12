/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "BandManager.h"
#include "IException.h"

using namespace std;
namespace Isis {

  /**
   * Constructs a BandManager object
   *
   * @param cube The cube this buffer manager will be associated with.
   *
   * @param reverse Modifies the order of progression BandManager
   *             takes through the cube.  By default, progresses
   *             samples first then lines. If reverse = true, then
   *             the buffer progresses lines first, then samples.
   */

  BandManager::BandManager(const Isis::Cube &cube, const bool reverse) :
    Isis::BufferManager(cube.sampleCount(), cube.lineCount(),
                        cube.bandCount(), 1, 1, cube.bandCount(),
                        cube.pixelType(), reverse) {
  }

  /**
   * Positions the buffer at the requested line and returns a status indicator
   * if the set was succesful or not
   *
   * @param sample The sample number within a band (1-based).
   * @param line The line number within a band (1-based). Defaults to 1
   *
   * @return bool Status indicator of the set being successful or not
   */

  bool BandManager::SetBand(const int sample, const int line) {
    if(sample < 1) {
      string message = "Invalid value for argument [sample]";
      throw IException(IException::Programmer, message, _FILEINFO_);
    }

    if(line < 1) {
      string message = "Invalid value for argument [line]";
      throw IException(IException::Programmer, message, _FILEINFO_);
    }

    int map = (line - 1) * MaxBands() + sample - 1;
    return setpos(map);
  }

} // end namespace isis
