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

#include "BulletTargetShape.h"

using namespace Isis;

/**
 * Unit test for the BulletTargetShape class
 */
int main(int argc, char *argv[]) {
  try {
    qDebug() << "Testing BulletTargetShape";
    qDebug() << endl;

    qDebug() << "Testing default constructor";
    qDebug() << "";
    BulletTargetShape defaultTargetShape;
    qDebug() << "Target name: " << defaultTargetShape.name();
    qDebug() << "Maximum distance in kilometers: " << defaultTargetShape.maximumDistance();
    qDebug() << "btCollisionBody pointer is valid? " << (bool) defaultTargetShape.body();
    qDebug() << endl;

    qDebug() << "Testing load constructor";
    QString dskfile("$base/testData/hay_a_amica_5_itokawashape_v1_0_64q.bds");
    qDebug() << "Testing with " << dskfile << "...";
    qDebug() << "";
    BulletTargetShape *itokawaTargetShape = BulletTargetShape::load(dskfile);
    qDebug() << "Target name: " << itokawaTargetShape->name();
    qDebug() << "Maximum distance in kilometers: " << itokawaTargetShape->maximumDistance();
    qDebug() << "btCollisionBody pointer is valid? " << (bool) itokawaTargetShape->body();
    qDebug() << endl;

    qDebug() << "Testing load constructor with cube";
    QString itokawaCube = "$hayabusa/testData/st_2391934788_v.cub";
    qDebug() << "Testing with " << itokawaCube << "...";
    qDebug() << "";
    BulletTargetShape *cubeTargetShape = BulletTargetShape::load(itokawaCube);
    qDebug() << "Target shape pointer is valid?" << (bool) cubeTargetShape;
    qDebug() << endl;

    qDebug() << "Testing load constructor with other extension";
    QString otherFile = "$base/testData/xmlTestLabel.xml";
    qDebug() << "Testing with " << otherFile << "...";
    qDebug() << "";
    BulletTargetShape *otherTargetShape = BulletTargetShape::load(otherFile);
    qDebug() << "Target shape pointer is valid?" << (bool) otherTargetShape;
    qDebug() << endl;

    qDebug() << "Testing collision body constructor";
    BulletTargetShape collisionTargetShape(itokawaTargetShape->body(), "Itokawa");
    qDebug() << "Target name: " << collisionTargetShape.name();
    qDebug() << "Maximum distance in kilometers: " << collisionTargetShape.maximumDistance();
    qDebug() << "btCollisionBody pointer is valid? " << (bool) collisionTargetShape.body();
    qDebug() << endl;

    qDebug() << "Testing null collision body constructor";
    BulletTargetShape nullTargetShape(0, "Null");
    qDebug() << "Target name: " << nullTargetShape.name();
    qDebug() << "Maximum distance in kilometers: " << nullTargetShape.maximumDistance();
    qDebug() << "btCollisionBody pointer is valid? " << (bool) nullTargetShape.body();
  }
  catch (IException &e) {
    qDebug() << "";
    qDebug() << "";
    QString msg = "**************** UNIT TEST FAILED! **************** ";
    IException(e, IException::Unknown, msg, _FILEINFO_).print();
  }
}
