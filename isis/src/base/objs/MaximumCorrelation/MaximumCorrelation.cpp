#include "MaximumCorrelation.h"
#include "Chip.h"
#include "MultivariateStatistics.h"

namespace Isis {
  double MaximumCorrelation::MatchAlgorithm (Chip &pattern, Chip &subsearch) {    
    MultivariateStatistics mv;
    std::vector <double> pdn, sdn;
    pdn.resize(pattern.Samples());
    sdn.resize(pattern.Samples());

    for (int l=1; l<=pattern.Lines(); l++) {
      for (int s=1; s<=pattern.Samples(); s++) {
        pdn[s-1] = pattern.GetValue(s,l);
        sdn[s-1] = subsearch.GetValue(s,l);
      }
      mv.AddData(&pdn[0],&sdn[0], pattern.Samples());
    }
    double percentValid = (double) mv.ValidPixels() / 
                     (pattern.Lines() * pattern.Samples());
    if (percentValid * 100.0 < this->PatternValidPercent()) return Isis::Null;

    double r = mv.Correlation();
    if (r == Isis::Null) return Isis::Null;
    return fabs(r);
  }

  /**
   * This virtual method must return if the 1st fit is equal to or better 
   * than the second fit.
   * 
   * @param fit1  1st goodness of fit 
   * @param fit2  2nd goodness of fit
   */
  bool MaximumCorrelation::CompareFits (double fit1, double fit2) {
    return (fit1 >= fit2);
  }
}

extern "C" Isis::AutoReg *MaximumCorrelationPlugin (Isis::Pvl &pvl) {
  return new Isis::MaximumCorrelation(pvl);
}

