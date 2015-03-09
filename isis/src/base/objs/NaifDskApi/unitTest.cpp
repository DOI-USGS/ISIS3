/**
 * @file
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
