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
#include "EmbreeTargetManager.h"
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
 * Unit test for Embree Ray Tracing Kernels
 *
 */
int main(int argc, char *argv[]) {
  try {
    Preference::Preferences(true);
    qDebug() << "Testing EmbreeTargetManager";
    qDebug() << endl;

    qDebug() << "Get an instance of the target manager";
    EmbreeTargetManager *manager = EmbreeTargetManager::getInstance();
    qDebug() << "Maximum cache size: " << manager->maxCacheSize();
    qDebug() << "Current cache size: " << manager->currentCacheSize();
    qDebug() << "";

    qDebug() << "Change the maximum cache size: ";
    manager->setMaxCacheSize(5);
    qDebug() << "New maximum cache size: " << manager->maxCacheSize();
    qDebug() << "";

    QString dskfile("$base/testData/hay_a_amica_5_itokawashape_v1_0_64q.bds");
    qDebug() << "Create a target shape for " << dskfile;
    EmbreeTargetShape *managedTargetShape = manager->create(dskfile);
    qDebug() << "Target shape status:";
    qDebug() << "  Number of polygons: " << managedTargetShape->numberOfPolygons();
    qDebug() << "  Number of vertices: " << managedTargetShape->numberOfVertices();
    qDebug() << "  Maximum distance:   " << managedTargetShape->maximumSceneDistance();
    qDebug() << "Current cache size: " << manager->currentCacheSize();
    qDebug() << "";

    qDebug() << "Create a new target shape for the same file";
    QString copyDSKFile = "$ISIS3DATA/base/testData/hay_a_amica_5_itokawashape_v1_0_64q.bds";
    EmbreeTargetShape *copyTargetShape = manager->create(copyDSKFile);
    qDebug() << "Target shape status:";
    qDebug() << "  Number of polygons: " << copyTargetShape->numberOfPolygons();
    qDebug() << "  Number of vertices: " << copyTargetShape->numberOfVertices();
    qDebug() << "  Maximum distance:   " << copyTargetShape->maximumSceneDistance();
    qDebug() << "Current cache size: " << manager->currentCacheSize();
    qDebug() << "";
    qDebug() << "Shape file is in the cache? " << manager->inCache(copyDSKFile);
    qDebug() << "";
    qDebug() << "Both instances point to the same object? "
             << (copyTargetShape == managedTargetShape);
    qDebug() << "";

    qDebug() << "Free one of them";
    manager->free(copyDSKFile);
    qDebug() << "Current cache size: " << manager->currentCacheSize();
    qDebug() << "";

    qDebug() << "Free the other";
    manager->free(dskfile);
    qDebug() << "Current cache size: " << manager->currentCacheSize();
    qDebug() << "Shape file is in the cache? " << manager->inCache(copyDSKFile);
    qDebug() << "";

    qDebug() << "Set the maximum number of target shapes to 0";
    manager->setMaxCacheSize(0);
    qDebug() << "New maximum cache size: " << manager->maxCacheSize();
    qDebug() << "";

    qDebug() << "Attempt to create a new target shape";
    try {
      manager->create(dskfile);
    }
    catch (IException &e) {
      e.print();
    }
    qDebug() << "";

    qDebug() << "Attempt to free a shape that doesn't exist";
    try {
      manager->free("Not a DSK file");
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
