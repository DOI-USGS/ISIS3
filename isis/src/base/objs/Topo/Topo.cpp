/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Topo.h"
#include "SpecialPixel.h"
#include "IException.h"

using std::min;
using std::max;

namespace Isis {
  Topo::Topo(Pvl &pvl, PhotoModel &pmodel) : NormModel(pvl, pmodel) {
    PvlGroup &algorithm = pvl.findObject("NormalizationModel").findGroup("Algorithm", Pvl::Traverse);

    SetNormPharef(0.0);
    SetNormIncref(0.0);
    SetNormEmaref(0.0);
    SetNormThresh(30.0);
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

    if(algorithm.hasKeyword("Thresh")) {
      SetNormThresh(algorithm["Thresh"]);
    }

    if(algorithm.hasKeyword("Albedo")) {
      SetNormAlbedo(algorithm["Albedo"]);
    }
  }

  void Topo::NormModelAlgorithm(double phase, double incidence, double emission,
                                double demincidence, double dememission, double dn,
                                double &albedo, double &mult, double &base) {
    static double rhobar;
    static double pprimeref;
    static double psurfref;
    static double psurf;
    static double psurf0;
    static double pprime;

    static double old_phase = -9999;
    static double old_incidence = -9999;
    static double old_emission = -9999;
    static double old_demincidence = -9999;
    static double old_dememission = -9999;

    if (old_phase != phase || old_incidence != incidence || old_emission != emission ||
        old_demincidence != demincidence || old_dememission != dememission) {

      GetPhotoModel()->SetStandardConditions(true);
      psurf0 = GetPhotoModel()->CalcSurfAlbedo(0.0, 0.0, 0.0);

      if(psurf0 == 0.0) {
        std::string msg = "Divide by zero error";
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }
      else {
        rhobar = p_normAlbedo / psurf0;
      }

      psurfref = GetPhotoModel()->CalcSurfAlbedo(p_normPharef, p_normIncref, p_normEmaref);
      pprimeref = GetPhotoModel()->PhtTopder(p_normPharef, p_normIncref, p_normEmaref);
      GetPhotoModel()->SetStandardConditions(false);

      // code for scaling each pixel
      psurf = GetPhotoModel()->CalcSurfAlbedo(phase, demincidence, dememission);
      pprime = GetPhotoModel()->PhtTopder(phase, demincidence, dememission);

      old_phase = phase;
      old_incidence = incidence;
      old_emission = emission;
      old_demincidence = demincidence;
      old_dememission = dememission;
    }

    if(psurf * pprimeref > pprime * p_normThresh) {
      albedo = NULL8;
    }
    else {
      if(pprime == 0.0) {
        std::string msg = "Divide by zero error";
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }
      else {
        albedo = dn * rhobar * (psurf * pprimeref) / pprime +
                 rhobar * psurfref - rhobar * (psurf * pprimeref) / pprime;
      }
    }
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
  void Topo::SetNormPharef(const double pharef) {
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
  void Topo::SetNormIncref(const double incref) {
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
  void Topo::SetNormEmaref(const double emaref) {
    if(emaref < 0.0 || emaref >= 90.0) {
      std::string msg = "Invalid value of normalization emaref [" +
                        IString(emaref) + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    p_normEmaref = emaref;
  }

  /**
   * Set the normalization function parameter. This is
   * the albedo that the image will be normalized to have. To
   * construct mosaics, the same value of albedo should be used
   * for all images to achieve a uniform result.
   *
   * @param albedo  Normalization function parameter
   */
  void Topo::SetNormAlbedo(const double albedo) {
    p_normAlbedo = albedo;
  }

  /**
   * Set the normalization function parameter.
   *
   * @param thresh  Normalization function parameter
   */
  void Topo::SetNormThresh(const double thresh) {
    p_normThresh = thresh;
  }
}

extern "C" Isis::NormModel *TopoPlugin(Isis::Pvl &pvl, Isis::PhotoModel &pmodel) {
  return new Isis::Topo(pvl, pmodel);
}
