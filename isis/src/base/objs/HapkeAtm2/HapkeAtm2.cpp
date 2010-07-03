#include <cmath>
#include "AtmosModel.h"
#include "Constants.h"
#include "HapkeAtm2.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "NumericalApproximation.h"
#include "iException.h"
#include "iString.h"

using std::max;

namespace Isis {
  HapkeAtm2::HapkeAtm2 (Pvl &pvl, PhotoModel &pmodel) : AtmosModel(pvl,pmodel) {
    PvlGroup &algo = pvl.FindObject("AtmosphericModel").FindGroup("Algorithm",Pvl::Traverse);

    SetAtmosHnorm(0.003);

    if (algo.HasKeyword("Hnorm")) { 
      SetAtmosHnorm(algo["Hnorm"]);
    }
  }
/** 
   * Henyey-Greenstein atmos scattering in the 1st approximation. 
   * Isotropic atmospheric scattering in the second approximation,         
   * with corrections to the singly-scattered terms (in the spirit of        
   * Hapke's photometric function for surfaces) for a strongly               
   * anisotropic single-particle phase function.  The particular             
   * phase function implemented is a single-term Henyey-Greenstein.          
   * The model for scattering for a general, non-Lambertian surface with     
   * an atmosphere looks like this:                                          
   *                                                                         
   * P = Pstd + trans*(rho*Ah*munot)/(1.d0-rho*Ab*sbar) +
   *     trans0*rho*(psurf-Ah*munot)                                       
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
   * @history 2008-11-05 Jeannie Walldren - Modified references 
   *          to NumericalMethods class. Replaced Isis::PI with
   *          PI since this is in Isis namespace.
   */
  void HapkeAtm2::AtmosModelAlgorithm (double phase, double incidence, 
      double p_emission)
  {
    double xx;
    double maxval;
    double munot,mu;
    double p_emunot,emu;
    double munotp,mup;
    double f1munot,f1mmunot;
    double xmunot,ymunot;
    double xmu,ymu;
    double gmunot,gmu;
    double hpsq1;
    double fix;
    double f1mu,f1mmu;
    double hahgt,hahgt0;
    double phasefn;

    if (p_atmosTau == 0.0) {
      p_pstd = 0.0;
      p_trans = 1.0;
      p_trans0 = 1.0;
      p_sbar = 0.0;
      return;
    }

    if (TauOrWhaChanged()) {

      // preparation includes exponential integrals p_e sub 2 through 4
      p_wha2 = 0.5 * p_atmosWha;
      p_e1   = AtmosModel::En(1,p_atmosTau);
      p_e1_2 = AtmosModel::En(1,2.0*p_atmosTau);
      p_e2   = AtmosModel::En(2,p_atmosTau);
      p_e3   = AtmosModel::En(3,p_atmosTau);
      p_e4   = AtmosModel::En(4,p_atmosTau);

      // chandra's gmn functions require fm and fn at mu=-1
      xx = -p_atmosTau;
      if (xx < -69.0) {
        p_em = 0.0;
      }
      else if (xx > 69.0) {
        p_em = 1.0e30;
      }
      else {
        p_em = exp(xx);
      }

      p_f1m = log(2.0) - p_em * p_e1 + p_e1_2;
      p_f2m = -1.0 * (p_f1m + p_em * p_e2 - 1.0);
      p_f3m = -1.0 * (p_f2m + p_em * p_e3 - 0.5);
      p_g12 = (p_atmosTau * p_e1 * p_e2 + p_f1m + p_f2m) * 0.5;
      p_g13 = (p_atmosTau * p_e1 * p_e3 + p_f1m + p_f3m) * (1.0/3.0);

      // chandra's g'mn functions require g'11 and f at mu=+1
      xx = p_atmosTau;
      if (xx < -69.0) {
        p_e = 0.0;
      }
      else if (xx > 69.0) {
        p_e = 1.0e30;
      }
      else {
        p_e = exp(xx);
      }

      p_f1 = Eulgam() + log(p_atmosTau) + p_e * p_e1;
      p_f2 = p_f1 + p_e * p_e2 - 1.0;
      p_f3 = p_f2 + p_e * p_e3 - 0.5;
      p_g11p = AtmosModel::G11Prime(p_atmosTau);
      p_g12p = (p_atmosTau * (p_e1 - p_g11p) + p_em * (p_f1 + p_f2)) * 0.25;
      p_g13p = (p_atmosTau * (0.5 * p_e1 - p_g12p) + p_em * (p_f1 + p_f3)) * 0.2;

      // zeroth moments of (uncorrected) x and y times characteristic fn
      p_x0 = p_wha2 * (1.0 + p_wha2 * p_g12);
      p_y0 = p_wha2 * (p_e2 + p_wha2 * p_g12p);

      // higher-order correction term for x and y
      p_delta = (1.0 - (p_x0 + p_y0) - (1.0 - p_atmosWha) / (1.0 - (p_x0 - p_y0))) / (p_atmosWha * (0.5 - p_e3));

      // moments of (corrected) x and y
      p_alpha0 = 1.0 + p_wha2 * p_g12 + p_delta * (0.5 - p_e3);
      p_alpha1 = 0.5 + p_wha2 * p_g13 + p_delta * ((1.0/3.0) - p_e4);
      p_beta0 = p_e2 + p_wha2 * p_g12p + p_delta * (0.5 - p_e3);
      p_beta1 = p_e3 + p_wha2 * p_g13p + p_delta * ((1.0/3.0) - p_e4);

      // prepare to find correct mixture of x and y in conservative case
      if (p_atmosWha == 1.0) {
        p_e5 = AtmosModel::En(5, p_atmosTau);
        p_f4m = -1.0 * (p_f3m + p_em * p_e4 - (1.0/3.0));
	      p_g14 = (p_atmosTau * p_e1 * p_e4 + p_f1m + p_f4m) * 0.25;
	      p_f4 = p_f3 + p_e * p_e4 - (1.0/3.0);
	      p_g14p = (p_atmosTau * (0.5 * p_e1 - p_g13p) + p_em * (p_f1 +  p_f4)) * (1.0/6.0);
	      p_alpha2 = (1.0/3.0) + p_wha2 * p_g14 + p_delta * (0.25 - p_e5);
        p_beta2 = p_e4 + p_wha2 * p_g14p + p_delta * (0.25 - p_e5);
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
    munot = cos((PI/180.0)*incidence);
    maxval = max(1.0e-30,hpsq1+munot*munot);
    munotp = p_atmosHnorm / (sqrt(maxval) - munot);
    munotp = max(munotp,p_atmosTau/69.0);
    mu = cos((PI/180.0)*p_emission);
    maxval = max(1.0e-30,hpsq1+mu*mu);
    mup = p_atmosHnorm / (sqrt(maxval) - mu);
    mup = max(mup,p_atmosTau/69.0);

    // build the x and y functions of mu0 and mu
    maxval = max(1.0e-30,munotp);
    xx = -p_atmosTau / maxval;

    if (xx < -69.0) {
      p_emunot = 0.0;
    }
    else if (xx > 69.0) {
      p_emunot = 1.0e30;
    }
    else {
      p_emunot = exp(-p_atmosTau/munotp);
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

    // in the second approximation the x and y include the p_f1 function
    xx = munotp;
    if (fabs(xx-1.0) < 1.0e-10) {
      f1munot = p_f1;
      f1mmunot = xx * (log(1.0+1.0/xx) - p_e1 * p_emunot + AtmosModel::En(1,p_atmosTau*(1.0+1.0/xx)));
    }
    else if (xx > 0.0) {
      f1munot = xx * (log(xx/(1.0-xx)) + p_e1 / p_emunot + AtmosModel::Ei(p_atmosTau*(1.0/xx-1.0)));
      f1mmunot = xx * (log(1.0+1.0/xx) - p_e1 * p_emunot + AtmosModel::En(1,p_atmosTau*(1.0+1.0/xx)));
    }
    else {
      std::string msg = "Negative length of planetary curvature ";
      msg += "encountered";
      throw iException::Message(iException::Math,msg,_FILEINFO_);
    }

    xx = mup;
    if (fabs(xx-1.0) < 1.0e-10) {
      f1mu = p_f1;
      f1mmu = xx * (log(1.0+1.0/xx) - p_e1 * emu + AtmosModel::En(1,p_atmosTau*(1.0+1.0/xx)));
    }
    else if (xx > 0.0) {
      f1mu = xx * (log(xx/(1.0-xx)) + p_e1 / emu + AtmosModel::Ei(p_atmosTau*(1.0/xx-1.0)));
      f1mmu = xx * (log(1.0+1.0/xx) - p_e1 * emu + AtmosModel::En(1,p_atmosTau*(1.0+1.0/xx)));
    } else {
      std::string msg = "Negative length of planetary curvature ";
      msg += "encountered";
      throw iException::Message(iException::Math,msg,_FILEINFO_);
    }

    xmunot = 1.0 + p_wha2 * f1mmunot + p_delta * munotp * (1.0 - p_emunot);
    ymunot = p_emunot * (1.0 + p_wha2 * f1munot) + p_delta * munotp * (1.0 - p_emunot);
    xmu = 1.0 + p_wha2 * f1mmu + p_delta * mup * (1.0 - emu);
    ymu = emu * (1.0 + p_wha2 * f1mu) + p_delta * mup * (1.0 - emu);

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

    hahgt = p_atmosHahgtSpline.Evaluate(p_emission,NumericalApproximation::Extrapolate);
    gmu = p_gammax * xmu + p_gammay * ymu + hahgt;

    // purely atmos term uses x and y (plus single-particle phase
    // function correction)
    phasefn = (1.0 - p_atmosHga * p_atmosHga) / pow(1.0+2.0*p_atmosHga*
                                                    cos((PI/180.0)*phase)+p_atmosHga*p_atmosHga,1.5);
    p_pstd = 0.25 * p_atmosWha * munotp / (munotp + mup) * ((xmunot * xmu - ymunot * ymu) + 
                                                            (phasefn - 1.0) * (1.0 - emu * p_emunot));

    // xmitted surface term uses gammas
    p_trans = gmunot * gmu;

    // finally, never-scattered term is given by pure attenuation, with
    // a correction for highly forward-scattered light (on the way down
    // but not on the way up) as tabulated in hahgt0Table
    hahgt0 = p_atmosHahgt0Spline.Evaluate(incidence,NumericalApproximation::Extrapolate);
    p_trans0 = (p_emunot + hahgt0) * emu;
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
  void HapkeAtm2::SetAtmosHnorm (const double hnorm) {
    if (hnorm < 0.0) {
      std::string msg = "Invalid value of Atmospheric hnorm [" + iString(hnorm) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    p_atmosHnorm = hnorm;
  }
}

extern "C" Isis::AtmosModel *HapkeAtm2Plugin (Isis::Pvl &pvl, Isis::PhotoModel &pmodel) {
  return new Isis::HapkeAtm2(pvl,pmodel);
}
