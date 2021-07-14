#if !defined(Anisotropic2_h)
#define Anisotropic2_h
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
   *                          code.
   *  @history 2007-02-20 Janet Barrett - Imported from Isis2.
   *  @history 2007-08-15 Steven Lambright - Refactored code
   *  @history 2008-03-07 Janet Barrett - Moved code to set standard
   *                          conditions to the AtmosModel class
   *  @history 2008-06-18 Christopher Austin - Fixed documentation error
   *  @history 2008-11-05 Jeannie Walldren - Replaced references
   *                          to NumericalMethods::r8expint() with
   *                          AtmosModel::En(), NumericalMethods::G11Prime()
   *                          with AtmosModel::G11Prime(), and
   *                          NumericalMethods::r8ei() with AtmosModel::Ei().
   *                          Added documentation from Isis2.
   *  @history 2011-08-17 Sharmila Prasad - Moved common HNORM to base AtmosModel
   *  @history 2011-12-19 Janet Barrett - Added code to estimate the
   *                          shadow brightness value (transs). Also got rid of
   *                          unnecessary check for identical photometric angle values
   *                          between successive calls. This check should only be
   *                          made in the photometric models.
   *  @history 2017-07-03 Makayla Shepherd - Updated documentation. References #4807.
   */
  class Anisotropic2 : public AtmosModel {
    public:
      Anisotropic2(Pvl &pvl, PhotoModel &pmodel);
      virtual ~Anisotropic2() {};

    protected:
      virtual void AtmosModelAlgorithm(double phase, double incidence,
                                       double emission);

    private:
      double p_wha2;      //!< ???
      double p_wham;      //!< ???
      double p_e1;        //!< ???
      double p_e1_2;      //!< ???
      double p_e2;        //!< ???
      double p_e3;        //!< ???
      double p_e4;        //!< ???
      double p_e5;        //!< ???
      double p_em;        //!< ???
      double p_e;         //!< ???
      double p_f1m;       //!< ???
      double p_f2m;       //!< ???
      double p_f3m;       //!< ???
      double p_f4m;       //!< ???
      double p_g12;       //!< ???
      double p_g13;       //!< ???
      double p_g14;       //!< ???
      double p_g32;       //!< ???
      double p_g33;       //!< ???
      double p_g34;       //!< ???
      double p_f1;        //!< ???
      double p_f2;        //!< ???
      double p_f3;        //!< ???
      double p_f4;        //!< ???
      double p_g11p;      //!< ???
      double p_g12p;      //!< ???
      double p_g13p;      //!< ???
      double p_g14p;      //!< ???
      double p_g32p;      //!< ???
      double p_g33p;      //!< ???
      double p_g34p;      //!< ???
      double p_x0_0;      //!< ???
      double p_y0_0;      //!< ???
      double p_x0_1;      //!< ???
      double p_y0_1;      //!< ???
      double p_delta_0;   //!< ???
      double p_delta_1;   //!< ???
      double p_alpha0_0;  //!< ???
      double p_alpha1_0;  //!< ???
      double p_beta0_0;   //!< ???
      double p_beta1_0;   //!< ???
      double p_fac;       //!< ???
      double p_den;       //!< ???
      double p_p0;        //!< ???
      double p_q0;        //!< ???
      double p_p1;        //!< ???
      double p_q1;        //!< ???
      double p_q02p02;    //!< ???
      double p_q12p12;    //!< ???
  };
};

#endif
