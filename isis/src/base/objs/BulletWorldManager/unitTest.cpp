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
#include "BulletTargetShape.h"
#include "IException.h"

#include "BulletWorldManager.h"

using namespace Isis;

/**
 * Unit test for the BulletWorldManager class
 */
int main(int argc, char *argv[]) {
  try {
    qDebug() << "Testing BulletWorldManager";
    qDebug() << endl;

    qDebug() << "Testing default constructor";
    qDebug() << "";
    BulletWorldManager defaultWorld;
    qDebug() << "World name: " << defaultWorld.name();
    qDebug() << "World size: " << defaultWorld.size();
    qDebug() << endl;

    qDebug() << "Testing with a name";
    BulletWorldManager namedWorld("TestWorld");
    qDebug() << "World name: " << namedWorld.name();
    qDebug() << "World size: " << namedWorld.size();
    qDebug() << endl;


    QString dskfile("$base/testData/hay_a_amica_5_itokawashape_v1_0_64q.bds");
    qDebug() << "Adding " << dskfile << "...";
    qDebug() << "";
    BulletTargetShape *itokawaShape = BulletTargetShape::load(dskfile);
    if (!itokawaShape) {
      QString msg = "Failed loading shapefile.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
    namedWorld.addTarget(itokawaShape);
    qDebug() << "World size: " << namedWorld.size();
    qDebug() << "";
    qDebug() << "Test intersection";
    btVector3 testFrom(-10.0, 0.0, 0.0);
    btVector3 testTo(10.0, 0.0, 0.0);
    BulletClosestRayCallback testHits(testFrom, testTo);
    qDebug() << "Ray start: ("<< testFrom.x() << ", "
                              << testFrom.y() << ", "
                              << testFrom.z() << ")";
    qDebug() << "Ray end: ("<< testTo.x() << ", "
                            << testTo.y() << ", "
                            << testTo.z() << ")";
    namedWorld.raycast(testFrom, testTo, testHits);
    qDebug() << "Closest intersection: (" << testHits.point().x() << ", "
                                          << testHits.point().y() << ", "
                                          << testHits.point().z() << ")";
    qDebug() << "";
    qDebug() << "Get the first target shape by index";
    BulletTargetShape *firstTarget = namedWorld.getTarget();
    if (firstTarget) {
      qDebug() << "Target name: " << firstTarget->name();
      qDebug() << "Get the first target shape by name";
      BulletTargetShape *namedTarget = namedWorld.getTarget(firstTarget->name());
      qDebug() << "Target name: " << namedTarget->name();
    }
    else {
      QString msg = "Failed accessing the first target shape.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
    qDebug() << "";

    qDebug() << "Get a non-existant target by name";
    QString badTargetName = "not_a_target";
    BulletTargetShape *badTarget = namedWorld.getTarget(badTargetName);
    qDebug() << "Target pointer is valid?" << (bool) badTarget;
    qDebug() << "";

    qDebug() << "Get the bullet world pointer";
    namedWorld.getWorld();
  }
  catch (IException &e) {
    qDebug() << "";
    qDebug() << "";
    QString msg = "**************** UNIT TEST FAILED! **************** ";
    IException(e, IException::Unknown, msg, _FILEINFO_).print();
  }
}