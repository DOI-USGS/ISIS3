#ifndef GaussianDistribution_h
#define GaussianDistribution_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Statistics.h"
#include "IException.h"
#include "Constants.h"

namespace Isis {
  /**
   * @brief gaussian distribution class
   *
   * This class is used to calculate the probability distribution
   * function, the cumulative distribution function, and the
   * inverse cumulative distribution function of a gaussian (or
   * normal) distribution.
   *
   * @ingroup Statistics
   *
   * @author 2006-05-25 Jacob Danton
   *
   * @internal
   *   @history 2006-05-25 Jacob Danton Original Version
   *   @history 2007-07-09 Janet Barrett - Removed invalid declaration of
   *                                       "static const double" variables
   *                                       p_lowCutoff and p_highCutoff from
   *                                       this file and moved them to the
   *                                       GaussianDistribution.cpp file. The
   *                                       variables were also renamed to
   *                                       lowCutoff and highCutoff.
   */
  class GaussianDistribution : public Isis::Statistics {
    public:
      GaussianDistribution(const double mean = 0.0, const double standardDeviation = 1.0) ;
      ~GaussianDistribution() {};

      double Probability(const double value);
      double CumulativeDistribution(const double value);
      double InverseCumulativeDistribution(const double percent);

      /**
      * Returns the mean.
      *
      * @returns The mean
      */
      inline double Mean() const {
        return p_mean;
      };

      /**
      * Returns the standard deviation.
      *
      * @returns The standard deviation
      */
      inline double StandardDeviation() const {
        return p_stdev;
      };

    private:
      //! Value of the mean
      double p_mean;
      //! Value of the standard deviation
      double p_stdev;

      // functions used for computing the ICDF
      double A(const double x);
      double B(const double x);
      double C(const double x);
      double D(const double x);
  };
};

#endif
