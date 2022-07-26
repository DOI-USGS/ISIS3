#include "CameraFixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "socetlinescankeywords.h"

#include "TestUtilities.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace Isis;
using namespace testing;

static QString APP_XML = FileName("$ISISROOT/bin/xml/socetlinescankeywords.xml").expanded();

TEST_F(MroHiriseCube, FunctionalTestSocetlinescankeywordsHirise) {
  testCube->reopen("rw");
  QString outFileName = tempDir.path() + "/outTEMP.txt";
  QVector<QString> args = {"to="+outFileName};

  UserInterface options(APP_XML, args);

  try {
    socetlinescankeywords(testCube, options);
  }
  catch (IException &e) {
    FAIL() << "Call failed, Unable to process cube: " << e.what() << std::endl;
  }

  QFile bo(outFileName);
  QString contents;

  if (bo.open(QIODevice::ReadOnly)) {
    contents = bo.read(bo.size());
  }
  else {
    FAIL() << "Failed to open bundleout.txt" << std::endl;
  }

  QStringList lines = contents.split("\n");

  QStringList line = lines[3].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[0], "GROUND_ZERO");
  EXPECT_NEAR(line[1].toDouble(), 1.78293110035684e-01, 0.0001);
  EXPECT_NEAR(line[2].toDouble(), -1.81777668639857, 0.0001);
  EXPECT_NEAR(line[3].toDouble(), 0, 0.0001);

  line = lines[7].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[0], "SENSOR_TYPE");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[1], "USGSAstroLineScanner");

  line = lines[23].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[0], "DT_EPHEM");
  EXPECT_NEAR(line[1].toDouble(), 3.238258064516128e-03, 0.0001);

  line = lines[24].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[0], "T0_EPHEM");
  EXPECT_NEAR(line[1].toDouble(), -8.095645904541e-02, 0.0001);
  line = lines[25].split(QRegExp("\\s+"), QString::SkipEmptyParts);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[0], "NUMBER_OF_EPHEM");
  EXPECT_EQ(line[1].toInt(), 51);

  line = lines[134].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[0], "DT_QUAT");
  EXPECT_NEAR(line[1].toDouble(), 3.23825806451612891e-03, 0.0001);

  line = lines[135].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[0], "T0_QUAT");
  EXPECT_NEAR(line[1].toDouble(), -8.0956459045410156e-02, 0.0001);

  line = lines[136].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[0], "NUMBER_OF_QUATERNIONS");
  EXPECT_EQ(line[1].toInt(), 51);

  line = lines[197].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[0], "CENTER_GP");
  EXPECT_NEAR(line[1].toDouble(), 1.782931100356e-01, 0.0001);
  EXPECT_NEAR(line[2].toDouble(), -1.81777668639857, 0.0001);
  EXPECT_NEAR(line[3].toDouble(), 0, 0.0001);

  // check some interior orientation stuff
  line = lines[207].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[0], "IKCODE");
  EXPECT_EQ(line[1].toInt(), -74999);

  line = lines[208].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[0], "ISIS_Z_DIRECTION");
  EXPECT_NEAR(line[1].toDouble(), 1, 0.0001);

  line = lines[210].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[0], "ITRANSS");
  EXPECT_NEAR(line[1].toDouble(), -1000.86, 0.001);
  EXPECT_NEAR(line[2].toDouble(), -0.00869999, 0.001);
  EXPECT_NEAR(line[3].toDouble(), -83.33299, 0.001);

  line = lines[211].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[0], "ITRANSL");
  EXPECT_NEAR(line[1].toDouble(), 7457.89999999, 0.001);
  EXPECT_NEAR(line[2].toDouble(), 83.3329999, 0.001);
  EXPECT_NEAR(line[3].toDouble(), -0.0086999, 0.001);

  line = lines[218].split(QRegExp("\\s+"), QString::SkipEmptyParts);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, line[0], "STARTING_EPHEMERIS_TIME");
  EXPECT_NEAR(line[1].toDouble(), 502476937.6769, 0.0001);
}


TEST_F(DefaultCube, FunctionalTestSocetlinescankeywordsLev2) {
  testCube->reopen("rw");
  QString outFileName =tempDir.path() + "/outTEMP.txt";
  QVector<QString> args = {"to="+outFileName};

  UserInterface options(APP_XML, args);

  try {
    socetlinescankeywords(projTestCube, options);
    FAIL() << "Should Fail";
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("Input images is a map projected cube"));;
  }
}


TEST_F(DefaultCube, FunctionalTestSocetlinescankeywordsNoBlob) {
  testCube->reopen("rw");
  QString outFileName = tempDir.path() + "/outTEMP.txt";
  QVector<QString> args = {"to="+outFileName};

  // trick it into thinking that spice is not attached
  Pvl *oglabel = testCube->label();
  PvlGroup &kernels = oglabel->findGroup("Kernels", Pvl::Traverse);
  PvlKeyword &name = kernels.findKeyword("InstrumentPointing");
  name.setValue("NotATable");

  UserInterface options(APP_XML, args);

  try {
    socetlinescankeywords(testCube, options);
    FAIL() << "Should Fail";
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("Input image does not contain needed SPICE blobs...run spiceinit with attach=yes"));;
  }
}
