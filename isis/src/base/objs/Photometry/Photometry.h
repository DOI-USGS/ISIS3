#ifndef Photometry_h
#define Photometry_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_min.h>
#include <gsl/gsl_roots.h>

namespace Isis {
  class Pvl;
  class PhotoModel;
  class AtmosModel;
  class NormModel;
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *  @history 2007-08-02 Steven Lambright - Fixed memory leak
   *  @history 2008-03-07 Janet Barrett - Added SetPhotomWl method to allow
   *                      the application to set the p_normWavelength variable for
   *                      use by MoonAlbedo normalization.
   *  @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
   *  @history 2008-07-09 Steven Lambright - Fixed unit test
   *  @history 2011-08-19 Sharmila Prasad - Implemented brentminimizer using GSL
   *  @history 2011-09-15 Sharmila Prasad - Implemented brent's root solver using GSL
   */
  class Photometry {
    public:
      Photometry(Pvl &pvl);
      Photometry() {};
      virtual ~Photometry();

      //! Calculate the surface brightness
      void Compute(double pha, double inc, double ema, double dn,
                   double &albedo, double &mult, double &base);
      void Compute(double pha, double inc, double ema, double deminc,
                   double demema, double dn, double &albedo,
                   double &mult, double &base);

      //! Set the wavelength
      virtual void SetPhotomWl(double wl);

      //! Double precision version of bracketing algorithm ported from Python.
      //! Solution bracketing for 1-D minimization routine.
      static void minbracket(double &xa, double &xb, double &xc, double &fa,
          double &fb, double &fc, double Func(double par, void *params),
          void *params);

      //! Brent's method 1-D minimization routine using GSL's r8Brent minimization Algorithm
      static int brentminimizer(double x_lower, double x_upper, gsl_function *Func, 
          double & x_minimum, double tolerance);
      
      //! GSL's the Brent-Dekker method (Brent's method) combines an interpolation strategy 
      //! with the bisection algorithm to estimate the root of the quadratic function.
      static int brentsolver(double x_lo, double x_hi, gsl_function *Func, double tolerance, double &root);

      PhotoModel *GetPhotoModel() const {
        return p_phtPmodel;
      }

      AtmosModel *GetAtmosModel() const {
        return p_phtAmodel;
      }

      NormModel *GetNormModel() const {
        return p_phtNmodel;
      }

    protected:      
      AtmosModel *p_phtAmodel;
      PhotoModel *p_phtPmodel;
      NormModel *p_phtNmodel;
  };
};

#endif
