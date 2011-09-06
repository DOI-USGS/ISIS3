#include <cmath>
#include "LunarLambertEmpirical.h"
#include "iException.h"

namespace Isis {
  LunarLambertEmpirical::LunarLambertEmpirical(Pvl &pvl) : PhotoModel(pvl) {
    PvlGroup &algo = pvl.FindObject("PhotometricModel")
                     .FindGroup("Algorithm", Pvl::Traverse);
    // There are no default values for the Lunar Lambert Empirical function; if user
    // does not provide information, then an exception is thrown
    if (algo.HasKeyword("PhaseList")) {
      SetPhotoPhaseList(algo["PhaseList"]);
    } else {
      std::string msg = "The empirical Lunar Lambert phase list was not provided by user";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    if (algo.HasKeyword("LList")) {
      SetPhotoLList(algo["LList"]);
    } else {
      std::string msg = "The empirical Lunar Lambert l exponent list was not provided by user";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    if (algo.HasKeyword("PhaseCurveList")) {
      SetPhotoPhaseCurveList(algo["PhaseCurveList"]);
    } else {
      std::string msg = "The empirical Lunar Lambert phase brightness list was not provided by user";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // Make sure all the vectors are the same size
    p_photoPhaseAngleCount = (int)p_photoPhaseList.size();

    if (p_photoPhaseAngleCount != (int)p_photoLList.size()) {
      std::string msg = "Number of empirical Lunar Lambert l list values must be equal";
      msg += "to number of phase angles provided";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    if (p_photoPhaseAngleCount != (int)p_photoPhaseCurveList.size()) {
      std::string msg = "Number of empirical Lunar Lambert phase curve list values must be equal";
      msg += "to number of phase angles provided";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
 
    // Create Cubic piecewise linear splines
    p_photoLSpline.Reset();
    p_photoLSpline.SetInterpType(NumericalApproximation::CubicClamped);
    p_photoLSpline.AddData(p_photoPhaseList,p_photoLList);
    p_photoLSpline.SetCubicClampedEndptDeriv(1.0e30,1.0e30);
    p_photoBSpline.Reset();
    p_photoBSpline.SetInterpType(NumericalApproximation::CubicClamped);
    p_photoBSpline.AddData(p_photoPhaseList,p_photoPhaseCurveList);
    p_photoBSpline.SetCubicClampedEndptDeriv(1.0e30,1.0e30);
  }

  LunarLambertEmpirical::~LunarLambertEmpirical() {
    p_photoLSpline.Reset();
    p_photoBSpline.Reset();
    p_photoPhaseList.clear();
    p_photoLList.clear();
    p_photoPhaseCurveList.clear();
  }

  /**
    * Set the empirical Lunar Lambert function phase angle list.  This is the list
    * of phase angles that Lunar Lambert L values and phase curve list values will
    * be provided for. A spline curve will be used to interpolate L values and
    * phase curve values that exist between the given phase angles. The values
    * in the phase angle list are limited to values that are >=0 and <=180.
    *
    * @param phaselist  List of phase angles to interpolate
    */
  void LunarLambertEmpirical::SetPhotoPhaseList(const string phasestrlist) {
    double phaseangle;
    iString strlist(phasestrlist);
    p_photoPhaseList.clear();

    while (strlist.length()) {
      phaseangle = strlist.Token(",");
      if (phaseangle < 0.0 || phaseangle > 180.0) {
        std::string msg = "Invalid value of empirical Lunar Lambert phase angle list value [" +
                          iString(phaseangle) + "]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
      p_photoPhaseList.push_back(phaseangle);
    }
  }

  /**
    * Set the empirical Lunar Lambert function L exponent list.  This is used to 
    * govern the limb-darkening in the Lunar Lambert photometric function.  Values
    * of the Lunar Lambert exponent generally fall in the range from 0.0 
    * (Lambert function) to 1.0 (Lommel-Seeliger or "lunar" function). There are
    * no limits on the value of this parameter, but values far outside the 0 to 1
    * range will not be very useful.
    *
    * @param llist  List of Lunar Lambert function exponents to interpolate
    */
  void LunarLambertEmpirical::SetPhotoLList(const string lstrlist) {
    double lvalue;
    iString strlist(lstrlist);
    p_photoLList.clear();

    while (strlist.length()) {
      lvalue = strlist.Token(",");
      p_photoLList.push_back(lvalue);
    }
  }

  /**
    * Set the empirical Lunar Lambert function phase curve list.  This list provides
    * the brightness values that correspond to the limb-darkening values in the
    * empirical Lunar Lambert photometric function. 
    *
    * @param phasecurvelist  List of brightness values corresponding to Lunar Lambert function exponents
    */
  void LunarLambertEmpirical::SetPhotoPhaseCurveList(const string phasecurvestrlist) {
    double phasecurve;
    iString strlist(phasecurvestrlist);
    p_photoPhaseCurveList.clear();

    while (strlist.length()) {
      phasecurve = strlist.Token(",");
      p_photoPhaseCurveList.push_back(phasecurve);
    }
  }

  double LunarLambertEmpirical::PhotoModelAlgorithm(double phase, double incidence,
                                       double emission) {
    static double pht_lunarlambert_empirical;
    double incrad;
    double emarad;
    double munot;
    double mu;
    double lInterpolated = 0;
    double bInterpolated = 0;

    static double old_phase = -9999;
    static double old_incidence = -9999;
    static double old_emission= -9999;

    // Don't need to do anything if the photometric angles are the same as before
    if (old_phase == phase && old_incidence == incidence && old_emission == emission) {
      return pht_lunarlambert_empirical;
    }

    old_incidence = incidence;
    old_emission = emission;

    incrad = incidence * Isis::PI / 180.0;
    emarad = emission * Isis::PI / 180.0;
    munot = cos(incrad);
    mu = cos(emarad);

    if (phase != old_phase) {
      lInterpolated = p_photoLSpline.Evaluate(phase, NumericalApproximation::Extrapolate);
      bInterpolated = p_photoBSpline.Evaluate(phase, NumericalApproximation::Extrapolate);
      old_phase = phase;
    }

    if(munot <= 0.0 || mu <= 0.0) {
      pht_lunarlambert_empirical = 0.0;
    }
    else if(lInterpolated == 0.0) {
      pht_lunarlambert_empirical = munot * bInterpolated;
    }
    else if(lInterpolated == 1.0) {
      pht_lunarlambert_empirical = bInterpolated * 2.0 * munot / (munot + mu);
    }
    else {
      pht_lunarlambert_empirical = bInterpolated * munot * ((1.0 - lInterpolated) + 2.0 * lInterpolated / (munot + mu));
    }

    return pht_lunarlambert_empirical;
  }
}

extern "C" Isis::PhotoModel *LunarLambertEmpiricalPlugin(Isis::Pvl &pvl) {
  return new Isis::LunarLambertEmpirical(pvl);
}
