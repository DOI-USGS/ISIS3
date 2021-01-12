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
    QString inputFile = "$messenger/testData/EW0211286081G.lev1.cub";
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


