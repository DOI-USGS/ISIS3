#include "PositionSpice.h"

namespace Isis {

  PositionSpice::PositionSpice(int targetCode, int observerCode) : Position(targetCode, observerCode) {}

  std::vector<std::vector<double>> PositionSpice::SetEphemerisTime(double et) {
    double state[6];
    bool hasVelocity;
    double lt;

    // NOTE:  Prefer method getter access as some are virtualized and portray
    // the appropriate internal representation!!!
    computeStateVector(getAdjustedEphemerisTime(), Position::getTargetCode(), getObserverCode(),
                       "J2000", GetAberrationCorrection(), state, hasVelocity,
                       lt);

    // Set the internal state
    setStateVector(state, hasVelocity);
    setLightTime(lt);
    std::vector<std::vector<double>> ephemerisData;
    ephemerisData.push_back(p_coordinate);
    if (p_hasVelocity) {
      ephemerisData.push_back(p_velocity);
    }
    return ephemerisData;
  }
}
