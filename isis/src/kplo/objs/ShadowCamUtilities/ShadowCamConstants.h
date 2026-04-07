#ifndef ShadowCamConstants_h
#define ShadowCamConstants_h

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