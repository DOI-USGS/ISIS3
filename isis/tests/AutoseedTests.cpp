#include <iostream>
#include <QTemporaryFile>
#include <QTemporaryDir>

#include "autoseed.h"
#include "findimageoverlaps.h"

#include "Cube.h"
#include "CubeAttribute.h"
#include "PixelType.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "TestUtilities.h"
#include "FileName.h"

#include "Fixtures.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/autoseed.xml").expanded();

TEST_F(ThreeImageNetwork, FunctionalAutoseedDefault) {
  QTemporaryDir prefix;
  ASSERT_TRUE(prefix.isValid());

  std::cout << "COMPUTING OVERLAP" << '\n';

  QTemporaryFile overlapList;
  overlapList.open();
  QVector<QString> overlapArgs = {"fromlist="+cubeListTempPath.fileName(),
                                  "overlaplist="+overlapList.fileName()};
  UserInterface overlapUi(APP_XML, overlapArgs);
  findimageoverlaps(overlapUi);
  std::cout << "OVERLAP FIN" << '\n';

  Pvl *log = NULL;

  QVector<QString> autoseedArgs = {"fromlist="+cubeListTempPath.fileName(),
                                    "onet="+prefix.path()+"seeded.net",
                                    "deffile=/usgs/cpkgs/isis3/testData/isis/src/control/apps/autoseed/tsts/seeddef/input/gridPixels.pvl",
                                    "overlaplist="+overlapList.fileName(),
                                    "networkid=1",
                                    "pointid=????",
                                    "description=autoseed test network"};
  UserInterface autoseedUi(APP_XML, autoseedArgs);
  autoseed(autoseedUi, log);
}
