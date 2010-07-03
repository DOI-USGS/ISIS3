/**                                                                       
 * @file                                                                  
 * $Revision: 1.7 $                                                             
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

#include "Affine.h"
#include "PolynomialBivariate.h"
#include "LeastSquares.h"
#include "iException.h"
#include "Constants.h"

using namespace std;

namespace Isis {
  /**
   * Constructs an Affine transform.  The default transform is 
   * the identity. 
   */
  Affine::Affine () {
    Identity();
    p_x = p_y = p_xp = p_yp = 0.0;
  }

  /**
   * @brief Create Affine transform from matrix 
   *  
   * This constructor creates the affine transform from a forward matrix.  The 
   * input matrix is checked for the proper dimensions (3x3) and is then 
   * inverted to complete the inverse functionality. 
   *  
   * The input matrix must be invertable or an exception will be thrown! 
   * 
   * @param a Forward affine matrix
   */
  Affine::Affine(const Affine::AMatrix &a) {
    checkDims(a);
    p_matrix = a.copy();
    p_invmat = invert(p_matrix);
    p_x = p_y = p_xp = p_yp = 0.0;
  }

  //! Destroys the Affine object
  Affine::~Affine() {}

  /**
   * @brief Return an Affine identity matrix
   * 
   * @return Affine::AMatrix The identity matrix
   */
  Affine::AMatrix Affine::getIdentity() {
    AMatrix ident(3,3, 0.0);
    for (int i = 0 ; i < ident.dim2() ; i++) {
      ident[i][i] = 1.0;
    }
    return (ident);
  }

  /**
   * Set the forward and inverse affine transform to the identity. 
   * That is, xp = x and yp = y for all (x,y).
   */
  void Affine::Identity() {
    p_matrix = getIdentity();
    p_invmat = getIdentity();
  }

  /** 
   * Given a set of coordinate pairs (n >= 3), compute the affine
   * transform that best fits the points.  If given exactly three 
   * coordinates that are not colinear, the fit will be guarenteed
   * to be exact through the points. 
   *  
   * @param x  The transformation x coordinates
   * @param y  The transformation y coordinates
   * @param xp The transformation xp coordinates
   * @param yp The transformation yp coordinates
   * @param n  The number of coordiante pairs
   * 
   * @throws Isis::iException::Math - Affine transform not invertible
   */
  void Affine::Solve (const double x[], const double y[], 
                      const double xp[], const double yp[], int n) {
    // We must solve two least squares equations
    PolynomialBivariate xpFunc(1);
    PolynomialBivariate ypFunc(1);
    LeastSquares xpLSQ(xpFunc);
    LeastSquares ypLSQ(ypFunc);

    // Push the knowns into the least squares class
    for (int i=0; i<n; i++) {
      vector<double> coord(2);
      coord[0] = x[i];
      coord[1] = y[i];
      xpLSQ.AddKnown(coord,xp[i]);
      ypLSQ.AddKnown(coord,yp[i]);
    }

    // Solve for A,B,C,D,E,F
    xpLSQ.Solve();
    ypLSQ.Solve();

    // Construct our affine matrix
    p_matrix[0][0] = xpFunc.Coefficient(1); // A
    p_matrix[0][1] = xpFunc.Coefficient(2); // B
    p_matrix[0][2] = xpFunc.Coefficient(0); // C
    p_matrix[1][0] = ypFunc.Coefficient(1); // D
    p_matrix[1][1] = ypFunc.Coefficient(2); // E
    p_matrix[1][2] = ypFunc.Coefficient(0); // F
    p_matrix[2][0] = 0.0;
    p_matrix[2][1] = 0.0;
    p_matrix[2][2] = 1.0;

    // invert the matrix
    p_invmat = invert(p_matrix);
  }

  /**
   * Apply a translation to the current affine transform
   * 
   * @param tx translatation to add to x'
   * @param ty translation to add to y'
   */
  void Affine::Translate (double tx, double ty) {
    AMatrix trans = getIdentity();

    trans[0][2] = tx;
    trans[1][2] = ty;
    p_matrix = TNT::matmult(trans,p_matrix);
    
    trans[0][2] = -tx;
    trans[1][2] = -ty;
    p_invmat = TNT::matmult(p_invmat,trans);
  }

  /**
   * Apply a translation to the current affine transform
   * 
   * @param angle degrees of counterclockwise rotation
   */
  void Affine::Rotate(double angle) {
    AMatrix rot = getIdentity();

    double angleRadians = angle * Isis::PI / 180.0;
    rot[0][0] = cos(angleRadians);
    rot[0][1] = -sin(angleRadians);
    rot[1][0] = sin(angleRadians);
    rot[1][1] = cos(angleRadians);
    p_matrix = TNT::matmult(rot,p_matrix);

    angleRadians = -angleRadians;
    rot[0][0] = cos(angleRadians);
    rot[0][1] = -sin(angleRadians);
    rot[1][0] = sin(angleRadians);
    rot[1][1] = cos(angleRadians);
    p_invmat = TNT::matmult(p_invmat,rot);
  }

  /**
   * Apply a scale to the current affine transform
   * 
   * @param scaleFactor The scale factor
   */
  void Affine::Scale (double scaleFactor) {
    AMatrix scale = getIdentity();
    scale[0][0] = scaleFactor;
    scale[1][1] = scaleFactor;
    p_matrix = TNT::matmult(scale,p_matrix);
    
    scale[0][0] = scaleFactor;
    scale[1][1] = scaleFactor;
    p_invmat = TNT::matmult(p_invmat,scale);
  }

  /**
   * Compute (xp,yp) given (x,y).  Use the methods xp() and yp() to
   * obtain the results. 
   *  
   * @param x The transformation x factor
   * @param y The transformation y factor
   */
  void Affine::Compute(double x, double y) {
    p_x = x;
    p_y = y;
    p_xp = p_matrix[0][0] * x + p_matrix[0][1] * y + p_matrix[0][2];
    p_yp = p_matrix[1][0] * x + p_matrix[1][1] * y + p_matrix[1][2];
  }

  /**
   * Compute (x,y) given (xp,yp).  Use the methods x() and y() to
   * obtain the results. 
   *  
   * @param xp The inverse transformation xp factor
   * @param yp The inverse transformation yp factor
   */
  void Affine::ComputeInverse(double xp, double yp) {
    p_xp = xp;
    p_yp = yp;
    p_x = p_invmat[0][0] * xp + p_invmat[0][1] * yp + p_invmat[0][2];
    p_y = p_invmat[1][0] * xp + p_invmat[1][1] * yp + p_invmat[1][2];
  }
   
  /**
   * Return the affine coeffients for the entered variable (1 or 2).  The coefficients
   * are returned in a 3-dimensional vector 
   *  
   * @param var The coefficient vector index (1 or 2)
   */
  vector<double> Affine::Coefficients( int var ) {
    int index = var-1;
    vector <double> coef;
    coef.push_back(p_matrix[index][0]);
    coef.push_back(p_matrix[index][1]);
    coef.push_back(p_matrix[index][2]);
    return coef;
  }

  /**
   * Return the inverse affine coeffients for the entered variable (1 or 2).
   * The coefficients are returned in a 3-dimensional vector 
   *  
   * @param var The inverse coefficient vector index
   */
  vector<double> Affine::InverseCoefficients( int var ) {
     int index = var-1;
     vector <double> coef;
     coef.push_back(p_invmat[index][0]);
     coef.push_back(p_invmat[index][1]);
     coef.push_back(p_invmat[index][2]);
     return coef;
   }

  /**
   * @brief Checks affine matrix to ensure it is a 3x3 standard form transform 
   * 
   * @param am Affine matrix to validate
   */
  void Affine::checkDims(const AMatrix &am) const throw (iException &) {
    if ((am.dim1() != 3) && (am.dim2() != 3)) {
      ostringstream mess;
      mess << "Affine matrices must be 3x3 - this one is " << am.dim1()
           << "x" << am.dim2();
      throw iException::Message(iException::Programmer, mess.str(), _FILEINFO_);
    }
    return;
  }

  /**
   * @brief Compute the inverse of a matrix 
   *  
   * This method will compute the inverse of an affine matrix for purposes of 
   * forward and inverse Affine computations. 
   * 
   * @param a Matrix to invert
   * 
   * @return Affine::AMatrix The inverted matrix
   */
  Affine::AMatrix Affine::invert(const AMatrix &a) const throw (iException &) {
   // Now compute the inverse affine matrix using singular value
    // decomposition A = USV'.  So invA = V invS U'.  Where ' represents
    // the transpose of a matrix and invS is S with the recipricol of the
    // diagonal elements
    JAMA::SVD<double> svd(a);

    AMatrix V;
    svd.getV(V);

    // The inverse of S is 1 over each diagonal element of S
    AMatrix invS;
    svd.getS(invS);
    for (int i=0; i<invS.dim1(); i++) {
      if (invS[i][i] == 0.0) {
        string msg = "Affine transform not invertible";
        throw iException::Message(iException::Math,msg,_FILEINFO_);
      }
      invS[i][i] = 1.0 / invS[i][i];
    }

    // Transpose U
    AMatrix U;
    svd.getU(U);
    AMatrix transU(U.dim2(),U.dim1());
    for (int r=0; r<U.dim1(); r++) {
      for (int c=0; c<U.dim2(); c++) {
        transU[c][r] = U[r][c];
      }
    }

    // Multiply stuff together to get the inverse of the affine
    AMatrix VinvS = TNT::matmult(V,invS);
    return (TNT::matmult(VinvS,transU));
  }

} // end namespace isis
