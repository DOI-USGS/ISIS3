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
  auto naif = NaifContext::acquire();

  footprintinit(naif, testCube, footprintUi);
  ASSERT_TRUE(testCube->label()->hasObject("Polygon"));
}
