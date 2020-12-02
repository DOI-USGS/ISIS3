#include <QTemporaryDir>
#include <memory>

#include "apollopanstitcher.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"
#include "Endian.h"
#include "PixelType.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Fixtures.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/apollopanstitcher.xml").expanded();

TEST_F(TempTestingFiles, FunctionalTestsApolloPanStitcherDefault) {
  QVector<QString> args = {"file_base=$ISISTESTDATA/isis/src/apollo/apps/apollopanstitcher/tsts/default/input/AS15_P_0177R10",
                           "to=" + tempDir.path() + "/reduced8.cub",
                           "microns=50"};

  UserInterface options(APP_XML, args);
  try {
   apolloPanStitcher(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to stitcher apollo images: " << e.toString().toStdString().c_str() << std::endl;
  }
  Cube outputCube(options.GetFileName("TO"));
}
