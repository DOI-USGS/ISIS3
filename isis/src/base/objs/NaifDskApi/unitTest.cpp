/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QDebug>

#include "IException.h"
#include "NaifDskApi.h"
#include "Preference.h"

using namespace Isis;
/**
 *  
 * @internal 
 *   @history 2015-02-25 Jeannie Backer - Original version.
 *                           Test coverage : 100% scope, line function.
 *
 */
int main(int argc, char *argv[]) {
  try {
    qDebug() << "Unit test for NaifDskApi utility methods.";
    Preference::Preferences(true);

    NaifVertex vertexA(3);
    vertexA[0] = 1; vertexA[1] = 2; vertexA[2] = 3;
    qDebug() << "vertex A = " << vertexA;
    qDebug() << "vertex A is valid?" << validate(vertexA);
    NaifVertex vertexB(2);
    vertexB[0] = 4; vertexB[1] = 5;
    qDebug() << "vertex B = " << vertexB;
    qDebug() << "vertex B is valid?" << validate(vertexB);
    NaifVector vectorA(3);
    vectorA[0] = 6; vectorA[1] = 7; vectorA[2] = 8;
    qDebug() << "vector A = " << vectorA;
    qDebug() << "vector A is valid?" << validate(vectorA);
    NaifVector vectorB;
    qDebug() << "vector B = " << vectorB;
    qDebug() << "vector B is valid?" << validate(vectorB);
    NaifTriangle triangleA(3,3);
    triangleA[0][0] = 9; triangleA[0][1] = 8; triangleA[0][2] = 7;
    triangleA[1][0] = 6; triangleA[1][1] = 5; triangleA[1][2] = 4;
    triangleA[2][0] = 3; triangleA[2][1] = 2; triangleA[2][2] = 1;
    qDebug() << "triangle A = " << triangleA;
    qDebug() << "triangle A is valid?" << validate(triangleA);
    NaifTriangle triangleB(2,3);
    triangleB[0][0] =  0; triangleB[0][1] = -1; triangleB[0][2] = -2;
    triangleB[1][0] = -3; triangleB[1][1] = -4; triangleB[1][2] = -5;
    qDebug() << "triangle B = " << triangleB;
    qDebug() << "triangle B is valid?" << validate(triangleB);
    NaifTriangle triangleC(3,2);
    triangleC[0][0] = -6;  triangleC[0][1] = -7;
    triangleC[1][0] = -8;  triangleC[1][1] = -9;
    triangleC[2][0] = -10; triangleC[2][1] = -11;
    qDebug() << "triangle C = " << triangleC;
    qDebug() << "triangle C is valid?" << validate(triangleC);
  
  }
  catch (IException &e) {
    qDebug();
    qDebug();
    IException(e, IException::Programmer,
              "\n------------Unit Test Failed.------------",
              _FILEINFO_).print();
  }
}
