#ifndef shadowcamconstants_h
#define shadowcamconstants_h
/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

namespace Isis {
namespace ShadowCam {
  constexpr int TERMS = 6;
  constexpr int DATA_OFFSET = 65536;
  constexpr int SHC_BYTES = 4096;

  constexpr int SHC_AFE_WIDTH = 524;
  constexpr int SHC_CHANNELS = 6;
  constexpr int SHC_SCENE = 512;
  constexpr int SHC_SCENE_OFFSET = 10;
  constexpr int SHC_BANDS = 1;
}
}

#endif