/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "BoxcarManager.h"

using namespace std;
namespace Isis {
  /**
   * Constructs a BoxcarManager object
   *
   * @param cube The cube this buffer will be associated with.
   *
   * @param boxSamples The number of samples in each boxcar buffer.
   *
   * @param boxLines The number of lines in each boxcar buffer.
   */
  BoxcarManager::BoxcarManager(const Isis::Cube &cube,
                               const int &boxSamples, const int &boxLines) :
    Isis::BufferManager(cube.sampleCount(), cube.lineCount(),
                        cube.bandCount(), boxSamples, boxLines, 1,
                        cube.pixelType()) {

    Isis::BufferManager::SetIncrements(1, 1, 1);
    int soff, loff, boff;
    soff = (int)((boxSamples - 1) / 2) * -1;
    loff = (int)((boxLines - 1) / 2) * -1;
    boff = 0;
    Isis::BufferManager::SetOffsets(soff, loff, boff);
  }
} // end namespace isis

