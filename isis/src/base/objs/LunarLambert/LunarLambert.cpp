/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <cmath>
#include "LunarLambert.h"

namespace Isis {
  LunarLambert::LunarLambert(Pvl &pvl) : PhotoModel(pvl) {
    PvlGroup &algo = pvl.findObject("PhotometricModel")
                     .findGroup("Algorithm", Pvl::Traverse);
    // Set default value
    SetPhotoL(1.0);

    // Get value from user
    if(algo.hasKeyword("L")) SetPhotoL(algo["L"]);
  }

  /**
   * Set the Lunar-Lambert function weight.  This is used to govern the
   * limb-darkening in the Lunar-Lambert photometric function.  Values of
   * the Lunar-Lambert weight generally fall in the range from 0.0
   * (Lambert function) to 1.0 (Lommel-Seeliger or "lunar" function).
   * There are no limits on the value of this parameter, but values far
   * outside the 0 to 1 range will not be very useful.
   *
   * @param l  Lunar-Lambert function weight, default is 1.0
   */
  void LunarLambert::SetPhotoL(const double l) {
    p_photoL = l;
  }

  double LunarLambert::PhotoModelAlgorithm(double phase, double incidence,
      double emission) {
    static double pht_lunlam;
    double incrad;
    double emarad;
    double munot;
    double mu;

    static double old_phase = -9999;
    static double old_incidence = -9999;
    static double old_emission= -9999;

    if (old_phase == phase && old_incidence == incidence && old_emission == emission) {
      return pht_lunlam;
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
      pht_lunlam = 0.0;
    }
//    else if(PhotoL() == 0.0) {
    else if(p_photoL == 0.0) {
      pht_lunlam = munot;
    }
//    else if(PhotoL() == 1.0) {
    else if(p_photoL == 1.0) {
      pht_lunlam = 2.0 * munot / (munot + mu);
    }
    else {
//      pht_lunlam = munot * ((1.0 - PhotoL()) + 2.0 *
//                            PhotoL() / (munot + mu));
      pht_lunlam = munot * ((1.0 - p_photoL) + 2.0 *
                            p_photoL / (munot + mu));
    }

    return pht_lunlam;
  }
}

extern "C" Isis::PhotoModel *LunarLambertPlugin(Isis::Pvl &pvl) {
  return new Isis::LunarLambert(pvl);
}
