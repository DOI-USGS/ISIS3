#if !defined(Anisotropic1_h)
#define Anisotropic1_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AtmosModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief
   *
   *
   * @ingroup RadiometricAndPhotometricCorrection
   * @author 1998-12-21 Randy Kirk
   *
   * @internal
   *  @history 1998-12-21 Randy Kirk - USGS, Flagstaff - Original
   *          code.
   *  @history 2007-02-20 Janet Barrett - Imported from Isis2.
   *  @history 2007-08-15 Steven Lambright - Refactored code
   *  @history 2008-03-07 Janet Barrett - Moved code to set standard
   *                          conditions to the AtmosModel class
   *  @history 2008-06-18 Christopher Austin - Fixed documentation error
   *  @history 2008-11-05 Jeannie Walldren - Replaced reference to
   *                          NumericalMethods::r8expint() with AtmosModel::En().
   *                          Added documentation from Isis2.
   *  @history 2011-08-17 Sharmila Prasad - Moved common HNORM to base AtmosModel
   *  @history 2011-12-19 Janet Barrett - Added code to estimate the
   *                          shadow brightness value (transs). Also got rid of
   *                          unnecessary check for identical photometric angle values
   *                          between successive calls. This check should only be
   *                          made in the photometric models.
   *  @history 2017-07-03 Makayla Shepherd - Updated documentation. References #4807.
   */
  class Anisotropic1 : public AtmosModel {
    public:
      Anisotropic1(Pvl &pvl, PhotoModel &pmodel);
      virtual ~Anisotropic1() {};

    protected:
      virtual void AtmosModelAlgorithm(double phase, double incidence,
                                       double emission);

    private:
      double p_atmosE2;         //!< ???
      double p_atmosE3;         //!< ???
      double p_atmosE4;         //!< ???
      double p_atmosE5;         //!< ???
      double p_atmosDelta_0;    //!< ???
      double p_atmosDelta_1;    //!< ???
      double p_atmosAlpha0_0;   //!< ???
      double p_atmosAlpha1_0;   //!< ???
      double p_atmosBeta0_0;    //!< ???
      double p_atmosBeta1_0;    //!< ???
      double p_atmosWha2;       //!< ???
      double p_atmosWham;       //!< ???
      double p_atmosX0_0;       //!< ???
      double p_atmosY0_0;       //!< ???
      double p_atmosX0_1;       //!< ???
      double p_atmosY0_1;       //!< ???
      double p_atmosFac;        //!< ???
      double p_atmosDen;        //!< ???
      double p_atmosQ0;         //!< ???
      double p_atmosQ1;         //!< ???
      double p_atmosP0;         //!< ???
      double p_atmosP1;         //!< ???
      double p_atmosQ02p02;     //!< ???
      double p_atmosQ12p12;     //!< ???
  };
};

#endif
