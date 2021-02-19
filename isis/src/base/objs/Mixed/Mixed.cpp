/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <cmath>
#include "Mixed.h"
#include "SpecialPixel.h"
#include "IException.h"

namespace Isis {
  Mixed::Mixed(Pvl &pvl, PhotoModel &pmodel) : NormModel(pvl, pmodel) {
    PvlGroup &algorithm = pvl.findObject("NormalizationModel").findGroup("Algorithm", Pvl::Traverse);

    // Set default value
    SetNormPharef(0.0);
    SetNormIncref(0.0);
    SetNormEmaref(0.0);
    SetNormPhamat(0.0);
    SetNormIncmat(0.0);
    SetNormEmamat(0.0);
    SetNormThresh(30.0);
    SetNormAlbedo(1.0);

    // Get value from user
    if(algorithm.hasKeyword("Pharef")) {
      SetNormPharef(algorithm["Pharef"]);
    }

    if(algorithm.hasKeyword("Incref")) {
      SetNormIncref(algorithm["Incref"]);
    }

    if(algorithm.hasKeyword("Emaref")) {
      SetNormEmaref(algorithm["Emaref"]);
    }

    if(algorithm.hasKeyword("Incmat")) {
      SetNormIncmat(algorithm["Incmat"]);
    }

    if(algorithm.hasKeyword("Phamat")) {
      SetNormPhamat(algorithm["Phamat"]);
    } else {
      p_normPhamat = p_normIncmat;
    }

    if(algorithm.hasKeyword("Emamat")) {
      SetNormEmamat(algorithm["Emamat"]);
    }

    if(algorithm.hasKeyword("Thresh")) {
      SetNormThresh(algorithm["Thresh"]);
    }

    if(algorithm.hasKeyword("Albedo")) {
      SetNormAlbedo(algorithm["Albedo"]);
    }

    // First-time setup
    // Calculate normalization at standard conditions
    // Turn off Hapke opposition effect
    GetPhotoModel()->SetStandardConditions(true);
    p_psurfref = GetPhotoModel()->CalcSurfAlbedo(p_normPharef, p_normIncref, p_normEmaref);
    double pprimeref = GetPhotoModel()->PhtTopder(p_normPharef, p_normIncref, p_normEmaref);

    if(p_psurfref == 0.0) {
      std::string err = "Divide by zero error";
      throw IException(IException::Unknown, err, _FILEINFO_);
    }
    else {
      p_rhobar = p_normAlbedo / p_psurfref;
    }

    // Calculate brightness and topo derivative at matchpoint incidence
    p_psurfmatch = GetPhotoModel()->CalcSurfAlbedo(p_normPhamat, p_normIncmat, p_normEmamat);
    p_pprimematch = GetPhotoModel()->PhtTopder(p_normPhamat, p_normIncmat, p_normEmamat);

    // Calculate numerator of the stretch coeff. a; if it is very
    // large or small we haven't chosen a good reference state
    double arg = pow(p_psurfref, 2.0) + pow(p_psurfmatch * pprimeref / std::max(1.0e-30, p_pprimematch), 2.0);
    if((arg < 1.0e-10) || (arg > 1.0e10)) {
      std::string err = "Bad reference state encountered";
      throw IException(IException::Unknown, err, _FILEINFO_);
    }

    p_anum = sqrt(arg);
    GetPhotoModel()->SetStandardConditions(false);
  }

  void Mixed::NormModelAlgorithm(double phase, double incidence, double emission,
                                 double demincidence, double dememission, double dn,
                                 double &albedo, double &mult, double &base) {
    static double psurf;
    static double pprime;
    static double aden;

    static double old_phase = -9999;
    static double old_incidence = -9999;
    static double old_emission = -9999;
    static double old_demincidence = -9999;
    static double old_dememission = -9999;

    if (old_phase != phase || old_incidence != incidence || old_emission != emission ||
        old_demincidence != demincidence || old_dememission != dememission) {

      // code for scaling each pixel
      psurf = GetPhotoModel()->CalcSurfAlbedo(phase, demincidence, dememission);
      pprime = GetPhotoModel()->PhtTopder(phase, demincidence, dememission);
      double arg = pow(psurf, 2.0) + pow(p_psurfmatch * pprime / std::max(1.0e-30, p_pprimematch), 2.0);
      aden = sqrt(std::max(1.0e-30, arg));

      old_phase = phase;
      old_incidence = incidence;
      old_emission = emission;
      old_demincidence = demincidence;
      old_dememission = dememission;
    }

    // thresh is a parameter limiting how much we amplify the dns
    // shouldn't actually get a large amplification in this mode because
    // of the growing pprime term in the denominator.

    if(aden > p_anum * p_normThresh) {
      albedo = NULL8;
    }
    else {
      albedo = dn * p_anum / aden + p_rhobar * (p_psurfref - p_anum / aden * psurf);
    }
  }

  /**
    * Set the normalization function parameter. This is the
    * reference phase angle to which the image photometry will
    * be normalized. This parameter is limited to values that are
    * >=0 and <180.
    *
    * @param pharef  Normalization function parameter, default
    *                is 0.0
    */
  void Mixed::SetNormPharef(const double pharef) {
    if(pharef < 0.0 || pharef >= 180.0) {
      std::string msg = "Invalid value of normalization pharef [" +
                        IString(pharef) + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    p_normPharef = pharef;
  }

  /**
    * Set the normalization function parameter. This is the
    * reference incidence angle to which the image photometry will
    * be normalized. This parameter is limited to values that are
    * >=0 and <90.
    *
    * @param incref  Normalization function parameter, default
    *                is 0.0
    */
  void Mixed::SetNormIncref(const double incref) {
    if(incref < 0.0 || incref >= 90.0) {
      std::string msg = "Invalid value of normalization incref [" +
                        IString(incref) + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    p_normIncref = incref;
  }

  /**
    * Set the normalization function parameter. This is the
    * reference emission angle to which the image photometry will
    * be normalized. This parameter is limited to values that are
    * >=0 and <90.
    *
    * @param emaref  Normalization function parameter, default
    *                is 0.0
    */
  void Mixed::SetNormEmaref(const double emaref) {
    if(emaref < 0.0 || emaref >= 90.0) {
      std::string msg = "Invalid value of normalization emaref [" +
                        IString(emaref) + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    p_normEmaref = emaref;
  }

  /**
   * Set the normalization function parameter. The image will be normalized
   * so that albedo variations are constant for small phase angles and
   * topographic shading is constant for large phase angles. The transition
   * from albedo normalization to phase normalization occurs around
   * the phase angle represented by this parameter. This
   * parameter is limited to values that are >=0 and <180.
   *
   * @param phamat  Normalization function parameter
   */
  void Mixed::SetNormPhamat(const double phamat) {
    if(phamat < 0.0 || phamat >= 180.0) {
      std::string msg = "Invalid value of normalization phamat [" +
                        IString(phamat) + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    p_normPhamat = phamat;
  }

  /**
   * Set the normalization function parameter. The image will be normalized
   * so that albedo variations are constant for small incidence angles and
   * topographic shading is constant for large incidence angles. The transition
   * from albedo normalization to incidence normalization occurs around
   * the incidence angle represented by this parameter. This
   * parameter is limited to values that are >=0 and <90.
   *
   * @param incmat  Normalization function parameter
   */
  void Mixed::SetNormIncmat(const double incmat) {
    if(incmat < 0.0 || incmat >= 90.0) {
      std::string msg = "Invalid value of normalization incmat [" +
                        IString(incmat) + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    p_normIncmat = incmat;
  }

  /**
   * Set the normalization function parameter. The image will be normalized
   * so that albedo variations are constant for small emission angles and
   * topographic shading is constant for large emission angles. The transition
   * from albedo normalization to emission normalization occurs around
   * the emission angle represented by this parameter. This
   * parameter is limited to values that are >=0 and <90.
   *
   * @param emamat  Normalization function parameter
   */
  void Mixed::SetNormEmamat(const double emamat) {
    if(emamat < 0.0 || emamat >= 90.0) {
      std::string msg = "Invalid value of normalization emamat [" +
                        IString(emamat) + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    p_normEmamat = emamat;
  }

  /**
   * Set the normalization function parameter.
   *
   * @param albedo  Normalization function parameter, default
   *                is 0.0690507
   */
  void Mixed::SetNormAlbedo(const double albedo) {
    p_normAlbedo = albedo;
  }

  /**
   * Set the normalization function parameter.
   * It is used to amplify variations in the input image in regions
   * of small incidence angle where the shading in the input
   * image is weak. This parameter sets the upper limit on the
   * amount of amplification that will be attempted. If it is
   * set too low, low incidence areas of the image may appear
   * bland. If it is set too high, then low incidence areas of
   * the image may contain amplified noise rather than useful
   * shading information.
   *
   * @param thresh  Normalization function parameter, default
   *                is 30.0
   */
  void Mixed::SetNormThresh(const double thresh) {
    p_normThresh = thresh;
  }
}

extern "C" Isis::NormModel *MixedPlugin(Isis::Pvl &pvl, Isis::PhotoModel &pmodel) {
  return new Isis::Mixed(pvl, pmodel);
}
