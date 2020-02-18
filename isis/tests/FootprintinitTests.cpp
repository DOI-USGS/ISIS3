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

TEST_F(DefaultCube, FunctionalFootprintinitDefault) {
  // QTemporaryDir prefix;
  // ASSERT_TRUE(prefix.isValid());
  //
  // QVector<QString> footprintArgs = {};
  // UserInterface footprintUi(APP_XML, footprintArgs);
  //
  // std::cout << tempFile.fileName() << '\n';
  //
  // footprintinit(testCube, footprintUi);
  // std::cout << cube.label()->findKeyword("IsisCore") << '\n';
}
