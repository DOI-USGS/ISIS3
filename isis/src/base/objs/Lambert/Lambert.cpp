#include <cmath>
#include "Lambert.h"

namespace Isis {
  double Lambert::PhotoModelAlgorithm (double phase, double incidence,
        double emission) {
    double pht_lambert;
    double incrad = incidence * Isis::PI / 180.0;
    double munot = cos(incrad);

    if (munot <= 0.0 || incidence == 90.0) {
      pht_lambert = 0.0;
    } else {
      pht_lambert = munot;
    }

    return pht_lambert;
  }
}

extern "C" Isis::PhotoModel *LambertPlugin (Isis::Pvl &pvl) {
  return new Isis::Lambert(pvl);
}
