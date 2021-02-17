/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>

#include "Basis1VariableFunction.h"
#include "IString.h"
#include "IException.h"

namespace Isis {

  /**
   * Creates a Basis Function with a single variable.
   *
   * @param name Name of the Basis1VariableFunction. For example, "affine".
   * @param numCoefs Number of coefficients in the equation. For example:
   * @f[
   * x = C1 + C2 * x + C3 * x**2
   * @f]
   * has three coefficients: C1, C2 & C3.
   */
  Basis1VariableFunction::Basis1VariableFunction(const QString &name, int numCoefs) :
    Isis::BasisFunction(name, 1, numCoefs) {
  }

}
