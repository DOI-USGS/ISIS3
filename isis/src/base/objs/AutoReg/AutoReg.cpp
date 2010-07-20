#include "AutoReg.h"
#include "Chip.h"
#include "Filename.h"
#include "Histogram.h"
#include "LeastSquares.h"
#include "iException.h"
#include "Interpolator.h"
#include "Matrix.h"
#include "Plugin.h"
#include "PolynomialBivariate.h"
#include "Pvl.h"

namespace Isis {
  /**
   * Create AutoReg object.  Because this is a pure virtual class you can
   * not create an AutoReg class directly.  Instead, see the AutoRegFactory
   * class.
   *
   * @param pvl  A pvl object containing a valid AutoReg specification
   *
   * @see patternMatch.doc under the coreg application
   */
  AutoReg::AutoReg(Pvl &pvl) {
    p_template = pvl.FindObject("AutoRegistration");

    // Set default parameters
    p_patternChip.SetSize(3, 3);
    p_searchChip.SetSize(5, 5);
    p_fitChip.SetSize(5, 5);
    p_reducedPatternChip.SetSize(3, 3);
    p_reducedSearchChip.SetSize(5, 5);
    p_reducedFitChip.SetSize(5, 5);

    SetPatternValidPercent(50.0);
    SetSubsearchValidPercent(50.0);
    SetPatternZScoreMinimum(1.0);
    SetTolerance(Isis::Null);

    SetSubPixelAccuracy(true);
    SetEccentricityTesting(false);
    SetResidualTesting(false);
    SetSurfaceModelDistanceTolerance(1.5);
    SetSurfaceModelWindowSize(5);

    SetSurfaceModelEccentricityRatio(2);  // 2:1
    SetSurfaceModelResidualTolerance(0.1);
    SetReductionFactor(1);

    // Clear statistics
    //TODO: Delete these after control net refactor.
    p_Total = 0;
    p_Success = 0;
    p_PatternChipNotEnoughValidData = 0;
    p_PatternZScoreNotMet = 0;
    p_FitChipNoData = 0;
    p_FitChipToleranceNotMet = 0;
    p_SurfaceModelNotEnoughValidData = 0;
    p_SurfaceModelSolutionInvalid = 0;
    p_SurfaceModelDistanceInvalid = 0;
    p_SurfaceModelEccentricityRatioNotMet = 0;
    p_SurfaceModelResidualToleranceNotMet = 0;

    p_sampMovement = 0.;
    p_lineMovement = 0.;

    Init();
    Parse(pvl);
  }

  /**
   * Initialize AutoReg object private variables.  Fill fit chip, reduced pattern
   * chip and reduced search chip with nulls.
   *
   */
  void AutoReg::Init() {
    // Set computed parameters to NULL so we don't use values from a previous
    // run
    p_ZScoreMin = Isis::Null;
    p_ZScoreMax = Isis::Null;
    p_goodnessOfFit = Isis::Null;
    p_surfaceModelEccentricity = Isis::Null;
    p_surfaceModelEccentricityRatio = Isis::Null;
    p_surfaceModelAvgResidual = Isis::Null;

    p_bestSamp = 0;
    p_bestLine = 0;
    p_bestFit = Isis::Null;

    // --------------------------------------------------
    // Nulling out the fit chip
    // --------------------------------------------------
    for(int line = 1; line <= p_fitChip.Lines(); line++) {
      for(int samp = 1; samp <= p_fitChip.Samples(); samp++) {
        p_fitChip.SetValue(samp, line, Isis::Null);
      }
    }
    // --------------------------------------------------
    // Nulling out the reduced pattern chip
    // --------------------------------------------------
    for(int line = 1; line <= p_reducedPatternChip.Lines(); line++) {
      for(int samp = 1; samp <= p_reducedPatternChip.Samples(); samp++) {
        p_reducedPatternChip.SetValue(samp, line, Isis::Null);
      }
    }
    // --------------------------------------------------
    // Nulling out the reduced search chip
    // --------------------------------------------------
    for(int line = 1; line <= p_reducedSearchChip.Lines(); line++) {
      for(int samp = 1; samp <= p_reducedSearchChip.Samples(); samp++) {
        p_reducedSearchChip.SetValue(samp, line, Isis::Null);
      }
    }

  }

  //! Destroy AutoReg object
  AutoReg::~AutoReg() {

  }

  /**
   * Initialize parameters in the AutoReg class using a PVL specification.
   * An example of the PVL required for this is:
   *
   * @code
   * Object = AutoRegistration
   *   Group = Algorithm
   *     Name      = MaximumCorrelation
   *     Tolerance = 0.7
   *   EndGroup
   *
   *   Group = PatternChip
   *     Samples = 21
   *     Lines   = 21
   *   EndGroup
   *
   *   Group = SearchChip
   *     Samples = 51
   *     Lines = 51
   *   EndGroup
   * EndObject
   * @endcode
   *
   * There are many other options that can be set via the pvl and are
   * described in other documentation (see below).
   *
   * @see patternMatch.doc under the coreg
   *      application
   *
   * @param pvl The pvl object containing the specification 
   * @throw  iException::User "Improper format for AutoReg PVL."
   * @internal
   *   @history 2010-06-15 Jeannie Walldren - Added ability to read
   *                          ChipInterpolator keyword from the Algorithm group.
   *   @history 2010-07-20 Jeannie Walldren - Added ability to read search sub
   *                          chip valid percent
  **/
  void AutoReg::Parse(Pvl &pvl) {
    try {
      // Get info from Algorithm group
      PvlGroup &algo = pvl.FindGroup("Algorithm", Pvl::Traverse);
      SetTolerance(algo["Tolerance"]);
      if(algo.HasKeyword("ChipInterpolator")) {
        SetChipInterpolator((std::string)algo["ChipInterpolator"]);
      }

      if(algo.HasKeyword("SubpixelAccuracy")) {
        SetSubPixelAccuracy((std::string)algo["SubpixelAccuracy"] == "True");
      }

      if(algo.HasKeyword("ReductionFactor")) {
        SetReductionFactor((int)algo["ReductionFactor"]);
      }

      // Setup the pattern chip
      PvlGroup &pchip = pvl.FindGroup("PatternChip", Pvl::Traverse);
      PatternChip()->SetSize((int)pchip["Samples"], (int)pchip["Lines"]);

      double minimum = Isis::ValidMinimum;
      double maximum = Isis::ValidMaximum;
      if(pchip.HasKeyword("ValidMinimum")) minimum = pchip["ValidMinimum"];
      if(pchip.HasKeyword("ValidMaximum")) maximum = pchip["ValidMaximum"];
      PatternChip()->SetValidRange(minimum, maximum);

      if(pchip.HasKeyword("MinimumZScore")) {
        SetPatternZScoreMinimum((double)pchip["MinimumZScore"]);
      }
      if(pchip.HasKeyword("ValidPercent")) {
        SetPatternValidPercent((double)pchip["ValidPercent"]);
      }

      // Setup the search chip
      PvlGroup &schip = pvl.FindGroup("SearchChip", Pvl::Traverse);
      SearchChip()->SetSize((int)schip["Samples"], (int)schip["Lines"]);

      minimum = Isis::ValidMinimum;
      maximum = Isis::ValidMaximum;
      if(schip.HasKeyword("ValidMinimum")) minimum = schip["ValidMinimum"];
      if(schip.HasKeyword("ValidMaximum")) maximum = schip["ValidMaximum"];
      SearchChip()->SetValidRange(minimum, maximum);
      if(schip.HasKeyword("SubchipValidPercent")) {
        SetSubsearchValidPercent((double)schip["SubchipValidPercent"]);
      }

      // Setup surface model
      PvlObject ar = pvl.FindObject("AutoRegistration");
      if(ar.HasGroup("SurfaceModel")) {
        PvlGroup &smodel = ar.FindGroup("SurfaceModel", Pvl::Traverse);
        if(smodel.HasKeyword("DistanceTolerance")) {
          SetSurfaceModelDistanceTolerance((double)smodel["DistanceTolerance"]);
        }

        if(smodel.HasKeyword("WindowSize")) {
          SetSurfaceModelWindowSize((int)smodel["WindowSize"]);
        }

        // What kind of eccentricity ratio will we tolerate?
        if(smodel.HasKeyword("EccentricityRatio")) {
          SetEccentricityTesting(true);
          SetSurfaceModelEccentricityRatio((double)smodel["EccentricityRatio"]);
        }

        // What kind of average residual will we tolerate?
        if(smodel.HasKeyword("ResidualTolerance")) {
          SetResidualTesting(true);
          SetSurfaceModelResidualTolerance((double)smodel["ResidualTolerance"]);
        }
      }

    }
    catch(iException &e) {
      std::string msg = "Improper format for AutoReg PVL [" + pvl.Filename() + "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    return;
  }

  /**
   * If the sub-pixel accuracy is enabled, the Register() method will attempt to
   * match the pattern chip to the search chip at sub-pixel accuracy, otherwise it
   * will be registered at whole pixel accuracy.
   *
   * @param on Set the state of registration accuracy.  The
   *           default is sub-pixel accuracy is on
   */
  void AutoReg::SetSubPixelAccuracy(bool on) {
    p_subpixelAccuracy = on;
  }

  /**
   * Set the amount of data in the pattern chip that must be valid.  For
   * example, a 21x21 pattern chip has 441 pixels.  If percent is 75 then
   * at least 330 pixels pairs must be valid in order for a comparision
   * between the pattern and search sub-region to occur.  That is, both
   * the pattern pixel and search pixel must be valid to be counted.  Pixels
   * are considered valid based on the min/max range specified on each of
   * the Chips (see Chip::SetValidRange method).
   *
   * If the pattern chip reduction option is used this percentage will
   * apply to all reduced patterns.  Additionally, the pattern sampling
   * effects the pixel count.  For example if pattern sampling is 50% then
   * only 220 pixels in the 21x21 pattern are considered so 165 must be
   * valid.
   *
   * @param percent   Percentage of valid data between 0 and 100,
   *                  default is 50% if never invoked
   * @throw iException::User - "Invalid value for PatternChip ValidPercent." 
   */
  void AutoReg::SetPatternValidPercent(const double percent) {
    if((percent <= 0.0) || (percent > 100.0)) {
      std::string msg = "Invalid value for PatternChip ValidPercent [" 
        + iString(percent) 
        + "].  Must be greater than 0.0 and less than or equal to 100.0 (Default is 50.0).";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    p_patternValidPercent = percent;
  }


  /**
   * Set the amount of data in the search chip's subchip that must be valid. 
   *
   *
   * @param percent   Percentage of valid data between 0 and 100,
   *                  default is 50% if never invoked
   * @see SetPatternValidPercent() 
   * @throw iException::User - "Invalid value for SearchChip 
   *        SubchipValidPercent."
   * @internal 
   *   @author 2010-07-20 Jeannie Walldren
   *   @history 2010-07-20 Jeannie Walldren - Original Version.
   */
  void AutoReg::SetSubsearchValidPercent(const double percent) {
    if((percent <= 0.0) || (percent > 100.0)) {
      std::string msg = "Invalid value for SearchChip SubchipValidPercent [" 
        + iString(percent) + "]"
        + "].  Must be greater than 0.0 and less than or equal to 100.0 (Default is 50.0).";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    p_subsearchValidPercent = percent;
  }


  /**
   * Set the minimum pattern zscore.  This option is used to
   * ignore pattern chips which are bland (low standard
   * deviation). If the minimum or maximum pixel value in the
   * pattern chip does not meet the minimum zscore value (see a
   * statisitcs book for definition of zscore) then invalid
   * registration will occur.
   *
   * @param minimum The minimum zscore value for the pattern chip.
   *                 Default is 1.0
   * @throw iException::User - "Invalid value for PatternChip MinimumZScore." 
   */
  void AutoReg::SetPatternZScoreMinimum(double minimum) {
    if(minimum <= 0.0) {
      std::string msg = "Invalid value for PatternChip MinimumZScore ["
        + iString(minimum)
        + "].  Must be greater than 0.0. (Default is 1.0).";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    p_minimumPatternZScore = minimum;
  }


  /**
   * Set the tolerance for an acceptable goodness of fit
   *
   * @param tolerance   This tolerance is used to test against the goodness
   *                    of fit returned by the MatchAlgorith method after
   *                    a surface model has been fit.  See TestGoodnessOfFit
   */
  void AutoReg::SetTolerance(const double tolerance) {
    p_tolerance = tolerance;
  }

  /**
   * Sets the Chip class interpolator type to be used to load pattern and search
   * chips.
   * Acceptable values for the interpolator parameter include:
   * <UL>
   *   <LI>NearestNeighborType</LI>
   *   <LI>BiLinearType</LI>
   *   <LI>CubicConvolutionType</LI>
   * </UL>
   *
   * @param interpolator Name of interpolator type to be used.  This is taken from
   *                     the Pvl's ChipInterpolator keyword value.
   * @throw iException::User - "Invalid Interpolator type." 
   * @author Jeannie Walldren
   * @internal
   *   @history 2010-06-15 Jeannie Walldren - Original version.
   */
  void AutoReg::SetChipInterpolator(const string interpolator) {

    Isis::Interpolator::interpType itype;
    if(interpolator == "NearestNeighborType") {
      itype = Isis::Interpolator::NearestNeighborType;
    }
    else if(interpolator == "BiLinearType") {
      itype = Isis::Interpolator::BiLinearType;
    }
    else if(interpolator == "CubicConvolutionType") {
      itype = Isis::Interpolator::CubicConvolutionType;
    }
    else {
      throw iException::Message(iException::User,
                                "Invalid Interpolator type.  Cannot use ["
                                + interpolator + "] to load chip",
                                _FILEINFO_);
    }

    // Set pattern and search chips to use this interpolator type when reading data from cube
    p_patternChip.SetReadInterpolator(itype);
    p_searchChip.SetReadInterpolator(itype);
    p_reducedPatternChip.SetReadInterpolator(itype);
    p_reducedSearchChip.SetReadInterpolator(itype);

  }

  /**
   * Set the surface model window size. The pixels in this window
   * will be used to fit a surface model in order to compute
   * sub-pixel accuracy.  In some cases the default (3x3) and
   * produces erroneous sub-pixel accuracy values.
   *
   * @param size The size of the window must be three or greater
   *             and odd.
   * @throw iException::User - "Invalid value for SurfaceModel WindowSize." 
   */
  void AutoReg::SetSurfaceModelWindowSize(int size) {
    if(size % 2 != 1 || size < 3) {
      std::string msg = "Invalid value for SurfaceModel WindowSize ["
        + iString(size) + "].  Must be an odd number greater than or equal to 3";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    p_windowSize = size;
  }


  /**
   * A 1:1 ratio represents a perfect circle.  Allowing the user
   * to set this ratio lets them determine which points to throw
   * out if the surface model gets too elliptical.
   *
   * @param eccentricityRatio Eccentricity ratio.  Must be greater than or equal
   *                          to 1.
   * @throw iException::User - "Invalid value for SurfaceModel 
   *        EccentricityRatio."
                                                                               */
  void AutoReg::SetSurfaceModelEccentricityRatio(double eccentricityRatio) {
    if(eccentricityRatio < 1) {
      std::string msg = "Invalid value for SurfaceModel EccentricityRatio [" 
        + iString(eccentricityRatio) + "].  Must greater than or equal to 1.0.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    p_surfaceModelEccentricityRatioTolerance = eccentricityRatio;
    p_surfaceModelEccentricityTolerance = sqrt(eccentricityRatio * eccentricityRatio - 1) / eccentricityRatio;
  }

  /**
   * Set the maximum average residual allowed from the surface
   * model. Changing this tolerance allows the user to throw out
   * points whose surfaces cannot be modeled well. The average
   * rersidual is derived from the least squares solution fitting
   * a surface model to a set of known data points, and computed
   * by summing the absolute values of all the residuals (computed
   * z minus actual z) and dividing by the number of residuals.
   *
   * @param residualTolerance Residual tolerance.  Must be greater than 0.
   * @throw iException::User - "Invalid value for SurfaceModel 
   *        ResidualTolerance."
   */
  void AutoReg::SetSurfaceModelResidualTolerance(double residualTolerance) {
    if(residualTolerance < 0) {
      std::string msg = "Invalid value for SurfaceModel ResidualTolerance [" 
        + iString(residualTolerance) + "].  Must greater than or equal to 0.0.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    p_surfaceModelResidualTolerance = residualTolerance;
  }

  /**
   * Set a distance the surface model solution is allowed to move
   * away from the best whole pixel fit in the fit chip.
   *
   * @param distance The distance allowed to move in pixels.  Must
   *                 be greater than 0.
   * @throw iException::User - "Invalid value for SurfaceModel 
   *        DistanceTolerance."
   */
  void AutoReg::SetSurfaceModelDistanceTolerance(double distance) {
    if(distance <= 0.0) {
      std::string msg = "Invalid value for SurfaceModel DistanceTolerance [" 
        + iString(distance) + "].  Must greater than 0.0.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    p_distanceTolerance = distance;
  }


  /**
   * Set the reduction factor used to speed up the pattern
   * matching algorithm.
   *
   * @param factor Reduction factor.  Must be greater than or equal to 1.
   * @throw iException::User - "Invalid value for Algorithm ReductionFactor." 
   */
  void AutoReg::SetReductionFactor(int factor) {
    if(factor < 1) {
      std::string msg = "Invalid value for Algorithm ReductionFactor ["
        + iString(factor) + "].  Must greater than or equal to 1.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    p_reduceFactor = factor;
  }


  /**
   * This method reduces the given chip by the given reduction
   * factor. Used to speed up the match algorithm.
   *
   * @param chip Chip to be reduced
   * @param reductionFactor Factor by which to reduce chip.
   *
   * @return @b Chip Reduced chip object
   */
  Chip AutoReg::Reduce(Chip &chip, int reductionFactor) {
    Chip rChip((int)chip.Samples() / reductionFactor,
               (int)chip.Lines() / reductionFactor);
    if((int)rChip.Samples() < 1 || (int)rChip.Lines() < 1) {
      return chip;
    }

    // ----------------------------------
    // Fill the reduced Chip with nulls.
    // ----------------------------------
    for(int line = 1; line <= rChip.Lines(); line++) {
      for(int samp = 1; samp <= rChip.Samples(); samp++) {
        rChip.SetValue(samp, line, Isis::Null);
      }
    }

    Statistics stats;
    for(int l = 1; l <= rChip.Lines(); l++) {
      int istartLine = (l - 1) * reductionFactor + 1;
      int iendLine = istartLine + reductionFactor - 1;
      for(int s = 1; s <= rChip.Samples(); s++) {

        int istartSamp = (s - 1) * reductionFactor + 1;
        int iendSamp = istartSamp + reductionFactor - 1;

        stats.Reset();
        for(int line = istartLine; line < iendLine; line++) {
          for(int sample = istartSamp; sample < iendSamp; sample++) {
            stats.AddData(chip.GetValue(sample, line));
          }
        }
        rChip.SetValue(s, l, stats.Average());
      }
    }
    return rChip;
  }


  /**
   * Walk the pattern chip through the search chip to find the best registration
   *
   * @return @b AutoReg::RegisterStatus  Returns the status of the registration.
   * @throw iException::User - "Search chips samples must be at least N pixels 
   *        wider than the pattern chip samples for successful surface modeling"
   * @throw iException::User - "Search chips lines must be at least N pixels 
   *        taller than the pattern chip lines for successful surface modeling"
   * @throw iException::User - "Reduction factor is too large"
   */
  AutoReg::RegisterStatus AutoReg::Register() {
    // The search chip must be bigger than the pattern chip by N pixels in
    // both directions for a successful surface model
    int N = p_windowSize / 2 + 1;

    if(p_searchChip.Samples() < p_patternChip.Samples() + N) {
      std::string msg = "Search chips samples [";
      msg += iString(p_searchChip.Samples()) + "] must be at ";
      msg += "least [" + iString(N) + "] pixels wider than the pattern chip samples [";
      msg += iString(p_patternChip.Samples()) + "] for successful surface modeling";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    if(p_searchChip.Lines() < p_patternChip.Lines() + N) {
      std::string msg = "Search chips lines [";
      msg += iString(p_searchChip.Lines()) + "] must be at ";
      msg += "least [" + iString(N) + "] pixels taller than the pattern chip lines [";
      msg += iString(p_patternChip.Lines()) + "] for successful surface modeling";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    Init();
    p_Total++;

    // See if the pattern chip has enough good data
    if(!p_patternChip.IsValid(p_patternValidPercent)) {
      p_PatternChipNotEnoughValidData++;
      p_status = PatternChipNotEnoughValidData;
      return PatternChipNotEnoughValidData;
    }

    if(!ComputeChipZScore(p_patternChip)) {
      p_PatternZScoreNotMet++;
      p_status = PatternZScoreNotMet;
      return PatternZScoreNotMet;
    }

    // ------------------------------------------------------------------
    // Prep for walking the search chip by computing the starting sample
    // and line for the search.  Also compute the sample and line
    // increment for sparse walking
    // ------------------------------------------------------------------
    int startSamp = (p_patternChip.Samples() - 1) / 2 + 1;
    int startLine = (p_patternChip.Lines() - 1) / 2 + 1;
    int endSamp = p_searchChip.Samples() - startSamp + 1;
    int endLine = p_searchChip.Lines() - startLine + 1;

    // ----------------------------------------------------------------------
    // Before we attempt to apply the reduction factor, we need to make sure
    // we won't produce a chip of a bad size.
    // ----------------------------------------------------------------------
    if(p_patternChip.Samples() / p_reduceFactor < 2 || p_patternChip.Lines() / p_reduceFactor < 2) {
      std::string msg = "Reduction factor is too large";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // Establish the center search tack point as best pixel to start for the
    // adaptive algorithm prior to reduction.
    int bestSearchSamp = p_searchChip.TackSample();
    int bestSearchLine = p_searchChip.TackLine();


    // ---------------------------------------------------------------------
    // if the reduction factor is still not equal to one, then we go ahead
    // with the reduction of the chips and call Match to get the first
    // estimate of the best line/sample.
    // ---------------------------------------------------------------------
    if(p_reduceFactor != 1) {
      {
        //Do not remove this brace.  It keeps startSamp, startLine, endSamp, and endLine in the
        //proper scope.

        p_reducedPatternChip.SetSize((int)p_patternChip.Samples() / p_reduceFactor,
                                     (int)p_patternChip.Lines() / p_reduceFactor);

        // ----------------------------------
        // Fill the reduced Chip with nulls.
        // ----------------------------------
        for(int line = 1; line <= p_reducedPatternChip.Lines(); line++) {
          for(int samp = 1; samp <= p_reducedPatternChip.Samples(); samp++) {
            p_reducedPatternChip.SetValue(samp, line, Isis::Null);
          }
        }

        p_reducedPatternChip = Reduce(p_patternChip, p_reduceFactor);
        if(!ComputeChipZScore(p_reducedPatternChip)) {
          p_PatternZScoreNotMet++;
          p_status = PatternZScoreNotMet;
          return PatternZScoreNotMet;
        }

        p_reducedSearchChip = Reduce(p_searchChip, p_reduceFactor);
        int startSamp = (p_reducedPatternChip.Samples() - 1) / 2 + 1;
        int startLine = (p_reducedPatternChip.Lines() - 1) / 2 + 1;
        int endSamp = p_reducedSearchChip.Samples() - startSamp + 1;
        int endLine = p_reducedSearchChip.Lines() - startLine + 1;

        Match(p_reducedSearchChip, p_reducedPatternChip, p_reducedFitChip, startSamp, endSamp, startLine, endLine);
        if(p_bestFit == Isis::Null) {
          p_FitChipNoData++;
          p_status = FitChipNoData;
          return FitChipNoData;
        }
      }

      // ------------------------------------------------------
      // p_bestSamp and p_bestLine are set in Match() which is
      // called above.
      // -----------------------------------------------------
      int bs = (p_bestSamp - 1) * p_reduceFactor + ((p_reduceFactor - 1) / 2) + 1;
      int bl = (p_bestLine - 1) * p_reduceFactor + ((p_reduceFactor - 1) / 2) + 1;

      // ---------------------------------------------------------------
      // Now we grow our window size according to the reduction factor.
      // And we grow around were the first call Match() told us was the
      // best line/sample.
      // ---------------------------------------------------------------
      int newstartSamp = bs - p_reduceFactor - p_windowSize - 1;
      int newendSamp = bs + p_reduceFactor + p_windowSize + 1;
      int newstartLine = bl - p_reduceFactor - p_windowSize - 1;
      int newendLine = bl + p_reduceFactor + p_windowSize + 1;

      if(newstartLine < startLine) newstartLine = startLine;
      if(newendSamp > endSamp) newendSamp = endSamp;
      if(newstartSamp < startSamp) newstartSamp = startSamp;
      if(newendLine > endLine) newendLine = endLine;

      startSamp = newstartSamp;
      endSamp = newendSamp;
      startLine = newstartLine;
      endLine = newendLine;
      // We have found a good pixel in the reduction chip, but we
      // don't want to use it's position, so reset in prep. for
      // non-adaptive registration.  Save it off for the adaptive algorithm.
      bestSearchSamp = bs;
      bestSearchLine = bl;
      p_bestSamp = 0;
      p_bestLine = 0;
      p_bestFit = Isis::Null;
    }

    // If the algorithm is adaptive then it expects the pattern and search chip
    // to be closely registered.  Within a few pixels.  So let it take over
    // doing the sub-pixel accuracy computation
    if(IsAdaptive()) {
      p_status = AdaptiveRegistration(p_searchChip, p_patternChip, p_fitChip,
                                      startSamp, startLine, endSamp, endLine, bestSearchSamp,
                                      bestSearchLine);
      if(p_status == AutoReg::Success) {
        p_searchChip.SetChipPosition(p_chipSample, p_chipLine);
        p_cubeSample = p_searchChip.CubeSample();
        p_cubeLine   = p_searchChip.CubeLine();
        p_goodnessOfFit = p_bestFit;
        p_Success++;
      }
      return p_status;
    }

    // Not adaptive continue with slower search traverse
    Match(p_searchChip, p_patternChip, p_fitChip, startSamp, endSamp, startLine, endLine);

    // Check to see if we went through the fit chip and never got a fit at
    // any location.
    if(p_bestFit == Isis::Null) {
      p_FitChipNoData++;
      p_status = FitChipNoData;
      return FitChipNoData;
    }

    // -----------------------------------------------------------------
    // We had a location in the fit chip.  Save the values even if they
    // may not meet tolerances.  This is also saves the value in the
    // event the user does not want a surface model fit
    // ----------------------------------------------------------------
    p_goodnessOfFit = p_bestFit;
    p_searchChip.SetChipPosition(p_bestSamp, p_bestLine);
    p_cubeSample = p_searchChip.CubeSample();
    p_cubeLine   = p_searchChip.CubeLine();

    // Now see if we satisified the goodness of fit tolerance
    if(!CompareFits(p_bestFit, Tolerance())) {
      p_FitChipToleranceNotMet++;
      p_status = FitChipToleranceNotMet;
      return FitChipToleranceNotMet;
    }

    // Try to fit a model for sub-pixel accuracy if necessary
    if(p_subpixelAccuracy && !IsIdeal(p_bestFit)) {
      std::vector<double> samps, lines, fits;
      for(int line = p_bestLine - p_windowSize / 2; line <= p_bestLine + p_windowSize / 2; line++) {
        if(line < 1) continue;
        if(line > p_fitChip.Lines()) continue;
        for(int samp = p_bestSamp - p_windowSize / 2; samp <= p_bestSamp + p_windowSize / 2; samp++) {
          if(samp < 1) continue;
          if(samp > p_fitChip.Samples()) continue;
          if(p_fitChip.GetValue(samp, line) == Isis::Null) continue;
          samps.push_back((double) samp);
          lines.push_back((double) line);
          fits.push_back(p_fitChip.GetValue(samp, line));
        }
      }

      // -----------------------------------------------------------
      // Make sure we have enough data for a surface fit.  That is,
      // we are not too close to the edge of the fit chip
      // -----------------------------------------------------------
      if((int)samps.size() < p_windowSize * p_windowSize * 2 / 3 + 1) {
        p_SurfaceModelNotEnoughValidData++;
        p_status = SurfaceModelNotEnoughValidData;
        return SurfaceModelNotEnoughValidData;
      }

      // -------------------------------------------------------------------
      // Now that we know we have enough data to model the surface we call
      // ModelSurface to get the sub-pixel accuracy we are looking for.
      // -------------------------------------------------------------------
      if(!ModelSurface(samps, lines, fits)) {
        return p_status;
      }

      // ---------------------------------------------------------------------
      // See if the surface model solution moved too far from our whole pixel
      // solution
      // ---------------------------------------------------------------------
      p_sampMovement = std::fabs(p_bestSamp - p_chipSample);
      p_lineMovement = std::fabs(p_bestLine - p_chipLine);
      if(p_sampMovement > p_distanceTolerance ||
          p_lineMovement > p_distanceTolerance) {
        p_SurfaceModelDistanceInvalid++;
        p_status = SurfaceModelDistanceInvalid;
        return SurfaceModelDistanceInvalid;
      }

      // Ok we have subpixel fits in chip space so convert to cube space
      p_searchChip.SetChipPosition(p_chipSample, p_chipLine);
      p_cubeSample = p_searchChip.CubeSample();
      p_cubeLine   = p_searchChip.CubeLine();
    }

    p_Success++;
    p_status = Success;
    return Success;
  }


  /**
   * This method computes the given Chip's Z-Score. If this value is less than the
   * minimum pattern Z-Score or greater than the negative of the minimum pattern
   * Z-Score, the method will return false.  Otherwise, it returns true.
   *
   * @param chip Chip object whose Z-Score is calculated
   *
   * @return @b bool Indicates whether Z-Score calculated lies between the minimum
   *         and Pattern Z-Score and its negative.
   */
  bool AutoReg::ComputeChipZScore(Chip &chip) {
    Statistics patternStats;
    for(int i = 0; i < chip.Samples(); i++) {
      double pixels[chip.Lines()];
      for(int j = 0; j < chip.Lines(); j++) {
        pixels[j] = chip.GetValue(i + 1, j + 1);
      }
      patternStats.AddData(pixels, chip.Lines());
    }

    // If it does not pass, return
    p_ZScoreMin = patternStats.ZScore(patternStats.Minimum());
    p_ZScoreMax = patternStats.ZScore(patternStats.Maximum());

    // p_ZScoreMin is made negative here so as to make it the equivalent of
    // taking the absolute value (because p_ZScoreMin is guaranteed to be
    // negative)
    if (p_ZScoreMax < p_minimumPatternZScore && -p_ZScoreMin < p_minimumPatternZScore) {
      return false;
    }
    else {
      return true;
    }
  }


  /**
   * Here we walk from start sample to end sample and start line to end line, and
   * compare the pattern chip against the search chip to find the
   * best line/sample.
   *
   *
   * @param sChip Search chip
   * @param pChip Pattern chip
   * @param fChip Fit chip
   * @param startSamp Start sample
   * @param endSamp End sample
   * @param startLine Start line
   * @param endLine End line 
   *  
   * @throw iException::Programmer - "StartSample = EndSample and StartLine = 
   *        EndLine."
   *
   */
  void AutoReg::Match(Chip &sChip, Chip &pChip, Chip &fChip, int startSamp, int endSamp, int startLine, int endLine) {
    // Sanity check.  Should have been caught by the two previous tests
    if(startSamp == endSamp && startLine == endLine) {
      std::string msg = "StartSample [" + iString(startSamp) + "] = EndSample ["
        + iString(endSamp) + "] and StartLine [" + iString(startLine) + " = EndLine ["
        + iString(endLine) + "].";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // Ok create a fit chip whose size is the same as the search chip
    // and then fill it with nulls
    fChip.SetSize(sChip.Samples(), sChip.Lines());
    for(int line = 1; line <= fChip.Lines(); line++) {
      for(int samp = 1; samp <= fChip.Samples(); samp++) {
        fChip.SetValue(samp, line, Isis::Null);
      }
    }

    // Create a chip the same size as the pattern chip.
    Chip subsearch(pChip.Samples(), pChip.Lines());

    for(int line = startLine; line <= endLine; line++) {
      for(int samp = startSamp; samp <= endSamp; samp++) {
        // Extract the subsearch chip and make sure it has enough valid data
        sChip.Extract(samp, line, subsearch);

        if(!subsearch.IsValid(p_subsearchValidPercent)) continue;

        // Try to match the two subchips
        double fit = MatchAlgorithm(pChip, subsearch);

        // If we had a fit save off information about that fit
        if(fit != Isis::Null) {
          fChip.SetValue(samp, line, fit);
          if((p_bestFit == Isis::Null) || CompareFits(fit, p_bestFit)) {
            p_bestFit = fit;
            p_bestSamp = samp;
            p_bestLine = line;
          }
        }
      }
    }
  }


  /**
   * We will model a 2-d surface as:
   *
   * z = a + b*x + c*y + d*x**2 + e*x*y + f*y**2
   *
   * Then the partial derivatives are two lines:
   *
   * dz/dx = b + 2dx + ey
   * dz/dy = c + ex + 2fy
   *
   * We will have a local min/max where dz/dx and dz/dy = 0.
   * Solve using that condition using linear algebra yields:
   *
   * xlocal = (ce - 2bf) / (4df - ee)
   * ylocal = (be - 2cd) / (4df - ee)
   *
   * These will be stored in p_chipSample and p_chipLine respectively.
   *
   * @param x   vector of x (sample) values
   * @param y   vector of y (line) values
   * @param z   vector of z (goodness-of-fit) values
   * @return @b bool  Indicates whether the surface model solution is valid
   *           with residual tolerance and eccentricity ratio is met.
   */
  bool AutoReg::ModelSurface(std::vector<double> &x,
                             std::vector<double> &y,
                             std::vector<double> &z) {
    PolynomialBivariate p(2);
    LeastSquares lsq(p);
    for(int i = 0; i < (int)x.size(); i++) {
      std::vector<double> xy;
      xy.push_back(x[i]);
      xy.push_back(y[i]);
      lsq.AddKnown(xy, z[i]);
    }
    try {
      lsq.Solve();
    }
    catch(iException &e) {
      e.Clear();
      p_status = SurfaceModelSolutionInvalid;
      p_SurfaceModelSolutionInvalid++;
      return false;
    }

    double a = p.Coefficient(0);
    double b = p.Coefficient(1);
    double c = p.Coefficient(2);
    double d = p.Coefficient(3);
    double e = p.Coefficient(4);
    double f = p.Coefficient(5);

    //----------------------------------------------------------
    // Compute eccentricity
    // For more information see:
    // http://mathworld.wolfram.com/Ellipse.html
    // Make sure delta matrix determinant is not equal to zero.
    // The general quadratic curve
    // dx^2+2exy+fy^2+2bx+2cy+a=0
    // is an ellipse when, after defining
    // Delta    =       |d    e/2   b|
    //          |e/2  f/2 c/2|
    //          |b    c/2   a|
    // J        =       |d   e/2|
    //      |e/2 f/e|
    // I        =       d + (f/2)
    // Delta!=0, J>0, and Delta/I<0. Also assume the ellipse is
    // nondegenerate (i.e., it is not a circle, so a!=c, and we have already
    // established is not a point, since J=ac-b^2!=0)
    // ---------------------------------------------------------
    if(p_testEccentricity) {
      bool canComputeEccentricity = true;

      Matrix delta(3, 3);
      delta[0][0] = d;
      delta[0][1] = e / 2;
      delta[0][2] = b / 2;
      delta[1][0] = e / 2;
      delta[1][1] = f;
      delta[1][2] = c / 2;
      delta[2][0] = b / 2;
      delta[2][1] = c / 2;
      delta[2][2] = a;
      if(delta.Determinant() == 0) {
        canComputeEccentricity = false;
      }

      //Make sure J matrix is greater than zero.
      Matrix J(2, 2);
      J[0][0] = d;
      J[0][1] = e / 2;
      J[1][0] = e / 2;
      J[1][1] = f;
      if(J.Determinant() <= 0) {
        canComputeEccentricity = false;
      }

      double I = d + (f);
      if(delta.Determinant() / I >= 0) {
        canComputeEccentricity = false;
      }

      if(canComputeEccentricity) {
        // If the eccentricity can be computed, go ahead and do so
        // Begin by calculating the semi-axis lengths
        double eA = sqrt((2 * (d * (c / 2) * (c / 2) + f * (b / 2) * (b / 2) + a * (e / 2) * (e / 2) - 2 * (e / 2) * (b / 2) * (c / 2) - d * f * a)) /
                         (((e / 2) * (e / 2) - d * f) * (sqrt((d - f) * (d - f) + 4 * (e / 2) * (e / 2)) - (d + f))));

        double eB = sqrt((2 * (d * (c / 2) * (c / 2) + f * (b / 2) * (b / 2) + a * (e / 2) * (e / 2) - 2 * (e / 2) * (b / 2) * (c / 2) - d * f * a)) /
                         (((e / 2) * (e / 2) - d * f) * (-sqrt((d - f) * (d - f) + 4 * (e / 2) * (e / 2)) - (d + f))));

        // Ensure that eA is always the semi-major axis, and eB the semi-minor
        if(eB > eA) {
          double tempVar = eB;
          eB = eA;
          eA = tempVar;
        }

        // Calculate eccentricity
        p_surfaceModelEccentricity = sqrt(eA * eA - eB * eB) / eA;
      }
      else {
        // If the eccentricity cannot be computed, assume it to be 0
        p_surfaceModelEccentricity = 0;
      }

      p_surfaceModelEccentricityRatio = sqrt(1.0 / (1.0 - p_surfaceModelEccentricity * p_surfaceModelEccentricity));

      // Ensure that the eccentricity is less than or equal to the tolerance
      if(p_surfaceModelEccentricity > p_surfaceModelEccentricityTolerance) {
        p_status = SurfaceModelEccentricityRatioNotMet;
        p_SurfaceModelEccentricityRatioNotMet++;
        return false;
      }
    }

    // Check if the user wants to test against the average residual
    if(p_testResidual) {
      // Compare the z computed with z actuals
      double meanAbsError = 0;
      for(int i = 0; i < (int) lsq.Residuals().size(); i++) {
        // Aggregate the absolute values of the residuals
        meanAbsError += fabs(lsq.Residual(i));
      }

      // The average residual, or mean absolute error
      meanAbsError = meanAbsError / lsq.Residuals().size();

      p_surfaceModelAvgResidual = meanAbsError;

      // Ensure the average residual is within the tolerance
      if(meanAbsError > p_surfaceModelResidualTolerance) {
        p_status = SurfaceModelResidualToleranceNotMet;
        p_SurfaceModelResidualToleranceNotMet++;
        return false;
      }
    }

    // Compute the determinant
    double det = 4.0 * d * f - e * e;
    if(det == 0.0) {
      p_status = SurfaceModelSolutionInvalid;
      p_SurfaceModelSolutionInvalid++;
      return false;
    }

    // Compute our chip position to sub-pixel accuracy
    p_chipSample = (c * e - 2.0 * b * f) / det;
    p_chipLine   = (b * e - 2.0 * c * d) / det;
    std::vector<double> temp;
    temp.push_back(p_chipSample);
    temp.push_back(p_chipLine);
    p_goodnessOfFit = lsq.Evaluate(temp);
    return true;
  }


  /**
   * This virtual method must return if the 1st fit is equal to or better
   * than the second fit.
   *
   * @param fit1  1st goodness of fit
   * @param fit2  2nd goodness of fit
   * @return @b bool Indicates whether the first fit is as good or better than the
   *         second
   */
  bool AutoReg::CompareFits(double fit1, double fit2) {
    return(std::fabs(fit1 - IdealFit()) <= std::fabs(fit2 - IdealFit()));
  }

  /**
   * Returns true if the fit parameter is arbitrarily close to the ideal fit
   * value.
   * @param fit Fit value to be compared to the ideal fit
   * @return @b bool Indicates whether the fit is ideal
   */
  bool AutoReg::IsIdeal(double fit) {
    return(std::fabs(IdealFit() - fit) < 0.00001);
  }

#if 0
  /**
   * This returns an AutoRegItem for each measure.
   *
   */
  AutoRegItem AutoReg::RegisterInformation() {
    AutoRegItem item;
    item.setSearchFile(p_searchChip.Filename());
    item.setPatternFile(p_patternChip.Filename());
    //item.setStatus(p_status);
    item.setGoodnessOfFit(p_goodnessOfFit);
    item.setEccentricity(p_surfaceModelEccentricity);
    item.setZScoreOne(p_ZScoreMin);
    item.setZScoreTwo(p_ZScoreMax);

    /*if(p_goodnessOfFit != Isis::Null)item.setGoodnessOfFit(p_goodnessOfFit);
    if(p_surfaceModelEccentricity != Isis::Null) item.setEccentricity(p_surfaceModelEccentricity);
    if(p_ZScoreMin != Isis::Null)item.setZScoreOne(p_ZScoreMin);
    if(p_ZScoreMax != Isis::Null)item.setZScoreTwo(p_ZScoreMax);*/

    // Set the autoRegItem's change in line/sample numbers.
    if(p_status == Success) {
      item.setDeltaSample(p_searchChip.TackSample() - p_searchChip.CubeSample());
      item.setDeltaLine(p_searchChip.TackLine() - p_searchChip.CubeLine());
    }
    else {
      item.setDeltaSample(Isis::Null);
      item.setDeltaLine(Isis::Null);
    }

    return item;
  }
#endif


  /**
   * This returns the cumulative registration statistics.  That
   * is, the Register() method accumulates statistics regard the
   * errors each time is called.  Invoking this method returns a
   * PVL summary of those statisitics
   *
   * @author janderson (3/26/2009)
   *
   * @return @b Pvl
   */
  Pvl AutoReg::RegistrationStatistics() {
    Pvl pvl;
    PvlGroup stats("AutoRegStatistics");
    stats += Isis::PvlKeyword("Total", p_Total);
    stats += Isis::PvlKeyword("Successful", p_Success);
    stats += Isis::PvlKeyword("Failure", p_Total - p_Success);
    pvl.AddGroup(stats);

    PvlGroup grp("PatternChipFailures");
    grp += PvlKeyword("PatternNotEnoughValidData", p_PatternChipNotEnoughValidData);
    grp += PvlKeyword("PatternZScoreNotMet", p_PatternZScoreNotMet);
    pvl.AddGroup(grp);

    PvlGroup fit("FitChipFailures");
    fit += PvlKeyword("FitChipNoData", p_FitChipNoData);
    fit += PvlKeyword("FitChipToleranceNotMet", p_FitChipToleranceNotMet);
    pvl.AddGroup(fit);

    PvlGroup model("SurfaceModelFailures");
    model += PvlKeyword("SurfaceModelNotEnoughValidData", p_SurfaceModelNotEnoughValidData);
    model += PvlKeyword("SurfaceModelSolutionInvalid", p_SurfaceModelSolutionInvalid);
    model += PvlKeyword("SurfaceModelEccentricityRatioNotMet", p_SurfaceModelEccentricityRatioNotMet);
    model += PvlKeyword("SurfaceModelDistanceInvalid", p_SurfaceModelDistanceInvalid);
    model += PvlKeyword("SurfaceModelResidualToleranceNotMet", p_SurfaceModelResidualToleranceNotMet);
    pvl.AddGroup(model);

    return (AlgorithmStatistics(pvl));
  }

  /**
   * This virtual method must be written for adaptive pattern
   * matching algorithms.  Adaptive algorithms are assumed to
   * compute the registration to sub-pixel accuracy and stay
   * within the defined window.  A status should be returned
   * indicating success for subpixel computation or failure and
   * the reason why via the enumeration, RegisterStatus.  If the
   * status is returned is success, the programmer needs to set
   * the sub-pixel chip coordinates using the protected methods
   * SetChipSample(), SetChipLine().
   *
   * For those algorithms that need it, the best sample and line in the search
   * chip is provided.  This is either the initial tack sample and line in the
   * search chip or it is the centered sample and line after the reduction
   * algorithm is applied (KJB, 2009-08-26).
   *
   * @author janderson (6/2/2009)
   *
   * @param sChip Search chip
   * @param pChip Pattern chip
   * @param fChip Fit chip
   * @param startSamp Defines the starting sample of the window
   *                  the adaptive algorithm should remain inside
   *                  this boundary.
   * @param startLine Defines the starting line of the window
   *                  the adaptive algorithm should remain inside
   *                  this boundary.
   * @param endSamp Defines the ending sample of the window
   *                  the adaptive algorithm should remain inside
   *                  this boundary.
   * @param endLine Defines the ending line of the window
   *                  the adaptive algorithm should remain inside
   *                  this boundary.
   * @param bestSamp Best sample
   * @param bestLine Best line
   *
   * @return @b AutoReg::RegisterStatus  Status of match 
   */
  AutoReg::RegisterStatus AutoReg::AdaptiveRegistration(Chip &sChip,
      Chip &pChip,
      Chip &fChip,
      int startSamp,
      int startLine,
      int endSamp,
      int endLine,
      int bestSamp,
      int bestLine) {
    std::string msg = "Programmer needs to write their own virtual AdaptiveRegistration method";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    return Success;
  }

  /**
   * This function returns the keywords that this object was
   * created from.
   *
   * @return @b PvlGroup The keywords this object used in
   *         initialization
   */
  PvlGroup AutoReg::RegTemplate() {
    PvlGroup reg("AutoRegistration");

    PvlGroup &algo = p_template.FindGroup("Algorithm", Pvl::Traverse);
    reg += PvlKeyword("Algorithm", algo["Name"][0]);
    reg += PvlKeyword("Tolerance", algo["Tolerance"][0]);
    if(algo.HasKeyword("SubpixelAccuracy")) {
      reg += PvlKeyword("SubpixelAccuracy", algo["SubpixelAccuracy"][0]);
    }
    if(algo.HasKeyword("ReductionFactor")) {
      reg += PvlKeyword("ReductionFactor", algo["ReductionFactor"][0]);
    }

    PvlGroup &pchip = p_template.FindGroup("PatternChip", Pvl::Traverse);
    reg += PvlKeyword("PatternSamples", pchip["Samples"][0]);
    reg += PvlKeyword("PatternLines", pchip["Lines"][0]);
    if(pchip.HasKeyword("ValidMinimum")) {
      reg += PvlKeyword("PatternMinimum", pchip["ValidMinimum"][0]);
    }
    if(pchip.HasKeyword("ValidMaximum")) {
      reg += PvlKeyword("PatternMaximum", pchip["ValidMaximum"][0]);
    }
    if(pchip.HasKeyword("MinimumZScore")) {
      reg += PvlKeyword("MinimumZScore", pchip["MinimumZScore"][0]);
    }
    if(pchip.HasKeyword("ValidPercent")) {
      SetPatternValidPercent((double)pchip["ValidPercent"]);
      reg += PvlKeyword("ValidPercent", pchip["ValidPercent"][0]);
    }

    PvlGroup &schip = p_template.FindGroup("SearchChip", Pvl::Traverse);
    reg += PvlKeyword("SearchSamples", schip["Samples"][0]);
    reg += PvlKeyword("SearchLines", schip["Lines"][0]);
    if(schip.HasKeyword("ValidMinimum")) {
      reg += PvlKeyword("SearchMinimum", schip["ValidMinimum"][0]);
    }
    if(schip.HasKeyword("ValidMaximum")) {
      reg += PvlKeyword("SearchMaximum", schip["ValidMaximum"][0]);
    }
    if(schip.HasKeyword("SubchipValidPercent")) {
      SetSubsearchValidPercent((double)schip["SubchipValidPercent"]);
      reg += PvlKeyword("SubchipValidPercent", schip["SubchipValidPercent"][0]);
    }

    if(p_template.HasGroup("SurfaceModel")) {
      PvlGroup &smodel = p_template.FindGroup("SurfaceModel", Pvl::Traverse);
      if(smodel.HasKeyword("DistanceTolerance")) {
        reg += PvlKeyword("DistanceTolerance", smodel["DistanceTolerance"][0]);
      }

      if(smodel.HasKeyword("WindowSize")) {
        reg += PvlKeyword("WindowSize", smodel["WindowSize"][0]);
      }

      if(smodel.HasKeyword("EccentricityRatio")) {
        reg += PvlKeyword("EccentricityRatio", smodel["EccentricityRatio"][0]);
      }

      if(smodel.HasKeyword("ResidualTolerance")) {
        reg += PvlKeyword("ResidualTolerance", smodel["ResidualTolerance"][0]);
      }
    }

    return reg;
  }
}
