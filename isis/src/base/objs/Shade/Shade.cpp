/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Shade.h"
#include "IException.h"


namespace Isis {
  Shade::Shade(Pvl &pvl, PhotoModel &pmodel) : NormModel(pvl, pmodel) {
    PvlGroup &algorithm = pvl.findObject("NormalizationModel").findGroup("Algorithm", Pvl::Traverse);

    SetNormPharef(0.0);
    SetNormIncref(0.0);
    SetNormEmaref(0.0);
    SetNormAlbedo(1.0);

    if(algorithm.hasKeyword("Incref")) {
      SetNormIncref(algorithm["Incref"]);
    }

    if(algorithm.hasKeyword("Pharef")) {
      SetNormPharef(algorithm["Pharef"]);
    }
    else {
      p_normPharef = p_normIncref;
    }

    if(algorithm.hasKeyword("Emaref")) {
      SetNormEmaref(algorithm["Emaref"]);
    }

    if(algorithm.hasKeyword("Albedo")) {
      SetNormAlbedo(algorithm["Albedo"]);
    }
  }

  void Shade::NormModelAlgorithm(double phase, double incidence, double emission,
                                 double demincidence, double dememission, double dn,
                                 double &albedo, double &mult, double &base) {
    double rho;
    double psurfref;

    // Calculate normalization at standard conditions
    GetPhotoModel()->SetStandardConditions(true);
    psurfref = GetPhotoModel()->CalcSurfAlbedo(p_normPharef, p_normIncref, p_normEmaref);
    GetPhotoModel()->SetStandardConditions(false);

    if(psurfref == 0.0) {
      std::string msg = "Divide by zero error";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    rho = p_normAlbedo / psurfref;

    albedo = rho * GetPhotoModel()->CalcSurfAlbedo(phase, demincidence, dememission);
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
  void Shade::SetNormPharef(const double pharef) {
    if(pharef < 0.0 || pharef >= 180.0) {
      std::string msg = "Invalid value of normalization pharef [" + IString(pharef) + "]";
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
  void Shade::SetNormIncref(const double incref) {
    if(incref < 0.0 || incref >= 90.0) {
      std::string msg = "Invalid value of normalization incref [" + IString(incref) + "]";
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
  void Shade::SetNormEmaref(const double emaref) {
    if(emaref < 0.0 || emaref >= 90.0) {
      std::string msg = "Invalid value of normalization emaref [" + IString(emaref) + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    p_normEmaref = emaref;
  }

  /**
   * Set the normalization function parameter.
   * This is the albedo (I/F value at incidence p_normIncref
   * and zero phase) used to simulate a shaded relief image. To
   * construct mosaics, the same value of albedo should be used
   * for all images to achieve a uniform result.
   *
   * @param albedo  Normalization function parameter
   */
  void Shade::SetNormAlbedo(const double albedo) {
    p_normAlbedo = albedo;
  }
}

extern "C" Isis::NormModel *ShadePlugin(Isis::Pvl &pvl, Isis::PhotoModel &pmodel) {
  return new Isis::Shade(pvl, pmodel);
}
