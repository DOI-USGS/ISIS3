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
