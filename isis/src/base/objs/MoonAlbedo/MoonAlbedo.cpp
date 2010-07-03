#include <cmath>
#include "MoonAlbedo.h"
#include "SpecialPixel.h"
#include "iException.h"

#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

namespace Isis {
  MoonAlbedo::MoonAlbedo (Pvl &pvl, PhotoModel &pmodel) :
      NormModel(pvl,pmodel) {
    PvlGroup &algo = pvl.FindObject("NormalizationModel")
                     .FindGroup("Algorithm",Pvl::Traverse);
    // Set default values
    SetNormD(0.14);
    SetNormE(-0.4179);
    SetNormF(0.55);
    SetNormG2(0.02);
    SetNormXmul(1.0);
    SetNormWl(p_normWavelength);
    SetNormH(0.048);
    SetNormBsh1(0.08);
    SetNormXb1(-0.0817);
    SetNormXb2(0.0081);

    // Get values from user
    if (algo.HasKeyword("D")) {
      SetNormD(algo["D"]);
    }

    if (algo.HasKeyword("Wl")) {
      SetNormWl(algo["Wl"]);
    }

    if (algo.HasKeyword("E")) {
      SetNormE(algo["E"]);
    } else {
      if (p_normWl < 1.0) {
        p_normE = -0.3575 * p_normWl - 0.0607;
      } else {
        p_normE = -0.4179;
      }
    }

    if (algo.HasKeyword("F")) {
      SetNormF(algo["F"]);
    }

    if (algo.HasKeyword("G2")) {
      SetNormG2(algo["G2"]);
    } else {
      if (p_normWl < 1.0) {
        p_normG2 = -0.9585 * p_normWl + 0.98;
      } else {
        p_normG2 = 0.02;
      }
    }

    if (algo.HasKeyword("Xmul")) {
      SetNormXmul(algo["Xmul"]);
    }

    if (algo.HasKeyword("H")) {
      SetNormH(algo["H"]);
    }

    if (algo.HasKeyword("Bsh1")) {
      SetNormBsh1(algo["Bsh1"]);
    } else {
      p_normBsh1 = 19.89 - 59.58 * p_normWl + 59.86 * pow(p_normWl,2) -
                    20.09 * pow(p_normWl,3);
      if (p_normBsh1 < 0.0) {
        p_normBsh1 = 0.0;
      }
    }

    if (algo.HasKeyword("Xb1")) {
      SetNormXb1(algo["Xb1"]);
    }

    if (algo.HasKeyword("Xb2")) {
      SetNormXb2(algo["Xb2"]);
    }

    // Initialize values that will be needed to normalize to a
    // Buratti function at phase = 2.0 degrees
    p_normF1 = 1.0 - p_normF;
    double g1 = p_normD * 0.1 + p_normE;
    double g1sq = g1 * g1;
    p_normG2sq = p_normG2 * p_normG2;
    double c30 = cos(30.0 * Isis::PI / 180.0);
    if (1.0 + g1sq + 2.0 * g1 * c30 <= 0.0) {
      std::string msg = "Error while initializing Buratti function";
      throw iException::Message(iException::Math,msg,_FILEINFO_);
    }
    double pg130 = p_normF1 * (1.0 - g1sq) / (pow((1.0+g1sq+2.0*g1*c30),1.5));
    if (1.0 + p_normG2sq + 2.0 * p_normG2 * c30 <= 0.0) {
      std::string msg = "Error while initializing Buratti function";
      throw iException::Message(iException::Math,msg,_FILEINFO_);
    }
    double pg230 = p_normF * (1.0 - p_normG2sq) / (pow((1.0+p_normG2sq+2.0*p_normG2*c30),1.5));
    if (p_normBsh1 < 0.0) p_normBsh1 = 0.0;
    if (1.0 + tan(15.0 * Isis::PI / 180.0) / p_normH == 0.0) {
      std::string msg = "Error while initializing Buratti function";
      throw iException::Message(iException::Math,msg,_FILEINFO_);
    }
    double bshad30 = 1.0 + p_normBsh1 / (1.0 + tan(15.0 * Isis::PI / 180.0) / p_normH);
    p_normPg30 = (pg130 + pg230) * bshad30;
    p_normBc1 = p_normXb1 + p_normXb2 * p_normWl;
    p_normFbc3 = 1.0 + p_normBc1 * 2.0;
    if (p_normFbc3 == 0.0) {
      std::string msg = "Error while initializing Buratti function";
      throw iException::Message(iException::Math,msg,_FILEINFO_);
    }
    p_normC3 = cos(2.0 * Isis::PI / 180.0);
    if (1.0 + p_normG2sq + 2.0 * p_normG2 * p_normC3 <= 0.0) {
      std::string msg = "Error while initializing Buratti function";
      throw iException::Message(iException::Math,msg,_FILEINFO_);
    }
    p_normPg32 = p_normF * (1.0 - p_normG2sq) / (pow((1.0+p_normG2sq+2.0*p_normG2*p_normC3),1.5));
    if (1.0 + tan(Isis::PI / 180.0) / p_normH == 0.0) {
      std::string msg = "Error while initializing Buratti function";
      throw iException::Message(iException::Math,msg,_FILEINFO_);
    }
    p_normBshad3 = 1.0 + p_normBsh1 / (1.0 + tan(Isis::PI / 180.0) / p_normH);
  }

  void MoonAlbedo::NormModelAlgorithm (double phase, double incidence, 
      double emission, double dn, double &albedo, double &mult,
      double &base)
  {
    double a;
    double cosa;
    double pg2;
    double bshad;
    double r;
    double g1;
    double g1sq;
    double pg1;
    double pg;
    double fbc;
    double pg31;
    double pg3;

    a = GetPhotoModel()->CalcSurfAlbedo(phase,incidence,emission);

    if (a != 0.0) {
      cosa = cos(phase * Isis::PI/180.0);
      if (1.0+p_normG2sq+2.0*p_normG2*cosa <= 0.0) {
        albedo = NULL8;
	return;
      }
      pg2 = p_normF * (1.0 - p_normG2sq) / (pow((1.0+p_normG2sq+2.0*p_normG2*cosa),1.5));
      if (1.0 + tan(phase*.5*Isis::PI/180.0) / p_normH == 0.0) {
        albedo = NULL8;
	return;
      }
      bshad = 1.0 + p_normBsh1 / (1.0 + tan(phase*.5*Isis::PI/180.0) / p_normH);
      r = dn * p_normXmul;
      // Estimate the albedo at 0.1, then iterate
      albedo = 0.1;
      for (int i=0; i<6; i++) {
        g1 = p_normD * albedo + p_normE;
	g1sq = g1 * g1;
	if (1.0 + g1sq + 2.0 * g1 * cosa <= 0.0) {
	  albedo = NULL8;
	  return;
	}
	pg1 = p_normF1 * (1.0 - g1sq) / (pow((1.0+g1sq+2.0*g1*cosa),1.5));
	pg = (pg1 + pg2) * bshad;
	if (phase <= 2.0) {
	  fbc = 1.0 + p_normBc1 * phase;
	  if (1.0 + g1sq + 2.0 * g1 * p_normC3 <= 0.0) {
	    albedo = NULL8;
	    return;
          }
	  pg31 = p_normF1 * (1.0 - g1sq) / (pow((1.0+g1sq+2.0*g1*p_normC3),1.5));
	  pg3 = (pg31 + p_normPg32) * p_normBshad3;
	  pg = fbc * (pg3 / p_normFbc3);
        }
	if (pg == 0.0) {
	  albedo = NULL8;
	  return;
        }
	albedo = r * a * p_normPg30 / pg;
      }
    } else {
      albedo = NULL8;
      return;
    }
  }

 /**
   * Set the albedo dependent phase function normalization parameter. 
   * This is an emperically derived coefficient. This parameter has
   * no limits.
   * 
   * @param d  Normalization function parameter, default is 0.14
   */
  void MoonAlbedo::SetNormD (const double d) {
    p_normD = d;
  }

 /**
   * Set the albedo dependent phase function normalization parameter. 
   * This is the wavelength in micrometers of the image being 
   * normalized (found in the BandBin Center keyword of the image). 
   * This parameter has no limits.
   * 
   * @param wl  Normalization function parameter, default is 1.0
   */
  void MoonAlbedo::SetNormWl (const double wl) {
    p_normWl = wl;
  }

 /**
   * Set the albedo dependent phase function normalization parameter. 
   * This is an emperically derived coefficient. This parameter has
   * no limits.
   * 
   * @param e  Normalization function parameter, default is -0.4179
   */
  void MoonAlbedo::SetNormE (const double e) {
    p_normE = e;
  }

 /**
   * Set the albedo dependent phase function normalization parameter. 
   * This is an emperically derived coefficient. This parameter has
   * no limits.
   * 
   * @param f  Normalization function parameter, default is 0.55
   */
  void MoonAlbedo::SetNormF (const double f) {
    p_normF = f;
  }

 /**
   * Set the albedo dependent phase function normalization parameter. 
   * This is an emperically derived coefficient. This parameter has
   * no limits.
   * 
   * @param g2  Normalization function parameter, default is 0.02
   */
  void MoonAlbedo::SetNormG2 (const double g2) {
    p_normG2 = g2;
  }

 /**
   * Set the albedo dependent phase function normalization parameter. 
   * This parameter is used to convert radiance to reflectance or
   * apply a calibration fudge factor. This parameter has no limits.
   * 
   * @param xmul  Normalization function parameter, default is 1.0
   */
  void MoonAlbedo::SetNormXmul (const double xmul) {
    p_normXmul = xmul;
  }

 /**
   * Set the albedo dependent phase function normalization parameter. 
   * This is an emperically derived coefficient. This parameter 
   * is limited to non-zero values.
   * 
   * @param h  Normalization function parameter, default is 0.048
   */
  void MoonAlbedo::SetNormH (const double h) {
    if (h == 0.0) {
      std::string msg = "Invalid value of normalization h [" +
          iString(h) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    p_normH = h;
  }

 /**
   * Set the albedo dependent phase function normalization parameter. 
   * This is an emperically derived coefficient. This parameter is
   * limited to values >= 0.
   * 
   * @param bsh1  Normalization function parameter, default is 0.08
   */
  void MoonAlbedo::SetNormBsh1 (const double bsh1) {
    if (bsh1 < 0.0) {
      std::string msg = "Invalid value of normalization bsh1 [" +
          iString(bsh1) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    p_normBsh1 = bsh1;
  }

 /**
   * Set the albedo dependent phase function normalization parameter. 
   * This is an emperically derived coefficient. This parameter has
   * no limits.
   * 
   * @param xb1  Normalization function parameter, default is -0.0817
   */
  void MoonAlbedo::SetNormXb1 (const double xb1) {
    p_normXb1 = xb1;
  }

 /**
   * Set the albedo dependent phase function normalization parameter. 
   * This is an emperically derived coefficient. This parameter has
   * no limits.
   * 
   * @param xb2  Normalization function parameter, default is 0.0081
   */
  void MoonAlbedo::SetNormXb2 (const double xb2) {
    p_normXb2 = xb2;
  }
}

extern "C" Isis::NormModel *MoonAlbedoPlugin (Isis::Pvl &pvl, Isis::PhotoModel &pmodel) {
  return new Isis::MoonAlbedo(pvl,pmodel);
}
