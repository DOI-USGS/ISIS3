#ifndef SurfaceModel_h
#define SurfaceModel_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>
#include "PolynomialBivariate.h"
#include "LeastSquares.h"

namespace Isis {
  /**
   * @brief Model a 3-D surface
   *
   * Given a set of (x,y,z) triplets, this class will model the surface
   * that best fits the points.  The equation to be modelled is:
   *
   * @f[
   * z = a + b*x + c*y + d*x^2 + e*x*y + f*y^2
   * @f]
   *
   * @ingroup Math
   *
   * @author 2005-05-09 Jeff Anderson
   *
   * @internal
   *   @history 2008-06-18 Steven Lambright Fixed ifndef command
   *
   * @todo Add plot and/or visualize method
   */
  class SurfaceModel {
    public:
      SurfaceModel();
      ~SurfaceModel();

      void AddTriplet(const double x, const double y, const double z);
      void AddTriplets(const double *x, const double *y, const double *z,
                       const int n);
      void AddTriplets(const std::vector<double> &x,
                       const std::vector<double> &y,
                       const std::vector<double> &z);

      void Solve();
      double Evaluate(const double x, const double y);

      int MinMax(double &x, double &y);

    private:
      LeastSquares *p_lsq;
      PolynomialBivariate *p_poly2d;
  };
};

#endif
