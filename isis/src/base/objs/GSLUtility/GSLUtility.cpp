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

#include "GSLUtility.h"
#include "IException.h"

using namespace std;

namespace Isis {
  namespace GSL {

///! Initialization of object reference
    GSLUtility *GSLUtility::_instance = 0;

    /**
     * @brief Contructs a GSLUtility object with an error handler
     *
     * Sets an error handler for the GSL library so fatal GSL errors are intercepted
     * and handled throught the ISIS exception utility.
     *
     * See http://www.gnu.org/software/gsl/manual/html_node/Error-Handling.html for
     * additional information.
     */
    GSLUtility::GSLUtility() {
      gsl_set_error_handler(handler);
    }


    /**
     * @brief Return a reference to the GSL (singleton) object
     *
     * This method returns a pointer reference to the GSL utility object.  If it is
     * not yet created, one is constructed and lives until the application
     * terminates.
     *
     * @return GSLUtility::GSLUtility* Pointer reference to GSLUtility singleton
     */
    GSLUtility *GSLUtility::getInstance() {
      if(_instance == 0) {
        _instance = new GSLUtility();
      }
      return (_instance);
    }


    /**
     * @brief Creates a GSL vector
     *
     * This convenience method creates a GSL vector for use within applications.
     * The memory is dynamically allocated and must be managed (i.e., freed) by the
     * caller.  See the free() method for this provision.
     *
     * @param n    Size of GSL vector to create
     * @param zero Set to true if you want GSL to assign 0 to the allocated vector
     *             otherwise, false will not initilize the memory.
     *
     * @return gsl_vector* Returns an allocated GSL vector
     */
    gsl_vector *GSLUtility::vector(size_t n, bool zero) const {
      if(zero) {
        return (gsl_vector_calloc(n));
      }
      else {
        return (gsl_vector_alloc(n));
      }
    }

    /**
     * @brief Creates a GSL matrix
     *
     * This convenience method creates a GSL matrix for use within applications. The
     * memory is dynamically allocated and must be managed (i.e., freed) by the
     * caller.  See the free() method for this provision.
     *
     * @param n1   Size of rows in the GSL matrix to create
     * @param n2   Size of columns in the GSL matrix to create
     * @param zero Set to true if you want GSL to assign 0 to the allocated matrix
     *             otherwise, false will not initilize the memory.
     *
     * @return gsl_matrix* Returns an allocated GSL matrix
     */
    gsl_matrix *GSLUtility::matrix(size_t n1, size_t n2, bool zero) const {
      if(zero) {
        return (gsl_matrix_calloc(n1, n2));
      }
      else {
        return (gsl_matrix_alloc(n1, n2));
      }
    }

    /**
     * @brief Returns a GSL identity matrix of the specified size
     *
     * This method allocates a square or rectanglar matrix and initilizes it to the
     * identity matrix.  The diagonal elements are all set to 1.0, all other
     * elements are set to 0.
     *
     * @param n1 Number rows to allocate
     * @param n2 Number columns to allocate
     *
     * @return gsl_matrix* Returns pointer to identity matrix
     */
    gsl_matrix *GSLUtility::identity(size_t n1, size_t n2) const {
      gsl_matrix *i = gsl_matrix_alloc(n1, n2);
      gsl_matrix_set_identity(i);
      return (i);
    }

    /**
     * @brief Initializes an existing GSL matrix to the identity matrix
     *
     * @param m Pointer to matrix to set to identity
     */
    void GSLUtility::setIdentity(gsl_matrix *m) const {
      gsl_matrix_set_identity(m);
      return;
    }

    /**
     * @brief Frees a GSL vector
     *
     * Frees the memory allocated to a GSL vector.  As with any free operation, the
     * vector cannot be used thereafter.
     *
     * It is up to the user to manage all GSL allocated elements.  It is not done
     * automatically.
     *
     * @param v GSL vector to free
     */
    void GSLUtility::free(gsl_vector *v) const {
      gsl_vector_free(v);
      return;
    }

    /**
     * @brief Frees a GSL matrix
     *
     * Frees the memory allocated to a GSL matrix.  As with any free operation, the
     * matrix cannot be used thereafter.
     *
     * It is up to the user to manage all GSL allocated elements.  It is not done
     * automatically.
     *
     * @param m GSL matrix to free
     */
    void GSLUtility::free(gsl_matrix *m) const {
      gsl_matrix_free(m);
      return;
    }

    /**
     * @brief Converts a GSL vector to a TNT-based vector
     *
     * Convenience method to convert to GSLVector type
     *
     * @param v GSL vector to convert
     *
     * @return GSLUtility::GSLVector TNT-based vector
     */
    GSLUtility::GSLVector GSLUtility::gslToGSL(const gsl_vector *v) const {
      size_t n = size(v);;
      GSLVector Nv(n);
      for(size_t i = 0 ; i < n ; i++) {
        Nv[i] = gsl_vector_get(v, i);
      }
      return (Nv);
    }

    /**
     * @brief Converts a GSL matrix to a TNT-based matrix
     *
     * Convenience method to convert to GSLMatrix type
     *
     * @param m GSL matrix to convert
     *
     * @return GSLUtility::GSLMatrix TNT-based matrix
     */
    GSLUtility::GSLMatrix GSLUtility::gslToGSL(const gsl_matrix *m) const {
      size_t nrows = Rows(m);
      size_t ncols = Columns(m);
      GSLMatrix Nm(nrows, ncols);
      for(size_t i = 0 ; i < nrows ; i++) {
        for(size_t j = 0 ; j < ncols ; j++) {
          Nm[i][j] = gsl_matrix_get(m, i, j);
        }
      }
      return (Nm);
    }


    /**
     * @brief Converts TNT-based vector to GSL vector
     *
     * Convenience method to convert TNT-based vector to a GSL vector.
     *
     * @param v TNT-based vector to convert
     * @param gv Optional GSL vector of same size to copy data to
     *
     * @return gsl_vector* Pointer to GSL vector copy
     */
    gsl_vector *GSLUtility::GSLTogsl(const GSLUtility::GSLVector &v,
                                     gsl_vector *gv) const {
      if(gv == 0) {
        gv = gsl_vector_alloc(v.dim());
      }
      else if(size(gv) != (size_t) v.dim()) {
        ostringstream mess;
        mess << "Size of NL vector (" << v.dim() << ") not same as GSL vector ("
             << gv->size << ")";
        throw IException(IException::Programmer,
                         mess.str().c_str(),
                         _FILEINFO_);
      }

      for(int i = 0 ; i < v.dim() ; i++) {
        gsl_vector_set(gv, i, v[i]);
      }
      return (gv);
    }

    /**
     * @brief Converts TNT-based matrix to GSL matrix
     *
     * Convenience method to convert TNT-based matrix to a GSL matrix.
     *
     * @param m TNT-based matrix to convert
     * @param gm Optional GSL matrix of same size to copy data to
     *
     * @return gsl_matrix* Pointer to GSL matrix copy
     */
    gsl_matrix *GSLUtility::GSLTogsl(const GSLUtility::GSLMatrix &m,
                                     gsl_matrix *gm) const {
      if(gm == 0) {
        gm = gsl_matrix_alloc(m.dim1(), m.dim2());
      }
      else if((Rows(gm) != (size_t) m.dim1()) &&
              (Columns(gm) != (size_t) m.dim2())) {
        ostringstream mess;
        mess << "Size of NL matrix (" << m.dim1() << "," << m.dim2()
             << ") not same as GSL matrix (" << Rows(gm) << "," << Columns(gm)
             << ")";
        throw IException(IException::Programmer,
                         mess.str().c_str(),
                         _FILEINFO_);
      }

      for(int i = 0 ; i < m.dim1() ; i++) {
        for(int j = 0 ; j < m.dim2() ; j++) {
          gsl_matrix_set(gm, i, j, m[i][j]);
        }
      }
      return (gm);
    }

    /** Returns number of rows in a GSL matrix */
    size_t GSLUtility::Rows(const gsl_matrix *m) const {
      return (m->size1);
    }

    /** Returns the number of coulumns in a GSL matrix */
    size_t GSLUtility::Columns(const gsl_matrix *m) const {
      return (m->size2);
    }

    /** Returns the number of columns in TNT-based matrix */
    size_t GSLUtility::Columns(const GSLMatrix &m) const {
      return (m.dim1());
    }

    /** Returns the number of rows in TNT-based matrix  */
    size_t GSLUtility::Rows(const GSLMatrix &m) const {
      return (m.dim2());
    }

    /** Returns the size of a GSL vector */
    size_t GSLUtility::size(const gsl_vector *v) const {
      return (v->size);
    }

    /** Returns the total number of elements in a GSL matrix */
    size_t GSLUtility::size(const gsl_matrix *m) const {
      return (Rows(m) * Columns(m));
    }

    /**
     * @brief Performs a check on GSL library function return status
     *
     * This covenience method performs a validity check on the return status of a
     * GSL function.  It will throw an ISIS exception should the status be anything
     * other than GSL_SUCCESS.
     *
     * @param gsl_status Return value of GSL function
     * @param src Name of the source file where the function was called.
     * @param line Line number in the source where the call/error occurs
     */
    void GSLUtility::check(int gsl_status, const char *src, int line) const {
      if(gsl_status != GSL_SUCCESS) {
        string msg = "GSL error occured: " + string(gsl_strerror(gsl_status));
        throw IException(IException::Programmer, msg.c_str(), src, line);
      }
      return;
    }

    /**
     * @brief Special GSL errror handler
     *
     * This method is the designated ISIS error handler for errors that occur
     * within the GSL library.  It will be called by the GSL library when errors
     * occur to handle failures.  It is designed to override GSL default behavior
     * which is to issue an error and abort the application.
     *
     * This method traps the error and throws an ISIS exception indicating the
     * error.
     *
     * @param reason GSL description of the error
     * @param file   Source file where the error originates
     * @param line   Line if source file where error occurred
     * @param gsl_errno Actual GSL error encountered
     */
    void GSLUtility::handler(const char *reason, const char *file, int line,
                             int gsl_errno) {
      ostringstream mess;
      mess << "GSLError (" << gsl_errno << ") -> " << reason;
      throw IException(IException::Programmer, mess.str().c_str(),
                       file, line);
    }

  }
} // namespace ISIS::GSL
