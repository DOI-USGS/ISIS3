/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <cmath>
#include "LommelSeeliger.h"

namespace Isis {
  double LommelSeeliger::PhotoModelAlgorithm(double phase, double incidence,
      double emission) {
    static double pht_lomsel;
    double incrad;
    double emarad;
    double munot;
    double mu;

    static double old_phase = -9999;
    static double old_incidence = -9999;
    static double old_emission= -9999;

    if (old_phase == phase && old_incidence == incidence && old_emission == emission) {
      return pht_lomsel;
    }

    old_phase = phase;
    old_incidence = incidence;
    old_emission = emission;

    incrad = incidence * Isis::PI / 180.0;
    emarad = emission * Isis::PI / 180.0;
    munot = cos(incrad);
    mu = cos(emarad);

    if(munot <= 0.0 || mu <= 0.0 || incidence == 90.0 ||
        emission == 90.0) {
      pht_lomsel = 0.0;
    }
    else {
      pht_lomsel = 2.0 * munot / (munot + mu);
    }

    return pht_lomsel;
  }
}

extern "C" Isis::PhotoModel *LommelSeeligerPlugin(Isis::Pvl &pvl) {
  return new Isis::LommelSeeliger(pvl);
}
