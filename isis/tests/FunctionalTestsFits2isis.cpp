#include <QTemporaryDir>

#include "fits2isis.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/fits2isis.xml").expanded();

TEST(Fits2Isis, Fits2IsisTestDefault) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/fits2isisTEMP.cub";
  QVector<QString> args = {"from=data/fits2isis/default.fits", "to=" + cubeFileName };

  UserInterface options(APP_XML, args);
  try {
   fits2isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest fits file: " << e.toString().toStdString().c_str() << std::endl;
  }
  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions Group
  ASSERT_EQ(cube.sampleCount(), 200);
  ASSERT_EQ(cube.lineCount(), 25);
  ASSERT_EQ(cube.bandCount(), 3);

    // Pixels group
  ASSERT_EQ(PixelTypeName(cube.pixelType()), "Real");
  ASSERT_EQ(ByteOrderName(cube.byteOrder()), "Lsb");
  ASSERT_DOUBLE_EQ(cube.base(), 0.0);
  ASSERT_DOUBLE_EQ(cube.multiplier(), 1.0);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["InstrumentId"][0].toStdString(), "WFPC2" );
  ASSERT_EQ(inst["StartTime"][0].toStdString(), "1999-02-20" );

  std::unique_ptr<Histogram> hist (cube.histogram());

  ASSERT_NEAR(hist->Average(), 0.811103, .000001);
  ASSERT_NEAR(hist->Sum(), 4055.5169, .0001);
  ASSERT_EQ(hist->ValidPixels(), 5000);
  ASSERT_NEAR(hist->StandardDeviation(), 20.1912, .0001);
}


TEST(Fits2Isis, Fits2IsisOrganizationBsq) {
  QTemporaryDir prefix;
  QString dAPP_XML = FileName("$ISISROOT/bin/xml/fits2isis.xml").expanded();
  QString cubeFileName = prefix.path() + "/fits2isisTEMP.cub";
  QVector<QString> args = {"from=data/fits2isis/organization.fits",
                           "to=" + cubeFileName,
                           "organization=bsq",
                           "imagenumber=1"};

  UserInterface options(dAPP_XML, args);
  try {
   fits2isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest fits file: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();
  // Dimensions Group
  ASSERT_EQ(cube.sampleCount(), 256);
  ASSERT_EQ(cube.lineCount(), 10);
  ASSERT_EQ(cube.bandCount(), 2);

    // Pixels group
  ASSERT_EQ(PixelTypeName(cube.pixelType()), "Real");
  ASSERT_EQ(ByteOrderName(cube.byteOrder()), "Lsb");
  ASSERT_DOUBLE_EQ(cube.base(), 0.0);
  ASSERT_DOUBLE_EQ(cube.multiplier(), 1.0);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["Target"][0].toStdString(), "JUPITER" );

  std::unique_ptr<Histogram> hist (cube.histogram());

  ASSERT_NEAR(hist->Average(), 2.45129, .00001);
  ASSERT_NEAR(hist->Sum(), 6275.2976, .0001);
  ASSERT_EQ(hist->ValidPixels(), 2560);
  ASSERT_NEAR(hist->StandardDeviation(), 0.0245064, .0000001);
}

TEST(Fits2Isis, Fits2IsisOrganizationBil) {
  QTemporaryDir prefix;
  QString dAPP_XML = FileName("$ISISROOT/bin/xml/fits2isis.xml").expanded();
  QString cubeFileName = prefix.path() + "/fits2isisTEMP.cub";
  QVector<QString> args = {"from=data/fits2isis/organization.fits",
                           "to=" + cubeFileName,
                           "organization=bsq",
                           "imagenumber=1"};

  UserInterface options(dAPP_XML, args);
  try {
   fits2isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest fits file: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions Group
  ASSERT_EQ(cube.sampleCount(), 256);
  ASSERT_EQ(cube.lineCount(), 10);
  ASSERT_EQ(cube.bandCount(), 2);

    // Pixels group
  ASSERT_EQ(PixelTypeName(cube.pixelType()), "Real");
  ASSERT_EQ(ByteOrderName(cube.byteOrder()), "Lsb");
  ASSERT_DOUBLE_EQ(cube.base(), 0.0);
  ASSERT_DOUBLE_EQ(cube.multiplier(), 1.0);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["Target"][0].toStdString(), "JUPITER" );

  std::unique_ptr<Histogram> hist (cube.histogram());

  ASSERT_NEAR(hist->Average(), 2.45129, .00001);
  ASSERT_NEAR(hist->Sum(), 6275.2976, .0001);
  ASSERT_EQ(hist->ValidPixels(), 2560);
  ASSERT_NEAR(hist->StandardDeviation(), 0.0245064, .0000001);
}
