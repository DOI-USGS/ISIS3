#include "Albedo.h"
#include "SpecialPixel.h"
#include "iException.h"

#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

namespace Isis {
  Albedo::Albedo (Pvl &pvl, PhotoModel &pmodel) : NormModel(pvl,pmodel) {
    PvlGroup &algorithm = pvl.FindObject("NormalizationModel").FindGroup("Algorithm", Pvl::Traverse);

    SetNormIncref(0.0);
    SetNormIncmat(0.0);
    SetNormThresh(30.0);
    SetNormAlbedo(1.0);

    // Get value from user
    if (algorithm.HasKeyword("Incref")) {
      SetNormIncref(algorithm["Incref"]);
    }

    if (algorithm.HasKeyword("Incmat")) {
      SetNormIncmat(algorithm["Incmat"]);
    }

    if (algorithm.HasKeyword("Thresh")) {
      SetNormThresh(algorithm["Thresh"]);
    }

    if (algorithm.HasKeyword("Albedo")) {
      SetNormAlbedo(algorithm["Albedo"]);
    }

    // Calculate normalization at standard conditions
    GetPhotoModel()->SetStandardConditions(true);
    p_normPsurfref = GetPhotoModel()->CalcSurfAlbedo(p_normIncref, p_normIncref, 0.0);
    GetPhotoModel()->SetStandardConditions(false);
  }

  void Albedo::NormModelAlgorithm (double phase, double incidence,
      double emission, double dn, double &albedo, double &mult,
      double &base)
  {
    double psurf;
    double result;

    // code for scaling each pixel
    psurf = GetPhotoModel()->CalcSurfAlbedo(phase, incidence, emission);

    // thresh is a parameter limiting how much we amplify the dns
    if (p_normPsurfref > psurf*p_normThresh) {
      result = NULL8;
      albedo = NULL8;
      mult = 0.0;
      base = 0.0;
    }
    else {
      if (psurf == 0.0) {
        std::string msg = "Divide by zero error";
        throw iException::Message(iException::Math,msg,_FILEINFO_);
      }
      else {
        result = dn * p_normPsurfref / psurf;
	albedo = result;
	mult = p_normPsurfref / psurf;
	base = 0.0;
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
  void Albedo::SetNormIncref (const double incref) {
    if (incref < 0.0 || incref >= 90.0) {
      std::string msg = "Invalid value of normalization incref [" +
          iString(incref) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    p_normIncref = incref;
  }

  /**
   * Set the normalization function parameter. This
   * parameter is limited to values that are >=0 and <90.
   *
   * @param incmat  Normalization function parameter, default is 0.0
   */
  void Albedo::SetNormIncmat (const double incmat) {
    if (incmat < 0.0 || incmat >= 90.0) {
      std::string msg = "Invalid value of normalization incmat [" +
          iString(incmat) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    p_normIncmat = incmat;
  }

  /**
   * Set the normalization function parameter. This is
   * the albedo that the image will be normalized to have. To
   * construct mosaics, the same value of albedo should be used
   * for all images to achieve a uniform result.
   *
   * @param albedo  Normalization function parameter
   */
  void Albedo::SetNormAlbedo (const double albedo) {
    p_normAlbedo = albedo;
  }

  /**
   * Set the normalization function parameter. It is
   * used to amplify variations in the input image in regions
   * of small incidence angle where the shading in the input
   * image is weak. This parameter sets the upper limit on the
   * amount of amplification that will be attempted. If it is
   * set too low, low incidence areas of the image may appear
   * bland. If it is set too high, then low incidence areas of
   * the image may contain amplified noise rather than useful
   * shading information.
   *
   * @param thresh  Normalization function parameter, default
   *                is 30.0
   */
  void Albedo::SetNormThresh (const double thresh) {
    p_normThresh = thresh;
  }
}

extern "C" Isis::NormModel *AlbedoPlugin (Isis::Pvl &pvl, Isis::PhotoModel &pmodel) {
  return new Isis::Albedo(pvl,pmodel);
}
