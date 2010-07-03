/**
 * @file
 * $Revision: 1.11 $
 * $Date: 2009/12/22 02:09:54 $
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
#include "jama/jama_svd.h"
#include "jama/jama_qr.h"

#if !defined(__sun__)
#include "gmm/gmm_superlu_interface.h"
#endif

#include <string>
#include "LeastSquares.h"
#include "iException.h"
#include "iString.h"

namespace Isis {
/**
 * Creates a LeastSquares Object.
 *
 * @param basis A BasisFunction. This parameter allows for the least squares
 *              fitting to be applied to arbitrary equations.
 */
  LeastSquares::LeastSquares (Isis::BasisFunction &basis,bool sparse,
                              int sparseRows,int sparseCols) {
    p_basis = &basis;
    p_solved = false;

    p_sparse = sparse;
#if defined(__sun__)
    p_sparse = false;
#endif
    if (p_sparse) {
      //  make sure sparse nrows/ncols have been set
      if (sparseRows ==  0 || sparseCols == 0) {
        std::string msg = "If solving using sparse matrices, you must enter";
        msg += " the number of rows/columns";
        throw Isis::iException::Message(Isis::iException::Programmer,msg,
                                        _FILEINFO_);
      }
#if !defined(__sun__)
      gmm::resize(p_sparseA,sparseRows,sparseCols);
#endif
      p_sparseRows = sparseRows;
      p_sparseCols = sparseCols;
    }
    p_currentFillRow = -1;
  }

  //! Destroys the LeastSquares object.
  LeastSquares::~LeastSquares () {
  }

/**
 * Invoke this method for each set of knowns. Given our example in the
 * description, we have three knowns and expecteds. They are
 * @f[
 * (1,1) = 3
 * @f]
 * @f[
 * (-2,3) = 1
 * @f]
 * @f[
 * (2,-1) = 2
 * @f]
 *
 * @param data A vector of knowns.
 *
 * @param result The expected value for the knowns.
 *
 * @param weight (Default = 1.0) How strongly to weight this known. Weight less
 *               than 1 increases residual for this known, while weight greater
 *               than 1 decreases the residual for this known.
 *
 * @throws Isis::iException::Programmer - Number of elements in data does not
 *                                        match basis requirements
 *  
 * @internal 
 * @history 2008-04-22  Tracie Sucharski,  Fill sparse matrix.
 * @history 2009-12-21  Jeannie Walldren,  Modified code to add 
 *          the square root of the weight to the vector
 *          p_sqrtweight.
 *  
 */
  void LeastSquares::AddKnown (const std::vector<double> &data, double result,
                               double weight) {
    if ((int) data.size() != p_basis->Variables()) {
      std::string msg = "Number of elements in data does not match basis [" +
                   p_basis->Name() + "] requirements";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    p_expected.push_back(result);
    if (weight == 1) {
      p_sqrtweight.push_back(weight);
    }
    else {
      p_sqrtweight.push_back(sqrt(weight));
    }

    if (p_sparse) {
#if !defined(__sun__)
      FillSparseA (data);
#endif
    }
    else {
      p_input.push_back(data);
    }
  }



/**
 *  Invoke this method for each set of knowns for sparse solutions.  The A
 *  sparse matrix must be filled as we go or we will quickly run out of memory
 *  for large solutions. So, expand the basis function, apply weights (which is
 *  done in the Solve method for the non-sparse case.
 *
 * @param data A vector of knowns.
 *  
 * @internal 
 * @history 2008-04-22  Tracie Sucharski - New method for sparse solutions. 
 * @history 2009-12-21  Jeannie Walldren - Changed variable name 
 *          from p_weight to p_sqrtweight.
 *                     
 */
#if !defined(__sun__)
  void LeastSquares::FillSparseA (const std::vector<double> &data) {

    p_basis->Expand(data);
    p_currentFillRow++;
    //  ??? Speed this up using iterator instead of array indices???
    for (int c=0;  c<(int)data.size(); c++) {
      p_sparseA(p_currentFillRow,c) = p_basis->Term(c) * p_sqrtweight[p_currentFillRow];
    }
  }
#endif


/**
 * This method returns the data at the given row.
 * 
 * @param row 
 * 
 * @return std::vector<double> 
 */
  std::vector<double> LeastSquares::GetInput(int row) const {
    if((row >= Rows()) || (row < 0)) {
      std::string msg = "Index out of bounds ";
      msg += "[Given = " + Isis::iString(row) + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);  
    }
    return p_input[row];
  }

/**
 * This method returns the expected value at the given row.
 * 
 * @param row 
 * 
 * @return double 
 */
  double LeastSquares::GetExpected(int row) const {
    if((row >= Rows()) || (row < 0)) {
      std::string msg = "Index out of bounds ";
      msg += "[Given = " + Isis::iString(row) + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);      
    }
    return p_expected[row];
  }

/**
 * This methods returns the number of rows in the matrix.
 * 
 * 
 * @return int 
 */
  int LeastSquares::Rows() const {
    return (int)p_input.size();
  }

/**
 * After all the data has been registered through AddKnown, invoke this
 * method to solve the system of equations. You can then use the Evaluate
 * and Residual methods freely. 
 *  
 * @internal 
 * @history  2008-04-16 Debbie Dook / Tracie Sucharski, Added SolveSparse. 
 * @history  2009-04-08 Tracie Sucharski - Added return value which will 
 *                          pass on what is returned from SolveSparse which
 *                          is a column number of a column that contained
 *                          all zeros.
 */
  int LeastSquares::Solve (Isis::LeastSquares::SolveMethod method) {

#if defined(__sun__)
    if (method == SPARSE) method = QRD;
#endif

    if (method == SVD) {
      SolveSVD ();
    }
    else if (method == QRD) {
      SolveQRD ();
    }
    else if (method == SPARSE) {
#if !defined(__sun__)
      int column = SolveSparse ();
      return column;
#endif
    }
    return 0;
  }

/**
 * After all the data has been registered through AddKnown, invoke this
 * method to solve the system of equations. You can then use the Evaluate
 * and Residual methods freely. 
 * @internal 
 * @history 2009-12-21  Jeannie Walldren - Changed variable name 
 *          from p_weight to p_sqrtweight.
 *  
 */
  void LeastSquares::SolveSVD () {

    // We are solving Ax=b ... start by creating A
    TNT::Array2D<double> A(p_input.size(),p_basis->Coefficients());
    for (int r=0; r<A.dim1(); r++) {
      p_basis->Expand(p_input[r]);
      for (int c=0; c<A.dim2(); c++) {
        A[r][c] = p_basis->Term(c) * p_sqrtweight[r];
      }
    }

    // Ok use singular value decomposition to solve for the coefficients
    // A = [U][S][V']  where [U] is MxN, [S] is NxN, [V'] is NxN transpose
    // of [V].  We are solving for [A]x=b and need inverse of [A] such
    // that x = [invA]b. Since inverse may not exist we use the
    // pseudo-inverse [A+] from SVD which is [A+] = [V][invS][U']
    // Our coefficents are then x = [A+]b where b is p_b.
    JAMA::SVD<double> svd(A);

    TNT::Array2D<double> V;
    svd.getV(V);

    // The inverse of S is the 1 over each diagonal element of S
    TNT::Array2D<double> invS;
    svd.getS(invS);
    for (int i=0; i<invS.dim1(); i++) {
      if (invS[i][i] != 0.0) invS[i][i] = 1.0 / invS[i][i];
    }

    // Transpose U
    TNT::Array2D<double> U;
    svd.getU(U);
    TNT::Array2D<double> transU(U.dim2(),U.dim1());
    for (int r=0; r<U.dim1(); r++) {
      for (int c=0; c<U.dim2(); c++) {
        transU[c][r] = U[r][c];
      }
    }

    // Now multiply everything together to get [A+]
    TNT::Array2D<double> VinvS = TNT::matmult(V,invS);
    TNT::Array2D<double> Aplus = TNT::matmult(VinvS,transU);

    // Using Aplus and our b we can solve for the coefficients
    TNT::Array2D<double> b(p_expected.size(),1);
    for (int r=0; r<(int)p_expected.size(); r++) {
      b[r][0] = p_expected[r] * p_sqrtweight[r];
    }
    TNT::Array2D<double> coefs = TNT::matmult(Aplus,b);

    // If the rank of the matrix is not large enough we don't
    // have enough coefficients for the solution
    if (coefs.dim1() < p_basis->Coefficients()) {
      std::string msg = "No solution available ... ";
      msg += "Not enough knowns or knowns are co-linear ... ";
      msg += "[Unknowns = " + Isis::iString(p_basis->Coefficients()) + "] ";
      msg += "[Knowns = " + Isis::iString(coefs.dim1()) + "]";
      throw Isis::iException::Message(Isis::iException::Math,msg,_FILEINFO_);
    }

    // Set the coefficients in our basis equation
    std::vector<double> bcoefs;
    for (int i=0; i<coefs.dim1(); i++) bcoefs.push_back(coefs[i][0]);
    p_basis->SetCoefficients(bcoefs);

    // Compute the errors
    for (int i=0; i<(int)p_input.size(); i++) {
      double value = p_basis->Evaluate(p_input[i]);
      p_residuals.push_back(value-p_expected[i]);
    }

    // All done
    p_solved = true;
  }

/**
 * After all the data has been registered through AddKnown, invoke this
 * method to solve the system of equations with a QR 
 * decomposition of A = QR. You can then use the Evaluate and 
 * Residual methods freely. The QR decomposition is only slightly
 * less reliable than the SVD, but much faster. 
 * @internal 
 * @history 2009-12-21  Jeannie Walldren - Changed variable name 
 *          from p_weight to p_sqrtweight.
 *  
 */
  void LeastSquares::SolveQRD () {

    // We are solving Ax=b ... start by creating A
    TNT::Array2D<double> A(p_input.size(),p_basis->Coefficients());
    for (int r=0; r<A.dim1(); r++) {
      p_basis->Expand(p_input[r]);
      for (int c=0; c<A.dim2(); c++) {
        A[r][c] = p_basis->Term(c) * p_sqrtweight[r];
      }
    }

    // Ok use  to solve for the coefficients
    // [A] = [Q][R]  where [Q] is MxN and orthogonal and  [R] is an NxN,
    // upper triangular matrix.  TNT provides the solve method that inverts
    // [Q] and backsolves [R] to get the coefficients in the vector x 
    JAMA::QR<double> qr(A);

    // Using A and our b we can solve for the coefficients
    TNT::Array1D<double> b(p_expected.size());
    for (int r=0; r<(int)p_expected.size();r++) {
      b[r] = p_expected[r] * p_sqrtweight[r];
    }

// Check to make sure the matrix is full rank before solving -- rectangular matrices must
// be full rank in order for the solve method to be successful
// 
    int full = qr.isFullRank(); 
    if (full == 0) {
      std::string msg = "Not full rank";
      throw Isis::iException::Message(Isis::iException::Math,msg,_FILEINFO_);
    }

    TNT::Array1D<double> coefs = qr.solve(b);

    // Set the coefficients in our basis equation
    std::vector<double> bcoefs;
    for (int i=0; i<coefs.dim1(); i++) bcoefs.push_back(coefs[i]);
    p_basis->SetCoefficients(bcoefs);

    // Compute the errors
    for (int i=0; i<(int)p_input.size(); i++) {
      double value = p_basis->Evaluate(p_input[i]);
      p_residuals.push_back(value-p_expected[i]);
    }

    // All done
    p_solved = true;
  }




/** 
 * @brief  Solve using sparse class 
 *  
 * After all the data has been registered through AddKnown, invoke this
 * method to solve the system of equations Ax = b, with a sparse solver which
 * solves the matrix by factorizing A.  You can then use the Evaluate and 
 * Residual methods freely. 
 *  
 * @internal 
 * @history  2008-04-16 Debbie Cook / Tracie Sucharski, New method 
 * @history  2008-04-23 Tracie Sucharski,  Fill sparse matrix as we go in 
 *                          AddKnown method rather than in the solve method,
 *                          otherwise we run out of memory very quickly.
 * @history  2009-04-08 Tracie Sucharski - Added return value which is a
 *                          column number of a column that contained all zeros.
 * @history 2009-12-21  Jeannie Walldren - Changed variable name 
 *          from p_weight to p_sqrtweight.
 *  
 */
#if !defined(__sun__)
  int LeastSquares::SolveSparse () {

    //??? Resize sparse to correct rows , accounting for held,ground,ignored
    //gmm::resize(p_sparseA,p_sparseRows,p_sparseCols);
    // We are solving Ax=b 

    // Create the right-hand-side column matrix B now.  Using A and our B we
    // can solve for the coefficients
    gmm::dense_matrix<double> b(p_sparseRows,1);
    std::vector<double> x(p_sparseCols);

    for (int r=0; r<p_sparseRows; r++) {
      b(r,0) = p_expected[r] * p_sqrtweight[r];
    }

    //  Create square matrix
    gmm::row_matrix<gmm::rsvector<double> > Asquare(p_sparseCols,p_sparseCols);
    gmm::mult(gmm::transposed(p_sparseA),p_sparseA,Asquare);

    //  Test for any columns with all 0's
    //  Return column number so caller can determine the appropriate error.
    int numNonZeros;
    for (int c=0; c<p_sparseCols; c++) {
      numNonZeros = gmm::nnz(gmm::sub_matrix(Asquare,
                             gmm::sub_interval(0,p_sparseCols),
                             gmm::sub_interval(c,1)));
      if (numNonZeros == 0) return c + 1;
    }

    gmm::dense_matrix<double> bAtrans(p_sparseCols,1);

    gmm::mult(gmm::transposed(p_sparseA),b,bAtrans);
//    int perm = 0;  //  use natural ordering
    int perm = 2;  //  use mmd_at_plus_a ordering
    double recond;

    gmm::SuperLU_solve(Asquare,x,gmm::mat_const_col(bAtrans,0),recond,perm);

    // Set the coefficients in our basis equation
    std::vector<double> bcoefs;

    for (int i=0; i<p_sparseCols; i++) bcoefs.push_back(x[i]);
    p_basis->SetCoefficients(bcoefs);

    // Compute the errors
    for (int i=0; i<p_sparseRows; i++) {
      std::vector<double> data(p_sparseCols);
      gmm::copy(gmm::mat_row(p_sparseA,i),data);
      double value = p_basis->Evaluate(data);
      p_residuals.push_back(value-p_expected[i]);
    }
    // All done
    p_solved = true;
    return 0;
  }
#endif


/**
 * Invokes the BasisFunction Evaluate method.
 *
 * @param data The input variables to evaluate.
 *
 * @return The evaluation for the input variable.
 *
 * @throws Isis::iException::Programmer - Unable to evaluate until a
 *                                        solution has been computed
 */
  double LeastSquares::Evaluate (const std::vector<double> &data) {
    if (!p_solved) {
      std::string msg = "Unable to evaluate until a solution has been computed";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    return p_basis->Evaluate(data);
  }

/**
 * Returns a vector of residuals (errors). That is, the difference between the
 * evaluation of a known with the solution against the expected value.
 *
 * @return The vector of residuals.
 *
 * @throws Isis::iException::Programmer - Unable to return residuals until a
 *                                        solution has been computed
 */
  std::vector<double> LeastSquares::Residuals () const {
    if (!p_solved) {
      std::string msg = "Unable to return residuals until a solution has been computed";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    return p_residuals;
  }

/**
 * Returns the ith residual. That is, the difference between the evaluation of
 * a known with the solution against the expected value. There is one residual
 * for each time AddKnown was invoked.
 *
 * @param i The number of times AddKnown was invoked to be evaluated.
 *
 * @return The output value of the residual.
 *
 * @throws Isis::iException::Programmer - Unable to return residuals until a
 *                                        solution has been computed
 */
  double LeastSquares::Residual (int i) const {
    if (!p_solved) {
      std::string msg = "Unable to return residuals until a solution has been computed";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    return p_residuals[i];
  }

/** 
 * Reset the weight for the ith known. This weight will not be used unless 
 * the system is resolved using the Solve method.
 * 
 * @param index The position in the array to assign the given weight value
 * @param weight A weight factor to apply to the ith known. A weight less 
 *               than one increase the residual for this known while a 
 *               weight greater than one decrease the residual for this 
 *               known.
 * @internal 
 * @history 2009-12-21  Jeannie Walldren,  Modified code to add 
 *          the square root of the weight to the vector
 *          p_sqrtweight.
 *  
 */
  void LeastSquares::Weight (int index, double weight) { 
    if (weight == 1) {
      p_sqrtweight[index] = weight;
    }
    else{
      p_sqrtweight[index] = sqrt(weight);
    }
  }

} // end namespace isis
