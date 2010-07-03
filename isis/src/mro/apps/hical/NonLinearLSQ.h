#if !defined(NonLinearLSQ_h)
#define NonLinearLSQ_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $
 * $Date: 2009/09/16 03:37:23 $
 * $Id: NonLinearLSQ.h,v 1.1 2009/09/16 03:37:23 kbecker Exp $
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

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

#include "iException.h"

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
      inline int status() const { return (_status); }
      inline bool success() const { return (_status == GSL_SUCCESS); }
      inline bool success(int status) const { return (status == GSL_SUCCESS); }
      inline std::string statusstr() const { 
        return (std::string(gsl_strerror(_status)));
      }
      inline std::string statusstr(int status) const { 
        return (std::string(gsl_strerror(status)));
      }

      virtual int checkIteration(const int Iter, const NLVector &fitcoefs, 
                                 const NLVector &uncerts, double cplxconj,
                                 int Istatus) { 
        return (Istatus);
      }

      inline NLVector coefs() const { return (_fitParms); }
      inline NLVector uncert() const { return (_uncert); }
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

