#ifndef PolynomialBivariate_h
#define PolynomialBivariate_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <vector>
#include "BasisFunction.h"

namespace Isis {
  /**
   * @brief Nth degree Polynomial with two variables
   *
   * This is a derived class from BasisFunction which provides the capabilities
   * of a polynomial equation in two variables with degree n, where n is specified
   * during the object construction. For example, Degree = 1 z = a + b*x + c*y
   * Degree = 2 z = a + b*x + c*y + d*x**2 + e*x*y + f*y**2
   * Degree = 3 z = a + b*x + c*y + d*x**2 + e*x*y + f*y**2 + g*x**3 + h*x**2*y
   * + i*x*y**2 + j*y**3 In general the number of coefficients will be
   * ((degree+1)*(degree+2))/2.
   *
   * @ingroup Math
   *
   * @author  2004-06-24 Jeff Anderson
   *
   * @internal
   *  @history 2005-03-11 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *
   *  @todo 2005-03-11 Jeff Anderson - add coded and implementation examples to
   *                                   class documentation
   *  @history 2008-02-05 Jeannie Walldren, Renamed from Poly2D.
   */

  class PolynomialBivariate : public Isis::BasisFunction {
    public:
      PolynomialBivariate(int degree);

      //! Destroys the PolynomialBivariate object
      ~PolynomialBivariate() {};

      void Expand(const std::vector<double> &vars);

    private:
      int p_degree;   //!< The order/degree of the polynomial
  };
};

#endif

