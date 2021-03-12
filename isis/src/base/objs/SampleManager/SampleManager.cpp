
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "SampleManager.h"

#include "IException.h"
#include "IString.h"

using namespace std;
namespace Isis {

  /**
   * Constructs a SampleManager object
   *
   * @param cube The cube this buffer manager will be associated with.
   *
   * @param reverse Modifies the order of progression
   *             SampleManager takes through the cube.  By
   *             default, progresses samples first then bands. If
   *             reverse = true, then the buffer progresses bands
   *             first, then samples.
   */

  SampleManager::SampleManager(const Isis::Cube &cube, const bool reverse) :
    Isis::BufferManager(cube.sampleCount(), cube.lineCount(),
                        cube.bandCount(), 1, cube.lineCount(), 1,
                        cube.pixelType(), reverse) {
  }

  /**
   * Positions the buffer at the requested line and returns a status indicator
   * if the set was succesful or not
   *
   * @param sample The sample number within a band (1-based).
   * @param band The band number within the cube (1-based). Defaults to 1
   *
   * @return bool Status indicator of the set being successful or not
   */

  bool SampleManager::SetSample(const int sample, const int band) {
    if(sample < 1) {
      string message = "Invalid value for argument [sample]";
      throw IException(IException::Programmer, message, _FILEINFO_);
    }

    if(band < 1) {
      string message = "Invalid value for argument [band]";
      throw IException(IException::Programmer, message, _FILEINFO_);
    }

    int map = (band - 1) * MaxSamples() + sample - 1;
    return setpos(map);
  }

} // end namespace isis
