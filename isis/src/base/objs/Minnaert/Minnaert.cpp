#include <cmath>
#include "Minnaert.h"
#include "iException.h"

namespace Isis {
  Minnaert::Minnaert (Pvl &pvl) : PhotoModel(pvl) {
    PvlGroup &algo = pvl.FindObject("PhotometricModel")
                     .FindGroup("Algorithm",Pvl::Traverse);
    // Set default value
    SetPhotoK(1.0);
    // Get value from user
    if (algo.HasKeyword("K")) SetPhotoK(algo["K"]);
  }

 /**
   * Set the Minnaert function exponent.  This is used to govern the limb-
   * darkening in the Minnaert photometric function.  Values of the
   * Minnaert exponent generally fall in the range from 0.5 ("lunar-like",
   * almost no limb darkening) to 1.0 (Lambert function).  This
   * parameter is limited to values that are >=0.
   *
   * @param k  Minnaert function exponent, default is 1.0
   */
  void Minnaert::SetPhotoK (const double k) {
    if (k < 0.0) {
      std::string msg = "Invalid value of Minnaert k [" +
          iString(k) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    p_photoK = k;
  }

  double Minnaert::PhotoModelAlgorithm (double phase, double incidence,
        double emission) {
    double pht_minnaert;
    double incrad = incidence * Isis::PI / 180.0;
    double emarad = emission * Isis::PI / 180.0;
    double munot = cos(incrad);
    double mu = cos(emarad);

    if (munot <= 0.0 || mu <= 0.0 || incidence == 90.0 ||
        emission == 90.0) {
      pht_minnaert = 0.0;
    } else if (p_photoK == 1.0) {
      pht_minnaert = munot;
    } else {
      pht_minnaert = munot * pow((munot*mu),(p_photoK-1.0));
    }

    return pht_minnaert;
  }
}

extern "C" Isis::PhotoModel *MinnaertPlugin (Isis::Pvl &pvl) {
  return new Isis::Minnaert(pvl);
}
