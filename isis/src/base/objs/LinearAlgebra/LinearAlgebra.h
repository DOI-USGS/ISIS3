#ifndef LinearAlgebra_h
#define LinearAlgebra_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
// std library
#include <iostream>

// boost library
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

// Qt Library
#include <QDebug>
#include <QPair>

// Prevent Boost from outputing debug information to
// standard error when throwing exceptions.
#define BOOST_UBLAS_NO_STD_CERR

namespace Isis {
  class Angle;
  class Distance;
  class Latitude;
  class Longitude;
  /**
   * This class holds all static methods to perform linear algebra
   * operations on vectors and matrices.
   *
   * This class has 4 representations of a 3 dimensional rotation
   * <ul>
   *   <li> A 3 X 3 rotation matrix </li>
   *   <li> An angle/axis pair - This pair is made up of a vector (axis)
   *   and angle of rotation about that axis.</li>
   *   <li> A quaternion - This is saved as a 4-valued unit vector of the
   *   form q = (q0, q1, q2, q3) where for the angle/axis pair of the form
   *   (A, (ux, uy, uz)), we have q0 = cos(A/2), q1 = ux*sin(A/2), q2 =
   *   uy*sin(A/2), q3 = uz*sin(A/2).</li>
   *   <li> A set of Euler angles - This is a set of three angles and
   *   their corresponding axis of rotation. In this case, each axis
   *   must be one of the x, y, or z axes for the coordinate system.</li>
   * </ul>
   *
   * @author 2013-06-01 Jeannie Backer
   *
   * @internal
   *   @history 2013-06-01 Jeannie Backer - Original version.
   *   @history 2013-08-07 Kimberly Oyama - Updated documentation.
   *   @history 2016-07-25 Jeannie Backer - Updated documentation and test.
   *   @history 2016-08-05 Jeannie Backer - Replace std abs() function with qAbs() from QtGlobal.
   *   @history 2016-08-16 Jesse Mapel - Added BOOST_UBLAS_NO_STD_CERR definition to
   *                           prevent Boost from outputing debug information to standard out
   *                           when throwing exceptions.  Fixes #2302.
   *   @history 2017-12-12 Jeannie Backer - Added SymmetricMatrix typedef.
   *   @history 2021-02-17 Jesse Mapel - Added pseudoinverse method.
   *
   *
   *   @todo document methods (a) add naif routine names to documentation where appropriate,
   *                          (b) clean up comments within methods,
   *                          (3) use naif documentation where appropriate
   *   @todo add links to naif documentation web pages???
   *   @todo add proper scopes
   *   @todo implement norm2 (L2 norm)?
   *   @todo implement trace?
   *   @todo common validate errors - vector size match, 3x3 matrix, 3D vector
   *   @todo what is more expensive - unitize vector and compare to 1 or isOrthogonal?
   *   @todo rename local variables m2q and q2m
   *   @todo implement qdq2av (Quaternion and quaternion derivative to angular velocity)?
   *   @todo implement qxq (Quaternion times quaternion)?
   *   @todo implement qderiv (Quadratic derivative)?
   *   @todo When converting quaternion, axis-angle, Euler angle to rot
   *         matrix, should we verify that the matrix is valid (is a rotation matrix)???
   *   @todo isQuaternion???
   *   @todo should we throw error in q2m if q = 0???
   *   @todo q2m - why check if sharpen when q is not unit if it is required
   *         to be a unit? throw an error?
   *   @todo axisar - throw error if axis is origin? If we rotate about
   *         about origin, then we get a zero matrix - not a rotation...
   *         except maybe trivially... (or do we get identity??? indicating no rotation)
   *   @todo double check all indexing on code copied from NAIF - indices
   *         should reverse col/row order and subtract 1
   */



  class LinearAlgebra {
    public:
      /**
       * Definition for an Isis::LinearAlgebra::Matrix of doubles. This is a
       * typedef for a boost matrix.
       *
       * Note: This typedef is used so that we can add functionality to an
       * existing matrix type and/or change which third party library's matrix
       * we are using without changing all references to this type in the ISIS
       * API.
       */
      typedef boost::numeric::ublas::matrix<double> Matrix;
      /**
       * Definition for an Isis::LinearAlgebra::SymmetrixMatrix of doubles with
       * an upper configuration. This is a typedef for a boost symmetric_matrix.
       *
       * Note: This typedef is used so that we can add functionality to an
       * existing matrix type and/or change which third party library's matrix
       * we are using without changing all references to this type in the ISIS
       * API.
       */
      typedef boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::upper> SymmetricMatrix;
      /**
       * Definition for an Isis::LinearAlgebra::MatrixUpperTriangular of doubles with
       * an upper configuration. This is a typedef for a boost symmetric_matrix.
       *
       * Note: This typedef is used so that we can add functionality to an
       * existing matrix type and/or change which third party library's matrix
       * we are using without changing all references to this type in the ISIS
       * API.
       */
      typedef boost::numeric::ublas::symmetric_matrix<double, boost::numeric::ublas::upper> MatrixUpperTriangular;
      /**
       * Definition for an Isis::LinearAlgebra::Vector of doubles. This is a
       * typedef for a boost vector.
       *
       * Note: This typedef is used so that we can add functionality to an
       * existing vector type and/or change which third party library's vector
       * we are using without changing all references to this type in the ISIS
       * API.
       */
      typedef boost::numeric::ublas::vector<double> Vector;
      /**
       * Definition for an Isis::LinearAlgebra::VectorCompressed of doubles. This is a
       * typedef for a boost vector. It stores the vector in a more memory efficient form.
       *
       * Note: This typedef is used so that we can add functionality to an
       * existing vector type and/or change which third party library's vector
       * we are using without changing all references to this type in the ISIS
       * API.
       */
       typedef boost::numeric::ublas::compressed_vector<double> VectorCompressed;

      // define AxisAngle and EulerAngle
      /**
       * Definition for an Axis-Angle pair. This is a three dimensional rotation
       * represented as an axis of rotation and a corresponding rotation angle.
       * AxisAngle is a typedef for a QPair of an Isis::LinearAlgebra::Vector
       * and an Isis::Angle.
       */
      typedef QPair<Vector, Angle> AxisAngle;
      /**
       * Definition for an EulerAngle pair. This is a three dimensional rotation
       * represented as an Euler angle and the number corresponding to its
       * rotation axis. EulerAngle is a typedef for a QPair of an Isis::Angle
       * and an integer (1, 2, or 3).
       */
      typedef QPair<Angle, int> EulerAngle;

      // check type of matrix/vector
      static bool isIdentity(const Matrix &matrix);
      static bool isOrthogonal(const Matrix &matrix);
      static bool isRotationMatrix(const Matrix &matrix);
      static bool isZero(const Matrix &matrix);
      static bool isZero(const Vector &vector);
      static bool isEmpty(const Vector &vector);
      static bool isUnit(const Vector &vector);

      // create special matrices
      static Matrix identity(int size);
      static Matrix transpose(const Matrix &matrix);
      static Matrix inverse(const Matrix &matrix);
      static Matrix pseudoinverse(const Matrix &matrix);
      static Matrix zeroMatrix(int rows, int columns);
      static Vector zeroVector(int size);

      //
      static double determinant(const Matrix &matrix);

      // magnitude/norm based calculations
      static Vector normalize(const Vector &vector);
      static double magnitude(const Vector &vector);
      static double absoluteMaximum(const Vector &vector);

      // arithmetic operations
      static Matrix multiply(const Matrix &matrix1, const Matrix &matrix2);
      static Vector multiply(const Matrix &matrix, const Vector &vector);
      static Vector multiply(double scalar, const Vector &vector);
      static Matrix multiply(double scalar, const Matrix &matrix);
      static Vector add(const Vector &vector1, const Vector &vector2);
      static Vector subtract(const Vector &vector1, const Vector &vector2);

      // vector products
      static Vector crossProduct(const Vector &vector1, const Vector &vector2);
      static Vector normalizedCrossProduct(const Vector &vector1, const Vector &vector2);
      static Matrix outerProduct(const Vector &vector1, const Vector &vector2);
      static double dotProduct(const Vector &vector1, const Vector &vector2);
      static double innerProduct(const Vector &vector1, const Vector &vector2);

      // vector movements (projections, rotations)
      static Vector project(const Vector &vector1, const Vector &vector2);
      static Vector rotate(const Vector &vector, const Vector &axis, Angle angle);

      //
      static Vector perpendicular(const Vector &vector1, const Vector &vector2);

      // converters
      static AxisAngle toAxisAngle(const Matrix &rotationMatrix);//raxisa
      static Matrix toMatrix(const AxisAngle &axisAngle); // axisar
      static Matrix toMatrix(const Vector &axis, Angle angle); // axisar

      static QList<EulerAngle> toEulerAngles(const Matrix &rotationMatrix, const QList<int> axes);// m2eul
      static Matrix toMatrix(const QList<EulerAngle> &eulerAngles); // eul2m
      static Matrix toMatrix(const EulerAngle &angle3, const EulerAngle &angle2, const EulerAngle &angle1); // eul2m

      static Vector toQuaternion(const Matrix &rotationMatrix);// m2q
      static Matrix toMatrix(const Vector &quaternion);// q2m

      //Matrix row/col to vector and vector to row/columns
      static void setRow(Matrix &matrix, const Vector &vector, int rowIndex);
      static void setColumn(Matrix &matrix, const Vector &vector, int columnIndex);
      static Vector row(const Matrix &matrix, int rowIndex);
      static Vector column(const Matrix &matrix, int columnIndex);

      static void setVec3(Vector *v, double v0, double v1, double v2);
      static void setVec4(Vector *v, double v0, double v1, double v2, double v3);
      static Vector vector(double v0, double v1, double v2);
      static Vector vector(double v0, double v1, double v2, double v3);

      static Vector subVector(const Vector &v, int start, int size);

      // No need to implement the std::cout operator<< since this is already
      // done in boost/numeric/ublas/io.hpp, which is included above
      // The print format for a size 3 vector is
      // [3] (1, 2, 3)
      //
      // friend ostream &operator<<(ostream &os, LinearAlgebra::Vector &vector);

    protected:
      LinearAlgebra();
      ~LinearAlgebra();

    private:
  };

  // these must be declared outside of the class (at the end since they must be
  // declared after the typedefs)
  QDebug operator<<(QDebug dbg, const LinearAlgebra::Vector &vector);
  QDebug operator<<(QDebug dbg, const LinearAlgebra::Matrix &matrix);
  QString toString(const LinearAlgebra::Vector &vector, int precision=15);
};

#endif
