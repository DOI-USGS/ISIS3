/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <cmath>
#include "ShadeAtm.h"
#include "AtmosModel.h"
#include "NumericalApproximation.h"
#include "IException.h"

namespace Isis {
  ShadeAtm::ShadeAtm(Pvl &pvl, PhotoModel &pmodel, AtmosModel &amodel) : NormModel(pvl, pmodel, amodel) {
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
    } else {
      p_normPharef = p_normIncref;
    }

    if(algorithm.hasKeyword("Emaref")) {
      SetNormEmaref(algorithm["Emaref"]);
    }

    if(algorithm.hasKeyword("Albedo")) {
      SetNormAlbedo(algorithm["Albedo"]);
    }
  }

  /*
   * @param phase Phase angle
   * @param incidence Incidence angle
   * @param emission Emission angle
   * @param dn
   * @param albedo
   * @param mult
   * @param base
   *
   * @history 2008-11-05 Jeannie Walldren - Modified references
   *           to NumericalMethods class and replaced Isis::PI
   *           with PI since this is in Isis namespace.
   *
   */
  void ShadeAtm::NormModelAlgorithm(double phase, double incidence, double emission,
                                    double demincidence, double dememission, double dn,
                                    double &albedo, double &mult, double &base) {
    double rho;
    double psurfref;
    static double psurf;
    static double ahInterp;
    static double munot;
    double pstd;
    double trans;
    double trans0;
    double transs;
    double sbar;

    static double old_phase = -9999;
    static double old_incidence = -9999;
    static double old_emission = -9999;
    static double old_demincidence = -9999;
    static double old_dememission = -9999;

    // Calculate normalization at standard conditions
    GetPhotoModel()->SetStandardConditions(true);
    psurfref = GetPhotoModel()->CalcSurfAlbedo(p_normPharef, p_normIncref, p_normEmaref);
    GetPhotoModel()->SetStandardConditions(false);

    // Get reference hemispheric albedo (Hapke opposition effect doesn't influence it much)
    GetAtmosModel()->GenerateAhTable();

    if(psurfref == 0.0) {
      std::string msg = "Divide by zero error";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    rho = p_normAlbedo / psurfref;

    if (old_phase != phase || old_incidence != incidence || old_emission != emission ||
        old_demincidence != demincidence || old_dememission != dememission) {

      psurf = GetPhotoModel()->CalcSurfAlbedo(phase, demincidence, dememission);

      ahInterp = (GetAtmosModel()->AtmosAhSpline()).Evaluate(incidence, NumericalApproximation::Extrapolate);

      munot = cos(incidence * (PI / 180.0));

      old_phase = phase;
      old_incidence = incidence;
      old_emission = emission;
      old_demincidence = demincidence;
      old_dememission = dememission;
    }

    GetAtmosModel()->CalcAtmEffect(phase, incidence, emission, &pstd, &trans, &trans0, &sbar,
                                   &transs);

    albedo = pstd + rho * (ahInterp * munot * trans /
                           (1.0 - rho * GetAtmosModel()->AtmosAb() * sbar) + (psurf - ahInterp * munot) * trans0);
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
  void ShadeAtm::SetNormPharef(const double pharef) {
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
  void ShadeAtm::SetNormIncref(const double incref) {
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
  void ShadeAtm::SetNormEmaref(const double emaref) {
    if(emaref < 0.0 || emaref >= 90.0) {
      std::string msg = "Invalid value of normalization emaref [" +
                        IString(emaref) + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    p_normEmaref = emaref;
  }

  /**
   * Set the normalization function parameter. This is the albedo (I/F
   * value at incidence p_normIncref and zero phase) used to
   * simulate a shaded relief image. To construct mosaics, the same value
   * of albedo should be used for all images to achieve a uniform result.
   *
   * @param albedo  Normalization function parameter
   */
  void ShadeAtm::SetNormAlbedo(const double albedo) {
    p_normAlbedo = albedo;
  }
}

extern "C" Isis::NormModel *ShadeAtmPlugin(Isis::Pvl &pvl, Isis::PhotoModel &pmodel, Isis::AtmosModel &amodel) {
  return new Isis::ShadeAtm(pvl, pmodel, amodel);
}
