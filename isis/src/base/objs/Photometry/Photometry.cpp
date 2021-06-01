/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "IException.h"
#include "IString.h"
#include "Pvl.h"
#include "Photometry.h"
#include "PhotoModelFactory.h"
#include "PhotoModel.h"
#include "AtmosModelFactory.h"
#include "AtmosModel.h"
#include "NormModelFactory.h"
#include "NormModel.h"
#include "Plugin.h"
#include "FileName.h"

namespace Isis {
  /**
   * Create Photometry object.
   *
   * @param pvl  A pvl object containing a valid Photometry specification
   *
   * @see photometry.doc
   */
  Photometry::Photometry(Pvl &pvl) {
    p_phtAmodel = NULL;
    p_phtPmodel = NULL;
    p_phtNmodel = NULL;
    if(pvl.hasObject("PhotometricModel")) {
      p_phtPmodel = PhotoModelFactory::Create(pvl);
    } else {
      std::string msg = "A Photometric model must be specified to do any type of photometry";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    if(pvl.hasObject("AtmosphericModel")) {
      p_phtAmodel = AtmosModelFactory::Create(pvl, *p_phtPmodel);
    }
    if (pvl.hasObject("NormalizationModel")) {
      if (p_phtAmodel != NULL) {
        p_phtNmodel = NormModelFactory::Create(pvl, *p_phtPmodel, *p_phtAmodel);
      } else {
        p_phtNmodel = NormModelFactory::Create(pvl, *p_phtPmodel);
      }
    }
  }

  //! Destroy Photometry object
  Photometry::~Photometry() {
    if(p_phtAmodel != NULL) {
      delete p_phtAmodel;
      p_phtAmodel = NULL;
    }

    if(p_phtPmodel != NULL) {
      delete p_phtPmodel;
      p_phtPmodel = NULL;
    }

    if(p_phtNmodel != NULL) {
      delete p_phtNmodel;
      p_phtNmodel = NULL;
    }
  }

  /**
   * Set the wavelength parameter. This value is obtained
   * from the BandBin Center keyword of the image.
   *
   */
  void Photometry::SetPhotomWl(double wl) {
    p_phtNmodel->SetNormWavelength(wl);
  }

  /**
   * Calculate the surface brightness using only ellipsoid
   *
   * @return  Returns the surface brightness
   *
   */
  void Photometry::Compute(double pha, double inc, double ema,
                           double dn, double &albedo, double &mult,
                           double &base) {

    // Calculate the surface brightness
    p_phtNmodel->CalcNrmAlbedo(pha, inc, ema, dn, albedo, mult, base);
    return;
  }

  /**
   * Calculate the surface brightness using ellipsoid and dem
   *
   * @return  Returns the surface brightness
   *
   */
  void Photometry::Compute(double pha, double inc, double ema,
                           double deminc, double demema, double dn,
                           double &albedo, double &mult, double &base) {

    // Calculate the surface brightness
    p_phtNmodel->CalcNrmAlbedo(pha, inc, ema, deminc, demema, dn, albedo, mult, base);
    return;
  }

  /**
   * GSL's the Brent-Dekker method (referred to here as Brent's method) combines an
   * interpolation strategy with the bisection algorithm. This produces a fast algorithm
   * which is still robust.On each iteration Brent's method approximates the function
   * using an interpolating curve. On the first iteration this is a linear interpolation
   * of the two endpoints. For subsequent iterations the algorithm uses an inverse quadratic
   * fit to the last three points, for higher accuracy. The intercept of the interpolating
   * curve with the x-axis is taken as a guess for the root. If it lies within the bounds
   * of the current interval then the interpolating point is accepted, and used to generate
   * a smaller interval. If the interpolating point is not accepted then the algorithm falls
   * back to an ordinary bisection step.
   *
   * The best estimate of the root is taken from the most recent interpolation or bisection.
   *
   * @author Sharmila Prasad (9/15/2011)
   *
   * @param x_lo      - Initial lower search interval
   * @param x_hi      - Initial higher search interval
   * @param Func      - Continuous function of one variable for the root finders to operate on
   * @param tolerance - Root Error Tolerance
   * @param root      - Output calculated root
   *
   * @return int      - Algorithm status
   */
  int Photometry::brentsolver(double x_lo, double x_hi, gsl_function *Func, double tolerance, double &root){
    int status;
    int iter=0, max_iter=100;
    const gsl_root_fsolver_type *T;
    gsl_root_fsolver *s;

    T = gsl_root_fsolver_brent;
    s = gsl_root_fsolver_alloc (T);
    gsl_root_fsolver_set(s, Func, x_lo, x_hi);

    do {
      iter++;
      status = gsl_root_fsolver_iterate(s);
      root   = gsl_root_fsolver_x_lower(s);
      x_lo   = gsl_root_fsolver_x_lower(s);
      x_hi   = gsl_root_fsolver_x_upper(s);
      status = gsl_root_test_interval(x_lo, x_hi, 0, tolerance);

    } while (status != GSL_SUCCESS && iter < max_iter);

    gsl_root_fsolver_free(s);
    return status;
  }

  /**
   * The Brent minimization algorithm combines a parabolic interpolation with the golden section algorithm.
   * This produces a fast algorithm which is still robust. The outline of the algorithm can be summarized as
   * follows: on each iteration Brent's method approximates the function using an interpolating parabola
   * through three existing points. The minimum of the parabola is taken as a guess for the minimum.
   * If it lies within the bounds of the current interval then the interpolating point is accepted,
   * and used to generate a smaller interval. If the interpolating point is not accepted then the
   * algorithm falls back to an ordinary golden section step. The full details of Brent's method
   * include some additional checks to improve convergence.
   *
   * @author Sharmila Prasad (8/15/2011)
   *
   * @param x_lower - x_lower interval
   * @param x_upper - x_upper interval
   * @param Func - gsl_function, high-level driver for the algorithm
   *               Continuous function of one variable for the minimizers to operate on
   * @param x_minimum - x_minimum calculated parabola min value
   * @return double - status
   */
  int Photometry::brentminimizer(double x_lower, double x_upper, gsl_function *Func,
      double & x_minimum, double tolerance) {
    int status;
    int iter=0, max_iter=100;

    const gsl_min_fminimizer_type *T;
    gsl_min_fminimizer *s;
    //double m_expected = M_PI;

    T = gsl_min_fminimizer_brent;
    s = gsl_min_fminimizer_alloc(T);

    // This function sets, or resets, an existing minimizer s to use the function Func and
    // the initial search interval [x_lower, x_upper], with a guess for the location of
    // the minimum x_minimum. If the interval given does not contain a minimum, then
    // the function returns an error code of GSL_EINVAL.
    gsl_min_fminimizer_set(s, Func, x_minimum, x_lower, x_upper);

    do {
      iter++;
      status    = gsl_min_fminimizer_iterate(s);
      x_minimum = gsl_min_fminimizer_x_minimum(s);
      x_lower   = gsl_min_fminimizer_x_lower(s);
      x_upper   = gsl_min_fminimizer_x_upper(s);

      status = gsl_min_test_interval(x_lower, x_upper, tolerance, 0.0);
    } while(status == GSL_CONTINUE && iter < max_iter);

    // This function frees all the memory associated with the minimizer s.
    gsl_min_fminimizer_free(s);

    return status;
  }

  /**
   * This bracketing algorithm was taken from
   *    http://cxc.harvard.edu/sherpa/methods/fminpowell.py.txt
   * and converted to C++.
   */
  void Photometry::minbracket(double &xa, double &xb, double &xc, double &fa, double &fb,
      double &fc, double Func(double par, void *params), void *params) {
    double eps = 1.0e-21;
    double Gold = 1.618034;
    double GrowLimit = 110;
    int maxiter = 1000;

    fa = Func(xa, params);
    fb = Func(xb, params);
    if (fa < fb) {
      double tmp = xa;
      xa = xb;
      xb = tmp;
      tmp = fa;
      fa = fb;
      fb = tmp;
    }
    xc = xb + Gold * (xb - xa);
    fc = Func(xc, params);
    int iter = 0;

    while (fc < fb) {
      double tmp1 = (xb - xa) * (fb - fc);
      double tmp2 = (xb - xc) * (fb - fa);
      double val = tmp2 - tmp1;
      double denom;
      if (fabs(val) < eps) {
        denom = 2.0 * eps;
      } else {
        denom = 2.0 * val;
      }
      double w = xb - ((xb -xc) * tmp2 - (xb - xa) * tmp1) / denom;
      double wlim = xb + GrowLimit * (xc - xb);
      if (iter > maxiter) {
        IString msg = "Maximum iterations exceeded in minimum bracketing ";
        msg += "algorithm (minbracket) - root cannot be bracketed";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      iter = iter + 1;
      double fw;
      if (((w-xc)*(xb-w)) > 0.0) {
        fw = Func(w, params);
        if (fw < fc) {
          xa = xb;
          xb = w;
          fa = fb;
          fb = fw;
          return;
        } else if (fw > fb) {
          xc = w;
          fc = fw;
          return;
        }
        w = xc + Gold * (xc - xb);
        fw = Func(w, params);
      } else if (((w-wlim)*(wlim-xc)) >= 0.0) {
        w = wlim;
        fw = Func(w, params);
      } else if (((w-wlim)*(xc-w)) > 0.0) {
        fw = Func(w, params);
        if (fw < fc) {
          xb = xc;
          xc = w;
          w = xc + Gold * (xc - xb);
          fb = fc;
          fc = fw;
          fw = Func(w, params);
        }
      } else {
        w = xc + Gold * (xc - xb);
        fw = Func(w, params);
      }
      xa = xb;
      xb = xc;
      xc = w;
      fa = fb;
      fb = fc;
      fc = fw;
    }
    return;
  }
}
