/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <cmath>
#include "NoNormalization.h"
#include "SpecialPixel.h"
#include "IException.h"

namespace Isis {
  NoNormalization::NoNormalization(Pvl &pvl, PhotoModel &pmodel) :
    NormModel(pvl, pmodel) {
  }

  void NoNormalization::NormModelAlgorithm(double phase, double incidence, double emission,
      double demincidence, double dememission, double dn, double &albedo, double &mult,
      double &base) {
    // apply the photometric correction
    albedo = GetPhotoModel()->CalcSurfAlbedo(phase, demincidence, dememission);
  }
}

extern "C" Isis::NormModel *NoNormalizationPlugin(Isis::Pvl &pvl, Isis::PhotoModel &pmodel) {
  return new Isis::NoNormalization(pvl, pmodel);
}
