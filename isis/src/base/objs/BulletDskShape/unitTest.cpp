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

#include "BulletDskShape.h"

using namespace Isis;

/**
 * Unit test for the BulletDskShape class
 */
int main(int argc, char *argv[]) {
  try {
    qDebug() << "Testing BulletDskShape";
    qDebug() << endl;

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
    qDebug() << "Normal for triangle 0: (" << normal.x() << ", "
                                          << normal.y() << ", "
                                          << normal.z() << ")";
    qDebug() << endl;

    qDebug() << "Testing loading non-existant file";
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
