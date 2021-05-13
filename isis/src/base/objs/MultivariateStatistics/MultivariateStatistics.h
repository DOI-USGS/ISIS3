
#ifndef MultivariateStatistics_h
#define MultivariateStatistics_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Constants.h"
#include "PvlObject.h"
#include "SpecialPixel.h"
#include "Statistics.h"

namespace Isis {

  /**
   * @brief Container of multivariate statistics.
   *
   * This class is used to accumulate multivariate statisics on two double arrays.
   * In particular, it is highly useful for obtaining the covariance, correlation,
   * and linear regression analysis on the the data.   It ignores input values
   * which are Isis special pixel values.  That is, if either co-aligned double
   * value is a special pixel then both values are not used in any statistical
   * computation.
   *
   * @see Histogram
   * @see Stats
   *
   * @ingroup Statistics
   *
   * @author 2004-06-08 Jeff Anderson
   *
   * @internal
   *   @history 2005-03-28 Leah Dahmer modified file to support Doxygen
   *                           documentation.
   *   @history 2005-05-23 Jeff Anderson - Added 2GB+ file support
   *   @history 2012-01-03 Steven Lambright - Added AddData(double, double,
   *                           unsigned int) for a significant performance
   *                           improvement and to increase the consistency in
   *                           the API relative to the Statistics class.
   *   @history 2016-07-15 Ian Humphrey - Added constructor to initialize a MultivariateStatistics
   *                           object from a PvlObject. Added fromPvl() and toPvl() methods to allow
   *                           for serialization/unserialization with PvlObjects. Updated unit test.
   *                           References #2282.
   *
   *   @todo This class needs an example.
   *   @todo For the below methods we will need to compute log x, loy y, sumx3,
   *    sumx4,sumx2y:
   *    void ExponentialRegression (double &a, double &b) const;
   *    void PowerRegression (double &a, double &b) const;
   *    void parabolicRegression (double &a, double &b, double &c);
   */
  class MultivariateStatistics {
    public:
      MultivariateStatistics();
      MultivariateStatistics(const PvlObject &inStats);
      ~MultivariateStatistics();

      void Reset();
      void AddData(const double *x, const double *y,
                   const unsigned int count);
      void AddData(double x, double y, unsigned int count = 1);
      void RemoveData(const double *x, const double *y,
                      const unsigned int count);

      Isis::Statistics X() const;
      Isis::Statistics Y() const;
      double SumXY() const;
      double Covariance() const;
      double Correlation() const;

      void LinearRegression(double &a, double &b) const;

      BigInt ValidPixels() const;
      BigInt InvalidPixels() const;
      BigInt TotalPixels() const;

      PvlObject toPvl(QString name = "MultivariateStatistics") const;

    private:

      void fromPvl(const PvlObject &inStats);

      //! A Statistics object holding x data.
      Isis::Statistics p_x;
      //! A Statistics object holding y data.
      Isis::Statistics p_y;

      //! The sum of x and y.
      double p_sumxy;
      /**
       * The number of valid (computed) pixels.
       * @internal
       */
      BigInt p_validPixels;
      /**
       * The number of invalid (ignored) pixels.
       * @internal
       */
      BigInt p_invalidPixels;
      /**
       * The total number of pixels (invalid and valid).
       * @internal
       */
      BigInt p_totalPixels;
  };
};

#endif
