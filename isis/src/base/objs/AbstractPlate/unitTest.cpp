/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QDebug>

#include "AbstractPlate.h"
#include "Angle.h"
#include "Displacement.h"
#include "Distance.h"
#include "IException.h"
#include "Intercept.h"
#include "NaifDskApi.h"
#include "Preference.h"
#include "SurfacePoint.h"

using namespace Isis;

/**
 *  
 * @internal 
 *   @history 2015-02-25 Jeannie Backer - Original version.
 *                           Code coverage: 100% scope, line, and function.
 *
 */
class MyPlate : public AbstractPlate {
  public:
    MyPlate() : AbstractPlate() {
    }

    ~MyPlate() {
    }
  
    QString name() const {
      return AbstractPlate::name();
    }

    Distance minRadius() const {
      return Distance(1.0, Distance::Meters);
    }

    Distance maxRadius() const {
      return Distance(2.0, Distance::Meters);
    }
  
    double area() const {
      return 3.0;
    }

    NaifVector normal() const {
      NaifVector v;
      return v;
    }

    Angle separationAngle(const NaifVector &raydir) const {
      Angle theta;
      return theta;
    }
  
    bool hasIntercept(const NaifVertex &vertex, 
                              const NaifVector &raydir) const {
      return true;
    }

    bool hasPoint(const Latitude &lat, 
                          const Longitude &lon) const {
      return false;
    }
  
    Intercept *intercept(const NaifVertex &vertex, 
                                 const NaifVector &raydir) const {
      Intercept *i = NULL;
      return i;
    }

    SurfacePoint *point(const Latitude &lat, 
                                const Longitude &lon) const {
      SurfacePoint *s = NULL;
      return s;
    }
  
    AbstractPlate *clone() const {
      return (new MyPlate());
    }

    Intercept *construct(const NaifVertex &vertex, const NaifVector &raydir,
                         SurfacePoint *ipoint) const {
      return AbstractPlate::construct(vertex, raydir, ipoint);  
    }

};



int main(int argc, char *argv[]) {
  try {
    qDebug() << "Unit test for Abstract Plate.";
    Preference::Preferences(true);

    qDebug() << "Virtual class... first create a child";
    MyPlate mp;
    qDebug() << "plate name = " << mp.name();
    NaifVertex vertex(3);
    vertex[0] = 0.0;    vertex[1] = 0.0;    vertex[2] = 0.0;
    NaifVector raydir(3);
    raydir[0] = 1.0;    raydir[1] = 1.0;    raydir[2] = 1.0;
    SurfacePoint *ipoint = new SurfacePoint(Displacement(2.0, Displacement::Meters),
                                            Displacement(2.0, Displacement::Meters),
                                            Displacement(2.0, Displacement::Meters));
    qDebug() << "Construct intercept from vertex (0,0,0), vector(1,1,1), and surface point(2,2,2).";
    Intercept *intercept = mp.construct(vertex, raydir, ipoint);
    qDebug() << "intercept plate name                 = " << intercept->shape()->name();
    qDebug() << "intercept vertex (observer position) = " << intercept->observer();
    qDebug() << "intercept vector (look direction)    = " << intercept->lookDirectionRay();
    qDebug() << "intercept surface point (location)   = " << intercept->location().GetX().meters()
                                                          << intercept->location().GetY().meters()
                                                          << intercept->location().GetZ().meters()
                                                          << " meters";
    delete ipoint;
    ipoint = NULL;
  }
  catch (IException &e) {
    qDebug() << "";
    qDebug() << "";
    IException(e, IException::Programmer,
              "\n------------Unit Test Failed.------------",
              _FILEINFO_).print();
  }
}
