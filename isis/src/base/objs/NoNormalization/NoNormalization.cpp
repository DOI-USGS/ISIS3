#include <cmath>
#include "NoNormalization.h"
#include "SpecialPixel.h"
#include "iException.h"

namespace Isis {
  NoNormalization::NoNormalization(Pvl &pvl, PhotoModel &pmodel) :
    NormModel(pvl, pmodel) {
  }

  void NoNormalization::NormModelAlgorithm(double phase, double incidence,
      double emission, double dn, double &albedo, double &mult,
      double &base) {
    albedo = dn;
  }
}

extern "C" Isis::NormModel *NoNormalizationPlugin(Isis::Pvl &pvl, Isis::PhotoModel &pmodel) {
  return new Isis::NoNormalization(pvl, pmodel);
}
