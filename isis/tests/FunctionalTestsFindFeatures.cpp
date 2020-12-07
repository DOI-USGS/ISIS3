#include "findfeatures.h"

#include <QTemporaryFile>
#include <QTextStream>
#include <QStringList>

#include "Fixtures.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "SurfacePoint.h"
#include "ControlPoint.h"
#include "Latitude.h"
#include "Longitude.h"

#include "gmock/gmock.h"

using namespace Isis;
using ::testing::HasSubstr;

static QString APP_XML = FileName("$ISISROOT/bin/xml/findfeatures.xml").expanded();

TEST_F(ThreeImageNetwork, FunctionalTestFindFeaturesDefault) {
  // Setup output file
  QVector<QString> args = {"algorithm=brisk/brisk",
                           "match=" + tempDir.path() + "/cube3.cub",
                           "fromlist=" + twoCubeListFile,
                           "tolist=" + tempDir.path() + "/toList.txt",
                           "tonotmatched=" + tempDir.path() + "/unmatched.txt",
                           "maxpoints=5000",
                           "epitolerance=1.0",
                           "ratio=.65",
                           "hmgtolerance=3.0",
                           "onet=" + tempDir.path() + "/network.net",
                           "networkid=new",
                           "pointid=test_network_????",
                           "description=new",
                           "debug=false"};
  UserInterface options(APP_XML, args);
  Pvl log;

  findfeatures(options, &log);
  ControlNet network(options.GetFileName("ONET"));

  ASSERT_EQ(network.GetNetworkId(), "new");
  ASSERT_EQ(network.Description().toStdString(), "brisk/brisk/BFMatcher@NormType:NORM_HAMMING@CrossCheck:false");
  ASSERT_EQ(network.GetNumPoints(), 50);
}


TEST_F(ThreeImageNetwork, FunctionalTestFindFeaturesGeomMatch) {
  // Setup output file
  QVector<QString> args = {"algorithm=brisk/brisk",
                           "match=" + tempDir.path() + "/cube3.cub",
                           "fromlist=" + twoCubeListFile,
                           "tolist=" + tempDir.path() + "/toList.txt",
                           "tonotmatched=" + tempDir.path() + "/unmatched.txt",
                           "maxpoints=5000",
                           "epitolerance=1.0",
                           "ratio=.65",
                           "hmgtolerance=3.0",
                           "onet=" + tempDir.path() + "/network.net",
                           "networkid=new",
                           "pointid=test_network_????",
                           "description=new",
                           "geomsource=match",
                           "debug=false"};
  UserInterface options(APP_XML, args);
  Pvl log;

  findfeatures(options, &log);
  ControlNet network(options.GetFileName("ONET"));
  ControlPoint *pt = network.GetPoint("test_network_0001");
  SurfacePoint sp = pt->GetAdjustedSurfacePoint();
  Latitude lat = sp.GetLatitude();
  Longitude lon = sp.GetLongitude();
  // Empty lat/lons because we're matching on a fixture with no geometry info
  ASSERT_EQ(lat.toString(), "");
  ASSERT_EQ(lon.toString(), "");
}


TEST_F(ThreeImageNetwork, FunctionalTestFindFeaturesMultiAlgo) {
  // Setup output file
  QVector<QString> args = {"algorithm=brisk/brisk|surf@hessianThreshold:100/surf",
                           "match=" + tempDir.path() + "/cube3.cub",
                           "fromlist=" + twoCubeListFile,
                           "tolist=" + tempDir.path() + "/toList.txt",
                           "tonotmatched=" + tempDir.path() + "/unmatched.txt",
                           "maxpoints=5000",
                           "epitolerance=1.0",
                           "ratio=.65",
                           "hmgtolerance=3.0",
                           "onet=" + tempDir.path() + "/network.net",
                           "networkid=new",
                           "pointid=test_network_????",
                           "description=new",
                           "debug=false"};
  UserInterface options(APP_XML, args);
  Pvl log;

  findfeatures(options, &log);
  ControlNet network(options.GetFileName("ONET"));

  ASSERT_EQ(network.GetNetworkId(), "new");
  ASSERT_EQ(network.Description().toStdString(), "brisk/brisk/BFMatcher@NormType:NORM_HAMMING@CrossCheck:false");
  ASSERT_EQ(network.GetNumPoints(), 50);
}

TEST_F(ThreeImageNetwork, FunctionalTestFindFeaturesMaxPoints) {
  // Tests that decreasing maxpoints decreases output points + tests pointindex
  QVector<QString> args = { "algorithm=brisk/brisk",
                           "match=" + tempDir.path() + "/cube3.cub",
                           "fromlist=" + twoCubeListFile,
                           "tolist=" + tempDir.path() + "/toList.txt",
                           "tonotmatched=" + tempDir.path() + "/unmatched.txt",
                           "maxpoints=1000",
                           "epitolerance=1.0",
                           "ratio=.65",
                           "hmgtolerance=3.0",
                           "onet=" + tempDir.path() + "/network.net",
                           "networkid=new",
                           "pointid=test_network_????",
                           "pointindex=100",
                           "description=new",
                           "debug=false"};

  UserInterface options(APP_XML, args);
  Pvl log;

  findfeatures(options, &log);
  ControlNet network(options.GetFileName("ONET"));

  ASSERT_EQ(network.GetNetworkId(), "new");
  ASSERT_EQ(network.Description().toStdString(), "brisk/brisk/BFMatcher@NormType:NORM_HAMMING@CrossCheck:false");

  ASSERT_TRUE(network.ContainsPoint("test_network_0100"));
  ASSERT_EQ(network.GetNumPoints(), 22);
}


TEST_F(ThreeImageNetwork, FunctionalTestFindFeaturesErrorListspecNoAlg) {
  QVector<QString> args = {"listspec=yes"};
  UserInterface options(APP_XML, args);
  Pvl log;

  try {
    findfeatures(options, &log);
    FAIL() << "Should throw an exception" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("**USER ERROR** No feature matcher algorithms provided!"));
  }
}


TEST_F(ThreeImageNetwork, FunctionalTestFindFeaturesErrorInputNoAlg) {
  QVector<QString> args = {"match=" + tempDir.path() + "/cube3.cub",
                           "from=" + tempDir.path() + "/cube2.cub"};
  UserInterface options(APP_XML, args);
  Pvl log;

  try {
    findfeatures(options, &log);
    FAIL() << "Should throw an exception" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("**USER ERROR** No feature matcher algorithms provided!"));
  }
}


TEST_F(ThreeImageNetwork, FunctionalTestFindFeaturesErrorNoInput) {
  QVector<QString> args = {"match=" + tempDir.path() + "/cube3.cub",
                           "algorithm=surf/surf"};
  UserInterface options(APP_XML, args);
  Pvl log;

  try {
    findfeatures(options, &log);
    FAIL() << "Should throw an exception" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("**USER ERROR** Must provide both a FROM/FROMLIST and MATCH cube or image filename"));
  }
}



TEST_F(ThreeImageNetwork, FunctionalTestFindFeaturesErrorNoMatch) {
  // Tests that decreasing maxpoints decreases output points + tests pointindex
  QVector<QString> args = { "algorithm=brisk/brisk",
                           "match=" + tempDir.path() + "/cube3.cub",
                           "fromlist=" + twoCubeListFile,
                           "tolist=" + tempDir.path() + "/toList.txt",
                           "tonotmatched=" + tempDir.path() + "/unmatched.txt",
                           "maxpoints=1",
                           "epitolerance=1.0",
                           "ratio=.65",
                           "hmgtolerance=3.0",
                           "onet=" + tempDir.path() + "/network.net",
                           "networkid=new",
                           "pointid=test_network_????",
                           "pointindex=100",
                           "description=new",
                           "debug=false"};

  UserInterface options(APP_XML, args);
  Pvl log;
  try {
    findfeatures(options, &log);
    FAIL() << "Should throw an exception" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("**USER ERROR** No control points found!"));
  }
}
