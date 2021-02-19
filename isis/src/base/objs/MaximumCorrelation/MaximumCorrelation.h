#ifndef MaximumCorrelation_h
#define MaximumCorrelation_h
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
   * @brief Maximum correlation pattern matching
   *
   * This class is used to construct a maximum correlation pattern matching
   * algorith.  That is, given a search chip and a pattern chip, the pattern
   * chip is walked through the search chip.  At each position the a sub-search
   * chip is extracted which is the same size as the pattern chip.  Then the
   * correlation between the two is computed.  The best fit = 1.0 which means
   * the pattern chip and sub-search chip are identical
   *
   * @ingroup PatternMatching
   *
   * @see MinimumDifference AutoReg
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2006-01-11 Jacob Danton Added idealFit value, unitTest
   *   @history 2006-03-08 Jacob Danton Added sampling options
   */
  class MaximumCorrelation : public AutoReg {
    public:
      MaximumCorrelation(Pvl &pvl) : AutoReg(pvl) { };
      virtual ~MaximumCorrelation() {};

    protected:
      virtual double MatchAlgorithm(Chip &pattern, Chip &subsearch);
      virtual bool CompareFits(double fit1, double fit2);
      virtual double IdealFit() const {
        return 1.0;
      };
      virtual QString AlgorithmName() const {
        return "MaximumCorrelation";
      };

  };
};

#endif
