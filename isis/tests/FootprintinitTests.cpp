#include <iostream>
#include <QTemporaryFile>
#include <QTemporaryDir>

#include "footprintinit.h"

#include "Cube.h"
#include "Pvl.h"
#include "TestUtilities.h"
#include "FileName.h"

#include "Fixtures.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/footprintinit.xml").expanded();

TEST_F(DefaultCube, FunctionalTestFootprintinitDefault) {
  QVector<QString> footprintArgs = {};
  UserInterface footprintUi(APP_XML, footprintArgs);

  footprintinit(testCube, footprintUi);
  ASSERT_TRUE(testCube->label()->hasObject("Polygon"));
}

TEST_F(DefaultCube, FunctionalTestFootprintinitBoundary) {
  QVector<QString> footprintArgs = {"linc=3000", "sinc=3000"};
  UserInterface footprintUi(APP_XML, footprintArgs);

  Pvl footprintLabel;

  std::ifstream isdFile("data/footprintinit/boundary.isd");
  std::ifstream cubeLabel("data/footprintinit/MessengerInstrument.pvl");

  isdFile >> isd;
  cubeLabel >> footprintLabel;

  PvlObject core = footprintLabel.findObject("IsisCube").findObject("Core");
  PvlGroup instrument = footprintLabel.findObject("IsisCube").findGroup("Instrument");

  label.findObject("IsisCube").deleteObject("Core");
  label.findObject("IsisCube").addObject(core);
  label.findObject("IsisCube").deleteGroup("Instrument");
  label.findObject("IsisCube").addGroup(instrument);

  label.findObject("IsisCube").findGroup("Kernels")["CameraVersion"] = "2";
  label.findObject("IsisCube").findGroup("Kernels")["NaifFrameCode"] = "-236820";
  label.findObject("IsisCube").findGroup("Kernels")["SpacecraftClock"] = "$messenger/kernels/sclk/messenger_2548.tsc";
  label.findObject("IsisCube").findGroup("Kernels")["ShapeModel"] = "$base/dems/MSGR_DEM_USG_EQ_I_V02_prep.cub";

  Cube *footprintCube = new Cube();
  footprintCube->fromIsd("/Users/acpaquette/Desktop/footprintCube.cub", label, isd, "rw");

  try {
    footprintinit(footprintCube, footprintUi);
  }
  catch(IException &e) {
    if (footprintCube->isOpen()) {
      footprintCube->close();
    }
    FAIL() << "Unable to generate image footprint: " << std::endl << e.what() << std::endl;
  }

  ImagePolygon poly;
  footprintCube->read(poly);

  QString polyString = QString::fromStdString(poly.toString());
  QString expectedString = QString::fromStdString("MULTIPOLYGON (((221.8595709924715607 19.8008190719555408, 222.6098248485523925 26.0646133250630392, 257.3607474400786828 25.6133189736515980, 262.5545863726566722 13.1480077576119019, 236.6175358467705223 12.9396510924725483, 221.8595709924715607 19.8008190719555408)))");
  ASSERT_EQ(polyString.compare(expectedString), 0);

  if (footprintCube->isOpen()) {
    footprintCube->close();
  }
}
