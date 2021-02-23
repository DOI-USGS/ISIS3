#ifndef MoonAlbedo_h
#define MoonAlbedo_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "NormModel.h"

namespace Isis {
  class Pvl;

  /**
   * @brief Albedo dependent phase function normalization for the Moon
   *
   * @author 1998-12-21 Randy Kirk
   *
   * @internal
   *  @history 2010-11-30 Janet Barrett - Added ability to use photometric angles
   *                      from the ellipsoid or the DEM
   *
   */
  class MoonAlbedo : public NormModel {
    public:
      MoonAlbedo(Pvl &pvl, PhotoModel &pmodel);
      virtual ~MoonAlbedo() {};

    protected:
      virtual void NormModelAlgorithm(double pha, double inc, double ema,
                                      double dn, double &albedo, double &mult, double &base) {};
      virtual void NormModelAlgorithm(double pha, double inc, double ema,
                                      double deminc, double demema, double dn, double &albedo,
                                      double &mult, double &base);

    private:
      //! Set parameters needed for albedo dependent phase function
      //! normalization for the Moon
      void SetNormD(const double d);
      void SetNormE(const double e);
      void SetNormF(const double f);
      void SetNormG2(const double g2);
      void SetNormXmul(const double xmul);
      void SetNormWl(const double wl);
      void SetNormH(const double h);
      void SetNormBsh1(const double bsh1);
      void SetNormXb1(const double xb1);
      void SetNormXb2(const double xb2);

      double p_normD;
      double p_normE;
      double p_normF;
      double p_normG2;
      double p_normXmul;
      double p_normWl;
      double p_normH;
      double p_normBsh1;
      double p_normXb1;
      double p_normXb2;
      double p_normF1;
      double p_normG2sq;
      double p_normPg30;
      double p_normBc1;
      double p_normFbc3;
      double p_normC3;
      double p_normPg32;
      double p_normBshad3;
  };
};

#endif
