#include <iostream>
#include <time.h>

#include <QRegExp>
#include <QString>
#include <nlohmann/json.hpp>

#include "Fixtures.h"
#include "Histogram.h"
#include "md5wrapper.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "TestUtilities.h"

#include "isisimport.h"

#include "gmock/gmock.h"

using namespace Isis;
using namespace testing;
using json = nlohmann::json;

static QString APP_XML = FileName("$ISISROOT/bin/xml/isisimport.xml").expanded();

/*
TEST(IsisImportTests, LroNacLFull) {
  QTemporaryDir prefix;
  ASSERT_TRUE(prefix.isValid());

  QString cubeFileName = prefix.path() + "/lo2isisTEMP.cub";
  QVector<QString> args = {"from=data/lronac/nacl.img", "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest LO image: " <<e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions group
  ASSERT_EQ(cube.sampleCount(), 5064);
  ASSERT_EQ(cube.lineCount(), 10);
  ASSERT_EQ(cube.bandCount(), 1);

  // Pixels group
  ASSERT_EQ(PixelTypeName(cube.pixelType()), "Real");
  ASSERT_EQ(ByteOrderName(cube.byteOrder()), "Lsb");
  ASSERT_DOUBLE_EQ(cube.base(), 0.0);
  ASSERT_DOUBLE_EQ(cube.multiplier(), 1.0);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["SpacecraftName"][0].toStdString(), "LUNAR RECONNAISSANCE ORBITER");
  ASSERT_EQ(inst["InstrumentId"][0].toStdString(), "NACL");
  ASSERT_EQ(inst["TargetName"][0].toStdString(), "MOON");
  ASSERT_EQ(inst["StartTime"][0].toStdString(), "2009-11-29T14:51:21.968000");

  ASSERT_EQ(inst["TemperatureSCS"][0].toStdString(), "3.88");
  ASSERT_EQ(inst["TemperatureFPA"][0].toStdString(), "17.73");
  ASSERT_EQ(inst["TemperatureFPGA"][0].toStdString(), "-12.94");
  ASSERT_EQ(inst["TemperatureTelescope"][0].toStdString(), "8.89");
  ASSERT_EQ(inst["SpatialSumming"][0].toStdString(), "1");

  ASSERT_EQ(inst["TemperatureSCSRaw"][0].toStdString(), "2770");
  ASSERT_EQ(inst["TemperatureFPARaw"][0].toStdString(), "2115");
  ASSERT_EQ(inst["TemperatureFPGARaw"][0].toStdString(), "3440");
  ASSERT_EQ(inst["TemperatureTelescopeRaw"][0].toStdString(), "2536");

  // Bandbin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  ASSERT_EQ(bandbin["FilterName"][0], "BroadBand");
  ASSERT_EQ(bandbin["Center"][0], "600");
  ASSERT_EQ(bandbin["Width"][0], "300");

  // Kernels Group
  PvlGroup &kern = isisLabel->findGroup("Kernels", Pvl::Traverse);
  ASSERT_EQ(int(kern["NaifFrameCode"]), -85600);

  std::unique_ptr<Histogram> hist (cube.histogram());

  ASSERT_NEAR(hist->Average(), 167.19855845181675, .00001);
  ASSERT_EQ(hist->Sum(), 8466935);
  ASSERT_EQ(hist->ValidPixels(), 50640);
  ASSERT_NEAR(hist->StandardDeviation(), 85.2134, .0001);
}

TEST(IsisImportTests, LroNacR) {
  QTemporaryDir prefix;
  ASSERT_TRUE(prefix.isValid());

  QString cubeFileName = prefix.path() + "/lo2isisTEMP.cub";
  QVector<QString> args = {"from=data/lronac/nacr.img", "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest LO image: " <<e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions group
  ASSERT_EQ(cube.sampleCount(), 5064);
  ASSERT_EQ(cube.lineCount(), 10);
  ASSERT_EQ(cube.bandCount(), 1);

  // Pixels group
  ASSERT_EQ(PixelTypeName(cube.pixelType()), "Real");
  ASSERT_EQ(ByteOrderName(cube.byteOrder()), "Lsb");
  ASSERT_DOUBLE_EQ(cube.base(), 0.0);
  ASSERT_DOUBLE_EQ(cube.multiplier(), 1.0);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["SpacecraftName"][0].toStdString(), "LUNAR RECONNAISSANCE ORBITER");
  ASSERT_EQ(inst["InstrumentId"][0].toStdString(), "NACR");
  ASSERT_EQ(inst["TargetName"][0].toStdString(), "MOON");
  ASSERT_EQ(inst["StartTime"][0].toStdString(), "2009-11-29T14:51:21.968000");

  ASSERT_EQ(inst["TemperatureSCS"][0].toStdString(), "3.88");
  ASSERT_EQ(inst["TemperatureFPA"][0].toStdString(), "17.67");
  ASSERT_EQ(inst["TemperatureFPGA"][0].toStdString(), "-11.38");
  ASSERT_EQ(inst["TemperatureTelescope"][0].toStdString(), "11.14");
  ASSERT_EQ(inst["SpatialSumming"][0].toStdString(), "1");

  ASSERT_EQ(inst["TemperatureSCSRaw"][0].toStdString(), "2770");
  ASSERT_EQ(inst["TemperatureFPARaw"][0].toStdString(), "2118");
  ASSERT_EQ(inst["TemperatureFPGARaw"][0].toStdString(), "3388");
  ASSERT_EQ(inst["TemperatureTelescopeRaw"][0].toStdString(), "2429");

  // Bandbin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  ASSERT_EQ(bandbin["FilterName"][0], "BroadBand");
  ASSERT_EQ(bandbin["Center"][0], "600");
  ASSERT_EQ(bandbin["Width"][0], "300");

  // Kernels Group
  PvlGroup &kern = isisLabel->findGroup("Kernels", Pvl::Traverse);
  ASSERT_EQ(int(kern["NaifFrameCode"]), -85610);

  std::unique_ptr<Histogram> hist (cube.histogram());

  ASSERT_NEAR(hist->Average(), 159.45262638230648, .00001);
  ASSERT_EQ(hist->Sum(), 8074681);
  ASSERT_EQ(hist->ValidPixels(), 50640);
  ASSERT_NEAR(hist->StandardDeviation(), 82.499865428882416, .0001);
}

TEST(IsisImportTests, LroLabelFail) {
  QTemporaryDir prefix;
  ASSERT_TRUE(prefix.isValid());

  QString badLabelPath = prefix.path() + "/badLabel.img";
  Pvl lab("data/lronac/nacr.img");
  PvlKeyword &bterm = lab.findKeyword("LRO:BTERM");
  bterm.setValue("fake");
  lab.write(badLabelPath);

  QString cubeFileName = prefix.path() + "/doesntMatter.cub";
  QVector<QString> args = {"from="+badLabelPath, "to="+cubeFileName};

  UserInterface options(APP_XML, args);
  try {
    isisimport(options);
    FAIL() << "Expected to throw exception";
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("The decompanding terms do not have the same dimensions"));
     }

}
*/
