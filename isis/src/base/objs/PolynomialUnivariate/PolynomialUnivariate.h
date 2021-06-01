#ifndef PolynomialUnivariate_h
#define PolynomialUnivariate_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <vector>
#include "Basis1VariableFunction.h"

namespace Isis {
  /**
   * @brief Nth degree Polynomial with one variable
   *
   * This is a derived class from Basis1VariableFunction which provides the
   * capabilities of a polynomial equation in one variable with degree n, where n
   * is specified during the object construction. For example, Degree = 1 z = a +
   * b*x Degree = 2 z = a + b*x + c*x**2 Degree = 3 z = a + b*x + c*x**2 + d*x**3
   * In general the number of coefficients will be degree + 1.
   *
   * @ingroup Math
   *
   * @author  Debbie A. Cook 2007-11-19 (modified from Poly2D, now
   *          named PolynominalBivariate, 2004-06-24 Jeff
   *          Anderson)
   *
   * @internal
   *
   *   @history 2008-01-11 Tracie Sucharski - Renamed from Poly1D, add derivative methods.
   *   @history 2008-02-05 Jeannie Walldren - Renamed from Polynomial1Variable.
   *   @history 2015-02-20 Jeannie Backer - Improved error messages.
   *   @history 2016-11-10 Kristin Berry - Added additional convenience constructor which
   *                                       accepts a vector of coeffs. References #3888. 
   */

  class PolynomialUnivariate : public Isis::Basis1VariableFunction {
    public:
      PolynomialUnivariate(int degree);
      PolynomialUnivariate(int degree, std::vector<double> coeffs);

      //! Destroys the PolynomialUnivariate object
      ~PolynomialUnivariate() {};

      void Expand(const std::vector<double> &vars);

      double DerivativeVar(const double value);
      double DerivativeCoef(const double value, const int coefIndex);

    private:
      int p_degree;   //!< The order/degree of the polynomial
  };
};

#endif

