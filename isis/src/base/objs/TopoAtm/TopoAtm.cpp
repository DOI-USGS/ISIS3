#include <cmath>
#include "TopoAtm.h"
#include "NumericalApproximation.h"
#include "iException.h"

namespace Isis {
  /*
   * @history 2008-11-05 Jeannie Walldren - Modified references to 
   *          NumericalMethods class and replaced Isis::PI with PI
   *          since this is in Isis namespace.
   */
  TopoAtm::TopoAtm (Pvl &pvl, PhotoModel &pmodel, AtmosModel &amodel) : NormModel(pvl,pmodel,amodel) {
    double psurf0;
    double emaref;
    double phaseref;
    double psurfref;
    double pprimeref;
    double ahref;
    double munotref;
    double pstdref;
    double transref;
    double trans0ref;
    double sbar;

    PvlGroup &algorithm = pvl.FindObject("NormalizationModel").FindGroup("Algorithm",Pvl::Traverse);

    SetNormIncref(0.0);
    SetNormAlbedo(1.0);

    if (algorithm.HasKeyword("Incref")) {
      SetNormIncref(algorithm["Incref"]);
    }

    if (algorithm.HasKeyword("Albedo")) {
      SetNormAlbedo(algorithm["Albedo"]);
    }

    // First-time setup:
    // Calculate normalization at standard conditions
    GetPhotoModel()->SetStandardConditions(true);
    psurf0 = GetPhotoModel()->CalcSurfAlbedo(0.0,0.0,0.0);

    if (psurf0 == 0.0) {
      std::string msg = "Divide by zero encountered";
      throw iException::Message(iException::Math,msg,_FILEINFO_);
    }
    else {
      p_normRhobar = p_normAlbedo / psurf0;
    }

    emaref = 0.0;
    phaseref = p_normIncref;
    psurfref = GetPhotoModel()->CalcSurfAlbedo(phaseref,p_normIncref,emaref);
    pprimeref = GetPhotoModel()->PhtTopder(phaseref,p_normIncref,emaref);
    GetPhotoModel()->SetStandardConditions(false);

    //  Get reference hemispheric albedo (p_photoB0 doesn't influence it much) 
    GetAtmosModel()->GenerateAhTable();

    ahref = (GetAtmosModel()->AtmosAhSpline()).Evaluate(p_normIncref,NumericalApproximation::Extrapolate);

    munotref = cos((PI/180.0)*p_normIncref);

    // Now calculate atmosphere at standard conditions
    GetAtmosModel()->SetStandardConditions(true);
    GetAtmosModel()->CalcAtmEffect(phaseref,p_normIncref,emaref,&pstdref,&transref,&trans0ref,&sbar);
    GetAtmosModel()->SetStandardConditions(false);

    // Finally, calculate the additive and multiplicative parts of the
    // output-normalized signal, from the point of view of fixed albedo 
    // and varying topography
    p_normAout = p_normRhobar * pprimeref * trans0ref;
    p_normBout = pstdref + p_normRhobar * (transref * ahref * munotref /
                               (1.0 - p_normRhobar * GetAtmosModel()->AtmosAb() * sbar) + trans0ref * 
                               (psurfref - ahref * munotref));
  }

  /*
   *@history 2008-11-05 Jeannie Walldren - Modified references to 
   *          NumericalMethods class and replaced Isis::PI with PI
   *          since this is in Isis namespace.
   */
  void TopoAtm::NormModelAlgorithm (double phase, double incidence,
      double emission, double dn, double &albedo, double &mult, double &base)
  {
    double eps = 0.1;
    double psurf;
    double pprime;
    double ahInterp;
    double munot;
    double pstd;
    double trans;
    double trans0;
    double sbar;
    double rhotlt;
    double dpo;
    double q;
    double slope;
    double pprimeeff;
    double ptilt;
    double dpm;
    double pflat;
    double rhoflat;

    psurf = GetPhotoModel()->CalcSurfAlbedo(phase,incidence,emission);
    pprime = GetPhotoModel()->PhtTopder(phase,incidence,emission);
    ahInterp = (GetAtmosModel()->AtmosAhSpline()).Evaluate(incidence,NumericalApproximation::Extrapolate);

    munot = cos(incidence*(PI/180.0));
    GetAtmosModel()->CalcAtmEffect(phase,incidence,emission,&pstd,&trans,&trans0,&sbar);
    pflat = pstd + p_normRhobar * (trans * ahInterp * munot / 
                             (1.0 - p_normRhobar * GetAtmosModel()->AtmosAb() * sbar) + trans0 * (psurf - 
                              ahInterp * munot));
    ptilt = pflat + p_normRhobar * pprime * trans0 * eps;
    dpo = ptilt - pstd;
    dpm = (psurf - ahInterp * munot) * trans0;
    q = ahInterp * munot * trans + GetAtmosModel()->AtmosAb() * sbar * dpo + dpm;
    rhotlt = 2.0 * dpo / (q + sqrt(pow(q,2.0)-4.0*GetAtmosModel()->AtmosAb()*sbar*dpo*dpm));
    dpo = pflat - pstd;
    q = ahInterp * munot * trans + GetAtmosModel()->AtmosAb() * sbar * dpo + dpm;
    rhoflat = 2.0 * dpo / (q + sqrt(pow(q,2.0)-4.0*GetAtmosModel()->AtmosAb()*sbar*dpo*dpm));
    pprimeeff = (rhotlt - rhoflat) / (rhoflat * eps);
    slope = (dn - 1.0) / pprimeeff;
    albedo = p_normAout * slope + p_normBout;
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
  void TopoAtm::SetNormIncref (const double incref) {
    if (incref < 0.0 || incref >= 90.0) {
      std::string msg = "Invalid value of normalization incref [" + iString(incref) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    p_normIncref = incref;
  }

  /**
   * Set the normalization function parameter. To
   * construct mosaics, the same value of albedo should be used
   * for all images to achieve a uniform result.
   *
   * @param albedo  Normalization function parameter, default
   *                is 0.0690507
   */
  void TopoAtm::SetNormAlbedo (const double albedo) {
    p_normAlbedo = albedo;
  }
}

extern "C" Isis::NormModel *TopoAtmPlugin (Isis::Pvl &pvl, Isis::PhotoModel &pmodel, Isis::AtmosModel &amodel) {
  return new Isis::TopoAtm(pvl,pmodel,amodel);
}
