#ifndef Basis1VariableFunction_h
#define Basis1VariableFunction_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>
#include <string>

#include <QString>

#include "BasisFunction.h"

namespace Isis {
  /**
   * @brief Time based linear equation class
   *
   * This is a class for generating a general one-variable equation for the Isis
   * least squares fitting algorithm (IsisLSQ). It allows the programmer to set up
   * equations in the form of:
   * @f[
   * x = C1*T1 + C2*T2 + ... + CN*TN;
   * @f]
   * where C1-CN are coefficients and T1-TN are terms with a single variable.
   * For example,
   * @f[
   * x = C1 + C2*t + C3*t**2
   * @f]
   *
   * @ingroup Math
   *
   * @author 2004-06-24 Jeff Anderson
   *
   * @internal
   *   @todo Add coded example
   *   @history 2005-03-16 Leah Dahmer - modified file to support Doxygen
   *                           documentation.
   *   @history 2008-01-08 Tracie Sucharski - Derived from BasisFunction class for
   *                          a single variable function.  Added Derivative
   *                          methods as pure virtuals.  This class was
   *                          developed as a convenience to simplify the
   *                          Derivative methods and any other methods that
   *                          might need to be developed in the future.
   *   @history 2017-08-30 Summer Stapleton - Updated documentation. References #4807.
   *
   */
  class Basis1VariableFunction : public Isis::BasisFunction {
    public:
      Basis1VariableFunction(const QString &name, int numCoefs);
      //! Destroys the Basis1VariableFunction object.
      virtual ~Basis1VariableFunction() {};

      /**
       * This will take the Derivative with respect to the variable and evaluate at
       * given value.
       *
       * @param [in] value   (const double)  value at which to evaluate derivative
       * 
       * @return (double) The derivative evaluated at given value
       *
       */
      virtual double DerivativeVar(const double value) = 0;

      /**
       *  Evaluate the derivative defined by the given coefficients with respect to the coefficient
       *  at the given index, at the current value.
       *
       * @param [in]  value      (const double) value at which to evaluate derivative
       * @param [in]  coefIndex  (const int)    The index of the coefficient to
       *                                          differentiate with respect to
       *
       * @return    (double) The derivative evaluated at given value
       *
       */
      virtual double DerivativeCoef(const double value, const int coefIndex) = 0;

    protected:

  };
};

#endif
