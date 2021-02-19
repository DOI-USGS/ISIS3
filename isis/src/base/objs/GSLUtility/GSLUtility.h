#ifndef GSLUtility_h
#define GSLUtility_h
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

#include <QString>

#include <tnt/tnt_array1d.h>
#include <tnt/tnt_array1d_utils.h>
#include <tnt/tnt_array2d.h>
#include <tnt/tnt_array2d_utils.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

//  Some GSL optimization on by default, off if DEBUG or SAFE_GSL is defined
#ifndef DEBUG
#ifndef SAFE_GSL
#define GSL_RANGE_CHECK_OFF 1
#endif
#endif

#include "IException.h"

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
         * @return QString Textual context of GSL error
         */
        inline QString status(int gsl_errno) const {
          return (QString(gsl_strerror(gsl_errno)));
        }

        void check(int gsl_status, const char *src = __FILE__, int line = __LINE__)
        const;


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

        static void handler(const char *reason, const char *file, int line,
                            int gsl_errno);


    };

  } // namespace GSL
}     // namespace Isis
#endif

