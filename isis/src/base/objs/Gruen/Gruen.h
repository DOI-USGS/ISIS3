#if !defined(Gruen_h)
#define Gruen_h
/**
 * @file
 * $Revision: 1.4 $
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


#include "AutoReg.h"
#include "GruenResult.h"
#include "DbProfile.h"
#include "CollectorMap.h"
#include "Statistics.h"
#include "PvlGroup.h"

namespace Isis {
  class Pvl;
  class Chip;

  /**
   * @brief Gruen pattern matching algorithm
   *
   * This class provides adaptive image (chip) registration using
   * the AutoReg factory architecture.  This algorithm uses an
   * Affine transform to iteratively adjust the search chip at
   * each iteration.  Each iteration solves for new adjustments to
   * the Affine transform until the 6 affine parameters fall below the
   * tolerances as specified in AffineThreshHold1 and AffineThreshHold2.
   *
   * This class minimizes the 6 specifiable Affine transform components of a 3x3
   * matrix.  The three Affine components for X (sample) and Y (line) are scale,
   * shear and translation.  Gruen provides control over the maximum values
   * these three components should attain in order for the registration to
   * converge to a successful match.  These limits are specified
   * by <b>AffineScaleTolerance</b>,  <b>AffineShearTolerance</b> and
   * <b>AffineTranslationTolerance</b>.   <b>AffineShearTolerance</b> is
   * optional and if not specified, it defaults to the value of
   * <b>AffineScaleTolerance</b>.  These tolerances specify the maximum amount
   * of translation pixels can be shifted between one Gruen iteration and
   * another.  For example, <b>AffineTranslationTolerance = 0.2</b> means that a
   * subsearch chip cannot move in sample or line direction more than 0.2 pixels
   * in order to satisfy convergence.  <b>AffineScaleTolerance</b> constrains
   * the sample and line scale elements of the Affine transformation.  And
   * <b>AffineShearTolerance</b> constrains the sample and line shear elements
   * of the Affine transformation.  The scale and shear parameters are scaled by
   * size of the Chip.  The sample scale and shear Affine components maximum
   * limit is computed as <b>AffineScaleTolerance/((#Samples-1)/2)</b> and
   * <b>AffineShearTolerance/((#Samples-1)/2)</b>.  Likewise, the line scale and
   * shear maximums are computed using <b>#Lines</b> in the preceding equation.
   *
   * @ingroup PatternMatching
   *
   * @see MinimumGruen AdaptiveGruen AutoReg
   *
   * @author  2009-09-09 Kris Becker
   *
   * @internal
   *   @history 2009-09-10 Kris Becker Changed AffineThreshHold1 to
   *            AffineTranslationTolerance and AffineThreshHold2 to
   *            AffineScaleTolerance.  Added AffineShearTolerance. Added logging
   *            of Gruen parameters.
   *   @history 2009-09-11 Kris Becker Minor error in computation of radiometric
   *            shift
   *   @history 2010-06-14 Jeannie Walldren - Capitalized getTransform and
   *            setTransform to match changes in Chip class
   */
  class Gruen : public AutoReg {
    public:
      /**
       * @brief Construct a GruenMinimization search algorithm
       *
       * This will construct a minimum difference search algorith.  It is
       * recommended that you use a AutoRegFactory class as opposed to
       * this constructor
       *
       * @param pvl  A Pvl object that contains a valid automatic registration
       * definition
       */
      Gruen(Pvl &pvl);

      //! Destructor
      virtual ~Gruen() { }

      /**
       * Gruen default mode is adaptive
       */
      virtual bool IsAdaptive() {
        return true;
      }

      /** Returns the radiometric gain value from the last solution */
      inline double Gain() const {
        return (_result.Gain());
      }
      /** Returns the radiometric shift value from the last solution */
      inline double Shift() const {
        return (_result.Shift());
      }

      void SetRadiometrics(const double &gain = 0.0, const double &shift = 0.0);

      /** Returns the SPICE tolerance constraint as read from config file */
      inline double SpiceTolerance() const {
        return (_spiceTol);
      }

      /** Returns the Affine tolerance constraint as read from config file */
      inline double AffineTolerance() const {
        return (_affineTol);
      }

      GruenResult algorithm(Chip &pattern, Chip &subsearch);
      /** Returns the results container from the last solution */
      const GruenResult &Results() const {
        return (_result);
      }

      /** Returns status of the last registration result */
      bool IsGood() const {
        return (IsGood(_result));
      }
      /** Returns the status of the given Gruen result container */
      bool IsGood(const GruenResult &result) const {
        return (result.IsGood());
      }

    protected:
      typedef GSL::GSLUtility::GSLMatrix GSLMatrix;
      typedef GSL::GSLUtility::GSLVector GSLVector;

      /** Returns the default name of the algorithm as Gruen */
      virtual std::string AlgorithmName() const {
        return ("Gruen");
      }

      bool solve(GruenResult &result);

      //  These methods are for AutoReg non-adaptive requirements
      virtual double MatchAlgorithm(Chip &pattern, Chip &subsearch);
      virtual bool CompareFits(double fit1, double fit2);
      /** Returns the ideal fit for a perfect Gruen result */
      virtual double IdealFit() const {
        return (0.0);
      }

      // AutoReg adaptive method
      virtual AutoReg::RegisterStatus AdaptiveRegistration(Chip &sChip,
          Chip &pChip,
          Chip &fChip,
          int startSamp,
          int startLine,
          int endSamp,
          int endLine,
          int bestSamp,
          int bestLine);

      virtual Pvl AlgorithmStatistics(Pvl &pvl);

    private:

      /** Error enumeration values */
      enum GruenErrors { NotEnoughPoints = 1, CholeskyFailed = 2,
                         EigenSolutionFailed = 3, AffineNotInvertable = 4,
                         MaxIterationsExceeded = 5, RadShiftExceeded = 6,
                         RadGainExceeded = 7, MaxEigenExceeded = 8,
                         AffineDistExceeded = 9
                       };

      /** Structure that maintains error counts */
      struct ErrorCounter {
        ErrorCounter() : _gerrno(0), _keyname("Unknown"), _count(0) { }
        ErrorCounter(int gerrno, const std::string &keyname) : _gerrno(gerrno),
          _keyname(keyname), _count(0) { }
        inline int  Errno() const {
          return (_gerrno);
        }
        inline BigInt Count() const {
          return (_count);
        }
        inline void BumpIt() {
          _count++;
        }
        PvlKeyword LogIt() const {
          return (PvlKeyword(_keyname, _count));
        }
        int _gerrno;
        std::string _keyname;
        BigInt _count;
      };

      /// Declaration of error count list
      typedef CollectorMap<int, ErrorCounter> ErrorList;

      DbProfile _prof;
      int    _maxIters;
      double _transTol;
      double _scaleTol;
      double _shearTol;

      double _affineTol;
      double _spiceTol;

      double _rgainMinTol;
      double _rgainMaxTol;
      double _rshiftTol;
      double _fitChipScale;

      // Iteration loop variables
      int _nIters;
      GruenResult _result;  //!< last result, cummulative
      BigInt    _totalIterations;
      ErrorList _errors;
      BigInt    _unclassified;

      // These are for recomputing SMTK points
      double _defGain;
      double _defShift;

      //  Statistics gathered during process
      Statistics _eigenStat;
      Statistics _iterStat;
      Statistics _shiftStat;
      Statistics _gainStat;


      /**
       * @brief Helper method to initialize parameters
       *
       * This method will check the existance of a keyword and extract the value

       * if it exists to the passed parameter (type).  If it doesn't exist, the
       * default values is returned.
       *
       * @param T Templated variable type
       * @param conf Parameter profile container
       * @param keyname Name of keyword to get a value from
       * @param defval Default value it keyword/value doesn't exist
       * @param index Optional index of the value for keyword arrays
       *
       * @return T Return type
       */
      template <typename T>
      T ConfKey(const DbProfile &conf, const std::string &keyname,
                const T &defval, int index = 0) const {
        if(!conf.exists(keyname)) {
          return (defval);
        }
        if(conf.count(keyname) < index) {
          return (defval);
        }
        iString iValue(conf.value(keyname, index));
        T value = iValue;  // This makes it work with a string?
        return (value);
      };

      /**
       * @brief Keyword formatter for Gruen parameters
       *
       * Constructs a keyword with actual user/programmer values if provided,
       * otherwise sets the value to "Unbounded".
       *
       * @param T       Type of value to record
       * @param keyname Name of keyword to create
       * @param value   Value to set keyword to if in profile
       * @param unit    Optional unit value
       *
       * @return PvlKeyword Constructed keyword for key/value
       */
      template <typename T>
      PvlKeyword ParameterKey(const std::string &keyname,
                              const T &value,
                              const std::string &unit = "") const {
        if(_prof.exists(keyname)) {
          return(ValidateKey(keyname, value, unit));
        }
        return (PvlKeyword(keyname, "Unbounded"));
      };


      /**
       * @brief Checks value of key, produces appropriate value
       *
       * This function checks the value of the keyword for specialness
       * and will create the appropriate keyword if it is special.
       *
       * @param keyname Name of keyword to create
       * @param value   Keyword value
       * @param unit    Optional unit qualifer with value
       *
       * @return PvlKeyword Returns newly created keyword/value
       */
      inline PvlKeyword ValidateKey(const std::string keyname,
                                    const double &value,
                                    const std::string &unit = "") const {
        if(IsSpecial(value)) {
          return (PvlKeyword(keyname, "NULL"));
        }
        else {
          return (PvlKeyword(keyname, value, unit));
        }
      }

      ErrorList initErrorList() const;
      void logError(int gerrno, const std::string &gerrmsg);
      PvlGroup StatsLog() const;
      PvlGroup ParameterLog() const;

      void init(Pvl &pvl);
      /** Returns the default radiometric gain value */
      inline double getDefaultGain() const {
        return (_defGain);
      }
      /** Returns the default radiometric shift value */
      inline double getDefaultShift() const {
        return (_defShift);
      }
      void reset();
      void resetStats();

      GSLVector getThreshHold(const Chip &chip) const;
      bool HasConverged(const GSLVector &alpha, const GSLVector &thresh,
                        const GruenResult &results) const;
      BigInt MinValidPoints(BigInt totalPoints) const;
      bool ValidPoints(BigInt totalPoints, BigInt nPoints) const;
      void ErrorAnalysis(GruenResult &result);
      bool TestConstraints(const bool &done, GruenResult &result);
      Affine UpdateAffine(GruenResult &result, const Affine &gtrans);
      void UpdateChip(Chip &sChip, const Affine &affine);
      bool CheckAffineTolerance();
      AutoReg::RegisterStatus Status(const GruenResult &result) const;
      /** Returns status of the last Gruen registration result */
      AutoReg::RegisterStatus Status() const {
        return (Status(_result));
      }
  };

}  // namespace Isis
#endif
