#ifndef PositionBuilder_h
#define PositionBuilder_h

#include "Position.h"
#include "PositionSpice.h"
#include "PositionMemCache.h"
#include "SpicePosition.h"
#include "SpacecraftPosition.h"
#include "LightTimeCorrectionState.h"
#include "Distance.h"

namespace Isis {

  class PositionFactory {

    public:
      PositionFactory();

      ~PositionFactory();

      static SpicePosition* spicePosition(int targetCode, int observerCode);

      static SpacecraftPosition* spacecraftPosition(int targetCode, int observerCode,
                              const LightTimeCorrectionState &ltState,
                              const Distance &radius);

      static Position* positionSpice(int targetCode, int observerCode);

      static Position* fromSpiceToMemCache(PositionSpice *positionSpice, int startTime, int endTime, int size);
  };
}

#endif
