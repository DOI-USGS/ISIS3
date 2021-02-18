/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>
#include <numeric>
#include <iostream>
#include <sstream>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>

#include "NonLinearLSQ.h"
#include "IException.h"
#include "IString.h"

using namespace std;

namespace Isis {


int NonLinearLSQ::curvefit() {

  size_t n(nSize());
  size_t p(nParms());

  //  Initialize the solver function information
  _nlsqPointer d = { this };
  gsl_multifit_function_fdf mf;
  mf.f      = &f;
  mf.df     = &df;
  mf.fdf    = &fdf;
  mf.n      =  n;
  mf.p      = p;
  mf.params =  &d;

  const gsl_multifit_fdfsolver_type *T = gsl_multifit_fdfsolver_lmsder;
  gsl_multifit_fdfsolver *s = gsl_multifit_fdfsolver_alloc(T, n, p);

  _fitParms = guess();
  gsl_vector *x = NlsqTogsl(_fitParms);
  gsl_matrix *covar = gsl_matrix_alloc(p, p);
  gsl_multifit_fdfsolver_set(s, &mf, x);

  _nIters = 0;
  checkIteration(_nIters, gslToNlsq(s->x), NLVector(p,999.0),
                  gsl_blas_dnrm2(s->f), GSL_CONTINUE);


  gsl_matrix *J = gsl_matrix_alloc(n, p);
  do {
    _nIters++;

    _status = gsl_multifit_fdfsolver_iterate(s);
    _fitParms = gslToNlsq(s->x);

    gsl_multifit_fdfsolver_jac(s, J);
    gsl_multifit_covar(J, 0.0, covar);
    _uncert = getUncertainty(covar);

    _status = checkIteration(_nIters, _fitParms, _uncert, gsl_blas_dnrm2(s->f),
                             _status);
    if ( _status  ) { break; }
    if(!doContinue()) { break; }

    _status = gsl_multifit_test_delta(s->dx, s->x, absErr(), relErr());
  } while ((_status == GSL_CONTINUE) && (_nIters < _maxIters));

  // Clean up
  gsl_multifit_fdfsolver_free(s);
  gsl_matrix_free(covar);
  gsl_matrix_free(J);

  return (_status);
}


void NonLinearLSQ::Terminate(const std::string &message) {
  _userMessage = message;
  _userTerminated = true;
  _status = GSL_SUCCESS;
  return;
}

void NonLinearLSQ::Abort(const std::string &reason) {
  _userMessage = reason;
  _userTerminated = true;
  _status = GSL_FAILURE;
  return;
}


int NonLinearLSQ::f(const gsl_vector *x, void *params, gsl_vector *fx) {
  NonLinearLSQ *nlsq = (static_cast<_nlsqPointer *> (params))->nlsq;
  int n = nlsq->nSize();
  NLVector fxv = nlsq->f_x(nlsq->gslToNlsq(x));
  for (int i = 0 ; i < n ; i++ ) {
    gsl_vector_set(fx, i, fxv[i]);
  }
  return  (GSL_SUCCESS);
}

int NonLinearLSQ::df(const gsl_vector *x, void *params, gsl_matrix *J) {
  NonLinearLSQ *nlsq = (static_cast<_nlsqPointer *> (params))->nlsq;
  int n = nlsq->nSize();
  int p = nlsq->nParms();

  NLMatrix m = nlsq->df_x(nlsq->gslToNlsq(x));

  for (int i = 0 ; i < n ; i++ ) {
    for (int j = 0 ; j < p ; j++ ) {
      gsl_matrix_set(J, i, j, m[i][j]);
    }
  }
  return  (GSL_SUCCESS);
}

int NonLinearLSQ::fdf(const gsl_vector *x, void *params, gsl_vector *fx,
                      gsl_matrix *J) {
  f(x,params,fx);
  df(x,params,J);
  return  (GSL_SUCCESS);
}


NonLinearLSQ::NLVector NonLinearLSQ::getUncertainty(const gsl_matrix *covar)
                                                    const {
   NLVector unc(covar->size1);
   for (size_t i = 0 ; i < covar->size1 ; i++ ) {
     unc[i] = sqrt(gsl_matrix_get(covar, i, i));
   }
   return (unc);
}

NonLinearLSQ::NLVector NonLinearLSQ::gslToNlsq(const gsl_vector *v) const {
  size_t n = v->size;
  NLVector Nv(n);
  for (size_t i = 0 ; i < n ; i++) {
      Nv[i] = gsl_vector_get(v, i);
  }
  return (Nv);
}

NonLinearLSQ::NLMatrix NonLinearLSQ::gslToNlsq(const gsl_matrix *m) const {
  size_t nrows = m->size1;
  size_t ncols = m->size2;
  NLMatrix Nm(nrows, ncols);
  for (size_t i = 0 ; i < nrows ; i++) {
    for (size_t j = 0 ; j < ncols ; j++) {
      Nm[i][j] = gsl_matrix_get(m,i,j);
    }
  }
  return (Nm);
}

gsl_vector *NonLinearLSQ::NlsqTogsl(const NonLinearLSQ::NLVector &v,
                                    gsl_vector *gv) const {
  if (gv == 0) {
    gv = gsl_vector_alloc(v.dim());
  }
  else if (gv->size != (size_t) v.dim()) {
    ostringstream mess;
    mess << "Size of NL vector (" << v.dim() << ") not same as GSL vector ("
         << gv->size << ")";
    throw IException(IException::Programmer, mess.str(), _FILEINFO_);
  }

  for (int i = 0 ; i < v.dim() ; i++) {
      gsl_vector_set(gv, i, v[i]);
  }
  return (gv);
}

gsl_matrix *NonLinearLSQ::NlsqTogsl(const NonLinearLSQ::NLMatrix &m,
                                    gsl_matrix *gm) const {
  if (gm == 0) {
    gm = gsl_matrix_alloc(m.dim1(), m.dim2());
  }
  else if ((gm->size1 != (size_t) m.dim1()) &&
           (gm->size2 != (size_t) m.dim2()) ) {
    ostringstream mess;
    mess << "Size of NL matrix (" << m.dim1() << "," << m.dim2()
         << ") not same as GSL matrix (" << gm->size1 << "," << gm->size2
         << ")";
    throw IException(IException::Programmer, mess.str(), _FILEINFO_);
  }

  for (int i = 0 ; i < m.dim1() ; i++) {
    for (int j = 0 ; j < m.dim2() ; j++) {
      gsl_matrix_set(gm, i, j, m[i][j]);
    }
  }
  return (gm);
}

} // namespace ISIS
