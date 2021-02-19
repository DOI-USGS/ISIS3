/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <cmath>
#include "Minnaert.h"
#include "IException.h"

namespace Isis {
  Minnaert::Minnaert(Pvl &pvl) : PhotoModel(pvl) {
    PvlGroup &algo = pvl.findObject("PhotometricModel")
                     .findGroup("Algorithm", Pvl::Traverse);
    // Set default value
    SetPhotoK(1.0);
    // Get value from user
    if(algo.hasKeyword("K")) SetPhotoK(algo["K"]);
  }

  /**
    * Set the Minnaert function exponent.  This is used to govern the limb-
    * darkening in the Minnaert photometric function.  Values of the
    * Minnaert exponent generally fall in the range from 0.5 ("lunar-like",
    * almost no limb darkening) to 1.0 (Lambert function). 
    *
    * @param k  Minnaert function exponent, default is 1.0
    */
  void Minnaert::SetPhotoK(const double k) {
    p_photoK = k;
  }

  double Minnaert::PhotoModelAlgorithm(double phase, double incidence,
                                       double emission) {
    static double pht_minnaert;
    double incrad;
    double emarad;
    double munot;
    double mu;

    static double old_phase = -9999;
    static double old_incidence = -9999;
    static double old_emission= -9999;

    if (old_phase == phase && old_incidence == incidence && old_emission == emission) {
      return pht_minnaert;
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
      pht_minnaert = 0.0;
    }
//    else if(PhotoK() == 1.0) {
    else if(p_photoK == 1.0) {
      pht_minnaert = munot;
    }
    else {
//      pht_minnaert = munot * pow((munot * mu), (PhotoK() - 1.0));
      pht_minnaert = munot * pow((munot * mu), (p_photoK - 1.0));
    }

    return pht_minnaert;
  }
}

extern "C" Isis::PhotoModel *MinnaertPlugin(Isis::Pvl &pvl) {
  return new Isis::Minnaert(pvl);
}
