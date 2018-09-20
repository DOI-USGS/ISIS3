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
#include <QVector>

#include "NaifDskPlateModel.h"
#include "NaifDskShape.h"
#include "Angle.h"
#include "Camera.h"
#include "Cube.h"
#include "Distance.h"
#include "BulletDskShape.h"
#include "BulletShapeFactory.h"
#include "BulletTargetShape.h"
#include "FileName.h"
#include "IException.h"
#include "Intercept.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Preference.h"
#include "Pvl.h"
#include "Spice.h"
#include "SurfacePoint.h"
#include "Target.h"

using namespace Isis;

/** 
 * Unit test for Bullet Ray Tracing Kernels
 *
 */
int main(int argc, char *argv[]) {
  try {
    Preference::Preferences(true);
    qDebug() << "Testing BulletWorldManager";
    qDebug() << endl;

    qDebug() << "Get an instance of the target manager";
    BulletShapeFactory *manager = BulletShapeFactory::getInstance();

    QString dskfile("$base/testData/hay_a_amica_5_itokawashape_v1_0_64q.bds");
    qDebug() << "Create a target shape for " << dskfile;
    BulletWorldManager *world = manager->createWorld(dskfile);
    BulletDskShape  *managedTargetShape = reinterpret_cast<BulletDskShape *> (world->getTarget());
    qDebug() << "Target shape status:";
    qDebug() << "  Number of polygons: " << managedTargetShape->getNumTriangles();
    qDebug() << "  Number of vertices: " << managedTargetShape->getNumVertices();
    qDebug() << "  Maximum distance:   " << managedTargetShape->maximumDistance();
    qDebug() << "Current cache size: " << manager->size();
    qDebug() << "";

    qDebug() << "Create a new target shape for the same file";
    QString copyDSKFile = "$ISIS3DATA/base/testData/hay_a_amica_5_itokawashape_v1_0_64q.bds";
    BulletWorldManager *copiedWorld = manager->createWorld(copyDSKFile);
    BulletDskShape  *copiedTargetShape = reinterpret_cast<BulletDskShape *> (copiedWorld->getTarget());
    qDebug() << "Target shape status:";
    qDebug() << "  Number of polygons: " << copiedTargetShape->getNumTriangles();
    qDebug() << "  Number of vertices: " << copiedTargetShape->getNumVertices();
    qDebug() << "  Maximum distance:   " << copiedTargetShape->maximumDistance();
    qDebug() << "Current cache size: " << manager->size();
    qDebug() << "";
    qDebug() << "Both shape instances point to the same object? "
             << (copiedTargetShape == managedTargetShape);
    qDebug() << "";
    qDebug() << "Both world instances point to the same object? "
             << (&copiedWorld->getWorld() == &world->getWorld() );
    qDebug() << "";

    qDebug() << "Free one of them";
    delete copiedWorld;
    qDebug() << "Current cache size: " << manager->size();
    qDebug() << "";

    qDebug() << "Remove the common copy";
    manager->remove(dskfile);
    qDebug() << "Current cache size: " << manager->size();
    qDebug() << "";

    qDebug() << "Ensure remaining world instance is still valid";
    qDebug() << "  Number of polygons: " << managedTargetShape->getNumTriangles();
    qDebug() << "  Number of vertices: " << managedTargetShape->getNumVertices();
    qDebug() << "  Maximum distance:   " << managedTargetShape->maximumDistance();
    qDebug() << "";

    qDebug() << "Free the other";
    delete world;
    qDebug() << "Current cache size: " << manager->size();
    qDebug() << "";

    qDebug() << "Attempt to create a new target shape";
    try {
      QScopedPointer<BulletWorldManager> tmp_world(manager->createWorld(dskfile));
      qDebug() << "Name: " << tmp_world->name();
      qDebug() << "Current cache size: " << manager->size();
      qDebug() << "";

    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "Shows instances live on in factory after last requested shape is deleted";
    qDebug() << "Current cache size: " << manager->size();
    qDebug() << "";


    qDebug() << "Attempt to free a shape that doesn't exist - ignored in this implementation";
    try {
      manager->remove("Not a DSK file");
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
