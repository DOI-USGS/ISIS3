#include <cmath>
#include "AlbedoAtm.h"
#include "NumericalApproximation.h"
#include "iException.h"

#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

namespace Isis {
  /**
   * Constructs AlbedoAtm object using a Pvl, PhotoModel, and
   * AtmosModel
   * @param pvl 
   * @param pmodel 
   * @param amodel 
   *  
   * @internal 
   *   @history 2008-11-05 Jeannie Walldren - Modified references
   *          to NumericalMethods class and replaced Isis::PI with
   *          PI since this is in Isis namespace.
   */
  AlbedoAtm::AlbedoAtm (Pvl &pvl, PhotoModel &pmodel, AtmosModel &amodel) :
      NormModel(pvl,pmodel,amodel) {
    PvlGroup &algo = pvl.FindObject("NormalizationModel")
                     .FindGroup("Algorithm",Pvl::Traverse);
    // Set default value
    SetNormIncref(0.0);
    // Get value from user
    if (algo.HasKeyword("Incref")) {
      SetNormIncref(algo["Incref"]);
    }

    // First-time setup:
    // Calculate normalization at standard conditions
    GetPhotoModel()->SetStandardConditions(true);
    p_normPsurfref = GetPhotoModel()->CalcSurfAlbedo(p_normIncref, p_normIncref,0.0);
    GetPhotoModel()->SetStandardConditions(false);

    // Get reference hemispheric albedo
    GetAtmosModel()->GenerateAhTable();

    p_normAhref = (GetAtmosModel()->AtmosAhSpline()).Evaluate(p_normIncref, NumericalApproximation::Extrapolate);

    p_normMunotref = cos((PI/180.0)*p_normIncref);

    // Now calculate atmosphere at standard conditions
    GetAtmosModel()->SetStandardConditions(true);
    GetAtmosModel()->CalcAtmEffect(p_normIncref,p_normIncref,0.0,
                                   &p_normPstdref,&p_normTransref,
                                   &p_normTrans0ref,&p_normSbar);
    GetAtmosModel()->SetStandardConditions(false);
  }

  /** 
   * @param phase Value of phase angle.
   * @param incidence  Value of incidence angle.
   * @param emission Value of emission angle.
   * @param dn
   * @param albedo
   * @param mult 
   * @param base 
   * @internal 
   *   @history 2008-11-05 Jeannie Walldren - Modified references
   *          to NumericalMethods class and replaced Isis::PI with
   *          PI since this is in Isis namespace.
   */
  void AlbedoAtm::NormModelAlgorithm (double phase, double incidence, 
      double emission, double dn, double &albedo, double &mult, double &base)
  {
    double psurf;
    double ahInterp;
    double munot;
    double pstd;
    double trans;
    double trans0;
    double rho;
    double dpo;
    double q;
    double dpm;
    double firsterm;
    double secondterm;
    double thirdterm;
    double fourthterm;
    double fifthterm;

    psurf = GetPhotoModel()->CalcSurfAlbedo(phase,incidence,
        emission);

    ahInterp = (GetAtmosModel()->AtmosAhSpline()).Evaluate(incidence, NumericalApproximation::Extrapolate);

    munot = cos(incidence*(PI/180.0));
    GetAtmosModel()->CalcAtmEffect(phase,incidence,emission,&pstd,&trans,&trans0,&p_normSbar);

    // With model at actual geometry, calculate rho from dn
    dpo = dn - pstd;
    dpm = (psurf - ahInterp * munot) * trans0;
    q = ahInterp * munot * trans + GetAtmosModel()->AtmosAb() * p_normSbar * dpo + dpm;

    if (dpo <= 0.0 && GetAtmosModel()->AtmosNulneg()) {
      rho = 0.0;
    }
    else {
      firsterm = GetAtmosModel()->AtmosAb() * p_normSbar;
      secondterm = dpo * dpm;
      thirdterm = firsterm * secondterm;
      fourthterm = pow(q,2.0) - 4.0 * thirdterm;

      if (fourthterm < 0.0) {
        std::string msg = "Square root of negative encountered";
        throw iException::Message(iException::Math,msg,_FILEINFO_);
      }
      else {
        fifthterm = q + sqrt(fourthterm);
      }

      rho = 2 * dpo / fifthterm;
    }

    // Now use rho and reference geometry to calculate output dnout
    if ((1.0-rho*GetAtmosModel()->AtmosAb()*p_normSbar) <= 0.0) {
      std::string msg = "Divide by zero encountered";
      throw iException::Message(iException::Math,msg,_FILEINFO_);
    }
    else {
      albedo = p_normPstdref + rho * (p_normAhref * p_normMunotref * 
          p_normTransref / (1.0 - rho * GetAtmosModel()->AtmosAb() * 
	        p_normSbar) + (p_normPsurfref - p_normAhref * 
	        p_normMunotref) * p_normTrans0ref);
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
  void AlbedoAtm::SetNormIncref (const double incref) {
    if (incref < 0.0 || incref >= 90.0) {
      std::string msg = "Invalid value of normalization incref [" +
          iString(incref) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    p_normIncref = incref;
  }
}

extern "C" Isis::NormModel *AlbedoAtmPlugin (Isis::Pvl &pvl, Isis::PhotoModel &pmodel, Isis::AtmosModel &amodel) {
  return new Isis::AlbedoAtm(pvl,pmodel,amodel);
}
