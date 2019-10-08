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

  Position* PositionFactory::fromSpiceToMemCache(PositionSpice *positionSpice, int startTime, int endTime, int size) {
    PositionMemCache *positionMemCache = new PositionMemCache(positionSpice->getTargetCode(), positionSpice->getObserverCode());
    std::vector<std::vector<double>> ephemerisData;
    std::vector<double> cacheTimes;

    cacheTimes = positionSpice->LoadTimeCache(startTime, endTime, size);

    // Loop and load the cache
    for(int i = 0; i < size; i++) {
      double et = cacheTimes[i];
      ephemerisData = positionSpice->SetEphemerisTime(et);
      positionMemCache->addCacheCoordinate(ephemerisData.at(0));
      positionMemCache->addCacheTime(et);
      if (positionMemCache->getHasVelocity()) {
        positionMemCache->addCacheVelocity(ephemerisData.at(1));
      }
    }

    return positionMemCache;
  }

}
