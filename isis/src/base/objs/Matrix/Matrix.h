#ifndef Matrix_h
#define Matrix_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <vector>
#include "tnt/tnt_array2d.h"

namespace Isis {
  /**
   * @brief Matrix class
   *
   * A Matrix
   *
   * This class stores a matrix and compute matrix calculations
   * such as addition, subtraction,multiplication, transposes,
   * inverses, and eigenvalues.
   *
   * @ingroup Math
   *
   * @author  2006-05-19 Jacob Danton
   *
   * @internal
   *   @history 2006-05-19 Jacob Danton Original Version
   *
   *   @history 2007-07-11 Brendan George Fixed unitTest to print values in
   *            "fixed" (non-scientific) format, added system specific unitTest
   *            for Linux i686 systems
   */

  class Matrix {
    public:
      Matrix(const int n, const int m, const double value = 0.0);
      Matrix(TNT::Array2D<double> matrix);
      ~Matrix();

      static Matrix Identity(const int n);

      inline int Rows() {
        return p_matrix.dim1();
      };
      inline int Columns() {
        return p_matrix.dim2();
      };

      double Determinant();
      double Trace();;

      std::vector<double> Eigenvalues();

      Matrix Add(Matrix &matrix);
      Matrix Subtract(Matrix &matrix);
      Matrix Multiply(Matrix &matrix);
      Matrix Multiply(double scalar);
      Matrix MultiplyElementWise(Matrix &matrix);
      Matrix Transpose();
      Matrix Inverse();
      Matrix Eigenvectors();

      inline double *operator[](int index) {
        return p_matrix[index];
      };
      Matrix operator+ (Matrix &matrix)  {
        return Add(matrix);
      };
      Matrix operator- (Matrix &matrix)  {
        return Subtract(matrix);
      };
      Matrix operator* (Matrix &matrix)  {
        return Multiply(matrix);
      };
      Matrix operator* (double scalar)  {
        return Multiply(scalar);
      };

    private:
      TNT::Array2D<double> p_matrix;
  };

  std::ostream &operator<<(std::ostream &os, Matrix &matrix);
};

#endif
