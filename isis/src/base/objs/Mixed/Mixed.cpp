#include <cmath>
#include "Mixed.h"
#include "SpecialPixel.h"
#include "iException.h"

namespace Isis {
  Mixed::Mixed (Pvl &pvl, PhotoModel &pmodel) : NormModel(pvl,pmodel) {
    PvlGroup &algorithm = pvl.FindObject("NormalizationModel").FindGroup("Algorithm",Pvl::Traverse);

    // Set default value
    SetNormIncref(0.0);
    SetNormIncmat(0.0);
    SetNormThresh(30.0);
    SetNormAlbedo(1.0);

    // Get value from user
    if (algorithm.HasKeyword("Incref")) {
      SetNormIncref(algorithm["Incref"]);
    }
    if (algorithm.HasKeyword("Incmat")) {
      SetNormIncmat(algorithm["Incmat"]);
    }

    if (algorithm.HasKeyword("Thresh")) {
      SetNormThresh(algorithm["Thresh"]);
    }

    if (algorithm.HasKeyword("Albedo")) {
      SetNormAlbedo(algorithm["Albedo"]);
    }

    // First-time setup
    // Calculate normalization at standard conditions
    // Turn off Hapke opposition effect
    GetPhotoModel()->SetStandardConditions(true);
    p_psurfref = GetPhotoModel()->CalcSurfAlbedo(0.0, p_normIncref, 0.0);
    double pprimeref = GetPhotoModel()->PhtTopder(0.0, p_normIncref, 0.0);

    if (p_psurfref == 0.0) {
      std::string err = "Divide by zero error";
      throw iException::Message(iException::Math, err, _FILEINFO_);
    }
    else {
      p_rhobar = p_normAlbedo / p_psurfref;
    }

    // Calculate brightness and topo derivative at matchpoint incidence
    p_psurfmatch = GetPhotoModel()->CalcSurfAlbedo(p_normIncmat, p_normIncmat, 0.0);
    p_pprimematch = GetPhotoModel()->PhtTopder(p_normIncmat, p_normIncmat, 0.0);

    // Calculate numerator of the stretch coeff. a; if it is very
    // large or small we haven't chosen a good reference state
    double arg = pow(p_psurfref,2.0) + pow(p_psurfmatch*pprimeref / std::max(1.0e-30,p_pprimematch),2.0);
    if ((arg < 1.0e-10) || (arg > 1.0e10)) {
      std::string err = "Bad reference state encountered";
      throw iException::Message(iException::Math, err, _FILEINFO_);
    }

    p_anum = sqrt(arg);
    GetPhotoModel()->SetStandardConditions(false);
  }

  void Mixed::NormModelAlgorithm (double phase, double incidence, 
      double emission, double dn, double &albedo, double &mult,
      double &base)
  {
    double psurf;
    double pprime;
    double aden;

    // code for scaling each pixel
    psurf = GetPhotoModel()->CalcSurfAlbedo(phase, incidence, emission);
    pprime = GetPhotoModel()->PhtTopder(phase, incidence, emission);
    double arg = pow(psurf,2.0) + pow(p_psurfmatch*pprime / std::max(1.0e-30, p_pprimematch), 2.0);
    aden = sqrt(std::max(1.0e-30,arg));

    // thresh is a parameter limiting how much we amplify the dns
    // shouldn't actually get a large amplification in this mode because 
    // of the growing pprime term in the denominator.

    if (aden > p_anum*p_normThresh) {
      albedo = NULL8;
    }
    else {
      albedo = dn * p_anum / aden + p_rhobar * (p_psurfref - p_anum / aden * psurf);
    }
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
  void Mixed::SetNormIncref (const double incref) {
    if (incref < 0.0 || incref >= 90.0) {
      std::string msg = "Invalid value of normalization incref [" +
          iString(incref) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    p_normIncref = incref;
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
  void Mixed::SetNormIncmat (const double incmat) {
    if (incmat < 0.0 || incmat >= 90.0) {
      std::string msg = "Invalid value of normalization incmat [" +
          iString(incmat) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    p_normIncmat = incmat;
  }

  /**
   * Set the normalization function parameter.
   *
   * @param albedo  Normalization function parameter, default
   *                is 0.0690507
   */
  void Mixed::SetNormAlbedo (const double albedo) {
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
  void Mixed::SetNormThresh (const double thresh) {
    p_normThresh = thresh;
  }
}

extern "C" Isis::NormModel *MixedPlugin (Isis::Pvl &pvl, Isis::PhotoModel &pmodel) {
  return new Isis::Mixed(pvl,pmodel);
}
