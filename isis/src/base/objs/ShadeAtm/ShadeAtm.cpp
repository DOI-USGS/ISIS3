#include <cmath>
#include "ShadeAtm.h"
#include "AtmosModel.h"
#include "NumericalApproximation.h"
#include "iException.h"

namespace Isis {
  ShadeAtm::ShadeAtm (Pvl &pvl, PhotoModel &pmodel, AtmosModel &amodel) : NormModel(pvl,pmodel,amodel) {
    PvlGroup &algorithm = pvl.FindObject("NormalizationModel").FindGroup("Algorithm",Pvl::Traverse);

    SetNormIncref(0.0);
    SetNormAlbedo(1.0);

    if (algorithm.HasKeyword("Incref")) {
      SetNormIncref(algorithm["Incref"]);
    }

    if (algorithm.HasKeyword("Albedo")) {
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
  void ShadeAtm::NormModelAlgorithm (double phase, double incidence,
      double emission, double dn, double &albedo, double &mult, double &base)
  {
    double rho;
    double psurfref;
    double psurf;
    double ahInterp;
    double munot;
    double pstd;
    double trans;
    double trans0;
    double sbar;

    // Calculate normalization at standard conditions
    GetPhotoModel()->SetStandardConditions(true);
    psurfref = GetPhotoModel()->CalcSurfAlbedo(p_normIncref, p_normIncref, 0.0);
    GetPhotoModel()->SetStandardConditions(false);

    // Get reference hemispheric albedo (Hapke opposition effect doesn't influence it much) 
    GetAtmosModel()->GenerateAhTable();

    if (psurfref == 0.0) {
      std::string msg = "Divide by zero error";
	    throw iException::Message(iException::Math,msg,_FILEINFO_);
    }

    rho = p_normAlbedo / psurfref;

    psurf = GetPhotoModel()->CalcSurfAlbedo(phase, incidence, emission);

    ahInterp = (GetAtmosModel()->AtmosAhSpline()).Evaluate(incidence,NumericalApproximation::Extrapolate);

    munot = cos(incidence*(PI/180.0));
    GetAtmosModel()->CalcAtmEffect(phase,incidence,emission,&pstd,&trans,&trans0,&sbar);

    albedo = pstd + rho * (ahInterp * munot * trans / 
                           (1.0 - rho * GetAtmosModel()->AtmosAb() * sbar) + (psurf - ahInterp * munot) * trans0);
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
  void ShadeAtm::SetNormIncref (const double incref) {
    if (incref < 0.0 || incref >= 90.0) {
      std::string msg = "Invalid value of normalization incref [" +
          iString(incref) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    p_normIncref = incref;
  }

  /**
   * Set the normalization function parameter. This is the albedo (I/F
   * value at incidence p_normIncref and zero phase) used to
   * simulate a shaded relief image. To construct mosaics, the same value 
   * of albedo should be used for all images to achieve a uniform result.
   *
   * @param albedo  Normalization function parameter
   */
  void ShadeAtm::SetNormAlbedo (const double albedo) {
    p_normAlbedo = albedo;
  }
}

extern "C" Isis::NormModel *ShadeAtmPlugin (Isis::Pvl &pvl, Isis::PhotoModel &pmodel, Isis::AtmosModel &amodel) {
  return new Isis::ShadeAtm(pvl,pmodel,amodel);
}
