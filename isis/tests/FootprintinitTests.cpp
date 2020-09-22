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

TEST_F(DefaultCube, FunctionalTestFootprintinitDefault) {
  QVector<QString> footprintArgs = {};
  UserInterface footprintUi(FileName("$ISISROOT/bin/xml/footprintinit.xml").expanded(), footprintArgs);

  footprintinit(testCube, footprintUi);
  ASSERT_TRUE(testCube->label()->hasObject("Polygon"));
}
