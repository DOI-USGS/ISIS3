#include <cmath>
#include "AtmosModel.h"
#include "Constants.h"
#include "HapkeAtm1.h"
#include "NumericalApproximation.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "iException.h"
#include "iString.h"

using std::max;

namespace Isis {
  HapkeAtm1::HapkeAtm1 (Pvl &pvl, PhotoModel &pmodel) : AtmosModel(pvl,pmodel) {
    PvlGroup &algo = pvl.FindObject("AtmosphericModel").FindGroup("Algorithm", Pvl::Traverse);

    SetAtmosHnorm(0.003);

    if (algo.HasKeyword("Hnorm")) {
      SetAtmosHnorm(algo["Hnorm"]);
    }
  }

  /** 
   * Henyey-Greenstein atmos scattering in the 1st approximation. 
   * Isotropic atmospheric scattering in the first approximation, 
   * with corrections to the singly-scattered terms (in the spirit of      
   * Hapke's photometric function for surfaces) for a strongly             
   * anisotropic single-particle phase function.  The particular           
   * phase function implemented is a single-term Henyey-Greenstein.        
   * The model for scattering for a general, non-Lambertian surface with   
   * an atmosphere looks like this:                                        
   *                                                                       
   * P = Pstd + trans*(rho*Ah*munot)/(1.d0-rho*Ab*sbar) + 
   *     trans0*rho*(Psurf-Ah*munot)                              
   *                                                                       
   * where P is the overall photometric function (the model of the data),  
   * PSTD is the pure atmospheric-scattering term, PSURF is the surface    
   * photometric function, AH*MUNOT is a Lambertian approximation to this  
   * with hemispheric albedo AH, TRANS and TRANS0 quantify transmission    
   * of surface reflected light through the atmosphere overall and with    
   * no scatterings in the atmosphere, and finally SBAR quantifies the     
   * illumination of the ground by the sky.  RHO is the ratio of the sur-  
   * face albedo to the albedo assumed in the functional form of PSURF.    
   *  
   * @param phase Value of the phase angle.
   * @param incidence Value of the incidence angle.
   * @param emission Value of the emission angle.
   * 
   * @history 2000-07-07 Randy Kirk - USGS, Flagstaff - Original
   *          code
   * @history 2000-12-18 K Teal Thompson  Port to Unix/ISIS;
   * @history 2007-02-20 Janet Barrett - Imported from Isis2
   *          pht_atm_functions to Isis3. 
   * @history 2008-11-05 Jeannie Walldren - Modified references to
   *          NumericalMethods class and replaced Isis::PI with PI
   *          since this is in Isis namespace.
   */
  void HapkeAtm1::AtmosModelAlgorithm (double phase, double incidence, double emission) {
    double munot,mu;
    double xx;
    double emunot,emu;
    double xmunot,ymunot;
    double xmu,ymu;
    double gmunot,gmu;
    double hpsq1;
    double munotp,mup;
    double fix;
    double hahgt,hahgt0;
    double phasefn;
    double maxval;

    if (p_atmosTau == 0.0) {
      p_pstd = 0.0;
      p_trans = 1.0;
      p_trans0 = 1.0;
      p_sbar = 0.0;
      return;
    }

    if (TauOrWhaChanged()) {
      // preparation includes exponential integrals e sub 2 through 4
      p_wha2 = 0.5 * p_atmosWha;
      p_e2 = AtmosModel::En(2,p_atmosTau);
      p_e3 = AtmosModel::En(3,p_atmosTau);
      p_e4 = AtmosModel::En(4,p_atmosTau);

      // zeroth moments of (uncorrected) x and y times characteristic fn
      p_x0 = p_wha2;
      p_y0 = p_wha2 * p_e2;

      // higher-order correction term for x and y
      p_delta = (1.0 - (p_x0 + p_y0) - (1.0 - p_atmosWha) / (1.0 - (p_x0 - p_y0))) / (p_atmosWha * (0.5 - p_e3));

      // moments of (corrected) x and y
      p_alpha0 = 1.0 + p_delta * (0.5 - p_e3);
      p_alpha1 = 0.5 + p_delta * ((1.0/3.0) - p_e4);
      p_beta0 = p_e2 + p_delta * (0.5 - p_e3);
      p_beta1 = p_e3 + p_delta * ((1.0/3.0) - p_e4);

      // prepare to find correct mixture of x and y in conservative case
      if (p_atmosWha == 1.0) {
        p_e5 = AtmosModel::En(5,p_atmosTau);
	      p_alpha2 = (1.0/3.0) + p_delta * (0.25 - p_e5);
	      p_beta2 = p_e4 + p_delta * (0.25 - p_e5);
	      p_fixcon = (p_beta0 * p_atmosTau - p_alpha1 + p_beta1) / 
                    ((p_alpha1 + p_beta1) * p_atmosTau + 2.0 * (p_alpha2 + p_beta2));
      }


      // gamma will be a weighted sum of x and y functions
      p_gammax = p_wha2 * p_beta0;
      p_gammay = 1.0 - p_wha2 * p_alpha0;


      // sbar is total diffuse illumination
      // isotropic part comes from moments, correction is numerical integral
      GenerateHahgTables();
      p_sbar = 1.0 - ((2.0 - p_atmosWha * p_alpha0) * p_alpha1 + p_atmosWha * p_beta0 * p_beta1) + p_atmosHahgsb;

      SetOldTau(p_atmosTau);
      SetOldWha(p_atmosWha);
    }

    // correct the path lengths for planetary curvature
    hpsq1 = pow((1.0+p_atmosHnorm),2.0) - 1.0;

    if (incidence == 90.0) {
      munot = 0.0;
    }
    else {
      munot = cos((PI/180.0)*incidence);
    }

    maxval = max(1.0e-30,hpsq1+munot*munot);
    munotp = p_atmosHnorm / (sqrt(maxval) - munot);
    munotp = max(munotp,p_atmosTau/69.0);

    if (emission == 90.0) {
      mu = 0.0;
    }
    else {
      mu = cos((PI/180.0)*emission);
    }

    maxval = max(1.0e-30, hpsq1 + mu*mu);
    mup = p_atmosHnorm / (sqrt(maxval) - mu);
    mup = max(mup, p_atmosTau / 69.0);
//  build the x and y functions of mu0 and mu
    maxval = max(1.0e-30,munotp);
    xx = -p_atmosTau / maxval;

    if (xx < -69.0) {
      emunot = 0.0;
    }
    else if (xx > 69.0) {
      emunot = 1.0e30;
    }
    else {
      emunot = exp(-p_atmosTau/munotp);
    }

    maxval = max(1.0e-30,mup);
    xx = -p_atmosTau / maxval;

    if (xx < -69.0) {
      emu = 0.0;
    }
    else if (xx > 69.0) {
      emu = 1.0e30;
    }
    else {
      emu = exp(-p_atmosTau/mup);
    }

    xmunot = 1.0 + p_delta * munotp * (1.0 - emunot);
    ymunot = emunot + p_delta * munotp * (1.0 - emunot);
    xmu = 1.0 + p_delta * mup * (1.0 - emu);
    ymu = emu + p_delta * mup * (1.0 - emu);

    // mix the x and y as required in the conservative case
    if (p_atmosWha == 1.0) {
      fix = p_fixcon * munotp * (xmunot + ymunot);
      xmunot = xmunot + fix;
      ymunot = ymunot + fix;
      fix = p_fixcon * mup * (xmu + ymu);
      xmu = xmu + fix;
      ymu = ymu + fix;
    }


    // gamma1 functions come from x and y, with a correction for
    // highly forward-scattered light as tabulated in hahgtTable
    hahgt = p_atmosHahgtSpline.Evaluate(incidence,NumericalApproximation::Extrapolate);
    gmunot = p_gammax * xmunot + p_gammay * ymunot + hahgt;

    hahgt = p_atmosHahgtSpline.Evaluate(emission,NumericalApproximation::Extrapolate);
    gmu = p_gammax * xmu + p_gammay * ymu + hahgt;

    // purely atmos term uses x and y (plus single-particle phase
    // function correction)
    if (phase == 90.0) {
      phasefn = (1.0 - p_atmosHga * p_atmosHga) / pow(1.0+2.0*p_atmosHga*0.0+p_atmosHga*p_atmosHga,1.5);
    }
    else {
      phasefn = (1.0 - p_atmosHga * p_atmosHga) / 
        pow(1.0 + 2.0 * p_atmosHga * cos((PI/180.0) * phase) + p_atmosHga * p_atmosHga,1.5);
    }

    p_pstd = 0.25 * p_atmosWha * munotp / (munotp + mup) * 
      ((xmunot * xmu - ymunot * ymu) + (phasefn - 1.0) * (1.0 - emu * emunot));

    // xmitted surface term uses gammas
    p_trans = gmunot * gmu;

    // finally, never-scattered term is given by pure attenuation, with
    // a correction for highly forward-scattered light (on the way down
    // but not on the way up) as tabulated in hahgt0Table
    hahgt0 = p_atmosHahgt0Spline.Evaluate(incidence,NumericalApproximation::Extrapolate);
    p_trans0 = (emunot + hahgt0) * emu;
  }

  /**
   * Set the Atmospheric function parameter. This is the
   * atmospheric shell thickness normalized to the planet radius
   * and is used to modify angles to get more accurate path
   * lengths near the terminator (ratio of scale height to the
   * planetary radius). This parameter is limited to values that
   * are >=0.
   *
   * @param hnorm  Atmospheric function parameter, default is 0.003
   */
  void HapkeAtm1::SetAtmosHnorm (const double hnorm) {
    if (hnorm < 0.0) {
      std::string msg = "Invalid value of Atmospheric hnorm [" + iString(hnorm) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    p_atmosHnorm = hnorm;
  }
}

extern "C" Isis::AtmosModel *HapkeAtm1Plugin (Isis::Pvl &pvl, Isis::PhotoModel &pmodel) {
  return new Isis::HapkeAtm1(pvl,pmodel);
}
