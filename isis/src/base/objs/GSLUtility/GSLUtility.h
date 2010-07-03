#if !defined(GSLUtility_h)
#define GSLUtility_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $
 * $Date: 2009/12/22 02:09:54 $
 * $Id: GSLUtility.h,v 1.2 2009/12/22 02:09:54 ehyer Exp $
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

#include "tnt/tnt_array1d.h"
#include "tnt/tnt_array1d_utils.h"
#include "tnt/tnt_array2d.h"
#include "tnt/tnt_array2d_utils.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

//  Some GSL optimization on by default, off if DEBUG or SAFE_GSL is defined
#if !defined(DEBUG)
#if !defined(SAFE_GSL)
#define GSL_RANGE_CHECK_OFF 1
#endif
#endif

#include "iException.h"

namespace Isis {
  namespace GSL {

  /**
   * @brief GSLUtility Provides top level interface to the GNU GSL
   *  
   * Provides GSL setup and interface utilities.  This object is provided for 
   * convenience of GSL vector and matrix manipulation as well as better 
   * management of GSL error handling. 
   *  
   * Without setting up GSL error handling, the GSL will abort when certain 
   * errors occur.  This singleton object, an object where there is never more 
   * than one instance, an error handler is established that captures GSL errors 
   * and formats them into ISIS exceptions. 
   *  
   * There are many convenience methods provided for manipulation of GSL vectors 
   * and matrixs.  Motivation for this is to address element access and 
   * efficient parameter and copy mechanisms (provided by the TNT library). 
   *  
   * There are some compile options on by default that help optimize the GSL. 
   * When the compile time DEBUG macro is set, range checking is turned on. 
   * Inline functions are also turned on by default unless the DEBUG macro is 
   * set. In addition, an additional compile time macro called SAFE_GSL is 
   * provided to emulate the DEBUG behavior, but does not invoke additional 
   * DEBUG behavior/side effects. 
   *  
   * See http://www.gnu.org/software/gsl/ for additional details on the GNU 
   * Scientific Library. 
   *  
   * @ingroup Utility
   * @author 2008-05-06 Kris Becker 
   * @internal 
   *   @history 2009-08-20 Kris Becker Completed documentation 
   */
  class GSLUtility {
    public:
      typedef TNT::Array1D<double> GSLVector;
      typedef TNT::Array2D<double> GSLMatrix;

      static GSLUtility *getInstance();  

      /** Tests if status is success */
      inline bool success(int status) const { 
        return (status == GSL_SUCCESS);
      } 

      /**
       * @brief Returns GSL specific error text
       * 
       * @param gsl_errno GSL error number
       * @return std::string Textual context of GSL error
       */
      inline std::string status(int gsl_errno) const { 
        return (std::string(gsl_strerror(gsl_errno)));
      }

      void check(int gsl_status, char *src = __FILE__, int line = __LINE__) 
                 const throw (iException &);


      size_t Rows(const gsl_matrix *m) const;
      size_t Columns(const gsl_matrix *m) const;

      size_t Rows(const GSLMatrix &m) const;
      size_t Columns(const GSLMatrix &m) const;

      size_t size(const gsl_vector *v) const;
      size_t size(const gsl_matrix *m) const;

      gsl_vector *vector(size_t n, bool zero = false) const;
      gsl_matrix *matrix(size_t n1, size_t n2, bool zero = false) const;
      gsl_matrix *identity(size_t n1, size_t n2) const;
      void setIdentity(gsl_matrix *m) const;

      void free(gsl_vector *v) const;
      void free(gsl_matrix *m) const;

      GSLVector gslToGSL(const gsl_vector *v) const;
      GSLMatrix gslToGSL(const gsl_matrix *m) const;
      gsl_vector *GSLTogsl(const GSLVector &v, gsl_vector *gv = 0) const;
      gsl_matrix *GSLTogsl(const GSLMatrix &m, gsl_matrix *gm = 0) const;


      private:
        //  Private Constructor/Destructor makes this a singleton
        GSLUtility();
        ~GSLUtility() { }

        static GSLUtility *_instance;  //!< Singleton self-reference pointer

        static void handler(const char * reason, const char * file, int line,
                            int gsl_errno);


  };

  } // namespace GSL
}     // namespace Isis
#endif

