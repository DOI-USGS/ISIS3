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

#include "BulletClosestRayCallback.h"
#include "BulletDskShape.h"
#include "BulletWorldManager.h"
#include "IException.h"
#include "IsisBullet.h"
#include "Preference.h"

using namespace Isis;

/**
 * Unit test for the BulletDskShape class
 */
int main(int argc, char *argv[]) {
  try {
    Preference::Preferences(true);

    qDebug() << "Testing BulletDskShape";
    qDebug() << endl;

#if 0  // This could change for any Bullet installation so we shouldn't report this
    qDebug() << "Maximum Bullet Shape Parts:     " << bt_MaxBodyParts();
    qDebug() << "Maximum Bullet Shape Triangles: " << bt_MaxTriangles();
    qDebug() << endl;
#endif

    qDebug() << "Testing default constructor";
    qDebug() << "";
    BulletDskShape defaultDskShape;
    qDebug() << "Target name: " << defaultDskShape.name();
    qDebug() << "Maximum distance in kilometers: " << defaultDskShape.maximumDistance();
    qDebug() << "btCollisionBody pointer is valid? " << (bool) defaultDskShape.body();
    qDebug() << "Number of triangles: " << defaultDskShape.getNumTriangles();
    qDebug() << "Number of vertices: " << defaultDskShape.getNumVertices();
    qDebug() << endl;

    qDebug() << "Testing DSK file constructor";
    QString dskfile("$base/testData/hay_a_amica_5_itokawashape_v1_0_64q.bds");
    // QString dskfile("$hayabusa/kernels/dsk/hay_a_amica_5_itokawashape_v1_0_512q.bds");
    qDebug() << "Testing with " << dskfile << "...";
    qDebug() << "";
    BulletDskShape itokawaShape(dskfile);
    qDebug() << "Target name: " << itokawaShape.name();
    qDebug() << "Maximum distance in kilometers: " << itokawaShape.maximumDistance();
    qDebug() << "btCollisionBody pointer is valid? " << (bool) itokawaShape.body();
    qDebug() << "Number of triangles: " << itokawaShape.getNumTriangles();
    qDebug() << "Number of vertices: " << itokawaShape.getNumVertices();
    qDebug() << "";
    btMatrix3x3 triangle = itokawaShape.getTriangle(0);
    qDebug() << "Vertices for triangle 0: (" << triangle[0].x() << ", "
                                             << triangle[0].y() << ", "
                                             << triangle[0].z() << ")";
    qDebug() << "                         (" << triangle[1].x() << ", "
                                             << triangle[1].y() << ", "
                                             << triangle[1].z() << ")";
    qDebug() << "                         (" << triangle[2].x() << ", "
                                             << triangle[2].y() << ", "
                                             << triangle[2].z() << ")";
    btVector3 normal = itokawaShape.getNormal(0);
    qDebug() << "Normal for triangle 0:   (" << normal.x() << ", "
                                             << normal.y() << ", "
                                             << normal.z() << ")";
    qDebug() << endl;

    //  TESTING TRIANGLES
    int nTriangles = itokawaShape.getNumTriangles();
    triangle = itokawaShape.getTriangle(nTriangles-1);
    qDebug() << "Vertices for last triangle: (" << triangle[0].x() << ", "
                                                << triangle[0].y() << ", "
                                                << triangle[0].z() << ")";
    qDebug() << "                            (" << triangle[1].x() << ", "
                                                << triangle[1].y() << ", "
                                                << triangle[1].z() << ")";
    qDebug() << "                            (" << triangle[2].x() << ", "
                                                << triangle[2].y() << ", "
                                                << triangle[2].z() << ")";


    qDebug() << "\nTesting some intersection...";
    BulletWorldManager world;
    world.addTarget(&itokawaShape);

    btVector3 origin(1000.0, 0.0, 0.0);
    btVector3 raydir(-1.0, 0.0, 0.0);

    qDebug() << "Origin:               (" << origin.x() << ", "
                                          << origin.y() << ", "
                                          << origin.z() << ")";
    qDebug() << "Raydir:               (" << raydir.x() << ", "
                                          << raydir.y() << ", "
                                          << raydir.z() << ")";

    BulletClosestRayCallback result(origin, raydir);
    bool hit = world.raycast(origin, raydir, result);
    qDebug() << "Got a hit?" << hit;
    btVector3 point = result.point();
    qDebug() << "Intersection:         (" << point.x() << ", "
                                          << point.y() << ", "
                                          << point.z() << ")";
    normal = result.normal();
    qDebug() << "Normal for intercept: (" << normal.x() << ", "
                                          << normal.y() << ", "
                                          << normal.z() << ")";
    qDebug() << "PlateId: " << result.triangleIndex();
    qDebug() << "PartId:  " << result.partId();

    qDebug() << "\nTesting loading non-existant file";
    try {
      BulletDskShape badShape("not_a_file");
    }
    catch (IException &e) {
      e.print();
    }
  }
  catch (IException &e) {
    qDebug() << "";
    qDebug() << "";
    QString msg = "**************** UNIT TEST FAILED! **************** ";
    IException(e, IException::Unknown, msg, _FILEINFO_).print();
  }
}
