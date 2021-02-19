/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QDebug>
#include <QScopedPointer>

#include "AbstractPlate.h"
#include "Angle.h"
#include "Displacement.h"
#include "IException.h"
#include "Intercept.h"
#include "NaifDskApi.h"
#include "Preference.h"
#include "SurfacePoint.h"
#include "TriangularPlate.h"

using namespace Isis;

/**
 *  
 * @internal 
 *   @history 2015-02-25 Jeannie Backer - Original version.
 *                           Code coverage: 96% scope, 100% line and function.
 *                           Coverage is complete - unable to test action=NoThrow
 *
 */
int main(int argc, char *argv[]) {
  try {
    Preference::Preferences(true);
    qDebug() << "Unit test for Intercept.";
    qDebug() << "";

    qDebug() << "Default constructor.";
    Intercept i;
    qDebug() << "default object is valid?" << i.isValid();
    qDebug() << "";


    qDebug() << "Construct Intercept object from ";
    qDebug() << "     observer position (0,0,0),";
    qDebug() << "     look dir (1,1,1),";
    qDebug() << "     surface point (2,2,2),";
    qDebug() << "     triangle [ (1,0,0), (0,1,0), (0,0,1) ].";
    NaifVertex observer(3);
    observer[0] = 0.0; observer[1] = 0.0; observer[2] = 0.0;
    NaifVector raydir(3);
    raydir[0] = 1.0; raydir[1] = 1.0; raydir[2] = 1.0;
    QScopedPointer<SurfacePoint> ipoint(new SurfacePoint(Displacement(2.0, Displacement::Meters),
                                                         Displacement(2.0, Displacement::Meters),
                                                         Displacement(2.0, Displacement::Meters)));
    NaifTriangle triangle(3,3);
    triangle[0][0] = 1.0; triangle[0][1] = 0.0; triangle[0][2] = 0.0;
    triangle[1][0] = 0.0; triangle[1][1] = 1.0; triangle[1][2] = 0.0;
    triangle[2][0] = 0.0; triangle[2][1] = 0.0; triangle[2][2] = 1.0;
    QScopedPointer<TriangularPlate> shape(new TriangularPlate(triangle));
    Intercept i2(observer, raydir, ipoint.take(), shape.take());
    qDebug() << "constructed object is valid?" << i2.isValid();
    
    qDebug() << "shape    = " << i2.shape()->name();
    qDebug() << "observer = " << i2.observer();
    qDebug() << "look dir = " << i2.lookDirectionRay();
    qDebug() << "location = " << i2.location().GetX().meters()
                              << i2.location().GetY().meters()
                              << i2.location().GetZ().meters()
                              << " meters";
    qDebug() << "normal   = " << i2.normal();
    qDebug() << "emission = " << i2.emission();
    qDebug() << "sepAngle = " << i2.separationAngle(raydir);
    qDebug() << "";


    qDebug() << "================================= Error Throws ==================================";
    qDebug() << "Invalid because observer(empty) is not valid NaifVertex:";
    try {
      i.location();  
    } 
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";
    qDebug() << "Invalid because look direction(empty) is not valid NaifVector:";
    try {
      NaifVector invalidLookDir;
      Intercept invalidIntercept(observer, invalidLookDir, ipoint.take(), shape.take());
      invalidIntercept.normal();  
    } 
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";
    qDebug() << "Invalid because surface point is null:";
    try {
      SurfacePoint *invalidSP = NULL;
      Intercept invalidIntercept(observer, raydir, invalidSP, shape.take());
      invalidIntercept.emission();  
    } 
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";
    qDebug() << "Invalid because shape is null:";
    try {
      TriangularPlate *invalidShape = NULL;
      Intercept invalidIntercept(observer, raydir, ipoint.take(), invalidShape);
      invalidIntercept.emission();  
    } 
    catch (IException &e) {
      e.print();
    }

  }
  catch (IException &e) {
    qDebug() << "";
    qDebug() << "";
    IException(e, IException::Programmer,
              "\n------------Unit Test Failed.------------",
              _FILEINFO_).print();
  }
}
