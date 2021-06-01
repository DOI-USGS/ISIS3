#ifndef LeastSquares_h
#define LeastSquares_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <vector>

#include "tnt/tnt_array2d.h"

#include <armadillo>

#include "BasisFunction.h"

namespace Isis {
  /**
   * @brief Generic least square fitting class
   *
   * This class can be used to solved systems of linear equations through least
   * squares fitting.  The solution is derived through
   *      singular value decomposition or QR decomposition.  For example:
   * @f[
   * x + y = 3
   * @f]
   * @f[
   * -2x + 3y = 1
   * @f]
   * @f[
   * 2x -  y = 2
   * @f]
   * Is a simple system of equations that can be solved using this class.
   * @code
   *   Isis::BasisFunction b("Linear",2,2);
   *   Isis::LeastSquares lsq(b);
   *
   *   std::vector<double> one;
   *   one.push_back(1.0);
   *   one.push_back(1.0);
   *   lsq.AddKnown(one,3.0);
   *
   *   std::vector<double> two;
   *   two.push_back(-2.0);
   *   two.push_back(3.0);
   *   lsq.AddKnown(two,1.0);
   *
   *   std::vector<double> tre;
   *   tre.push_back(2.0);
   *   tre.push_back(-1.0);
   *   lsq.AddKnown(tre,2.0);
   *
   *   lsq.Solve();
   *
   *   double out1 = lsq.Evaluate(one);
   *   double out2 = lsq.Evaluate(two);
   *   double out3 = lsq.Evaluate(tre);
   *  @endcode
   *
   * @see PolynomialUnivariate
   * @see PolynomialBiVariate
   * @see BasisFunction
   *
   * @ingroup Math
   *
   * @author 2004-06-24 Jeff Anderson
   *
   * @internal
   *   @history  2004-06-24 Jeff Anderson, Original version
   *   @history  2007-11-16 Tracie Sucharski and Debbie A. Cook Added SolveQRD
   *                            method for a faster solve
   *   @history  2008-02-06 Tracie Sucharski, Renamed Solve to SolveSVD and make
   *                            private.  Add public Solve, with a new parameter,
   *                            method which defaults to SVD.  This calls the
   *                            correct solution method. This was done for
   *                            documentation purposes-clarifies the default
   *                            solves by SVD.
   *   @history  2008-04-16 Debbie Cook / Tracie Sucharski, Added SolveSparse.
   *   @history  2008-06-09 Tracie Sucharski, Added conditional compilations
   *                            for Solaris.  We could not get SuperLu to build
   *                            under Solaris due to a confilict with Complex
   *                            definitions.
   *   @history  2009-04-06 Tracie Sucharski, Added return value to SolveSparse,
   *                            which indicates a column containing all zeroes
   *                            which causes superlu to bomb.
   *   @history  2009-12-21 Jeannie Walldren, Changed p_weight to
   *                            p_sqrtweight. Modified Weight()
   *                            and AddKnown() to take the square
   *                            root of the given weight and add
   *                            it to the p_sqrtweight vector.
   *   @history  2010-04-20 Debbie A. Cook, Replaced SparseReset with Reset
   *                            to reset all solution methods
   *   @history  2010-11-22 Debbie A. Cook - Merged with Ken Edmundson version
   *   @history  2013-12-29 Jeannie Backer - Improved error messages. Fixes #962.
   *   @history  2019-09-05 Makayla Shepherd & Jesse Mapel - Changed sparse solution to use
   *                            Armadillo library's SuperLU interface instead of GMM.
   *
   */
  class LeastSquares {
    public:

      LeastSquares(Isis::BasisFunction &basis, bool sparse = false,
                   int sparseRows = 0, int sparseCols = 0, bool jigsaw = false);
      ~LeastSquares();
      void AddKnown(const std::vector<double> &input, double expected,
                    double weight = 1.0);

      std::vector<double> GetInput(int row) const;
      double GetExpected(int row) const;
      int Rows() const;

      enum SolveMethod { SVD,     //!<  Singular Value Decomposition
                         QRD,     //!<  QR Decomposition
                         SPARSE   //!<  Sparse
                       };

      int Solve(Isis::LeastSquares::SolveMethod method = SVD);
      double Evaluate(const std::vector<double> &input);
      std::vector<double> Residuals() const;
      double Residual(int i) const;
      void Weight(int index, double weight);

      /**
       * The number of knowns (or times AddKnown was invoked) linear combination
       * of the variables.
       *
       * @return int The number of knowns
       */
      int Knowns() const {
        return p_expected.size();
      };

      double GetSigma0() { return p_sigma0; }
      int GetDegreesOfFreedom() { return p_degreesOfFreedom; }
      void Reset ();

      void ResetSparse() { Reset(); }
      std::vector<double> GetEpsilons () const { return p_epsilonsSparse; }
      void SetParameterWeights(const std::vector<double> weights) { p_parameterWeights = weights; }
      void SetNumberOfConstrainedParameters(int n) { p_constrainedParameters = n; }

    private:
      void SolveSVD();
      void SolveQRD();
      void SolveCholesky () {}

      int SolveSparse();
      void FillSparseA(const std::vector<double> &data);
      bool ApplyParameterWeights();

      arma::mat p_xSparse;          /**<sparse solution matrix*/
      std::vector<double> p_epsilonsSparse;   /**<sparse vector of total parameter corrections*/
      std::vector<double> p_parameterWeights; /**<vector of parameter weights*/

      arma::SpMat<double> p_sparseA; /**<design matrix 'A' */
      arma::SpMat<double> p_normals; /**<normal equations matrix 'N'*/
      arma::mat p_ATb;                   /**<right-hand side vector*/
      arma::mat p_SLU_Factor;          /**<decomposed normal equations matrix*/

      bool p_jigsaw;
      bool p_sparse;
      bool p_solved;  /**<Boolean value indicating solution is complete*/

      int p_currentFillRow;
      int p_sparseRows;
      int p_sparseCols;
      int p_constrainedParameters; /**<constrained parameters*/
      int p_degreesOfFreedom;      /**<degrees of freedom (redundancy)*/

      double p_sigma0;               /**<sigma nought - reference variance*/

      std::vector<std::vector<double> > p_input; /**<A vector of the input
                                                       variables to evaluate.*/
      std::vector<double> p_expected;            /**<A vector of the expected
                                                       values when solved.*/
      std::vector<double> p_sqrtWeight;          /**<A vector of the square
                                                       roots of the weights
                                                       for each known value.*/
      std::vector<double> p_residuals;           /**<A vector of the residuals
                                                       (or difference between
                                                       expected and solved
                                                       values).*/
      Isis::BasisFunction *p_basis; //!<Pointer to the BasisFunction object



  };
};


#endif
