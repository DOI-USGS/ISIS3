#ifndef Affine_h
#define Affine_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.6 $                                                             
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
#include "iException.h"

namespace Isis {
/**                                                                       
 * @brief Affine basis function                
 *                                                                        
 * An affine transform in two-dimensional space is defined as
 *
 * @code
 * x' = Ax + By + C
 * y' = Dx + Ey + F
 * @endcode
 * 
 * This routine allows the programmer to define three or more
 * mappings from (x,y) to (x',y') and will solve for A,B,C,D,E,F.
 * 
 * If the above coefficients can be computed then the inverse of 
 * the affine transform exists and will be computed such that
 * 
 * @code
 * x = A'x' + B'y' + C'
 * y = D'x' + E'y' + F'
 * @endcode
 *                                                                         
 * Alternatively (or in combination), translations and rotations 
 * can be applied to create a transform.
 * 
 * @see http://www.gnome.org/~mathieu/libart/libart-affine-transformation-matrices.html 
 * 
 * @ingroup Math                                                  
 *                                                                        
 * @author  2005-03-24 Jeff Anderson                                    
 *                                                                        
 * @internal                                                              
 *  @todo Allow the programmer to apply scale and shear.
 *  @todo Write multiplaction method (operator*) for Affine * Affine.
 * 
 *  @history 2006-08-03  Tracie Sucharski, Added Scale method
 *  @history 2007-07-12  Debbie A. Cook, Added methods Coefficients
 *           and InverseCoefficients
 *  @history 2008-06-18 Christopher Austin - Added documentation
 *  @history 2008-10-29 Steven Lambright - Corrected usage of std::vector,
 *                      problem pointed out by "novas0x2a" (Support Forum
 *                      Member)
 * @history 2009-07-24 Kris Becker Introduced the AMatrix typedef; added new 
 *          constructor that accepts an AMatrix; added static method to return
 *          an Affine identity matrix; added methods to retrieve forward and
 *          inverse AMatrixs; added new method that inverts the matrix.
 * 
 */                                                                       

  class Affine {
    public:
      typedef TNT::Array2D<double> AMatrix;
      Affine ();
      Affine(const AMatrix &a);
      ~Affine ();
      void Solve (const double x[], const double y[], 
                  const double xp[], const double yp[], int n);
      static AMatrix getIdentity();
      void Identity ();
      void Translate (double tx, double ty);
      void Rotate(double rot);
      void Scale(double scaleFactor);

      void Compute(double x, double y);

      //! Returns the computed x'
      double xp() const { return p_xp; };  

      //! Returns the computed y'
      double yp() const { return p_yp; };

      void ComputeInverse(double xp, double yp);

      //! Returns the computed x
      double x() const { return p_x; };

      //! Returns the computed y
      double y() const { return p_y; };

      std::vector<double> Coefficients( int var );
      std::vector<double> InverseCoefficients( int var );
      //! Returns the forward Affine matrix
      AMatrix Forward() const { return (p_matrix.copy()); }
      //! Returns the inverse Affine matrix
      AMatrix Inverse() const { return (p_invmat.copy()); }

    private:
      AMatrix p_matrix;   //!< Affine forward matrix
      AMatrix p_invmat;   //!< Affine inverse matrix

      double p_x;         //!< x value of the (x,y) coordinate
      double p_y;         //!< y value of the (x,y) coordinate
      double p_xp;        //!< x' value of the (x',y') coordinate
      double p_yp;        //!< y' value of the (x',y') coordinate

      void checkDims(const AMatrix &am) const throw (iException &);
      AMatrix invert(const AMatrix &a) const throw (iException &);
  };
};

#endif

