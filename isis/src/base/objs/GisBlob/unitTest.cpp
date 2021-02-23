/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QTextStream>

#include "Cube.h"
#include "GisBlob.h"
#include "IException.h"
#include "IString.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

/**
 * Unit test for GisBlob class
 *
 *
 * @author 2016-02-23 Jeannie Backer
 *
 * @internal
 *   @history 2016-02-23 Jeannie Backer - Original version.
 *  
 */
int main() {
  try {
    Preference::Preferences(true);
    qDebug() << "";
    qDebug() << "Testing GisBlob's constructor from cube...";
    QString inputFile = "$ISISTESTDATA/isis/src/messenger/unitTestData/EW0211286081G.lev1.cub";
    Cube cube;
    cube.open(inputFile);
    GisBlob cubeBlob(cube);
    qDebug() << "    Polygon = " << cubeBlob.polygon();

    qDebug() << "";
    qDebug() << "Testing GisBlob's default constructor...";
    GisBlob blob;
    qDebug() << "    Polygon = " << blob.polygon();
    
    qDebug() << "";
    qDebug() << "Adding WKT polygon to GisBlob...";
    /*
    QFile file("unitTest.wkt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      throw IException(IException::Io, "Unable to open wkt file, [unitTest.wkt]", _FILEINFO_);
    }
    QString wkt;
    QTextStream stream(&file);
    wkt.append(stream.readAll());
    */
    QString wkt = cubeBlob.polygon();
    blob.setPolygon(wkt);
    qDebug() << "    Polygon = " << blob.polygon();
  }
  catch (IException &e) {
    qDebug() << "";
    qDebug() << "";
    QString msg = "**************** UNIT TEST FAILED! **************** ";
    IException(e, IException::Unknown, msg, _FILEINFO_).print();
  }
}


