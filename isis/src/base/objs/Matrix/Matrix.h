#ifndef Matrix_h
#define Matrix_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
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
    Matrix (const int n, const int m, const double value = 0.0);
    Matrix (TNT::Array2D<double> matrix);
    ~Matrix ();

    static Matrix Identity(const int n);

    inline int Rows() { return p_matrix.dim1();};
    inline int Columns() { return p_matrix.dim2();};

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

    inline double * operator[] (int index) { return p_matrix[index];};
    Matrix operator+ (Matrix &matrix)  { return Add(matrix);};
    Matrix operator- (Matrix &matrix)  { return Subtract(matrix);};
    Matrix operator* (Matrix &matrix)  { return Multiply(matrix);};
    Matrix operator* (double scalar)  { return Multiply(scalar);};

  private:
    TNT::Array2D<double> p_matrix;
  };

  std::ostream& operator<<(std::ostream &os, Matrix &matrix);
};

#endif
