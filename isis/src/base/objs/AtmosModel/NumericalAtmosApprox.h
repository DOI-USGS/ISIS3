#ifndef NUMERICALATMOSAPPROX_H
#define NUMERICALATMOSAPPROX_H
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>
#include <vector>

#include "NumericalApproximation.h"
using namespace std;
namespace Isis {
  class AtmosModel;
  /**
   * This class extends @b Isis::NumericalApproximation. It was
   * created to handle numerical integration methods for specific
   * atmospheric functions in the @b Isis::AtmosModel class.
   * Rather than using a data set to interpolate a function, these
   * methods can take in a pointer to an AtmosModel object and an
   * enumerated value for the specific function that needs to be
   * integrated.
   *
   * @ingroup Math and RadiometricAndPhotometricCorrection
   *
   * @author 2008-11-05 Jeannie Walldren
   * @internal
   *   @history 2008-11-05 Jeannie Walldren - Original Version.
   *   @history 2008-11-07 Jeannie Walldren - Fixed documentation
   */
  class NumericalAtmosApprox : public NumericalApproximation {
    public:
      //! Uses @b Isis::NumericalApproximation constructor
      NumericalAtmosApprox(const NumericalApproximation::InterpType &itype = CubicNatural): NumericalApproximation(itype) {};
      //! Empty destructor.
      virtual ~NumericalAtmosApprox() {};

      /**
       * This enum defines function to be integrated by Romberg's
       * method.
       */
      enum IntegFunc { OuterFunction, //!< Indicates that Romberg's method will integrate the function OutrFunc2Bint()
                       InnerFunction  //!< Indicates that Romberg's method will integrate the function InrFunc2Bint()
                     };
      double RombergsMethod(AtmosModel *am, IntegFunc sub, double a, double b);
      double RefineExtendedTrap(AtmosModel *am, IntegFunc sub, double a, double b, double s, unsigned int n);

      static double OutrFunc2Bint(AtmosModel *am, double phi);
      static double InrFunc2Bint(AtmosModel *am, double mu);

  };
};
#endif


