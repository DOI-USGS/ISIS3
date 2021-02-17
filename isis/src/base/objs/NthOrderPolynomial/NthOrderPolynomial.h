
#ifndef NthOrderPolynomial_h
#define NthOrderPolynomial_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <vector>
#include "BasisFunction.h"

namespace Isis {

  /**
   * @brief NthOrderPolynomial basis function
   *
   * This is a derived class from the BasisFunction class which creates an nth order polynomial.
   *  
   * @ingroup Math
   *
   * @author  2018-01-01 Unknown
   *
   * @internal
   *  @history 2018-01-01 Unknown - Initial Version
   *  @history 2020-01-08 Kristin Berry - Update documentation prior to checkin to dev.
   */

  class NthOrderPolynomial : public Isis::BasisFunction {
    public:
      NthOrderPolynomial(int degree);

      //! Destroys the NthOrderPolynomial object
      ~NthOrderPolynomial() {}

      void Expand(const std::vector<double> &vars);
    
  private:
    int p_degree;
  };

}
#endif

