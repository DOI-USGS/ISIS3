#include "Topo.h"
#include "SpecialPixel.h"
#include "iException.h"

using std::min;
using std::max;

namespace Isis {
  Topo::Topo (Pvl &pvl, PhotoModel &pmodel) : NormModel(pvl,pmodel) {
    PvlGroup &algorithm = pvl.FindObject("NormalizationModel").FindGroup("Algorithm",Pvl::Traverse);

    SetNormIncref(0.0);
    SetNormThresh(30.0);
    SetNormAlbedo(1.0);

    if (algorithm.HasKeyword("Incref")) {
      SetNormIncref(algorithm["Incref"]);
    }

    if (algorithm.HasKeyword("Thresh")) {
      SetNormThresh(algorithm["Thresh"]);
    }

    if (algorithm.HasKeyword("Albedo")) {
      SetNormAlbedo(algorithm["Albedo"]);
    }
  }

  void Topo::NormModelAlgorithm (double phase, double incidence, 
      double emission, double dn, double &albedo, double &mult,
      double &base)
  {
    double rhobar;
    double pprimeref;
    double psurfref;
    double emaref;
    double phaseref;
    double psurf;
    double psurf0;
    double pprime;

    GetPhotoModel()->SetStandardConditions(true);
    psurf0 = GetPhotoModel()->CalcSurfAlbedo(0.0,0.0,0.0);

    if (psurf0 == 0.0) {
      std::string msg = "Divide by zero error";
      throw iException::Message(iException::Math,msg,_FILEINFO_);
    }
    else {
      rhobar = p_normAlbedo / psurf0;
    }

    emaref = 0.0;
    phaseref = p_normIncref;
    psurfref = GetPhotoModel()->CalcSurfAlbedo(phaseref,p_normIncref,emaref);
    pprimeref = GetPhotoModel()->PhtTopder(phaseref,p_normIncref,emaref);
    GetPhotoModel()->SetStandardConditions(false);

    // code for scaling each pixel
    psurf = GetPhotoModel()->CalcSurfAlbedo(phase,incidence,emission);
    pprime = GetPhotoModel()->PhtTopder(phase,incidence,emission);

    if (psurf*pprimeref > pprime*p_normThresh) {
      albedo = NULL8;
    }
    else {
      if (pprime == 0.0) {
        std::string msg = "Divide by zero error";
        throw iException::Message(iException::Math,msg,_FILEINFO_);
      }
      else {
        albedo = dn * rhobar * (psurf * pprimeref) / pprime +
	                rhobar * psurfref - rhobar * (psurf * pprimeref) / pprime;
      }
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
  void Topo::SetNormIncref (const double incref) {
    if (incref < 0.0 || incref >= 90.0) {
      std::string msg = "Invalid value of normalization incref [" +
          iString(incref) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    p_normIncref = incref;
  }

  /**
   * Set the normalization function parameter. This is
   * the albedo that the image will be normalized to have. To
   * construct mosaics, the same value of albedo should be used
   * for all images to achieve a uniform result.
   *
   * @param albedo  Normalization function parameter
   */
  void Topo::SetNormAlbedo (const double albedo) {
    p_normAlbedo = albedo;
  }

  /**
   * Set the normalization function parameter.
   *
   * @param thresh  Normalization function parameter
   */
  void Topo::SetNormThresh (const double thresh) {
    p_normThresh = thresh;
  }
}

extern "C" Isis::NormModel *TopoPlugin (Isis::Pvl &pvl, Isis::PhotoModel &pmodel) {
  return new Isis::Topo(pvl,pmodel);
}
