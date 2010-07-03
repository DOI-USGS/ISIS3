#include <cmath>
#include "LunarLambertMcEwen.h"
#include "iException.h"

namespace Isis {
  LunarLambertMcEwen::LunarLambertMcEwen (Pvl &pvl) : PhotoModel(pvl) {
    p_photoM1 = -0.019;
    p_photoM2 = 0.000242;
    p_photoM3 = -0.00000146;

    double c30 = cos(30.0*Isis::PI/180.0);
    double xl30 = 1.0 + p_photoM1 * 30.0 + p_photoM2 * pow(30.0,2.0) +
                  p_photoM3 * pow(30.0,3);
    p_photoR30 = 2.0 * xl30 * c30 / (1.0 + c30) + (1.0 - xl30) * c30;
  }

  double LunarLambertMcEwen::PhotoModelAlgorithm (double phase, double incidence,
        double emission) {
    double pht_moonpr;
    double incrad = incidence * Isis::PI / 180.0;
    double emarad = emission * Isis::PI / 180.0;
    double munot = cos(incrad);
    double mu = cos(emarad);

    double xl = 1.0 + p_photoM1 * phase + p_photoM2 * pow(phase,2) +
                p_photoM3 * pow(phase,3);
    double r = 2.0 * xl * munot / (mu + munot) + (1.0 - xl) * munot;

    if (r <= 0.0) {
      pht_moonpr = 0.0;
    } else {
      pht_moonpr = p_photoR30 / r;
    }

    return pht_moonpr;
  }
}

extern "C" Isis::PhotoModel *LunarLambertMcEwenPlugin (Isis::Pvl &pvl) {
  return new Isis::LunarLambertMcEwen(pvl);
}
