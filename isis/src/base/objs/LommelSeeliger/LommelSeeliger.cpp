#include <cmath>
#include "LommelSeeliger.h"

namespace Isis {
  double LommelSeeliger::PhotoModelAlgorithm (double phase, double incidence,
        double emission) {
    double pht_lomsel;
    double incrad = incidence * Isis::PI / 180.0;
    double emarad = emission * Isis::PI / 180.0;
    double munot = cos(incrad);
    double mu = cos(emarad);

    if (munot <= 0.0 || mu <= 0.0 || incidence == 90.0 ||
        emission == 90.0) {
      pht_lomsel = 0.0;
    } else {
      pht_lomsel = 2.0 * munot / (munot + mu);
    }

    return pht_lomsel;
  }
}

extern "C" Isis::PhotoModel *LommelSeeligerPlugin (Isis::Pvl &pvl) {
  return new Isis::LommelSeeliger(pvl);
}
