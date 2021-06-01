#ifndef MinimumDifference_h
#define MinimumDifference_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AutoReg.h"

namespace Isis {
  class Pvl;
  class Chip;

  /**
   * @brief Minimum difference pattern matching
   *
   * This class is used to construct a minimum difference pattern matching
   * algorith.  That is, given a search chip and a pattern chip, the pattern
   * chip is walked through the search chip.  At each position the a sub-search
   * chip is extracted which is the same size as the pattern chip.  Then the
   * absolute value of the difference is computed at each matching pixel in the
   * pattern and sub-search chip.  These differences are then summed to produce
   * the goodness of fit.  The sub-search chip with the lowest goodness of fit
   * will be identified as the pattern match (if a tolerance is met).  The best
   * fit = 0 which means the pattern chip and sub-search chip are identical
   *
   * @ingroup PatternMatching
   *
   * @see MinimumDifference AutoReg
   *
   * @author  2005-05-05 Jeff Anderson
   *
   * @internal
   *   @history 2006-01-11 Jacob Danton Added idealFit value, unitTest
   *   @history 2006-03-08 Jacob DAnton Added sampling options
   *   @history 2006-03-20 Jacob Danton Changed to *average* minimum
   *                                     difference algorithm.
   */
  class MinimumDifference : public AutoReg {
    public:
      /**
       * @brief Construct a MinimumDifference search algorithm
       *
       * This will construct a minimum difference search algorith.  It is
       * recommended that you use a AutoRegFactory class as opposed to
       * this constructor
       *
       * @param pvl  A Pvl object that contains a valid automatic registration
       * definition
       */
      MinimumDifference(Pvl &pvl) : AutoReg(pvl) { };

      //! Destructor
      virtual ~MinimumDifference() {};

      /**
       * Minimum tolerance specific to algorithm
       */
      virtual double MostLenientTolerance() {
        return DBL_MAX;
      }

    protected:
      virtual double MatchAlgorithm(Chip &pattern, Chip &subsearch);
      virtual bool CompareFits(double fit1, double fit2);
      virtual double IdealFit() const {
        return 0.0;
      };
      virtual QString AlgorithmName() const {
        return "MinimumDifference";
      };

  };
};

#endif
