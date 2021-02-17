#ifndef Affine_h
#define Affine_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>
#include "tnt/tnt_array2d.h"

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
   * @history 2010-11-18 Kris Becker Fixed bug in inverse representation when
   *          scaling is applied to the current transform.
   * @history 2017-06-28 Makayla Shepherd - Updated documentation. References #4807.
   *
   */

  class Affine {
    public:
      typedef TNT::Array2D<double> AMatrix; //!< Affine Matrix
      Affine();
      Affine(const AMatrix &a);
      ~Affine();
      void Solve(const double x[], const double y[],
                 const double xp[], const double yp[], int n);
      static AMatrix getIdentity();
      void Identity();
      void Translate(double tx, double ty);
      void Rotate(double rot);
      void Scale(double scaleFactor);

      void Compute(double x, double y);

      /**
       * Returns the computed x'
       * 
       * @return double Computed x'
       */
      double xp() const {
        return p_xp;
      };

      /**
       * Returns the computed y'
       * 
       * @return double Computed y'
       */
      double yp() const {
        return p_yp;
      };

      void ComputeInverse(double xp, double yp);

      /**
       * Returns the computed x
       * 
       * @return double Computed x
       */
      double x() const {
        return p_x;
      };

      /** 
       * Returns the computed y
       * 
       * @return double Computed y
       */
      double y() const {
        return p_y;
      };

      std::vector<double> Coefficients(int var);
      std::vector<double> InverseCoefficients(int var);
      
      /**
       * Returns the forward Affine matrix
       * 
       * @return AMatrix Forward Affine matrix
       */
      AMatrix Forward() const {
        return (p_matrix.copy());
      }
      
      /**
       * Returns the inverse Affine matrix
       * 
       * @return AMatrix Inverse Affine matrix
       */
      AMatrix Inverse() const {
        return (p_invmat.copy());
      }

    private:
      AMatrix p_matrix;   //!< Affine forward matrix
      AMatrix p_invmat;   //!< Affine inverse matrix

      double p_x;         //!< x value of the (x,y) coordinate
      double p_y;         //!< y value of the (x,y) coordinate
      double p_xp;        //!< x' value of the (x',y') coordinate
      double p_yp;        //!< y' value of the (x',y') coordinate

      void checkDims(const AMatrix &am) const;
      AMatrix invert(const AMatrix &a) const;
  };
};

#endif

