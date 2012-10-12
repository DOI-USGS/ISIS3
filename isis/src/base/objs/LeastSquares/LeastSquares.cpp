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
#include "IException.h"
#include "IString.h"

namespace Isis {
  /**
   * Creates a LeastSquares Object.
   *
   * @param basis A BasisFunction. This parameter allows for the least squares
   *              fitting to be applied to arbitrary equations.
   */
  LeastSquares::LeastSquares(Isis::BasisFunction &basis, bool sparse,
                             int sparseRows, int sparseCols, bool jigsaw) {
    p_jigsaw = jigsaw;
    p_basis = &basis;
    p_solved = false;
    p_sparse = sparse;
    p_sigma0 = 0.;

#if defined(__sun__)
    p_sparse = false;
#endif

    if (p_sparse) {

      //  make sure sparse nrows/ncols have been set
      if (sparseRows == 0  ||  sparseCols == 0) {
        std::string msg = "If solving using sparse matrices, you must enter";
        msg += " the number of rows/columns";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

#if !defined(__sun__)
      gmm::resize(p_sparseA, sparseRows, sparseCols);
      gmm::resize(p_normals, sparseCols, sparseCols);
      gmm::resize(p_ATb, sparseCols, 1);
      p_xSparse.resize(sparseCols);

      if( p_jigsaw ) {
        p_epsilonsSparse.resize(sparseCols);
        std::fill_n(p_epsilonsSparse.begin(), sparseCols, 0.0);

        p_parameterWeights.resize(sparseCols);
      }

#endif
      p_sparseRows = sparseRows;
      p_sparseCols = sparseCols;
    }
    p_currentFillRow = -1;
  }

  //! Destroys the LeastSquares object.
  LeastSquares::~LeastSquares() {
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
   * @throws Isis::IException::Programmer - Number of elements in data does not
   *                                        match basis requirements
   *
   * @internal
   * @history 2008-04-22  Tracie Sucharski,  Fill sparse matrix.
   * @history 2009-12-21  Jeannie Walldren,  Modified code to add
   *          the square root of the weight to the vector
   *          p_sqrtweight.
   *
   */
  void LeastSquares::AddKnown(const std::vector<double> &data, double result,
                              double weight) {
    if((int) data.size() != p_basis->Variables()) {
      std::string msg = "Number of elements in data does not match basis [" +
                        p_basis->Name() + "] requirements";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    p_expected.push_back(result);

    if (weight == 1) {
      p_sqrtWeight.push_back(weight);
    }
    else {
      p_sqrtWeight.push_back(sqrt(weight));
    }

    if(p_sparse) {
#if !defined(__sun__)
      FillSparseA(data);
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
  void LeastSquares::FillSparseA(const std::vector<double> &data) {

    p_basis->Expand(data);

    p_currentFillRow++;

    //  ??? Speed this up using iterator instead of array indices???
    int ncolumns = (int)data.size();

    for(int c = 0;  c < ncolumns; c++) {
      p_sparseA(p_currentFillRow, c) = p_basis->Term(c) * p_sqrtWeight[p_currentFillRow];
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
      msg += "[Given = " + Isis::IString(row) + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
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
      msg += "[Given = " + Isis::IString(row) + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
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
   * @history  2008-04-16 Debbie Cook / Tracie Sucharski, Added SolveSparse.
   * @history  2009-04-08 Tracie Sucharski - Added return value which will
   *                          pass on what is returned from SolveSparse which
   *                          is a column number of a column that contained
   *                          all zeros.
   * @history  2010-12-12 Debbie A. Cook  Fixed "no data" test for SPARSE
   *                          case
   */
  int LeastSquares::Solve(Isis::LeastSquares::SolveMethod method) {

#if defined(__sun__)
    if(method == SPARSE) method = QRD;
#endif

    if((method == SPARSE  &&  p_sparseRows == 0)  ||
       (method != SPARSE  &&  Rows() == 0 )) {
      p_solved = false;
      std::string msg = "No solution available because no input data was "
                        "provided";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    if(method == SVD) {
      SolveSVD();
    }
    else if(method == QRD) {
      SolveQRD();
    }
    else if(method == SPARSE) {
#if !defined(__sun__)
      int column = SolveSparse();
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
  void LeastSquares::SolveSVD() {

    // We are solving Ax=b ... start by creating A
    TNT::Array2D<double> A(p_input.size(), p_basis->Coefficients());
    for(int r = 0; r < A.dim1(); r++) {
      p_basis->Expand(p_input[r]);
      for(int c = 0; c < A.dim2(); c++) {
        A[r][c] = p_basis->Term(c) * p_sqrtWeight[r];
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

    for(int i = 0; i < invS.dim1(); i++) {
      if(invS[i][i] != 0.0) invS[i][i] = 1.0 / invS[i][i];
    }

    // Transpose U
    TNT::Array2D<double> U;
    svd.getU(U);
    TNT::Array2D<double> transU(U.dim2(), U.dim1());

    for(int r = 0; r < U.dim1(); r++) {
      for(int c = 0; c < U.dim2(); c++) {
        transU[c][r] = U[r][c];
      }
    }

    // Now multiply everything together to get [A+]
    TNT::Array2D<double> VinvS = TNT::matmult(V, invS);
    TNT::Array2D<double> Aplus = TNT::matmult(VinvS, transU);

    // Using Aplus and our b we can solve for the coefficients
    TNT::Array2D<double> b(p_expected.size(), 1);

    for(int r = 0; r < (int)p_expected.size(); r++) {
      b[r][0] = p_expected[r] * p_sqrtWeight[r];
    }

    TNT::Array2D<double> coefs = TNT::matmult(Aplus, b);

    // If the rank of the matrix is not large enough we don't
    // have enough coefficients for the solution
    if (coefs.dim1() < p_basis->Coefficients()) {
      std::string msg = "No solution available ... ";
      msg += "Not enough knowns or knowns are co-linear ... ";
      msg += "[Unknowns = " + Isis::IString(p_basis->Coefficients()) + "] ";
      msg += "[Knowns = " + Isis::IString(coefs.dim1()) + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    // Set the coefficients in our basis equation
    std::vector<double> bcoefs;
    for (int i = 0; i < coefs.dim1(); i++) bcoefs.push_back(coefs[i][0]);

    p_basis->SetCoefficients(bcoefs);

    // Compute the errors
    for(int i = 0; i < (int)p_input.size(); i++) {
      double value = p_basis->Evaluate(p_input[i]);
      p_residuals.push_back(value - p_expected[i]);
      p_sigma0 += p_residuals[i]*p_residuals[i]*p_sqrtWeight[i]*p_sqrtWeight[i];
    }
    // calculate degrees of freedom (or redundancy)
    // DOF = # observations + # constrained parameters - # unknown parameters
    p_degreesOfFreedom = p_basis->Coefficients() - coefs.dim1();

    if( p_degreesOfFreedom > 0.0 )  {
      p_sigma0 = p_sigma0/(double)p_degreesOfFreedom;
    }

    // check for p_sigma0 < 0
    p_sigma0 = sqrt(p_sigma0);

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
  void LeastSquares::SolveQRD() {

    // We are solving Ax=b ... start by creating A
    TNT::Array2D<double> A(p_input.size(), p_basis->Coefficients());
    for(int r = 0; r < A.dim1(); r++) {
      p_basis->Expand(p_input[r]);
      for(int c = 0; c < A.dim2(); c++) {
        A[r][c] = p_basis->Term(c) * p_sqrtWeight[r];
      }
    }

    // Ok use  to solve for the coefficients
    // [A] = [Q][R]  where [Q] is MxN and orthogonal and  [R] is an NxN,
    // upper triangular matrix.  TNT provides the solve method that inverts
    // [Q] and backsolves [R] to get the coefficients in the vector x
    JAMA::QR<double> qr(A);

    // Using A and our b we can solve for the coefficients
    TNT::Array1D<double> b(p_expected.size());
    for(int r = 0; r < (int)p_expected.size(); r++) {
      b[r] = p_expected[r] * p_sqrtWeight[r];
    }

// Check to make sure the matrix is full rank before solving -- rectangular matrices must
// be full rank in order for the solve method to be successful
//
    int full = qr.isFullRank();
    if(full == 0) {
      std::string msg = "Not full rank";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    TNT::Array1D<double> coefs = qr.solve(b);

    // Set the coefficients in our basis equation
    std::vector<double> bcoefs;
    for(int i = 0; i < coefs.dim1(); i++) bcoefs.push_back(coefs[i]);
    p_basis->SetCoefficients(bcoefs);

    // Compute the errors
    for(int i = 0; i < (int)p_input.size(); i++) {
      double value = p_basis->Evaluate(p_input[i]);
      p_residuals.push_back(value - p_expected[i]);
    }

    // All done
    p_solved = true;
  }




  /**
   * @brief  Solve using sparse class
   *
   * After all the data has been registered through AddKnown, invoke this
   * method to solve the system of equations Nx = b, where
   *
   * N = ATPA
   * b = ATPl, and
   *
   * N is the "normal equations matrix;
   * A is the so-called "design" matrix;
   * P is the observation weight matrix (typically diagonal);
   * l is the "computed - observed" column vector;
   *
   * The solution is achieved using a sparse matrix formulation of
   * the LU decomposition of the normal equations.
   *
   * You can then use the Evaluate and Residual methods freely.
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
   * @history 2010        Ken Edmundson
   * @history 2010-11-20  Debbie A. Cook Merged Ken Edmundson verion with system version
   * @history 2011-03-17  Ken Edmundson Corrected computation of residuals
   *
   */
#if !defined(__sun__)
  int LeastSquares::SolveSparse() {

    // form "normal equations" matrix by multiplying ATA
    gmm::mult(gmm::transposed(p_sparseA), p_sparseA, p_normals);

    //  Test for any columns with all 0's
    //  Return column number so caller can determine the appropriate error.
    int numNonZeros;
    for(int c = 0; c < p_sparseCols; c++) {
      numNonZeros = gmm::nnz(gmm::sub_matrix(p_normals,
                             gmm::sub_interval(0,p_sparseCols),
                             gmm::sub_interval(c,1)));

      if(numNonZeros == 0) return c + 1;
    }

    // Create the right-hand-side column vector 'b'
    gmm::dense_matrix<double> b(p_sparseRows, 1);

    // multiply each element of 'b' by it's associated weight
    for ( int r = 0; r < p_sparseRows; r++ )
      b(r,0) = p_expected[r] * p_sqrtWeight[r];

    // form ATb
    gmm::mult(gmm::transposed(p_sparseA), b, p_ATb);

    // apply parameter weighting if Jigsaw (bundle adjustment)
    if ( p_jigsaw ) {
      for( int i = 0; i < p_sparseCols; i++) {
        double weight = p_parameterWeights[i];

        if( weight <= 0.0 )
          continue;

        p_normals[i][i] += weight;
        p_ATb[i] -= p_epsilonsSparse[i]*weight;
      }
    }

//    printf("printing rhs\n");
//    for( int i = 0; i < m_nSparseCols; i++ )
//      printf("%20.10e\n",m_ATb[i]);

    // decompose normal equations matrix
    p_SLU_Factor.build_with(p_normals);

    // solve with decomposed normals and right hand side
    // int perm = 0;  //  use natural ordering
    int perm = 2;     //  confirm meaning and necessity of
//  double recond;    //  variables perm and recond
    p_SLU_Factor.solve(p_xSparse,gmm::mat_const_col(p_ATb,0), perm);

    // Set the coefficients in our basis equation
    p_basis->SetCoefficients(p_xSparse);

    // if Jigsaw (bundle adjustment)
    // add corrections into epsilon vector (keeping track of total corrections)
    if ( p_jigsaw ) {
      for( int i = 0; i < p_sparseCols; i++ )
        p_epsilonsSparse[i] += p_xSparse[i];
    }

    // test print solution
//    printf("printing solution vector and epsilons\n");
//    for( int a = 0; a < p_sparseCols; a++ )
//      printf("soln[%d]: %lf epsilon[%d]: %lf\n",a,p_xSparse[a],a,p_epsilonsSparse[a]);

//    printf("printing design matrix A\n");
//    for (int i = 0; i < p_sparseRows; i++ )
//    {
//      for (int j = 0; j < p_sparseCols; j++ )
//      {
//        if ( j == p_sparseCols-1 )
//          printf("%20.20e \n",(double)(p_sparseA(i,j)));
//        else
//          printf("%20.20e ",(double)(p_sparseA(i,j)));
//      }
//    }

    // Compute the image coordinate residuals and sum into Sigma0
    // (note this is exactly what was being done before, but with less overhead - I think)
    // ultimately, we should not be using the A matrix but forming the normals
    // directly. Then we'll have to compute the residuals by back projection

    p_residuals.resize(p_sparseRows);
    gmm::mult(p_sparseA, p_xSparse, p_residuals);
    p_sigma0 = 0.0;

    for ( int i = 0; i < p_sparseRows; i++ ) {
        p_residuals[i] = p_residuals[i]/p_sqrtWeight[i];
        p_residuals[i] -= p_expected[i];
        p_sigma0 += p_residuals[i]*p_residuals[i]*p_sqrtWeight[i]*p_sqrtWeight[i];
    }

//    std::cout << p_residuals << std::endl;

    // if Jigsaw (bundle adjustment)
    // add contibution to Sigma0 from constrained parameters
    if ( p_jigsaw ) {
      double constrained_vTPv = 0.0;

      for ( int i = 0; i < p_sparseCols; i++ ) {
        double weight = p_parameterWeights[i];

        if ( weight <= 0.0 )
          continue;

        constrained_vTPv += p_epsilonsSparse[i]*p_epsilonsSparse[i]*weight;
      }

//      std::cout << "vtpv image = " << p_sigma0 << std::endl;
//      std::cout << "sqrtWeight = " << p_sqrtWeight[0] << std::endl;

      p_sigma0 += constrained_vTPv;

//      std::cout << "vtpv constrained = " << constrained_vTPv << std::endl;

    }
    // calculate degrees of freedom (or redundancy)
    // DOF = # observations + # constrained parameters - # unknown parameters
    p_degreesOfFreedom = p_sparseRows + p_constrainedParameters - p_sparseCols;

    if( p_degreesOfFreedom <= 0.0 ) {
      printf("Observations: %d\nConstrained: %d\nParameters: %d\nDOF: %d\n",
             p_sparseRows,p_constrainedParameters,p_sparseCols,p_degreesOfFreedom);
      p_sigma0 = 1.0;
    }
    else
      p_sigma0 = p_sigma0/(double)p_degreesOfFreedom;


//    std::cout << "degrees of freedom = " << p_degreesOfFreedom << std::endl;

    // check for p_sigma0 < 0
    p_sigma0 = sqrt(p_sigma0);

    // kle testing - output residuals and some stats
    printf("Sigma0 = %20.10lf\nNumber of Observations = %d\nNumber of Parameters = %d\nNumber of Constrained Parameters = %d\nDOF = %d\n",p_sigma0,p_sparseRows,p_sparseCols,p_constrainedParameters,p_degreesOfFreedom);
//    printf("printing residuals\n");
//    for( int k = 0; k < p_sparseRows; k++ )
//    {
//      printf("%lf %lf\n",p_residuals[k],p_residuals[k+1]);
//      k++;
//    }

    // All done
    p_solved = true;
    return 0;
  }
#endif

  /**
   * @brief  Error propagation for sparse least-squares solution
   *
   * Computes the variance-covariance matrix of the parameters.
   * This is the inverse of the normal equations matrix, scaled by
   * the reference variance (also called variance of unit weight,
   * etc).
   *
   * @internal
   * @history  2009-11-19 Ken Edmundson, New method
   *
   * Notes:
   *
   * 1) The SLU_Factor (Super LU Factor) has already been
   * factorised in each iteration but there is no gmm++ method to
   * get the inverse of the sparse matrix implementation. so we
   * have to get it ourselves. This is don by solving the
   * factorized normals repeatedly with right-hand sides that are
   * the columns of the identity matrix (which is how gmm - or
   * anybody else would do it).
   *
   * 2) We should create our own matrix library, probably wrapping
   * the gmm++ library (and perhaps other(s) that may have
   * additional desired functionality). The inverse function
   * should be part of the library, along with the capacity for
   * triangular storage and other decomposition techniques -
   * notably Cholesky.
   *
   * 3) The LU decomposition can be be performed even if the
   * normal equations are singular. But, we should always be
   * dealing with a positive-definite matrix (for the bundle
   * adjustment). Cholesky is faster, more efficient, and requires
   * a positive-definite matrix, so if it fails, it is an
   * indication of a singular matrix - bottom line - we should be
   * using Cholesky. Or a derivative of Cholesky, i.e., UTDU
   * (LDLT).
   *
   * 4) As a consequence of 3), we should be checking for
   * positive-definite state of the normals, perhaps via the
   * matrix determinant, prior to solving. There is a check in
   * place now that checks to see if a column of the design matrix
   * (or basis?) is all zero. This is equivalent - if a set of
   * vectors contains the zero vector, then the set is linearly
   * dependent, and the matrix is not positive-definite. In
   * Jigsaw, the most likely cause of the normals being
   * non-positive-definite probably is failure to establish the
   * datum (i.e. constraining a minimum of seven parameters -
   * usually 3 coordinates of two points and 1 of a third).
   */
#if !defined(__sun__)
  bool LeastSquares::SparseErrorPropagation ()
  {
    // clear memory
    gmm::clear(p_ATb);
    gmm::clear(p_xSparse);

    // for each column of the inverse, solve with a right-hand side consisting
    // of a column of the identity matrix, putting each resulting solution vectfor
    // into the corresponding column of the inverse matrix
    for ( int i = 0; i < p_sparseCols; i++ )
    {
      if( i > 0 )
        p_ATb(i-1,0) = 0.0;

      p_ATb(i,0) = 1.0;

      // solve with decomposed normals and right hand side
      p_SLU_Factor.solve(p_xSparse,gmm::mat_const_col(p_ATb,0));

      // put solution vector x into current column of inverse matrix
      gmm::copy(p_xSparse, gmm::mat_row(p_normals, i));
    }

    // scale inverse by Sigma0 squared to get variance-covariance matrix
    // if simulated data, we don't scale (effectively scaling by 1.0)
    //    printf("scaling by Sigma0\n");
    gmm::scale(p_normals,(p_sigma0*p_sigma0));

//    printf("covariance matrix\n");
//    for (int i = 0; i < p_sparseCols; i++ )
//    {
//      for (int j = 0; j < p_sparseCols; j++ )
//      {
//        if ( j == p_sparseCols-1 )
//          printf("%20.20e \n",(double)(p_sparseInverse(i,j)));
//        else
//          printf("%20.20e ",(double)(p_sparseInverse(i,j)));
//      }
//    }

    // standard deviations from covariance matrix
//    printf("parameter standard deviations\n");
//  for (int i = 0; i < p_sparseCols; i++ )
//    {
//      printf("Sigma Parameter %d = %20.20e \n",i+1,sqrt((double)(p_sparseInverse(i,i))));
//    }

    return true;
  }
#endif


  void LeastSquares::Reset ()
  {
    if ( p_sparse ) {
      gmm::clear(p_sparseA);
      gmm::clear(p_ATb);
      gmm::clear(p_normals);
      p_currentFillRow = -1;
    }
    else {
      p_input.clear();
      //      p_sigma0 = 0.;
    }
      p_sigma0 = 0.;
    p_residuals.clear();
    p_expected.clear();
    p_sqrtWeight.clear();
    p_solved = false;
  }



  /**
   * Invokes the BasisFunction Evaluate method.
   *
   * @param data The input variables to evaluate.
   *
   * @return The evaluation for the input variable.
   *
   * @throws Isis::IException::Programmer - Unable to evaluate until a
   *                                        solution has been computed
   */
  double LeastSquares::Evaluate(const std::vector<double> &data) {
    if(!p_solved) {
      std::string msg = "Unable to evaluate until a solution has been computed";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    return p_basis->Evaluate(data);
  }

  /**
   * Returns a vector of residuals (errors). That is, the difference between the
   * evaluation of a known with the solution against the expected value.
   *
   * @return The vector of residuals.
   *
   * @throws Isis::IException::Programmer - Unable to return residuals until a
   *                                        solution has been computed
   */
  std::vector<double> LeastSquares::Residuals() const {
    if(!p_solved) {
      std::string msg = "Unable to return residuals until a solution has been computed";
      throw IException(IException::Programmer, msg, _FILEINFO_);
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
   * @throws Isis::IException::Programmer - Unable to return residuals until a
   *                                        solution has been computed
   */
  double LeastSquares::Residual(int i) const {
    if(!p_solved) {
      std::string msg = "Unable to return residuals until a solution has been computed";
      throw IException(IException::Programmer, msg, _FILEINFO_);
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

  void LeastSquares::Weight(int index, double weight) {
    if(weight == 1) {
      p_sqrtWeight[index] = weight;
    }
    else {
      p_sqrtWeight[index] = sqrt(weight);
    }
  }

} // end namespace isis
