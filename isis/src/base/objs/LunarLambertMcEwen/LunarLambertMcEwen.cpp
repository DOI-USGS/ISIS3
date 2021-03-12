/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <cmath>
#include "LunarLambertMcEwen.h"
#include "IException.h"

namespace Isis {
  LunarLambertMcEwen::LunarLambertMcEwen(Pvl &pvl) : PhotoModel(pvl) {
    p_photoM1 = -0.019;
    p_photoM2 = 0.000242;
    p_photoM3 = -0.00000146;

    double c30 = cos(30.0 * Isis::PI / 180.0);
    double xl30 = 1.0 + p_photoM1 * 30.0 + p_photoM2 * pow(30.0, 2.0) +
                  p_photoM3 * pow(30.0, 3);
    p_photoR30 = 2.0 * xl30 * c30 / (1.0 + c30) + (1.0 - xl30) * c30;
  }

  double LunarLambertMcEwen::PhotoModelAlgorithm(double phase, double incidence,
      double emission) {
    static double pht_moonpr;
    double incrad;
    double emarad;
    double munot;
    double mu;

    static double old_phase = -9999;
    static double old_incidence = -9999;
    static double old_emission= -9999;

    if (old_phase == phase && old_incidence == incidence && old_emission == emission) {
      return pht_moonpr;
    }

    old_phase = phase;
    old_incidence = incidence;
    old_emission = emission;

    incrad = incidence * Isis::PI / 180.0;
    emarad = emission * Isis::PI / 180.0;
    munot = cos(incrad);
    mu = cos(emarad);

    double xl = 1.0 + p_photoM1 * phase + p_photoM2 * pow(phase, 2) +
                p_photoM3 * pow(phase, 3);
    double r = 2.0 * xl * munot / (mu + munot) + (1.0 - xl) * munot;

    if(r <= 0.0) {
      pht_moonpr = 0.0;
    }
    else {
      pht_moonpr = p_photoR30 / r;
    }

    return pht_moonpr;
  }
}

extern "C" Isis::PhotoModel *LunarLambertMcEwenPlugin(Isis::Pvl &pvl) {
  return new Isis::LunarLambertMcEwen(pvl);
}
