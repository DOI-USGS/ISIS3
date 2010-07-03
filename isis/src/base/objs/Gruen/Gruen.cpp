/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2010/06/14 20:45:52 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "Gruen.h"
#include "Chip.h"

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>


namespace Isis {

  using namespace GSL;

  /** Basic Gruen algorithm constructor */
  Gruen::Gruen(Pvl &pvl) : AutoReg(pvl) {
    init(pvl);
  }

  /**
   * @brief Sets established radiometric parameters for registration processes
   *
   * This method provides a mechanism for establishing predetermined values for
   * registration activities.  This is intended to be used for the DEM
   * generation processing where points are grown around seed points.  This is
   * intended to lead to rapid convergence of points surrounding established
   * seed points.
   *
   * This should be used in conjuction with the resulting Affine transform as
   * determined from the seed point.
   *
   * Note that once this is established, it remains constant for all subsequent
   * registration processes.  To reset the default, all this method with no
   * arguments.
   *
   * Also note that these defaults can be established in the input AutoReg
   * definition file.
   *
   * These values are set when reset() is called - typically at the start of any
   * adaptive application of the Gruen algorithm.
   *
   * @param gain Precomputed radiometric gain value to use as default
   * @param shift Precomputed radiometric shift value to use as default
   */
  void Gruen::SetRadiometrics(const double &gain, const double &shift) {
    _defGain = gain;
    _defShift = shift;
    return;
  }

  /**
   * @brief Real workhorse of the computational Gruen algorithm
   *
   * This method is called for all registration requests and actually performs
   * the registration of two chips.
   *
   * The pattern chip is deemed constant.  The subsearch chip is generally an
   * extraction from the search chip that has had an affine transform applied to
   * fill it.
   *
   * At each iteration of the Gruen algorithm, the affine transform is
   * incrementally updated based upon the results from this method.  There are
   * six affine parameters and two radiometric (shift and gain) parameters that
   * are solved/computed here.
   *
   * The algorithm itself is a first derivative computation of the subsearch
   * chip with small radiometric adjustments applied to better tone match the
   * two chips.  This is intended to minimize the affine variability.
   *
   * @param pattern   Fixed pattern chip which subsearch is trying to match
   * @param subsearch Affined extraction of the search chip
   *
   * @return GruenResult Container storing the results and helper methods of the
   *         solution
   */
  GruenResult Gruen::algorithm(Chip &pattern, Chip &subsearch) {

    _totalIterations++;  //  Bump iteration counter

    // Initialize loop variables
    int tackSamp = pattern.TackSample();
    int tackLine = pattern.TackLine();

    //  Internal variables
    GruenResult result;
    result.nIters = _nIters + 1;
    BigInt npts(0);

    //  pattern chip is rh image , subsearch chip is lh image
    GSLVector a(8);
    for(int line = 2 ; line <= pattern.Lines() - 1; line++) {
      for(int samp = 2; samp <= pattern.Samples() - 1; samp++) {

        if(!pattern.IsValid(samp, line)) continue;
        if(!subsearch.IsValid(samp, line)) continue;
        if(!subsearch.IsValid(samp + 1, line)) continue;
        if(!subsearch.IsValid(samp - 1, line)) continue;
        if(!subsearch.IsValid(samp, line - 1)) continue;
        if(!subsearch.IsValid(samp, line + 1)) continue;

        //  Compute parameters
        npts++;
        double x0 = (double)(samp - tackSamp);
        double y0 = (double)(line - tackLine);

        double gxtemp = subsearch.GetValue(samp + 1, line) -
                        subsearch.GetValue(samp - 1, line);
        double gytemp = subsearch.GetValue(samp, line + 1) -
                        subsearch.GetValue(samp, line - 1);

        a[0] = gxtemp * x0;
        a[1] = gxtemp * y0;
        a[2] = gxtemp;
        a[3] = gytemp * x0;
        a[4] = gytemp * y0;
        a[5] = gytemp;
        a[6] = 1.0;
        a[7] = subsearch.GetValue(samp, line);
        double ell = pattern.GetValue(samp, line) -
                     (((1.0 + Gain()) * subsearch.GetValue(samp, line)) +
                      Shift());

        //  Compute residual
        result.resid += (ell * ell);

        // Add this point to the ATA and ATL summation matrices.
        // This eliminates the need to store all the points as was
        // in the original ISIS2 algorithm.
        for(int i = 0 ; i < 8 ; i++) {
          for(int j = 0 ; j < 8 ; j++) {
            result.ata[i][j] += (a[i] * a[j]);
          }
          result.atl[i] += (a[i] * ell);
        }
      }
    }

    // Check for enough points
    result.npts = npts;
    BigInt tPnts = (pattern.Lines() - 1) * (pattern.Samples() - 1);
    if(!ValidPoints(tPnts, npts)) {
      result.gerrno = NotEnoughPoints;
      std::ostringstream mess;
      mess << "Minimum points (" << MinValidPoints(tPnts)
           << ") criteria not met (" << result.npts << ")";
      result.gerrmsg = mess.str();
      result.isGood = false;
      logError(result.gerrno, result.gerrmsg);
      return (result);
    }

    //  Compute error analysis
    solve(result);
//    GSLMatrix aff(2,3, &result.alpha[0]);
//    std::cout << "DAffine = " << aff << std::endl;
    return (result);
  }

  /**
   * @brief Computes solution and error analysis using Cholesky/Jacobi methods
   *
   * This method computes the affine and radiometric parameter solution and
   * associated errors/uncertainty from the algorithm() processing.
   *
   * The affine parameters are solved using Cholesky decomposition.  Error
   * analysis is computed using Jacobian eigenvector methods.  The GNU
   * Scientific Library (GSL) is used to apply these routines.
   *
   * See http://www.gnu.org/software/gsl/ for additional details on the GNU
   * Scientific Library.
   *
   * @param result Input parameters provided to compute solution.  This
   *               container is also updated by this method with the solution
   *               and error analysis.
   *
   * @return bool True if successful, false if an error occurs
   */
  bool Gruen::solve(GruenResult &result) {
    GSLUtility *gsl = GSLUtility::getInstance();

    size_t nRows = gsl->Rows(result.ata);
    size_t nCols = gsl->Columns(result.ata);
//    std::cout << "Ata = " << result.ata << std::endl;
//    std::cout << "Resid = " << result.resid << std::endl;
//    std::cout << "npts = " << result.npts << std::endl;
//    std::cout << "Atl = " << result.atl << std::endl;

    // Compute variance
    double variance = result.Variance();
    gsl_matrix *A, *atai;
    try {
      A = gsl->GSLTogsl(result.ata);
      gsl->check(gsl_linalg_cholesky_decomp(A));
      atai = gsl->identity(nRows, nCols);

      for(size_t i = 0 ; i < nRows ; i++) {
        gsl_vector_view x = gsl_vector_view_array(gsl_matrix_ptr(atai, i, 0),
                            nCols);
        gsl->check(gsl_linalg_cholesky_svx(A, &x.vector));
      }
    }
    catch(iException &ie) {
      result.gerrno = CholeskyFailed;
      result.gerrmsg = "Cholesky Failed:: " + ie.Errors();
      result.isGood = false;
      ie.Clear();
      logError(result.gerrno, result.gerrmsg);
      return (false);
    }

    //  Solve Gruen parameter contributions
    for(size_t r = 0 ; r < nRows ; r++) {
      result.alpha[r] = 0.0;
      for(size_t c = 0 ; c < nCols ; c++) {
        double ataiV = gsl_matrix_get(atai, r, c);
        result.alpha[r] += ataiV * result.atl[c];
        result.kmat[r][c] = variance * ataiV;
      }
    }

    // Set up submatrix
    result.skmat[0][0] = result.kmat[2][2];
    result.skmat[0][1] = result.kmat[2][5];
    result.skmat[1][0] = result.kmat[5][2];
    result.skmat[1][1] = result.kmat[5][5];
    try {
      gsl_matrix_view skmat  = gsl_matrix_view_array(&result.skmat[0][0], 2, 2);
      gsl_vector_view evals = gsl_vector_view_array(&result.eigen[0], 2);
      gsl_eigen_symm_workspace *w = gsl_eigen_symm_alloc(2);
      gsl->check(gsl_eigen_symm(&skmat.matrix, &evals.vector, w));
      gsl_eigen_symm_free(w);
      gsl_eigen_symmv_sort(&evals.vector, &skmat.matrix, GSL_EIGEN_SORT_VAL_DESC);
      gsl->free(A);
      gsl->free(atai);
    }
    catch(iException &ie) {
      result.gerrno = EigenSolutionFailed;
      result.gerrmsg = "Eigen Solution Failed:: " + ie.Errors();
      result.isGood = false;
      ie.Clear();
      logError(result.gerrno, result.gerrmsg);
      return (false);
    }
    result.isGood = true;
    return (result.isGood);
  }

  /**
   * @brief Minimization of data set using Gruen algorithm
   *
   * This is a very minimal application of the Gruen algorithm that provides the
   * ability to use it in a non-adaptive capacity.  This method processes two
   * chips of the same size, pattern and subsearch.  The subsearch has typically
   * been extracted in the same manner as the MinimumDifference or
   * MaximumCorrelation routines are utilized.
   *
   * It simply applies the algorithm to the current state of the two chips,
   * computes the error analysis on it and returns the eigen vector solution as an
   * indication of chip registration integrity.
   *
   * Note that in this mode, most all the parameters found in the definition file
   * that apply to the adaptive mode are ignored when utilizing the algorithm in
   * this fashion.
   *
   * @param pattern [in] A Chip object usually containing an nxm area of a cube.
   *                     Must be the same diminsions as \b subsearch.
   * @param subsearch [in] A Chip object usually containing an nxm area of a cube.
   *                  Must be the same diminsions as \b pattern. This is normally
   *                  a subarea of a larger portion of the image.
   *
   * @return The square root of the eigen values of DN differences OR Isis::NULL
   *         if the Gruen algorithm fails.
   */
  double Gruen::MatchAlgorithm(Chip &pattern, Chip &subsearch) {
    reset();
    GruenResult result  = algorithm(pattern, subsearch);
    _result.update(result);
    if(IsGood()) return (_result.Eigen());
    return (Null);
  }

  /**
   * This virtual method must return if the 1st fit is equal to or better
   * than the second fit.
   *
   * @param fit1  1st goodness of fit
   * @param fit2  2nd goodness of fit
   */
  bool Gruen::CompareFits(double fit1, double fit2) {
    return (fit1 <= fit2);
  }


  /**
   * @brief Applies the adaptive Gruen algorithm to pattern and search chips
   *
   * This method computes the adaptive Gruen algorithm for a pattern chip and
   * search chip.  The search chip is assumed to be of a larger size than the
   * pattern chip as dictated by the contents of the registration definition
   * file.
   *
   * This algorithm can be used with or without "fast geoming" the search chip.
   * It works quite well where the two images are assumed to be nearly spatially
   * registered.  Its real intent is to compute parallax angles between two
   * images taken at different viewing geometry.  This provides an efficient
   * process for deriving a digital elevation model (DEM) from two datasets.
   *
   * The Gruen algorithm is applied to the chips until the algorithm converges
   * (current iteration yields a detla affine within tolerance limits), an error
   * is encountered, or the maximum number of iterations is exceeded.
   *
   * Note that bestSamp and bestLine may not be the original center of the
   * search chip.  It is subject to chip reduction matching as specified by the
   * user.  All distance tolerances are compute from this postion.  The
   * process of chip reduction processing is handled by AutoReg prior to calling
   * this routine.
   *
   * @see getThreshHold() for Affine limit information
   *
   * @param sChip Full search chip as rendered from the search image
   * @param pChip Full pattern chip as rendered from the pattern/match image
   * @param fChip Maintains the solution vector at each chip location
   * @param startSamp Starting sample of the search image range
   * @param startLine Starting line of the search image range
   * @param endSamp  Ending sample of the search image range
   * @param endLine Ending line of the search image range
   * @param bestSamp Best registering sample of search chip
   * @param bestLine best registering line of search chip
   *
   * @return AutoReg::RegisterStatus Returns AutoReg::Success if the chip is
   *         successfully registered, otherwise returns
   *         AutoReg::AdaptiveAlgorithmFailed.
   */
  AutoReg::RegisterStatus Gruen::AdaptiveRegistration(Chip &sChip, Chip &pChip,
      Chip &fChip, int startSamp,
      int startLine, int endSamp,
      int endLine, int bestSamp,
      int bestLine) {
#if defined(WRITE_CHIPS)
    static int callNo(0);
    callNo++;
#endif

    // Reset internal parameters
    reset();

    // Set initial chip location
    sChip.SetChipPosition(bestSamp, bestLine);
    _result.setStartImage(sChip.CubeSample(), sChip.CubeLine());

    // Ok create a fit chip whose size is the same as the search chip
    // and then fill it with nulls.  Then adjust the Affine mapping such that
    // it scales to the user specified precision (1/10th) so that fits can
    // be recorded
    fChip.SetSize(sChip.Samples(), sChip.Lines());
    fChip.SetAllValues(Null);
    Affine faffine = sChip.GetTransform();
    faffine.Translate(bestSamp - sChip.TackSample(), bestLine - sChip.TackLine());
    faffine.Scale(_fitChipScale);
    fChip.SetTransform(faffine, false);

    // Create a chip the same size as the pattern chip.  It is critical to use
    // the original search chip to create the subsearch.  Copying the orginal
    // search chip and then resizing preserves established minimum/maximum
    // value ranges.
    Chip subsearch = sChip;
    subsearch.SetSize(pChip.Samples(), pChip.Lines());

    // Set up Affine transform by establishing search tack point
    Affine tform;
    tform.Translate(bestSamp - sChip.TackSample(), bestLine - sChip.TackLine());

    bool done(false);
    GSLVector alpha(GruenResult::Constraints(), 0.0);
    GSLVector threshold = getThreshHold(subsearch);
    for(_nIters = 0 ; _nIters < _maxIters ; _nIters++) {
      // Extract the sub search chip.  The algorithm method handles the
      // determination of good data volumes
      sChip.Extract(subsearch, tform);

#if defined(WRITE_CHIPS)
      std::ostringstream ss;
      ss << "C" << callNo << "I" << _nIters;
      std::string sfname = "subchip" + ss.str() + ".cub";
      subsearch.Write(sfname);
#endif

      // Try to match the two subchips
      GruenResult result = algorithm(pChip, subsearch);
      if(!result.IsGood()) {
        return (Status(result));
      }

      //  Test for termination conditions - errors or convergence
      if(_nIters > 0) {
        if((done = HasConverged(alpha, threshold, result)) == true) {
          ErrorAnalysis(result);
          break;
        }
      }

      // Update the affine transform, other internal parameters
      alpha = result.Alpha().copy();
      try {
        tform = UpdateAffine(result, tform);
      }
      catch(iException &ie) {
        result.gerrno = AffineNotInvertable;
        result.gerrmsg = "Affine invalid/not invertable";
        result.isGood = false;
        logError(result.gerrno, result.gerrmsg);
        ie.Clear();
        return (Status(result));
      }

      // Set output to result
      subsearch.SetChipPosition(subsearch.TackSample(), subsearch.TackLine());
      fChip.SetCubePosition(subsearch.CubeSample(), subsearch.CubeLine());
      if(fChip.IsInsideChip(fChip.ChipSample(), fChip.ChipLine())) {
        fChip.SetValue((int) fChip.ChipSample(), (int) fChip.ChipLine(),
                       result.Eigen());
      }
    }    // Adaptive Loop Terminate

    // Test for solution constraints and update chip if valid
    if(TestConstraints(done, _result)) {
      UpdateChip(sChip, tform);
      SetChipSample(sChip.TackSample());
      SetChipLine(sChip.TackLine());
      _result.setChipTransform(sChip.GetTransform());
      SetGoodnessOfFit(_result.Eigen());
      sChip.SetChipPosition(bestSamp, bestLine);
      _result.setFinalImage(sChip.CubeSample(), sChip.CubeLine());
      CheckAffineTolerance();
    }

    return (Status());
  }

  /**
   * @brief Create Gruen error and processing statistics Pvl output
   *
   * This method generates two groups specific to the Gruen algorithm:  The
   * GruenFailures group which logs all the errors enountered during processing
   * and the GruenStatistics group which logs selected statistics gathered
   * during a registration run.
   *
   * These groups are added to the AutoReg log output Pvl container for
   * reporting to user/log files.
   *
   * @param pvl Input AutoReg Pvl container to add results to
   *
   * @return Pvl Output Pvl container with Gruen information
   */
  Pvl Gruen::AlgorithmStatistics(Pvl &pvl) {
    PvlGroup algo("GruenFailures");
    algo += PvlKeyword("Name", AlgorithmName());
    algo += PvlKeyword("Mode", (IsAdaptive() ? "Adaptive" : "NonAdaptive"));

    //  Log errors
    for(int e = 0 ; e <  _errors.size() ; e++) {
      algo += _errors.getNth(e).LogIt();
    }

    if(_unclassified > 0) {
      algo += PvlKeyword("UnclassifiedErrors", _unclassified);
    }
    pvl.AddGroup(algo);
    pvl.AddGroup(StatsLog());
    pvl.AddGroup(ParameterLog());
    return (pvl);
  }

  /**
   * @brief Create a PvlGroup with the Gruen specific statistics
   *
   * This method generates a PvlGroup from statistics collected for a particular
   * Gruen algorithm application.  This routine is called from the AutoReg
   * algorithm specific statistics routine and augments the AutoReg statistics
   * log output.
   *
   * @return PvlGroup Group containing Pvl keywords with collected statistics
   */
  PvlGroup Gruen::StatsLog() const {
    PvlGroup stats("GruenStatistics");

    stats += PvlKeyword("TotalIterations", _totalIterations);
    stats += ValidateKey("IterationMinimum", _iterStat.Minimum());
    stats += ValidateKey("IterationAverage", _iterStat.Average());
    stats += ValidateKey("IterationMaximum", _iterStat.Maximum());
    stats += ValidateKey("IterationStandardDeviation", _iterStat.StandardDeviation());

    stats += ValidateKey("EigenMinimum", _eigenStat.Minimum());
    stats += ValidateKey("EigenAverage", _eigenStat.Average());
    stats += ValidateKey("EigenMaximum", _eigenStat.Maximum());
    stats += ValidateKey("EigenStandardDeviation", _eigenStat.StandardDeviation());

    stats += ValidateKey("RadioShiftMinimum", _shiftStat.Minimum());
    stats += ValidateKey("RadioShiftAverage", _shiftStat.Average());
    stats += ValidateKey("RadioShiftMaximum", _shiftStat.Maximum());
    stats += ValidateKey("RadioShiftStandardDeviation", _shiftStat.StandardDeviation());

    stats += ValidateKey("RadioGainMinimum", _gainStat.Minimum());
    stats += ValidateKey("RadioGainAverage", _gainStat.Average());
    stats += ValidateKey("RadioGainMaximum", _gainStat.Maximum());
    stats += ValidateKey("RadioGainStandardDeviation", _gainStat.StandardDeviation());

    return (stats);

  }

  /**
   * @brief Create a PvlGroup with the Gruen specific parameters
   *
   * This method generates a PvlGroup of Gruen algorithm parameters. This
   * routine is called from the AutoReg algorithm specific statistics routine
   * and augments the AutoReg log output.
   *
   * @return PvlGroup Group containing Pvl keywords of Gruen parameters
   */
  PvlGroup Gruen::ParameterLog() const {
    PvlGroup parms("GruenParameters");

    parms += PvlKeyword("MaximumIterations", _maxIters);
    parms += ValidateKey("AffineScaleTolerance", _scaleTol);
    parms += ValidateKey("AffineShearTolerance", _shearTol);
    parms += ValidateKey("AffineTranslationTolerance", _transTol);

    parms += ParameterKey("AffineTolerance", _affineTol);
    parms += ParameterKey("SpiceTolerance", _spiceTol);

    parms += ParameterKey("RadioShiftTolerance", _rshiftTol);

    parms += ParameterKey("RadioGainMinTolerance", _rgainMinTol);
    parms += ParameterKey("RadioGainMaxTolerance", _rgainMaxTol);

    parms += ValidateKey("FitChipScale", _fitChipScale, "pixels");
    parms += ValidateKey("DefaultRadioGain", _defGain);
    parms += ValidateKey("DefaultRadioShift", _defShift);

    return (parms);
  }


  /**
   * @brief Creates an error list from know Gruen errors
   *
   * This method creates the list of known/expected Gruen errors that might
   * occur during processing.  This should be closely maintained with the
   * GruenErrors enum list.
   *
   * @return Gruen::ErrorList Error list container
   */
  Gruen::ErrorList Gruen::initErrorList() const {
    ErrorList elist;
    elist.add(1, ErrorCounter(1, "NotEnoughPoints"));
    elist.add(2, ErrorCounter(2, "CholeskyFailed"));
    elist.add(3, ErrorCounter(3, "EigenSolutionFailed"));
    elist.add(4, ErrorCounter(4, "AffineNotInvertable"));
    elist.add(5, ErrorCounter(5, "MaxIterationsExceeded"));
    elist.add(6, ErrorCounter(6, "RadShiftExceeded"));
    elist.add(7, ErrorCounter(7, "RadGainExceeded"));
    elist.add(8, ErrorCounter(8, "MaxEigenExceeded"));
    elist.add(9, ErrorCounter(9, "AffineDistExceeded"));
    return (elist);
  }


  /**
   * @brief Logs a Gruen error
   *
   * A running count of errors that occur is maintained through this method.  If
   * an error occurs that is not in the list, it will also be counted.  This
   * would indicate that a new error condition has occured and needs to be added
   * to the list.
   *
   * @param gerrno  One of the errors as defined by GruenError enum
   * @param gerrmsg  Optional message although it is ignored in this context
   */
  void Gruen::logError(int gerrno, const std::string &gerrmsg) {
    if(!_errors.exists(gerrno)) {
      _unclassified++;
    }
    else {
      _errors.get(gerrno).BumpIt();
    }
    return;
  }

  /**
   * @brief Initialize the object
   *
   * This method reads from the Algorithm group (if it exists) to set variables
   * used in this object.  If not all the keywords are present, then appropriate
   * values are provided.
   *
   * @param PvlObject &pvl PVL object/groups that contain algorithm parameters
   */
  void Gruen::init(Pvl &pvl) {
    //  Establish the parameters
    if(pvl.HasObject("AutoRegistration")) {
      _prof = DbProfile(pvl.FindGroup("Algorithm", Pvl::Traverse));
    }
    else {
      _prof = DbProfile(pvl);
    }

    if(_prof.Name().empty())  _prof.setName("Gruen");

    // Define internal parameters
    _maxIters = ConfKey(_prof, "MaximumIterations", 25);

    _transTol = ConfKey(_prof, "AffineTranslationTolerance", 0.1);
    _scaleTol = ConfKey(_prof, "AffineScaleTolerance", 0.5);
    _shearTol = ConfKey(_prof, "AffineShearTolerance", _scaleTol);
    _affineTol = ConfKey(_prof, "AffineTolerance", DBL_MAX);

    _spiceTol = ConfKey(_prof, "SpiceTolerance", DBL_MAX);

    _rshiftTol = ConfKey(_prof, "RadioShiftTolerance", DBL_MAX);
    _rgainMinTol = ConfKey(_prof, "RadioGainMinTolerance", -DBL_MAX);
    _rgainMaxTol = ConfKey(_prof, "RadioGainMaxTolerance", DBL_MAX);

    _fitChipScale = ConfKey(_prof, "FitChipScale", 0.1); // Set to 10th pixel

    // Set radiometric defaults
    SetRadiometrics();
    _defGain  =  ConfKey(_prof, "DefaultRadioGain", _defGain);
    _defShift =  ConfKey(_prof, "DefaultRadioShift", _defShift);

    _nIters = 0;
    _totalIterations = 0;
    _errors = initErrorList();
    _unclassified = 0;

    reset();
    return;
  }


  /**
   * @brief Reset registration-dependant counters only
   *
   * This method is intended to be invoked to reset interal variables that track
   * or govern behavior pertaining to the registration of two chips. It should
   * be invoked as the first call prior to calling the algorithm method for a
   * new registration.
   */
  void Gruen::reset() {
    _result = GruenResult();
    _result.setGain(getDefaultGain());
    _result.setShift(getDefaultShift());
    _nIters = 0;
    return;
  }

  /**
   * @brief Reset Gruen statistics as needed
   *
   */
  void Gruen::resetStats() {
    _eigenStat.Reset();
    _iterStat.Reset();
    _shiftStat.Reset();
    _gainStat.Reset();
    return;
  }

  /**
   * @brief Compute the Affine convergence parameters
   *
   * This method should be invoked using either the subsearch or pattern chip
   * since they are both the same size.  The six Affine convergence parameters
   * are computed from the size of the chip and the AffineThreshHold1 and
   * AffineThreshHold2 registration parameters.
   *
   * AffineThreshHold1 governs the shift in line and sample and is used directly
   * as specified in the registration config file.
   *
   * AffineThreshHold2 governs scaling of sample and line as a function of the
   * number of lines and samples in the chip provided. The value from the
   * registration config file is divided by half the samples for the X Affine
   * component;  the Y Affine component is divided by half the number of lines
   * in the chip.
   *
   *
   * @param chip Chip to use to compute affine convergence threshholds
   *
   * @see HasConverged().
   *
   * @return Gruen::GSLVector Returns a vector of convergence thresholds that
   *         coincides with the linear order of the affine transform components.
   */
  Gruen::GSLVector Gruen::getThreshHold(const Chip &chip) const {
    GSLVector thresh(6);
    thresh[0] = _scaleTol / (((double)(chip.Samples() - 1)) / 2.0);
    thresh[1] = _shearTol / (((double)(chip.Lines() - 1)) / 2.0);
    thresh[2] = _transTol;
    thresh[3] = _shearTol / (((double)(chip.Samples() - 1)) / 2.0);
    thresh[4] = _scaleTol / (((double)(chip.Lines() - 1)) / 2.0);
    thresh[5] = _transTol;
    return (thresh);
  }

  /**
   * @brief Tests Affine parameters for convergence
   *
   * This method is invoked after the first iteration to test for convergence
   * of the affine transform components of the Gruen registration algorithm.
   * When the absolute value of all the affine components are less than or equal
   * to its coinciding limit, convergence has been reached.
   *
   * This method does not consider the radiometric shift and gain parameters in
   * its determination of convergence.
   *
   * @param alpha  Affine transform change from last iteration to test
   * @param thresh Six element array of affine threshholds to test against
   * @param results Results container should any information be needed from it
   *
   * @return bool  True if we have converged, false if convergence is not yet
   *         reached.
   *
   * @internal
   *   @history 2009-09-17 Kris Becker Test should be >= rather than >.
   */
  bool Gruen::HasConverged(const GSLVector &alpha, const GSLVector &thresh,
                           const GruenResult &results) const {
    int maxholds = std::min(alpha.dim(), thresh.dim());
    for(int nhold = 0 ; nhold < maxholds ; nhold++) {
      if(fabs(alpha[nhold]) >= thresh[nhold]) return (false);
    }
    return (true);
  }


  /**
   * @brief Computes the number of minimum valid points
   *
   * This method uses the pattern valid percent as specified in the registration
   * config file (or the programmer) to compute the minimum number of valid
   * points from the total.
   *
   * @param totalPoints Assumed to be total number of relavent pixels in a chip
   *
   * @return BigInt Minimum number of valid points determined from the
   *         percentage specified by user/programmer.
   */
  BigInt Gruen::MinValidPoints(BigInt totalPoints) const {
    double pts = (double) totalPoints * (PatternValidPercent() / 100.0);
    return ((BigInt) pts);
  }

  /**
   * @brief Determines if number of points is valid percentage of all points
   *
   * Computes the number of minimum valid points from user specified percentage
   * and tests the acutal number used.
   *
   * @param totalPoints Total number of possible valid points in chip
   * @param nPoints Actual number of valid points used in chips
   *
   * @return bool True if number of actual points exceeds percent mimimum valid,
   *         otherwise returns false.
   */
  bool Gruen::ValidPoints(BigInt totalPoints, BigInt nPoints) const {
    return (nPoints > MinValidPoints(totalPoints));
  }

  /**
   * @brief Computes/determines error analysis after the solution converges
   *
   * This method maintains the error analysis computed from the Gruen algorithm
   * when a convergent condition is encountered.  This essentially is a copy of
   * the iterative analysis that takes place at each application of the Gruen
   * algorithm.  It ensures the error analysis is current by making a copy of
   * the last error analysis perform as found in the results container.
   *
   * It moves the iterative error analysis to the cummulative result container.
   *
   * @param result Iterative solution container with last error analysis that is
   *               to be preserved
   */
  void Gruen::ErrorAnalysis(GruenResult &result) {
    _result.setErrorAnalysis(result);
    return;
  }

  /**
   * @brief Test user limits/contraints after the algorithm has converged
   *
   * This method is invoked immediately after the Gruen algorithm has converged
   * to test against user specified limits.  This call is only valid in the
   * adaptive context as much of the error checking is handled by AutoReg when
   * using the non-adaptive algorithm.
   *
   * This method tests for convergence, maximum iterations exceeded, tolerance
   * limits of radiometric shift and gain and whether the eigenvalue of the
   * solution exceeds the limit.
   *
   * The result container is altered should a constraint not be meet which
   * indicates the registration failed.
   *
   * @param done   Input parameter that indicates convergence has occurred
   * @param result Container with results update by status of contraint check
   *
   * @return bool Returns true if all constraints tests are valid, otherwise
   *         returns false indicating an error.
   */
  bool Gruen::TestConstraints(const bool &done, GruenResult &result) {

    if(!done) {
      result.isGood = false;
      result.gerrno = MaxIterationsExceeded;
      result.gerrmsg = "Maximum Iterations exceeded";
      logError(result.gerrno, result.gerrmsg);
      return (false);
    }
    else {


      if(result.Iterations() > _maxIters) {
        result.isGood = false;
        result.gerrno = MaxIterationsExceeded;
        result.gerrmsg = "Maximum Iterations exceeded";
        logError(result.gerrno, result.gerrmsg);
        return (false);
      }
      _iterStat.AddData(result.Iterations());

      if(result.Shift() > _rshiftTol) {
        result.isGood = false;
        result.gerrno = RadShiftExceeded;
        result.gerrmsg = "Radiometric shift exceeds tolerance";
        logError(result.gerrno, result.gerrmsg);
        return (false);
      }
      _shiftStat.AddData(result.Shift());

      if(((1.0 + result.Gain()) > _rgainMaxTol) ||
          ((1.0 + result.Gain()) < _rgainMinTol)) {
        result.isGood = false;
        result.gerrno = RadGainExceeded;
        result.gerrmsg = "Radiometric gain exceeds tolerances";
        logError(result.gerrno, result.gerrmsg);
        return (false);
      }
      _gainStat.AddData(result.Gain());

      if(result.Eigen() > Tolerance()) {
        result.isGood = false;
        result.gerrno = MaxEigenExceeded;
        result.gerrmsg = "Eigen value exceeds tolerance";
        logError(result.gerrno, result.gerrmsg);
        return (false);
      }
      _eigenStat.AddData(result.Eigen());
    }

    return (result.IsGood());
  }


  /**
   * @brief Updates the affine transform with the final iterative solution
   *
   * This method is called at the end of each iteration that updates the affine
   * transform with the sum of all prior affine changes.  The affine for the
   * current result is added to the cummulative result container and the
   * incremental affine is added to the cummulate transform.
   *
   * @param result Container representing the last iteration solution
   * @param gtrans Accumulating affine transform for search chip
   *
   * @return Affine Returns the newly updated Affine transform
   */
  Affine Gruen::UpdateAffine(GruenResult &result, const Affine &gtrans) {
    _result.update(result);
    const GSLVector &alpha = result.Alpha();
    Affine::AMatrix dAffine = gtrans.Forward();
    for(int i = 0, a = 0 ; i < 2 ; i++) {
      for(int j = 0 ; j < 3 ; j++, a++) {
        dAffine[i][j] += alpha[a];
      }
    }
    return (Affine(dAffine));
  }

  /**
   * @brief Updates the (search) chip with the final Affine transform
   *
   * This method applies the convergent Affine transform parameters to the chip
   * provided.  The accummulated transform only represents the result of the
   * Gruen algorithm.  Therefore, any existing Affine transform used to load the
   * orginal chip will be added to it for the final resulting solution.
   *
   * When completed, in theory, the chip can be used to reload from the file and
   * it should match well with the original pattern chip on the final iteration
   * of the Gruen algorithm which converged.
   *
   * @param chip   Chip to update with accummulated Affine transform
   * @param affine Gruen accummulated Affine transform to add to chip
   */
  void Gruen::UpdateChip(Chip &chip, const Affine &affine) {
    Affine::AMatrix c = chip.GetTransform().Forward() +
                        (affine.Forward() - Affine::getIdentity());
    chip.SetTransform(Affine(c));
    return;
  }

  /**
   * @brief Check affine tolerance for validity
   *
   * This method checks for a convergent solution that travels to far from the
   * original tack point (best point in most cases).  The user can control this
   * tolerance with the AffineTolerance parameter in the registration config
   * file.  The check is a sample/line magnitude check from the original
   * starting pixel location to the one after the affine transform has
   * converged to match to a new cube pixel coordinate.
   *
   * Note this check is independant of TestConstraints() method since the update
   * of the chip only occurs after other limits pass.  The chip must be updated
   * and the new tack point cube pixel location must be determined prior to
   * calling this method.
   *
   * @return bool True if the new pixel location does not exceed the defined
   *         tolerance, otherwise returns false.
   */
  bool Gruen::CheckAffineTolerance() {
    if(_result.ErrorMagnitude() > AffineTolerance()) {
      _result.isGood = false;
      _result.gerrno = AffineDistExceeded;
      _result.gerrmsg = "Affine tolerance exceeded";
      logError(_result.gerrno, _result.gerrmsg);
      return (false);
    }
    return (true);
  }

  /**
   * @brief Returns the proper status given a Gruen result container
   *
   * This method will return registration status consistant with
   * AutoReg::RegisterStatus return codes.
   *
   * @param result Gruen result container used to determine status
   * @return AutoReg::RegisterStatus Returns AutoReg::Success if the Gruen
   *         registration is successful, otherwise returns
   *         AutoReg::AdaptiveAlgorithmFailed.
   */
  AutoReg::RegisterStatus Gruen::Status(const GruenResult &result) const {
    if(result.IsGood()) {
      return (AutoReg::Success);
    }
    return (AutoReg::AdaptiveAlgorithmFailed);
  }
}
