#include <cmath>
#include "Lambert.h"

namespace Isis {
  double Lambert::PhotoModelAlgorithm(double phase, double incidence,
                                      double emission) {
    static double pht_lambert;
    double incrad;
    double munot;

    static double old_phase = -9999;
    static double old_incidence = -9999;
    static double old_emission= -9999;

    if (old_phase == phase && old_incidence == incidence && old_emission == emission) {
      return pht_lambert;
    }

    old_phase = phase;
    old_incidence = incidence;
    old_emission = emission;

    incrad = incidence * Isis::PI / 180.0;
    munot = cos(incrad);

    if(munot <= 0.0 || incidence == 90.0) {
      pht_lambert = 0.0;
    }
    else {
      pht_lambert = munot;
    }

    return pht_lambert;
  }
}

extern "C" Isis::PhotoModel *LambertPlugin(Isis::Pvl &pvl) {
  return new Isis::Lambert(pvl);
}
