#ifndef LeastSquares_h
#define LeastSquares_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.9 $                                                             
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
#include <vector>

#include "tnt/tnt_array2d.h"

#if !defined(__sun__)
#include "gmm/gmm.h"
#endif

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
 *   @history  2008-04-16 Debbie Dook / Tracie Sucharski, Added SolveSparse.
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
 *  
 */                                                                       
  class LeastSquares {
    public:

      LeastSquares (Isis::BasisFunction &basis,bool sparse = false,
                    int sparseRows=0,int sparseCols=0);
      ~LeastSquares ();
      void AddKnown (const std::vector<double> &input, double expected, 
                     double weight = 1.0);

      std::vector<double> GetInput(int row) const;
      double GetExpected(int row) const;
      int Rows() const;

      enum SolveMethod { SVD,     //!<  Singular Value Decomposition
                         QRD,     //!<  QR Decomposition
                         SPARSE   //!<  Sparse
                       };

      int Solve (Isis::LeastSquares::SolveMethod method = SVD);
      double Evaluate (const std::vector<double> &input); 
      std::vector<double> Residuals () const;
      double Residual (int i) const;
      void Weight (int index, double weight);

     /** 
      * The number of knowns (or times AddKnown was invoked) linear combination 
      * of the variables.
      * 
      * @return int The number of knowns
      */
      int Knowns () const { return p_expected.size(); };
  
      private:
        void SolveSVD ();
        void SolveQRD ();

#if !defined(__sun__)
        int SolveSparse ();
        void FillSparseA (const std::vector<double> &data);
        gmm::row_matrix<gmm::rsvector<double> > p_sparseA;
#endif
        bool p_sparse;
        int p_sparseRows;
        int p_sparseCols;
        int p_currentFillRow;

        bool p_solved;                /**<Boolean value representing whether or   
                                          not the function has been solved.*/
        Isis::BasisFunction *p_basis; //!<Pointer to the BasisFunction object


        std::vector<std::vector<double> > p_input; /**<A vector of the input 
                                                       variables to evaluate.*/
        std::vector<double> p_expected;            /**<A vector of the expected 
                                                       values when solved.*/
        std::vector<double> p_sqrtweight;          /**<A vector of the square 
                                                       roots of the weights 
                                                       for each known value.*/
        std::vector<double> p_residuals;           /**<A vector of the residuals
                                                       (or difference between 
                                                       expected and solved 
                                                       values).*/

  };
};


#endif
