/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// std library
#include <cstdlib>

// boost library
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

// Qt Library
#include <QDebug>

// Isis library
#include "Angle.h"
#include "Constants.h"
#include "IException.h"
#include "LinearAlgebra.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

namespace Isis {
  /**
   * Test child class created to test constructor/destructor.
   */
  class TestLinearAlgebra : public LinearAlgebra {
    public:
      TestLinearAlgebra() : LinearAlgebra() {
        qDebug() << "***************************************************";
        qDebug() << "Test Protected Constructor...";
        qDebug() << "***************************************************";
      }
      ~TestLinearAlgebra() {
      }
  };
};


/**
 * Unit Test for LinearAlgebra class. 
 *  
 * @author 2013-06-01 Jeannie Backer
 *
 * @internal
 *   @history 2013-06-01 Jeannie Backer - Original version.
 */
int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  try {
    LinearAlgebra::Matrix emptyMatrix(0, 0);
    qDebug() << "UnitTest for LinearAlgebra" << endl;

    qDebug() << "";
    TestLinearAlgebra constructorTest;
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test identity() method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Matrix id3x3 = LinearAlgebra::identity(3);
    qDebug() << id3x3;
    qDebug() << "";
    try { // error message
      LinearAlgebra::identity(-1);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test zeroMatrix() method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Matrix z3x3 = LinearAlgebra::zeroMatrix(3, 3);
    qDebug() << z3x3;
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test 3D vector setters...";
    qDebug() << "***************************************************";
    LinearAlgebra::Vector i = LinearAlgebra::vector(1.0, 0.0, 0.0);
    qDebug() << "i = vector(1,0,0) = " << i;
    LinearAlgebra::Vector j = LinearAlgebra::vector(0.0, 1.0, 0.0);
    qDebug() << "j = vector(0,1,0) = " << j;
    LinearAlgebra::Vector v456(3);
    LinearAlgebra::setVec3(&v456, 4.0, 5.0, 6.0);
    qDebug() << "setVec3(v, 4, 5, 6) = " << v456;
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test 4D vector setters...";
    qDebug() << "***************************************************";
    LinearAlgebra::Vector v1234 = LinearAlgebra::vector(1.0, 2.0, 3.0, 4.0);
    qDebug() << "vector(1, 2, 3, 4) = " << v1234;
    LinearAlgebra::setVec4(&v1234, 5.0, 6.0, 7.0, 8.0);
    qDebug() << "setVec4(v, 5, 6, 7, 8) = " << v1234;
    qDebug() << "";

    qDebug() << "";
    qDebug() << "**************************************************************************";
    qDebug() << "Test subVector(vector, startIndex, size) method...";
    qDebug() << "**************************************************************************";
    LinearAlgebra::Vector subVector = LinearAlgebra::subVector(v1234, 1, 2);
    qDebug() << "subVector(v5678, 1, 2) = " << subVector;
    qDebug() << "";

    // create more matrices to be used later...
    qDebug() << "";
    qDebug() << "*****************************************************************************";
    qDebug() << "Test setRow(), row(), setColumn() and column() methods...";
    qDebug() << "*****************************************************************************";
    // non-sqare empty matrices
    LinearAlgebra::Matrix m2x3(2, 3);
    LinearAlgebra::Matrix m3x2(3, 2);
    LinearAlgebra::setRow(m2x3, i, 0);
    LinearAlgebra::setRow(m2x3, j, 1);
    qDebug() << m2x3;
    qDebug() << "column 2 (index 1) = " << LinearAlgebra::column(m2x3, 1);
    qDebug() << "";

    LinearAlgebra::setColumn(m3x2, -i, 0);
    LinearAlgebra::setColumn(m3x2, -j, 1);
    qDebug() << m3x2;
    qDebug() << "row 2 (index 1) = " << LinearAlgebra::row(m3x2, 1);
    qDebug() << "";

    //2x2 rotation
    LinearAlgebra::Matrix rot2x2 = LinearAlgebra::identity(2);
    LinearAlgebra::Vector col1 = LinearAlgebra::column(rot2x2, 0);
    LinearAlgebra::Vector col2 = LinearAlgebra::column(rot2x2, 1);
    LinearAlgebra::setColumn(rot2x2, -col2, 0);
    LinearAlgebra::setColumn(rot2x2,  col1, 1);
    //3x3 rotation kij
    LinearAlgebra::Matrix rot3x3 = LinearAlgebra::identity(3);
    LinearAlgebra::setColumn(rot3x3, LinearAlgebra::column(id3x3, 2), 0);
    LinearAlgebra::setColumn(rot3x3, i, 1);
    LinearAlgebra::setColumn(rot3x3, j, 2);
    // a 2x2 matrix of ones
    LinearAlgebra::Matrix ones = LinearAlgebra::identity(2);
    ones(0, 1) = 1.0; ones(1, 0) = 1.0;
    // create a matrix that has unitized column vecotors but determinant less than 1
    LinearAlgebra::Matrix notRot = ones / sqrt(2);
    // create a matrix that has unitized column vecotors but determinant greater than 1
    // not mathematically possible???

    try { // setRow index out of bounds
      LinearAlgebra::setRow(m2x3, j, 2);
    }
    catch (IException &e) {
      e.print();
    }
    try { // setColumn index out of bounds
      LinearAlgebra::setColumn(m2x3, j, 3);
    }
    catch (IException &e) {
      e.print();
    }
    try { // row index out of bounds
      LinearAlgebra::row(m2x3, 2);
    }
    catch (IException &e) {
      e.print();
    }
    try { // column index out of bounds
      LinearAlgebra::column(m2x3, 3);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test isIdentity() methods...";
    qDebug() << "***************************************************";
    qDebug() << "Is emptyMatrix identity?" << LinearAlgebra::isIdentity(emptyMatrix);
    qDebug() << "Is 3x3 identity matrix identity?" << LinearAlgebra::isIdentity(id3x3);
    qDebug() << "Is 3x3 2*identity matrix identity?" << LinearAlgebra::isIdentity(id3x3 * 2.0);
    qDebug() << "Is 2x2 ones matrix identity?" << LinearAlgebra::isIdentity(ones);
    qDebug() << "Is 2x3 matrix identity?" << LinearAlgebra::isIdentity(m2x3);
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test isOrthogonal() method...";
    qDebug() << "***************************************************";
    qDebug() << "Is emptyMatrix orthogonal?" << LinearAlgebra::isOrthogonal(emptyMatrix);
    qDebug() << "Is 3x3 identity matrix orthogonal?" << LinearAlgebra::isOrthogonal(id3x3);
    qDebug() << "Is [0 -1; 1 0] orthogonal?" << LinearAlgebra::isOrthogonal(rot2x2);
    qDebug() << "Is 1/sqrt2 * [1 1; 1 1] orthogonal?" << LinearAlgebra::isOrthogonal(notRot);
    qDebug() << "Is 3x3 zero matrix orthogonal?" << LinearAlgebra::isOrthogonal(z3x3);
    qDebug() << "Is 2x3 matrix orthogonal?" << LinearAlgebra::isOrthogonal(m2x3);
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test isRotationMatrix() method...";
    qDebug() << "***************************************************";
    qDebug() << "Is 3x3 identity matrix rotation matrix?" << LinearAlgebra::isRotationMatrix(id3x3);
    qDebug() << "Is 2x2 identity matrix rotation matrix?" << LinearAlgebra::isRotationMatrix(LinearAlgebra::identity(2));
    qDebug() << "Is [0 -1; 1 0] rotation?" << LinearAlgebra::isRotationMatrix(rot2x2);
    qDebug() << "Is 1/sqrt2 * [1 1; 1 1] rotation?" << LinearAlgebra::isRotationMatrix(notRot);
    qDebug() << "Is 3x3 2*identity matrix rotation matrix?" << LinearAlgebra::isRotationMatrix(id3x3 * 2.0);
    qDebug() << "Is 3x3 zero matrix rotation matrix?" << LinearAlgebra::isRotationMatrix(z3x3);
    qDebug() << "Is 2x3 matrix rotation matrix?" << LinearAlgebra::isRotationMatrix(m2x3);
    try { // method works for 2x2 or 3x3 only
      LinearAlgebra::isRotationMatrix(LinearAlgebra::identity(4));
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test isZero(matrix) method...";
    qDebug() << "***************************************************";
    qDebug() << "Is emptyMatrix zero?" << LinearAlgebra::isZero(emptyMatrix);
    qDebug() << "Is 3x3 zero matrix zero?" << LinearAlgebra::isZero(z3x3);
    qDebug() << "Is 3x3 identity matrix zero?" << LinearAlgebra::isZero(id3x3);
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test determinant() method...";
    qDebug() << "***************************************************";
    qDebug() << "Determinant of 3x3 2*identity = " << LinearAlgebra::determinant(id3x3 * 2.0);
    qDebug() << "Determinant of 2x2 2*identity = " << LinearAlgebra::determinant(LinearAlgebra::identity(2) * 2.0);
    try { // method works for 2x2 or 3x3 only
      LinearAlgebra::determinant(LinearAlgebra::identity(4) * 2.0);
    }
    catch (IException &e) {
      e.print();
    }
    try {
      LinearAlgebra::determinant(m2x3);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test transpose() method...";
    qDebug() << "***************************************************";
    qDebug() << "Input matrix";
    id3x3(0, 1) = 2;
    qDebug() << id3x3;
    qDebug() << "";
    LinearAlgebra::Matrix mT = LinearAlgebra::transpose(id3x3);
    qDebug() << "transpose matrix";
    qDebug() << mT;
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test inverse() method...";
    qDebug() << "***************************************************";
    qDebug() << "The inverse of the rotation matrix is the transpose. ";
    qDebug() << "rotation matrix = ";
    qDebug() << rot2x2;
    qDebug() << "inverse = ";
    LinearAlgebra::Matrix invRot2x2 = LinearAlgebra::inverse(rot2x2);
    qDebug() << invRot2x2;
    qDebug() << "other matrix = ";
    qDebug() << id3x3;
    qDebug() << "inverse = ";
    LinearAlgebra::Matrix inv = LinearAlgebra::inverse(id3x3);
    qDebug() << inv;
    try {
      // determinant zero
      LinearAlgebra::inverse(notRot);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";
     
    // vector with no elements
    LinearAlgebra::Vector emptyVector;
    // zero vector with 3 elements
    LinearAlgebra::Vector z3 = LinearAlgebra::zeroVector(3);
    // vector [0 1 2]
    LinearAlgebra::Vector v012(3);
    for (unsigned int i = 0; i < 3; i++) {
      v012(i) = i;
    }

    LinearAlgebra::Vector w345(3);
    for (unsigned int i = 0; i < 3; i++) {
      w345(i) = i + 3;
    }

    qDebug() << "";
    qDebug() << "v = " << v012;
    qDebug() << "w = " << w345;
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test isZero() method...";
    qDebug() << "***************************************************";
    qDebug() << "Is emptyVector a zero vector? " << LinearAlgebra::isZero(emptyVector);
    qDebug() << "Is zero a zero vector? " << LinearAlgebra::isZero(z3);
    qDebug() << "Is v a zero vector? " << LinearAlgebra::isZero(v012);
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test isEmpty() method...";
    qDebug() << "***************************************************";
    qDebug() << "Is emptyVector an empty vector? " << LinearAlgebra::isEmpty(emptyVector);
    qDebug() << "Is zero an empty vector? " << LinearAlgebra::isEmpty(z3);
    qDebug() << "Is v an empty vector? " << LinearAlgebra::isEmpty(v012);
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test isUnit() method...";
    qDebug() << "***************************************************";
    qDebug() << "Is emptyVector a unit vector? " << LinearAlgebra::isUnit(emptyVector);
    qDebug() << "Is zero a unit vector? " << LinearAlgebra::isUnit(z3);
    qDebug() << "Is v a unit vector? " << LinearAlgebra::isUnit(v012);
    qDebug() << "Is i a unit vector? " << LinearAlgebra::isUnit(i);
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test magnitude() method...";
    qDebug() << "***************************************************";
    qDebug() << "Magnitude of zero = " << LinearAlgebra::magnitude(z3);
    qDebug() << "Magnitude of v = " << LinearAlgebra::magnitude(v012);
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test absoluteMaximum() method...";
    qDebug() << "***************************************************";
    qDebug() << "Max Norm of v = " << LinearAlgebra::absoluteMaximum(v012);
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test normalize() method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Vector u012 = LinearAlgebra::normalize(v012);
    qDebug() << "u = " << u012;
    try {// cannot normalize zero vector
      LinearAlgebra::normalize(z3);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test multiply(matrix1, matrix2) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Matrix matrixMult = LinearAlgebra::multiply(mT, id3x3);
    qDebug() << "MT * M = ";
    qDebug() << matrixMult;
    qDebug() << "";
    try {// left columns must match right rows
      LinearAlgebra::multiply(id3x3, m2x3);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test multiply(matrix, vector) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Vector matrixVectorMult = LinearAlgebra::multiply(id3x3, v012);
    cout << "M * v = " << matrixVectorMult << endl; // test cout with boost vector
    try { // matrix columns must equal vector size
      LinearAlgebra::multiply(LinearAlgebra::transpose(m2x3), v012);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test multiply(scalar, vector) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Vector scalarVectorMult = LinearAlgebra::multiply(2.0, v012);
    qDebug() << "2.0 * v = " << scalarVectorMult;
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test multiply(scalar, matrix) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Matrix scalarMatrixMult = LinearAlgebra::multiply(2.0, id3x3);
    qDebug() << "2.0 * M = ";
    qDebug() << scalarMatrixMult;
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test add(vector1, vector2) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Vector vectorAdd = LinearAlgebra::add(u012, v012);
    qDebug() << "u + v = " << vectorAdd;
    try { // sizes must match
      LinearAlgebra::add(emptyVector, v012);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test subtract(vector1, vector2) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Vector vectorSubtract = LinearAlgebra::subtract(u012, v012);
    qDebug() << "u - v = " << vectorSubtract;
    try { // sizes must match
      LinearAlgebra::subtract(emptyVector, v012);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test crossProduct(vector1, vector2) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Vector vectorCross = LinearAlgebra::crossProduct(u012, v012);
    qDebug() << "(parallel vectors) u x v = " << vectorCross;
    vectorCross = LinearAlgebra::crossProduct(i, j);
    qDebug() << "(basis vectors) i x j = k = " << vectorCross;
    vectorCross = LinearAlgebra::crossProduct(i, v012);
    qDebug() << "i x v = " << vectorCross;
    vectorCross = LinearAlgebra::crossProduct(v012, i);
    qDebug() << "v x i = " << vectorCross;
    try { // vector1 is not size 3
      LinearAlgebra::crossProduct(emptyVector, v012);
    }
    catch (IException &e) {
      e.print();
    }
    try { // vector2 is not size 3
      LinearAlgebra::crossProduct(v012, emptyVector);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test normalizedCrossProduct(vector1, vector2) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Vector normVectorCross = LinearAlgebra::normalizedCrossProduct(i, v012);
    qDebug() << "i x v = " << normVectorCross;
    normVectorCross = LinearAlgebra::normalizedCrossProduct(i, u012);
    qDebug() << "i x u = " << normVectorCross;
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test outerProduct(vector1, vector2) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Matrix vectorOuter = LinearAlgebra::outerProduct(u012, v012);
    qDebug() << vectorOuter;
    try { // vectors must be same size
      LinearAlgebra::outerProduct(v012, emptyVector);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test dotProduct(vector1, vector2) method...";
    qDebug() << "***************************************************";
    qDebug() << "u . v = " << LinearAlgebra::dotProduct(u012, v012);
    try { // vectors must be same size
      LinearAlgebra::dotProduct(v012, emptyVector);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test innerProduct(vector1, vector2) method...";
    qDebug() << "***************************************************";
    qDebug() << "<u,v> = " << LinearAlgebra::innerProduct(u012, v012);
    try { // vectors must be same size
      LinearAlgebra::innerProduct(v012, emptyVector);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test project(vector1, vector2) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Vector vectorProj = LinearAlgebra::project(u012, v012);
    qDebug() << "Proj(u, v) = " << vectorProj;
    LinearAlgebra::Vector vectorProjZ = LinearAlgebra::project(u012, z3);
    qDebug() << "Proj(u, zero) is zero? " << LinearAlgebra::isZero(vectorProjZ);
    try { // vectors must be same size
      LinearAlgebra::project(emptyVector, v012);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test rotate(vector1, vector2, theta) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Vector vectorRotate = LinearAlgebra::rotate(u012, v012, Angle(HALFPI, Angle::Radians));
    qDebug() << "rotate(u,v,PI/2) = " << vectorRotate;
    LinearAlgebra::Vector vectorRotateZ = LinearAlgebra::rotate(u012, z3, Angle(HALFPI, Angle::Radians));
    qDebug() << "rotate(u,zero,PI/2) = " << vectorRotateZ;
    try { // vector size must be 3
      LinearAlgebra::rotate(emptyVector, v012, Angle(HALFPI, Angle::Radians));
    }
    catch (IException &e) {
      e.print();
    }
    try { // axis size must be 3
      LinearAlgebra::rotate(v012, emptyVector, Angle(HALFPI, Angle::Radians));
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test perpendicular(vector1, vector2) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Vector perpendicularVector = LinearAlgebra::perpendicular(z3, v012);
    qDebug() << "perpendicular(zero, [0 1 2]) = " << perpendicularVector;
    perpendicularVector = LinearAlgebra::perpendicular(v012, z3);
    qDebug() << "perpendicular([0 1 2], zero) = " << perpendicularVector;
    perpendicularVector = LinearAlgebra::perpendicular(v012, u012);
    qDebug() << "perpendicular([0 1 2], c*[0 1 2]) = " << perpendicularVector;
    perpendicularVector = LinearAlgebra::perpendicular(v012, w345);
    qDebug() << "perpendicular([0 1 2], [3 4 5]) = " << perpendicularVector;
    qDebug() << "verify: [0 1 2] - perp = " << v012 - perpendicularVector;
    qDebug() << "        is parallel to [3 4 5] since 0.28*[3 4 5 ] = " << (0.28 * w345);
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test toMatrix(quaternion) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Vector idQ(4);
    idQ(0) = 1;    idQ(1) = 0;    idQ(2) = 0;    idQ(3) = 0;
    LinearAlgebra::Vector q1(4);
    q1(0) = 0;    q1(1) = 1;    q1(2) = 0;    q1(3) = 0;
    LinearAlgebra::Vector q2(4);
    q2(0) = 0;    q2(1) = 0;    q2(2) = 1;    q2(3) = 0;
    LinearAlgebra::Vector q3(4);
    q3(0) = 0;    q3(1) = 0;    q3(2) = 0;    q3(3) = 1;
    LinearAlgebra::Vector q4(4);
    q4(0) = 0.91219099598223; q4(1) = -0.22019944193656; 
    q4(2) = -0.2554485981203; q4(3) = -0.23273548577332;
    LinearAlgebra::Vector q5(4);
    q5(0) = 1.0; q5(1) = 1.0; q5(2) = 1.0; q5(3) = 1.0;
    LinearAlgebra::Matrix z2m = LinearAlgebra::toMatrix(LinearAlgebra::zeroVector(4));
    qDebug() << "From the zero quaternion we get identity...";
    // because rotation about origin is just the origin???
    qDebug() << "matrix = ";
    qDebug() << z2m;
    qDebug() << "--------------------------";
    LinearAlgebra::Matrix id2m = LinearAlgebra::toMatrix(idQ);
    qDebug() << "From the identity quaternion we get identity...";
    qDebug() << "matrix = ";
    qDebug() << id2m;
    qDebug() << "--------------------------";
    LinearAlgebra::Matrix q12m = LinearAlgebra::toMatrix(q1);
    qDebug() << "From quaternion 1 = " << q1;
    qDebug() << "matrix 1 = ";
    qDebug() << q12m;
    qDebug() << "--------------------------";
    LinearAlgebra::Matrix q22m = LinearAlgebra::toMatrix(q2);
    qDebug() << "From quaternion 2 = " << q2;
    qDebug() << "matrix 2 = ";
    qDebug() << q22m;
    qDebug() << "--------------------------";
    LinearAlgebra::Matrix q32m = LinearAlgebra::toMatrix(q3);
    qDebug() << "From quaternion 3 = " << q3;
    qDebug() << "matrix 3 = ";
    qDebug() << q32m;
    qDebug() << "--------------------------";
    LinearAlgebra::Matrix q42m = LinearAlgebra::toMatrix(q4);
    qDebug() << "From quaternion 4 = " << q4;
    qDebug() << "matrix 4 = ";
    qDebug() << q42m;
    qDebug() << "--------------------------";
    LinearAlgebra::Matrix q52m = LinearAlgebra::toMatrix(q5);
    qDebug() << "From quaternion 5 (not unit quaternion) = " << q5;
    qDebug() << "matrix 5 = ";
    qDebug() << q52m;
    qDebug() << "--------------------------";
    try { // quaternion must be size 4
      LinearAlgebra::toMatrix(z3);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test toQuaternion(matrix) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::Vector id2q = LinearAlgebra::toQuaternion(LinearAlgebra::identity(3));
    qDebug() << "From the identity matrix we get... ";
    qDebug() << "quaternion = " << id2q;
    qDebug() << "";
    qDebug() << "--------------------------";
    LinearAlgebra::Vector q12m2q = LinearAlgebra::toQuaternion(q12m);
    qDebug() << "From matrix 1 = ";
    qDebug() << q12m;
    qDebug() << "quaternion 1 = " << q12m2q;
    qDebug() << "";
    qDebug() << "--------------------------";
    LinearAlgebra::Vector q22m2q = LinearAlgebra::toQuaternion(q22m);
    qDebug() << "From matrix 2 = ";
    qDebug() << q22m;
    qDebug() << "quaternion 2 = " << q22m2q;
    qDebug() << "";
    qDebug() << "--------------------------";
    LinearAlgebra::Vector q32m2q = LinearAlgebra::toQuaternion(q32m);
    qDebug() << "From matrix 3 = ";
    qDebug() << q32m;
    qDebug() << "quaternion 3 = " << q32m2q;
    qDebug() << "";
    qDebug() << "--------------------------";
    LinearAlgebra::Vector q42m2q = LinearAlgebra::toQuaternion(q42m);
    qDebug() << "From matrix 4 = ";
    qDebug() << q42m;
    qDebug() << "quaternion 4 = " << q42m2q;
    qDebug() << "";
    qDebug() << "--------------------------";
    LinearAlgebra::Vector q52m2q = LinearAlgebra::toQuaternion(q52m);
    qDebug() << "From matrix 5 = ";
    qDebug() << q52m;
    qDebug() << "quaternion 5 = " << q52m2q;
    qDebug() << "";
    qDebug() << "--------------------------";
    try { // matrix must be 3x3
      LinearAlgebra::toQuaternion(m2x3);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try { // matrix must be 3x3
      LinearAlgebra::toQuaternion(m3x2);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try { // matrix must be rotation
      LinearAlgebra::toQuaternion(id3x3 * 2.0);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test toAxisAngle(matrix) method...";
    qDebug() << "***************************************************";
    LinearAlgebra::AxisAngle id2aa = LinearAlgebra::toAxisAngle(LinearAlgebra::identity(3));
    qDebug() << "From the identity matrix we get...";
    qDebug() << "axis = " << id2aa.first;
    qDebug() << "angle = " << id2aa.second.radians();
    qDebug() << "";
    qDebug() << "--------------------------";
    LinearAlgebra::AxisAngle q12m2aa = LinearAlgebra::toAxisAngle(q12m);
    qDebug() << "From matrix 1 = ";
    qDebug() << q12m;
    qDebug() << "axis 1 = " << q12m2aa.first;
    qDebug() << "angle 1 = " << q12m2aa.second.radians();
    qDebug() << "";
    qDebug() << "--------------------------";
    LinearAlgebra::AxisAngle q22m2aa = LinearAlgebra::toAxisAngle(q22m);
    qDebug() << "From matrix 2 = ";
    qDebug() << q22m;
    qDebug() << "axis 2 = " << q22m2aa.first;
    qDebug() << "angle 2 = " << q22m2aa.second.radians();
    qDebug() << "";
    qDebug() << "--------------------------";
    LinearAlgebra::AxisAngle q32m2aa = LinearAlgebra::toAxisAngle(q32m);
    qDebug() << "From matrix 3 = ";
    qDebug() << q32m;
    qDebug() << "axis 3 = " << q32m2aa.first;
    qDebug() << "angle 3 = " << q32m2aa.second.radians();
    qDebug() << "";
    qDebug() << "--------------------------";
    LinearAlgebra::AxisAngle q42m2aa = LinearAlgebra::toAxisAngle(q42m);
    qDebug() << "From matrix 4 = ";
    qDebug() << q42m;
    qDebug() << "axis 4 = " << q42m2aa.first;
    qDebug() << "angle 4 = " << q42m2aa.second.radians();
    qDebug() << "";
    qDebug() << "--------------------------";
    LinearAlgebra::AxisAngle q52m2aa = LinearAlgebra::toAxisAngle(q52m);
    qDebug() << "From matrix 5 = ";
    qDebug() << q52m;
    qDebug() << "axis 5 = " << q52m2aa.first;
    qDebug() << "angle 5 = " << q52m2aa.second.radians();
    qDebug() << "";
    qDebug() << "--------------------------";
    try { // matrix must be 3x3
      LinearAlgebra::toAxisAngle(m2x3);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try {  // matrix must be 3x3
      LinearAlgebra::toAxisAngle(m3x2);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try { // must be rotation matrix
      LinearAlgebra::toAxisAngle(id3x3 * 2.0);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test toMatrix(AxisAngle) and toMatrix(axis, angle) methods...";
    qDebug() << "***************************************************";
    LinearAlgebra::Matrix id2aa2m = LinearAlgebra::toMatrix(id2aa);
    qDebug() << "From the identity axis/angle we get...";
    qDebug() << "axis = " << id2aa.first;
    qDebug() << "angle = " << id2aa.second.radians();
    qDebug() << "matrix = ";
    qDebug() << id2aa2m;
    qDebug() << "--------------------------";
    LinearAlgebra::Matrix q12m2aa2m = LinearAlgebra::toMatrix(q12m2aa);
    qDebug() << "From axis = " << q12m2aa.first;
    qDebug() << "     angle = " << q12m2aa.second.radians();
    qDebug() << "matrix = ";
    qDebug() << q12m2aa2m;
    qDebug() << "--------------------------";
    LinearAlgebra::Matrix q22m2aa2m = LinearAlgebra::toMatrix(q22m2aa);
    qDebug() << "From axis = " << q22m2aa.first;
    qDebug() << "     angle = " << q22m2aa.second.radians();
    qDebug() << "matrix = ";
    qDebug() << q22m2aa2m;
    qDebug() << "--------------------------";
    LinearAlgebra::Matrix q32m2aa2m = LinearAlgebra::toMatrix(q32m2aa);
    qDebug() << "From axis = " << q32m2aa.first;
    qDebug() << "     angle = " << q32m2aa.second.radians();
    qDebug() << "matrix = ";
    qDebug() << q32m2aa2m;
    qDebug() << "--------------------------";
    LinearAlgebra::Matrix q42m2aa2m = LinearAlgebra::toMatrix(q42m2aa);
    qDebug() << "From axis = " << q42m2aa.first;
    qDebug() << "     angle = " << q42m2aa.second.radians();
    qDebug() << "matrix = ";
    qDebug() << q42m2aa2m;
    qDebug() << "--------------------------";
    LinearAlgebra::Matrix q52m2aa2m = LinearAlgebra::toMatrix(q52m2aa);
    qDebug() << "From axis = " << q52m2aa.first;
    qDebug() << "     angle = " << q52m2aa.second.radians();
    qDebug() << "matrix = ";
    qDebug() << q52m2aa2m;
    qDebug() << "--------------------------";
    try { // vector must be size 3
      LinearAlgebra::toMatrix(emptyVector, Angle(0.0, Angle::Radians));
    }
    catch(Isis::IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test toEulerAngles(matrix, axes) method...";
    qDebug() << "***************************************************";
    QList<int> axes323;
    axes323 << 3 << 2 << 3;
    QList<LinearAlgebra::EulerAngle> id2ea = 
                                     LinearAlgebra::toEulerAngles(LinearAlgebra::identity(3), axes323);
    qDebug() << "From axes 323 and the identity matrix we get...";
    qDebug() << "Euler Angle 1 (angle/axis number) = " << id2ea[0].first.radians() << id2ea[0].second;
    qDebug() << "Euler Angle 2 (angle/axis number) = " << id2ea[1].first.radians() << id2ea[1].second;
    qDebug() << "Euler Angle 3 (angle/axis number) = " << id2ea[2].first.radians() << id2ea[2].second;
    qDebug() << "";
    qDebug() << "--------------------------";
    QList<int> axes313;
    axes313 << 3 << 1 << 3;
    QList<LinearAlgebra::EulerAngle> q12m2ea = LinearAlgebra::toEulerAngles(q12m, axes313);
    qDebug() << "From Axes: " << axes313;
    qDebug() << "     matrix 1 = ";
    qDebug() << q12m;
    qDebug() << "Euler Angle 1 (angle/axis number) = " << q12m2ea[0].first.radians() << q12m2ea[0].second;
    qDebug() << "Euler Angle 2 (angle/axis number) = " << q12m2ea[1].first.radians() << q12m2ea[1].second;
    qDebug() << "Euler Angle 3 (angle/axis number) = " << q12m2ea[2].first.radians() << q12m2ea[2].second;
    qDebug() << "";
    qDebug() << "--------------------------";
    QList<int> axes213;
    axes213 << 2 << 1 << 3;
    QList<LinearAlgebra::EulerAngle> q22m2ea = LinearAlgebra::toEulerAngles(q22m, axes213);
    qDebug() << "From Axes: " << axes213;
    qDebug() << "     matrix 2 = ";
    qDebug() << q22m;
    qDebug() << "Euler Angle 1 (angle/axis number) = " << q22m2ea[0].first.radians() << q22m2ea[0].second;
    qDebug() << "Euler Angle 2 (angle/axis number) = " << q22m2ea[1].first.radians() << q22m2ea[1].second;
    qDebug() << "Euler Angle 3 (angle/axis number) = " << q22m2ea[2].first.radians() << q22m2ea[2].second;
    qDebug() << "";
    qDebug() << "--------------------------";
    QList<int> axes123;
    axes123 << 1 << 2 << 3;
    QList<LinearAlgebra::EulerAngle> q32m2ea = LinearAlgebra::toEulerAngles(q32m, axes123);
    qDebug() << "From Axes: " << axes123;
    qDebug() << "     matrix 3 = ";
    qDebug() << q32m;
    qDebug() << "Euler Angle 1 (angle/axis number) = " << q32m2ea[0].first.radians() << q32m2ea[0].second;
    qDebug() << "Euler Angle 2 (angle/axis number) = " << q32m2ea[1].first.radians() << q32m2ea[1].second;
    qDebug() << "Euler Angle 3 (angle/axis number) = " << q32m2ea[2].first.radians() << q32m2ea[2].second;
    qDebug() << "";
    qDebug() << "--------------------------";
    QList<LinearAlgebra::EulerAngle> rot123 = LinearAlgebra::toEulerAngles(rot3x3, axes123);
    qDebug() << "From Axes: " << axes123;
    qDebug() << "     matrix = ";
    qDebug() << rot3x3;
    qDebug() << "Euler Angle 1 (angle/axis number) = " << rot123[0].first.radians() << rot123[0].second;
    qDebug() << "Euler Angle 2 (angle/axis number) = " << rot123[1].first.radians() << rot123[1].second;
    qDebug() << "Euler Angle 3 (angle/axis number) = " << rot123[2].first.radians() << rot123[2].second;
    qDebug() << "";
    qDebug() << "--------------------------";
    QList<LinearAlgebra::EulerAngle> rot313 = LinearAlgebra::toEulerAngles(rot3x3, axes313);
    qDebug() << "From Axes: " << axes313;
    qDebug() << "     matrix = ";
    qDebug() << rot3x3;
    qDebug() << "Euler Angle 1 (angle/axis number) = " << rot313[0].first.radians() << rot313[0].second;
    qDebug() << "Euler Angle 2 (angle/axis number) = " << rot313[1].first.radians() << rot313[1].second;
    qDebug() << "Euler Angle 3 (angle/axis number) = " << rot313[2].first.radians() << rot313[2].second;
    qDebug() << "";
    qDebug() << "--------------------------";
    try { // matrix must be 3x3
      LinearAlgebra::toEulerAngles(m2x3, axes313);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try { // matrix must be 3x3
      LinearAlgebra::toEulerAngles(m3x2, axes313);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try {
      // 3x3 but not rotation
      LinearAlgebra::toEulerAngles(z3x3, axes313);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try {
      // axes 3 1 0
      axes313[2] = 0;
      LinearAlgebra::toEulerAngles(q12m, axes313);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try {
      // axes 3 -1 0
      axes313[1] = -1;
      LinearAlgebra::toEulerAngles(q12m, axes313);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try {
      // axes 4 -1 0
      axes313[0] = 4;
      LinearAlgebra::toEulerAngles(q12m, axes313);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try {
      // axis2 = axis1
      axes313[0] = 1;
      axes313[1] = 1;
      axes313[2] = 1;
      LinearAlgebra::toEulerAngles(q12m, axes313);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try {
      // axis2 = axis3
      axes313[0] = 2;
      axes313[1] = 1;
      axes313[2] = 1;
      LinearAlgebra::toEulerAngles(q12m, axes313);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try {
      // axis != 3
      axes313.pop_back();
      LinearAlgebra::toEulerAngles(q12m, axes313);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "";
    qDebug() << "***************************************************";
    qDebug() << "Test toMatrix(EulerAngles) methods...";
    qDebug() << "***************************************************";
    LinearAlgebra::Matrix q12m2ea2m = LinearAlgebra::toMatrix(q12m2ea);
    qDebug() << "From Euler Angle 1 (angle/axis number) = " << q12m2ea[0].first.radians() << q12m2ea[0].second;
    qDebug() << "     Euler Angle 2 (angle/axis number) = " << q12m2ea[1].first.radians() << q12m2ea[1].second;
    qDebug() << "     Euler Angle 3 (angle/axis number) = " << q12m2ea[2].first.radians() << q12m2ea[2].second;
    qDebug() << "matrix = ";
    qDebug() << q12m2ea2m;
    qDebug() << "--------------------------";
    LinearAlgebra::Matrix q12m2ea2mB = LinearAlgebra::toMatrix(q12m2ea[0], q12m2ea[1], q12m2ea[2]);
    qDebug() << "From Euler Angle 1 (angle/axis number) = " << q12m2ea[0].first.radians() << q12m2ea[0].second;
    qDebug() << "     Euler Angle 2 (angle/axis number) = " << q12m2ea[1].first.radians() << q12m2ea[1].second;
    qDebug() << "     Euler Angle 3 (angle/axis number) = " << q12m2ea[2].first.radians() << q12m2ea[2].second;
    qDebug() << "matrix = ";
    qDebug() << q12m2ea2mB;
    qDebug() << "--------------------------";
    try { // invalid axis (must be 1, 2, or 3)
      LinearAlgebra::toMatrix(qMakePair(Angle(0.0, Angle::Radians), 4), 
                              qMakePair(Angle(0.0, Angle::Radians), 1), 
                              qMakePair(Angle(0.0, Angle::Radians), 1));
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try { // invalid axis (must be 1, 2, or 3)
      LinearAlgebra::toMatrix(qMakePair(Angle(0.0, Angle::Radians), 1), 
                              qMakePair(Angle(0.0, Angle::Radians), 0), 
                              qMakePair(Angle(0.0, Angle::Radians), 1));
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try { // invalid axis (must be 1, 2, or 3)
      LinearAlgebra::toMatrix(qMakePair(Angle(0.0, Angle::Radians), 2), 
                              qMakePair(Angle(0.0, Angle::Radians), 1), 
                              qMakePair(Angle(0.0, Angle::Radians), -1));
    }
    catch(Isis::IException &e) {
      e.print();
    }
    try { // list must be size 3
      q12m2ea.pop_back();
      LinearAlgebra::toMatrix(q12m2ea);
    }
    catch(Isis::IException &e) {
      e.print();
    }
    qDebug() << "";
 }
  catch(Isis::IException &e) {
    e.print();
  }
}
