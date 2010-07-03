#include <cmath>
#include "LunarLambert.h"

namespace Isis {
  LunarLambert::LunarLambert (Pvl &pvl) : PhotoModel(pvl) {
    PvlGroup &algo = pvl.FindObject("PhotometricModel")
                     .FindGroup("Algorithm",Pvl::Traverse);
    // Set default value
    SetPhotoL(1.0);

    // Get value from user
    if (algo.HasKeyword("L")) SetPhotoL(algo["L"]);
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
  void LunarLambert::SetPhotoL (const double l) {
    p_photoL = l;
  }

  double LunarLambert::PhotoModelAlgorithm (double phase, double incidence,
        double emission) {
    double pht_lunlam;
    double incrad = incidence * Isis::PI / 180.0;
    double emarad = emission * Isis::PI / 180.0;
    double munot = cos(incrad);
    double mu = cos(emarad);

    if (munot <= 0.0 || mu <= 0.0 || incidence == 90.0 ||
        emission == 90.0) {
      pht_lunlam = 0.0;
    } else if (p_photoL == 0.0) {
      pht_lunlam = munot;
    } else if (p_photoL == 1.0) {
      pht_lunlam = 2.0 * munot / (munot + mu);
    } else {
      pht_lunlam = munot * ((1.0 - p_photoL) + 2.0 *
                   p_photoL / (munot + mu));
    }

    return pht_lunlam;
  }
}

extern "C" Isis::PhotoModel *LunarLambertPlugin (Isis::Pvl &pvl) {
  return new Isis::LunarLambert(pvl);
}
