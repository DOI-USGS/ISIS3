/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2009/12/22 02:09:54 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.                                                           
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website,              
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

#include <vector>
#include <iostream>
#include <string>
#include "jama/jama_svd.h"
#include "jama/jama_eig.h"
#include "jama/jama_lu.h"

#include "Matrix.h"
#include "iException.h"
#include "Constants.h"

namespace Isis {
  /**
   * Constructs an n x m Matrix containing the specified default
   * value.
   */
  Matrix::Matrix (const int n, const int m, const double value) {
    if (n <1 || m < 1) {
      std::string m="Invalid matrix dimensions";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    p_matrix = TNT::Array2D<double>(n,m,value);
  }

  /**
   * Constructs a Matrix from the specified TNT Array2D
   */
  Matrix::Matrix (TNT::Array2D<double> matrix) {
    p_matrix = matrix.copy();
  }

  //! Destroys the Matrix object
  Matrix::~Matrix() {}

  /**
   * Create an n x n identity matrix
   */
  Matrix Matrix::Identity(const int n) {
    if (n<1) {
      std::string m="Invalid matrix dimensions";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    Matrix identity(n,n);
    for (int i=0; i<identity.Rows(); i++) {
      identity[i][i] = 1.0;
    }
    return identity;
  }

  /**
   * Compute the determinant of the matrix
   */
  double Matrix::Determinant() {
    if (Rows() != Columns()) {
      std::string m="Unable to calculate the determinant, the matrix is not square.";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    JAMA::LU<double> lu(p_matrix);
    return lu.det();
  }

    /**
   * Compute the determinant of the matrix
   */
  double Matrix::Trace() {
    if (Rows() != Columns()) {
      std::string m="Unable to calculate the trace, the matrix is not square.";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    double trace = 0.0;
    for (int i=0; i<Rows(); i++) {
      trace += p_matrix[i][i];
    }
    return trace;
  }

  /** 
   * Multiply the two matrices
   */
  Matrix Matrix::Multiply (Matrix &matrix) {
    if (Columns() != matrix.Rows()) {
      std::string m="Incompatible matrix dimensions, cannot multiply the matrices.";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    TNT::Array2D<double> m(matrix.Rows(), matrix.Columns());
    for (int i=0; i<m.dim1(); i++) {
      for (int j=0; j<m.dim2(); j++) {
        m[i][j] = matrix[i][j];
      }
    }
    return Matrix(TNT::matmult(p_matrix, m));
  }

  /** 
  * Add the two matrices
  */
  Matrix Matrix::Add (Matrix &matrix) {
    if (Rows() != matrix.Rows() || Columns() != matrix.Columns()) {
      std::string m="Incompatible matrix dimensions, cannot add the matrices.";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    TNT::Array2D<double> m(matrix.Rows(), matrix.Columns());
    for (int i=0; i<m.dim1(); i++) {
      for (int j=0; j<m.dim2(); j++) {
        m[i][j] = matrix[i][j];
      }
    }
    return Matrix(p_matrix + m);
  }

  /** 
  * Subtract the input matrix from this matrix
  */
  Matrix Matrix::Subtract (Matrix &matrix) {
    if (Rows() != matrix.Rows() || Columns() != matrix.Columns()) {
      std::string m="Incompatible matrix dimensions, cannot subtract the matrices.";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    TNT::Array2D<double> m(matrix.Rows(), matrix.Columns());
    for (int i=0; i<m.dim1(); i++) {
      for (int j=0; j<m.dim2(); j++) {
        m[i][j] = matrix[i][j];
      }
    }

    return Matrix(p_matrix - m);
  }

  /** 
   * Multiply the two matrices element-wise (ie compute C such
   * that C[i][j] = A[i][j]*B[i][j])
   */
  Matrix Matrix::MultiplyElementWise(Matrix &matrix) {
    if (Rows() != matrix.Rows() || Columns() != matrix.Columns()) {
      std::string m="Incompatible matrix dimensions, cannot multiply the matrices.";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    TNT::Array2D<double> m(matrix.Rows(), matrix.Columns());
    for (int i=0; i<m.dim1(); i++) {
      for (int j=0; j<m.dim2(); j++) {
        m[i][j] = matrix[i][j];
      }
    }
    return Matrix(p_matrix * m);
  }

/** 
   * Multiply the matrix by a scalar value
   */
  Matrix Matrix::Multiply (double scalar) {
    Matrix product(Rows(), Columns());
    for (int i=0; i<Rows(); i++) {
      for (int j=0; j<Columns(); j++) {
        product[i][j] = p_matrix[i][j] * scalar;
      }
    }
    return product;
  }

  /**
   * Compute the transpose of the matrix
   */
  Matrix Matrix::Transpose() {
    TNT::Array2D<double> transpose(p_matrix.dim2(), p_matrix.dim1());
    for (int i=0; i< transpose.dim1(); i++) {
      for (int j=0; j<transpose.dim2(); j++) {
        transpose[i][j] = p_matrix[j][i];
      }
    }
    return Matrix(transpose);
  }

  /**
   * Compute the inverse of the matrix
   */
  Matrix Matrix::Inverse() {
    if (Rows() != Columns()) {
      std::string m="Unable to calculate the inverse, the matrix is not square.";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    TNT::Array2D<double> id(p_matrix.dim1(), p_matrix.dim2(), 0.0);
    for (int i = 0; i < p_matrix.dim1(); i++) id[i][i] = 1;

    JAMA::LU<double> lu(p_matrix);
    if (lu.det() == 0.0) {
      std::string m="Cannot take the inverse of the matrix";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }

    return Matrix( lu.solve(id) );
  }

/**
 *  Compute the eigenvalues of the matrix
 */
  std::vector<double> Matrix::Eigenvalues() {
    if (Rows() != Columns()) {
      std::string m="Unable to calculate eigenvalues, the matrix is not square.";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    JAMA::Eigenvalue<double> E(p_matrix);
    TNT::Array2D<double> D;
    E.getD(D);

    std::vector<double> eigenvalues(D.dim1());
    for (int i=0; i<D.dim1(); i++) {
      eigenvalues[i] = D[i][i];
    }

    return eigenvalues;
  }

/**
 *  Compute the eigenvectors of the matrix and return them as
 *  columns of a matrix in ascending order
 */
  Matrix Matrix::Eigenvectors() {
    if (Rows() != Columns()) {
      std::string m="Unable to calculate eigenvectors, the matrix is not square.";
      throw Isis::iException::Message(Isis::iException::Programmer,m,_FILEINFO_);
    }
    JAMA::Eigenvalue<double> E(p_matrix);
    TNT::Array2D<double> V;
    E.getV(V);

    return Matrix(V);
  }

  /**
  *  Write the matrix to the output stream
  */
  ostream& operator<<(ostream &os, Matrix &matrix) {
    for (int i=0; i<matrix.Rows(); i++) {
      for (int j=0; j<matrix.Columns(); j++) {
        os << matrix[i][j];
        if (j<matrix.Columns()-1) os << " ";
      }
      if (i<matrix.Rows()-1) os << endl;
    }
    return os;
  }
} // end namespace isis
