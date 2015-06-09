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

#include "Distance.h"
#include "IException.h"
#include "Intercept.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifDskApi.h"
#include "Preference.h"
#include "SurfacePoint.h"
#include "TriangularPlate.h"

using namespace Isis;

/**
 *  
 * @internal 
 *   @history 2015-02-25 Jeannie Backer - Original version.
 *                           Code coverage: 100% scope, line and function.
 *
 */
int main(int argc, char *argv[]) {
  try {
    qDebug() << "Unit test for TriangularPlate.";
    qDebug();
    Preference::Preferences(true);

    NaifTriangle plate(3,3);
    plate[0][0] = 1.0;    plate[0][1] = 0.0;    plate[0][2] = 0.0;
    plate[1][0] = 0.0;    plate[1][1] = 1.0;    plate[1][2] = 0.0;
    plate[2][0] = 0.0;    plate[2][1] = 0.0;    plate[2][2] = 1.0;
    TriangularPlate tp(plate, 2);

    qDebug() << "TriangularPlate basic info for plate...";
    qDebug() << plate;
    qDebug() << "Name:       " << tp.name();
    qDebug() << "ID:         " << tp.id();
    qDebug() << "Min Radius: " << tp.minRadius().meters() << " meters";
    qDebug() << "Max Radius: " << tp.maxRadius().meters() << " meters";
    qDebug() << "Area:       " << tp.area();
    qDebug() << "Normal:     " << tp.normal();
    qDebug() << "Center:     " << tp.center();
    qDebug();
    qDebug();


    NaifVector raydir(3);
    NaifVertex vertex(3);
    // No Plate Intercept - Case (en == 0)
    raydir[0] = 1.0;    raydir[1] = 1.0;    raydir[2] = 1.0;
    vertex[0] = 0.0;    vertex[1] = 0.0;    vertex[2] = 1.0;
    qDebug() << "Look for intercept using ray direction and observer";
    qDebug() << "Ray Dir:            " << raydir;
    qDebug() << "Observer:           " << vertex;
    qDebug() << "Has intercept?      " << tp.hasIntercept(vertex, raydir);
    Intercept *i = tp.intercept(vertex, raydir);
    qDebug() << "Intercept is null?     " << toString(i == NULL);
    qDebug();
    // No Plate Intercept - Case (en < 0 && tdot12 > 0)
    raydir[0] = 0.0;    raydir[1] = 0.0;    raydir[2] = 1.0;
    vertex[0] = 0.0;    vertex[1] = 0.0;    vertex[2] =  2.0;
    qDebug() << "Look for intercept using ray direction and observer";
    qDebug() << "Ray Dir:            " << raydir;
    qDebug() << "Observer:           " << vertex;
    qDebug() << "Has intercept?      " << tp.hasIntercept(vertex, raydir);
    i = tp.intercept(vertex, raydir);
    qDebug() << "Intercept is null?     " << toString(i == NULL);
    qDebug();
    // No Plate Intercept - Case (en < 0, tdot12 == 0, && tdot23 > 0)
    raydir[0] = -1.0;    raydir[1] = 1.0;    raydir[2] = 0.0;
    vertex[0] =  0.0;    vertex[1] = 0.0;    vertex[2] = 2.0;
    qDebug() << "Look for intercept using ray direction and observer";
    qDebug() << "Ray Dir:            " << raydir;
    qDebug() << "Observer:           " << vertex;
    qDebug() << "Has intercept?      " << tp.hasIntercept(vertex, raydir);
    i = tp.intercept(vertex, raydir);
    qDebug() << "Intercept is null?     " << toString(i == NULL);
    qDebug();
    // No Plate Intercept - Case (en < 0, tdot12 == 0, tdot23 < 0, && tdot31 > 0)
    raydir[0] = 1.0;    raydir[1] = -1.0;    raydir[2] = 0.0;
    vertex[0] = 0.0;    vertex[1] = 0.0;    vertex[2] =  2.0;
    qDebug() << "Look for intercept using ray direction and observer";
    qDebug() << "Ray Dir:            " << raydir;
    qDebug() << "Observer:           " << vertex;
    qDebug() << "Has intercept?      " << tp.hasIntercept(vertex, raydir);
    i = tp.intercept(vertex, raydir);
    qDebug() << "Intercept is null?     " << toString(i == NULL);
    qDebug();
    // No Plate Intercept - Case (en > 0 && tdot12 < 0)
    raydir[0] = 0.0;    raydir[1] = 0.0;    raydir[2] = -1.0;
    vertex[0] = 0.0;    vertex[1] = 0.0;    vertex[2] =  0.0;
    qDebug() << "Look for intercept using ray direction and observer";
    qDebug() << "Ray Dir:            " << raydir;
    qDebug() << "Observer:           " << vertex;
    qDebug() << "Has intercept?      " << tp.hasIntercept(vertex, raydir);
    i = tp.intercept(vertex, raydir);
    qDebug() << "Intercept is null?     " << toString(i == NULL);
    qDebug();
    // No Plate Intercept - Case (en > 0, tdot12 == 0, && tdot23 < 0)
    raydir[0] = -1.0;    raydir[1] = 0.0;    raydir[2] = 0.0;
    vertex[0] =  0.0;    vertex[1] = 0.0;    vertex[2] = 0.0;
    qDebug() << "Look for intercept using ray direction and observer";
    qDebug() << "Ray Dir:            " << raydir;
    qDebug() << "Observer:           " << vertex;
    qDebug() << "Has intercept?      " << tp.hasIntercept(vertex, raydir);
    i = tp.intercept(vertex, raydir);
    qDebug() << "Intercept is null?     " << toString(i == NULL);
    qDebug();
    // No Plate Intercept - Case (en > 0, tdot12 == tdot23 == 0, && tdot31 < 0)
    raydir[0] = 0.0;    raydir[1] = -1.0;    raydir[2] = 0.0;
    vertex[0] = 0.0;    vertex[1] = 0.0;    vertex[2] =  0.0;
    qDebug() << "Look for intercept using ray direction and observer";
    qDebug() << "Ray Dir:            " << raydir;
    qDebug() << "Observer:           " << vertex;
    qDebug() << "Has intercept?      " << tp.hasIntercept(vertex, raydir);
    i = tp.intercept(vertex, raydir);
    qDebug() << "Intercept is null?     " << toString(i == NULL);
    qDebug();
    // No Plate Intercept - Case (en != 0 && tdot12 == tdot23 == tdot31 == 0)
    raydir[0] = 0.0;    raydir[1] = 0.0;    raydir[2] = 0.0;
    vertex[0] = 0.0;    vertex[1] = 0.0;    vertex[2] =  0.0;
    qDebug() << "Look for intercept using ray direction and observer";
    qDebug() << "Ray Dir:            " << raydir;
    qDebug() << "Observer:           " << vertex;
    qDebug() << "Has intercept?      " << tp.hasIntercept(vertex, raydir);
    i = tp.intercept(vertex, raydir);
    qDebug() << "Intercept is null?     " << toString(i == NULL);
    qDebug();
    // Plate Intercept = true 
    raydir[0] = 1.0;    raydir[1] = 1.0;    raydir[2] = 1.0;
    vertex[0] = 0.0;    vertex[1] = 0.0;    vertex[2] =  0.0;
    qDebug() << "Look for intercept using ray direction and observer";
    qDebug() << "Ray Dir:            " << raydir;
    qDebug() << "Observer:           " << vertex;
    qDebug() << "Has intercept?      " << tp.hasIntercept(vertex, raydir);
    i = tp.intercept(vertex, raydir);
    qDebug() << "Intercept Shape:    " << i->shape()->name();
    qDebug() << "Intercept Observer: " << i->observer();
    qDebug() << "Intercept Look Dir: " << i->lookDirectionRay();
    qDebug() << "Intercept Location: " << i->location().GetX().meters()
                                       << i->location().GetY().meters()
                                       << i->location().GetZ().meters()
                                       << " meters";
    qDebug() << "Sep Angle:          " << tp.separationAngle(raydir);
    qDebug();
    qDebug();


    Latitude pole(-90.0, Angle::Degrees);
    Longitude lon(0.0, Angle::Degrees);
    qDebug() << "Look for surface point using south pole";
    qDebug() << "Latitude:      " << pole;
    qDebug() << "Longitude:     " << lon;
    qDebug() << "Has point?     " << tp.hasPoint(pole, lon);
    SurfacePoint *spPole = tp.point(pole, lon);
    qDebug() << "Surface point at pole is null?     " << toString(spPole == NULL);
    qDebug();
    Latitude equator(0.0, Angle::Degrees);
    qDebug() << "Look for surface point using equator";
    qDebug() << "Latitude:      " << equator;
    qDebug() << "Longitude:     " << lon;
    qDebug() << "Has point?     " << tp.hasPoint(equator, lon);
    SurfacePoint *sp = tp.point(equator, lon);
    qDebug() << "Surface point: " << sp->GetX().meters()
                                  << sp->GetY().meters()
                                  << sp->GetZ().meters()
                                  << " meters";
    qDebug();
    qDebug();


    AbstractPlate *cloneTP = tp.clone();
    qDebug() << "Create clone";
    qDebug() << "Clone Name:       " << cloneTP->name();
    qDebug() << "Clone Min Radius: " << cloneTP->minRadius().meters() << " meters";
    qDebug() << "Clone Max Radius: " << cloneTP->maxRadius().meters() << " meters";
    qDebug() << "Clone Area:       " << cloneTP->area();
    qDebug() << "Clone Normal:     " << cloneTP->normal();
    qDebug();
    qDebug();


    qDebug() << "Get vertex at index 0";
    qDebug() << tp.vertex(0);
    qDebug();
    qDebug();


    qDebug() << "================================= Error Throws ==================================";
    qDebug() << "Get vertex at index -1";
    try {
      tp.vertex(-1);
    } 
    catch (IException &e) {
      e.print();
    }
    qDebug();
    qDebug() << "Get vertex at index 5";
    try {
      tp.vertex(5);
    } 
    catch (IException &e) {
      e.print();
    }
  }
  catch (IException &e) {
    qDebug();
    qDebug();
    IException(e, IException::Programmer,
              "\n------------Unit Test Failed.------------",
              _FILEINFO_).print();
  }
}
