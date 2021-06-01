/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include "PolynomialBivariate.h"

namespace Isis {
  /**
   * Create a PolynomialBivariate object
   *
   * @param degree The order/degree of the polynomial
   *
   */
  PolynomialBivariate::PolynomialBivariate(int degree) :
    Isis::BasisFunction("PolynomialBivariate", 2, ((degree + 1) * (degree + 2)) / 2) {
    p_degree = degree;
  }

  /**
   * This is the the overriding virtual function that provides the expansion of
   * the two input variables into the polynomial equation.
   * See BasisFunction for more information.
   *
   * @param vars A vector of double values to use for the expansion.
   */
  void PolynomialBivariate::Expand(const std::vector<double> &vars) {
    std::vector<std::vector<double> > terms;
    terms.resize(p_degree + 1);
    terms[0].push_back(1.0);
    for(int i = 1; i <= p_degree; i++) {
      for(int t = 0; t < (int)terms[i-1].size(); t++) {
        terms[i].push_back(terms[i-1][t] * vars[0]);
        if(t == ((int)terms[i-1].size() - 1)) {
          terms[i].push_back(terms[i-1][t] * vars[1]);
        }
      }
    }

    p_terms.clear();
    for(int i = 0; i <= p_degree; i++) {
      for(int t = 0; t < (int)terms[i].size(); t++) {
        p_terms.push_back(terms[i][t]);
      }
    }
  }
} // end namespace isis

