#include "Shade.h"
#include "iException.h"

#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

namespace Isis {
  Shade::Shade (Pvl &pvl, PhotoModel &pmodel) : NormModel(pvl,pmodel) {
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

  void Shade::NormModelAlgorithm (double phase, double incidence, 
      double emission, double dn, double &albedo, double &mult,
      double &base)
  {
    double rho;
    double psurfref;

    // Calculate normalization at standard conditions
    GetPhotoModel()->SetStandardConditions(true);
    psurfref = GetPhotoModel()->CalcSurfAlbedo(p_normIncref, p_normIncref, 0.0);
    GetPhotoModel()->SetStandardConditions(false);

    if (psurfref == 0.0) {
      std::string msg = "Divide by zero error";
	    throw iException::Message(iException::Math,msg,_FILEINFO_);
    }

    rho = p_normAlbedo / psurfref;
  
    albedo = rho * GetPhotoModel()->CalcSurfAlbedo(phase, incidence, emission);
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
  void Shade::SetNormIncref (const double incref) {
    if (incref < 0.0 || incref >= 90.0) {
      std::string msg = "Invalid value of normalization incref [" + iString(incref) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    p_normIncref = incref;
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
  void Shade::SetNormAlbedo (const double albedo) {
    p_normAlbedo = albedo;
  }
}

extern "C" Isis::NormModel *ShadePlugin (Isis::Pvl &pvl, Isis::PhotoModel &pmodel) {
  return new Isis::Shade(pvl,pmodel);
}
