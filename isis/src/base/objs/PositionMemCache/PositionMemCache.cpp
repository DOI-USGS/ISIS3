#include "PositionMemCache.h"

namespace Isis {

  PositionMemCache::PositionMemCache(int targetCode, int observerCode) : Position(targetCode, observerCode) {}

  void PositionMemCache::SetEphemerisTimeMemcache(double et) {
    // If the cache has only one position return it
    if(p_cache.size() == 1) {
      p_coordinate[0] = p_cache[0][0];
      p_coordinate[1] = p_cache[0][1];
      p_coordinate[2] = p_cache[0][2];
      if(p_hasVelocity) {
        p_velocity[0] = p_cacheVelocity[0][0];
        p_velocity[1] = p_cacheVelocity[0][1];
        p_velocity[2] = p_cacheVelocity[0][2];
      }
    }

    else {
      // Otherwise determine the interval to interpolate
      std::vector<double>::iterator pos;
      pos = upper_bound(p_cacheTime.begin(), p_cacheTime.end(), p_et);

      int cacheIndex;
      if(pos != p_cacheTime.end()) {
        cacheIndex = distance(p_cacheTime.begin(), pos);
        cacheIndex--;
      }
      else {
        cacheIndex = p_cacheTime.size() - 2;
      }

      if(cacheIndex < 0) cacheIndex = 0;

      // Interpolate the coordinate
      double mult = (p_et - p_cacheTime[cacheIndex]) /
                    (p_cacheTime[cacheIndex+1] - p_cacheTime[cacheIndex]);
      std::vector<double> p2 = p_cache[cacheIndex+1];
      std::vector<double> p1 = p_cache[cacheIndex];

      p_coordinate[0] = (p2[0] - p1[0]) * mult + p1[0];
      p_coordinate[1] = (p2[1] - p1[1]) * mult + p1[1];
      p_coordinate[2] = (p2[2] - p1[2]) * mult + p1[2];

      if(p_hasVelocity) {
        p2 = p_cacheVelocity[cacheIndex+1];
        p1 = p_cacheVelocity[cacheIndex];
        p_velocity[0] = (p2[0] - p1[0]) * mult + p1[0];
        p_velocity[1] = (p2[1] - p1[1]) * mult + p1[1];
        p_velocity[2] = (p2[2] - p1[2]) * mult + p1[2];
      }
    }
  }

  void PositionMemCache::addCacheCoordinate(std::vector<double> coordinate) {
    p_cache.push_back(coordinate);
  }

  void PositionMemCache::addCacheVelocity(std::vector<double> velocity) {
    p_cacheVelocity.push_back(velocity);
  }

  void PositionMemCache::addCacheTime(double time) {
    p_cacheTime.push_back(time);
  }

  bool PositionMemCache::getHasVelocity() {
    return p_hasVelocity;
  }
}
