#ifndef NonLinearLSQ_h
#define NonLinearLSQ_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */                                                                    

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include "tnt_array1d.h"
#include "tnt_array1d_utils.h"
#include "tnt_array2d.h"
#include "tnt_array2d_utils.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>

#include "IException.h"

namespace Isis {

  /**
   * @brief NonLinearLSQ Computes a fit using a Levenberg-Marquardt algorithm
   *
   * This virtual base class uses the GSL toolkit to apply the
   * Levenberg-Marquardt algorithm to fit data to a non-linear equation using
   * least squares.
   *
   * @ingroup Utility
   * @author 2007-11-15 Kris Becker
   */
  class NonLinearLSQ {
    public:
      typedef TNT::Array1D<double> NLVector;
      typedef TNT::Array2D<double> NLMatrix;

      //  Constructors and Destructor
      NonLinearLSQ() : _fitParms(), _uncert(), _nIters(0), _maxIters(50),
                       _status(0), _userTerminated(false), _userMessage() {}

      /** Destructor */
      virtual ~NonLinearLSQ() { }

      virtual int nSize() const = 0;
      virtual int nParms() const = 0;

      /**
       * @brief Sets the maximum number of iterations
       *
       * @param m User provides the maximum number iterations
       */
      void setMaxIters(int m) { _maxIters = m; }

      /**
       * @brief Maximum number iterations for valid solution
       *
       *
       * @return int Maximum resolutions
       */
      int maxIters() const { return (_maxIters); }

      virtual NLVector guess()  = 0;
      virtual NLVector f_x(const NLVector &x) = 0;
      virtual NLMatrix df_x(const NLVector &x) = 0;

      virtual double absErr() const { return (1.0E-4); }
      virtual double relErr() const { return (1.0E-4); }

      int curvefit();
      /** Return status of last fit processing */
      inline int status() const { return (_status); }
      /** Determine success from last fit processing */
      inline bool success() const { return (_status == GSL_SUCCESS); }
      /** Check status for success of the given condition  */
      inline bool success(int status) const { return (status == GSL_SUCCESS); }
      /** Return error message pertaining to last fit procesing */
      inline std::string statusstr() const {
        return (std::string(gsl_strerror(_status)));
      }
      /** Return error message given status condition */
      inline std::string statusstr(int status) const {
        return (std::string(gsl_strerror(status)));
      }

      /** Default interation test simply returns input status */
      virtual int checkIteration(const int Iter, const NLVector &fitcoefs,
                                 const NLVector &uncerts, double cplxconj,
                                 int Istatus) {
        return (Istatus);
      }

      /** Return coefficients from last fit processing */
      inline NLVector coefs() const { return (_fitParms); }
      /** Return uncertainties from last fit processing */
      inline NLVector uncert() const { return (_uncert); }
      /** Return number of iterations from last fit processing */
      inline int nIterations() const { return (_nIters); }

    protected:
      void Terminate(const std::string &message = "");
      void Abort(const std::string &reason = "");

      bool doContinue() const { return (!_userTerminated); }


    private:
      NLVector    _fitParms;
      NLVector    _uncert;
      int         _nIters;
      int         _maxIters;
      int         _status;
      bool        _userTerminated;
      std::string _userMessage;

      struct _nlsqPointer {
          NonLinearLSQ *nlsq;
      };
      static int f(const gsl_vector *x, void *params, gsl_vector *fx);
      static int df(const gsl_vector *x, void *params, gsl_matrix *J);
      static int fdf(const gsl_vector *x, void *params, gsl_vector *fx,
                     gsl_matrix *J);

      NLVector gslToNlsq(const gsl_vector *v) const;
      NLMatrix gslToNlsq(const gsl_matrix *m) const;
      gsl_vector *NlsqTogsl(const NLVector &v, gsl_vector *gv = 0) const;
      gsl_matrix *NlsqTogsl(const NLMatrix &m, gsl_matrix *gm = 0) const;
      NLVector getUncertainty(const gsl_matrix *m) const;
  };

}     // namespace Isis
#endif
