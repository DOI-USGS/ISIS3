#include "PositionFactory.h"

namespace Isis {

  SpicePosition* PositionFactory::spicePosition(int targetCode, int observerCode) {
    return new SpicePosition(targetCode, observerCode);
  }

  SpacecraftPosition* PositionFactory::spacecraftPosition(int targetCode, int observerCode,
                     const LightTimeCorrectionState &ltState,
                     const Distance &radius) {
    return new SpacecraftPosition(targetCode, observerCode, ltState, radius);
  }

  Position* PositionFactory::positionSpice(int targetCode, int observerCode) {
    return new PositionSpice(targetCode, observerCode);
  }

}
