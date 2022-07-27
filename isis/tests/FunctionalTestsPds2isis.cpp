#include <QTemporaryDir>

#include "pds2isis.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/pds2isis.xml").expanded();

TEST(Pds2Isis, Pds2isisTestDefault) {
  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/pds2isis_out.cub";
  QVector<QString> args = { "from=data/pds2isis/ab102401_cropped.img",
                            "to=" + cubeFileName };
  UserInterface options(APP_XML, args);
  try {
    pds2isis(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest file: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(cubeFileName);
  Pvl *outLabel = outCube.label();

  PvlGroup dimensions = outLabel->findGroup("Dimensions", Pvl::Traverse);
  ASSERT_EQ((int)dimensions["Samples"], 640);
  ASSERT_EQ((int)dimensions["Lines"], 5);
  ASSERT_EQ((int)dimensions["Bands"], 1);

  PvlGroup pixels = outLabel->findGroup("Pixels", Pvl::Traverse);
  ASSERT_EQ(pixels["Type"][0].toStdString(), "UnsignedByte");
  ASSERT_EQ(pixels["ByteOrder"][0].toStdString(), "Lsb");
  ASSERT_EQ((double)pixels["Base"], 0.0);
  ASSERT_EQ((double)pixels["Multiplier"], 1.0);

  PvlGroup archive = outLabel->findGroup("Archive", Pvl::Traverse);
  ASSERT_EQ(archive["DataSetId"][0].toStdString(), "MGS-M-MOC-NA/WA-2-DSDP-L0-V1.0");
  ASSERT_EQ(archive["ProducerId"][0].toStdString(), "MGS_MOC_TEAM");
  ASSERT_EQ(archive["ProductId"][0].toStdString(), "AB-1-024/01");
  ASSERT_EQ(archive["InstrumentId"][0].toStdString(), "MOC-WA");
  ASSERT_EQ(archive["TargetName"][0].toStdString(), "MARS");
  ASSERT_EQ(archive["MissionPhaseName"][0].toStdString(), "AB-1");
  ASSERT_EQ(archive["RationaleDescription"][0].toStdString(), "OLYMPUS MONS SPECIAL RED WIDE ANGLE");


  std::unique_ptr<Histogram> hist (outCube.histogram());
  ASSERT_NEAR(hist->Average(), 81.5828125, .00001);
  ASSERT_EQ(hist->Sum(), 261065);
  ASSERT_EQ(hist->ValidPixels(), 3200);
  ASSERT_NEAR(hist->StandardDeviation(), 30.5674, .0001);
}


TEST(Pds2Isis, Pds2isisTestBandBin) {
  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/pds2isis_out.cub";
  QVector<QString> args = { "from=data/pds2isis/gaspra_nims_hires_radiance_cropped.lbl",
                            "to=" + cubeFileName };
  UserInterface options(APP_XML, args);

  try {
    pds2isis(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest file: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(cubeFileName);
  Pvl *outLabel = outCube.label();

  PvlGroup dimensions = outLabel->findGroup("Dimensions", Pvl::Traverse);
  ASSERT_EQ((int)dimensions["Samples"], 100);
  ASSERT_EQ((int)dimensions["Lines"], 3);
  ASSERT_EQ((int)dimensions["Bands"], 3);

  bool ok = false;
  PvlGroup bandbin = outLabel->findGroup("BandBin", Pvl::Traverse);
  ASSERT_EQ(bandbin["Center"][0].toDouble(&ok),  0.7360);
  ASSERT_EQ(bandbin["Center"][1].toDouble(&ok),  0.8750);
  ASSERT_EQ(bandbin["Center"][2].toDouble(&ok),  1.0546);
  ASSERT_EQ(bandbin["Center"][3].toDouble(&ok),  1.3326);
  ASSERT_EQ(bandbin["Center"][4].toDouble(&ok),  1.6110);
  ASSERT_EQ(bandbin["Center"][5].toDouble(&ok),  1.8914);
  ASSERT_EQ(bandbin["Center"][6].toDouble(&ok),  2.1728);
  ASSERT_EQ(bandbin["Center"][7].toDouble(&ok),  2.4529);
  ASSERT_EQ(bandbin["Center"][8].toDouble(&ok),  2.7382);
  ASSERT_EQ(bandbin["Center"][9].toDouble(&ok),  3.0178);
  ASSERT_EQ(bandbin["Center"][10].toDouble(&ok), 3.3012);
  ASSERT_EQ(bandbin["Center"][11].toDouble(&ok), 3.5839);
  ASSERT_EQ(bandbin["Center"][12].toDouble(&ok), 3.8670);
  ASSERT_EQ(bandbin["Center"][13].toDouble(&ok), 4.1503);
  ASSERT_EQ(bandbin["Center"][14].toDouble(&ok), 4.4343);
  ASSERT_EQ(bandbin["Center"][15].toDouble(&ok), 4.7164);
  ASSERT_EQ(bandbin["Center"][16].toDouble(&ok), 4.9988);

  ASSERT_EQ(bandbin["Width"][0].toDouble(&ok),  0.0125);
  ASSERT_EQ(bandbin["Width"][1].toDouble(&ok),  0.0125);
  ASSERT_EQ(bandbin["Width"][2].toDouble(&ok),  0.0250);
  ASSERT_EQ(bandbin["Width"][3].toDouble(&ok),  0.0250);
  ASSERT_EQ(bandbin["Width"][4].toDouble(&ok),  0.0250);
  ASSERT_EQ(bandbin["Width"][5].toDouble(&ok),  0.0250);
  ASSERT_EQ(bandbin["Width"][6].toDouble(&ok),  0.0250);
  ASSERT_EQ(bandbin["Width"][7].toDouble(&ok),  0.0250);
  ASSERT_EQ(bandbin["Width"][8].toDouble(&ok),  0.0250);
  ASSERT_EQ(bandbin["Width"][9].toDouble(&ok),  0.0250);
  ASSERT_EQ(bandbin["Width"][10].toDouble(&ok), 0.0250);
  ASSERT_EQ(bandbin["Width"][11].toDouble(&ok), 0.0250);
  ASSERT_EQ(bandbin["Width"][12].toDouble(&ok), 0.0250);
  ASSERT_EQ(bandbin["Width"][13].toDouble(&ok), 0.0250);
  ASSERT_EQ(bandbin["Width"][14].toDouble(&ok), 0.0250);
  ASSERT_EQ(bandbin["Width"][15].toDouble(&ok), 0.0250);
  ASSERT_EQ(bandbin["Width"][16].toDouble(&ok), 0.0250);

  ASSERT_EQ(bandbin["FilterNumber"][0].toInt(&ok, 10),  1);
  ASSERT_EQ(bandbin["FilterNumber"][1].toInt(&ok, 10),  2);
  ASSERT_EQ(bandbin["FilterNumber"][2].toInt(&ok, 10),  3);
  ASSERT_EQ(bandbin["FilterNumber"][3].toInt(&ok, 10),  4);
  ASSERT_EQ(bandbin["FilterNumber"][4].toInt(&ok, 10),  5);
  ASSERT_EQ(bandbin["FilterNumber"][5].toInt(&ok, 10),  6);
  ASSERT_EQ(bandbin["FilterNumber"][6].toInt(&ok, 10),  7);
  ASSERT_EQ(bandbin["FilterNumber"][7].toInt(&ok, 10),  8);
  ASSERT_EQ(bandbin["FilterNumber"][8].toInt(&ok, 10),  9);
  ASSERT_EQ(bandbin["FilterNumber"][9].toInt(&ok, 10),  10);
  ASSERT_EQ(bandbin["FilterNumber"][10].toInt(&ok, 10), 11);
  ASSERT_EQ(bandbin["FilterNumber"][11].toInt(&ok, 10), 12);
  ASSERT_EQ(bandbin["FilterNumber"][12].toInt(&ok, 10), 13);
  ASSERT_EQ(bandbin["FilterNumber"][13].toInt(&ok, 10), 14);
  ASSERT_EQ(bandbin["FilterNumber"][14].toInt(&ok, 10), 15);
  ASSERT_EQ(bandbin["FilterNumber"][15].toInt(&ok, 10), 16);
  ASSERT_EQ(bandbin["FilterNumber"][16].toInt(&ok, 10), 17);

  std::unique_ptr<Histogram> hist (outCube.histogram(0));
  ASSERT_NEAR(hist->Average(), 0.205984, 1e-3);
  ASSERT_NEAR(hist->Sum(), 185.386, 1e-3);
  ASSERT_EQ(hist->ValidPixels(), 900);
  ASSERT_NEAR(hist->StandardDeviation(), 0.606295, 1e-3);
}


TEST(Pds2Isis, Pds2isisTestOffset) {
  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/pds2isis_out.cub";
  QVector<QString> args = { "from=data/pds2isis/ldem_4_cropped.img",
                            "to=" + cubeFileName };
  UserInterface options(APP_XML, args);

  try {
    pds2isis(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest file: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(cubeFileName);
  Pvl *outLabel = outCube.label();

  PvlGroup mapping = outLabel->findGroup("Mapping", Pvl::Traverse);
  ASSERT_EQ(mapping["ProjectionName"][0].toStdString(), "SimpleCylindrical");
  ASSERT_EQ((double)mapping["CenterLongitude"], 180.0);
  ASSERT_EQ(mapping["TargetName"][0].toStdString(), "Moon");
  ASSERT_EQ((double)mapping["EquatorialRadius"], 1737400.0);
  ASSERT_EQ((double)mapping["PolarRadius"], 1737400.0);
  ASSERT_EQ(mapping["LatitudeType"][0].toStdString(), "Planetocentric");
  ASSERT_EQ(mapping["LongitudeDirection"][0].toStdString(), "PositiveEast");
  ASSERT_EQ((int)mapping["LongitudeDomain"], 360);
  ASSERT_EQ((double)mapping["MinimumLatitude"], -90.0);
  ASSERT_EQ((double)mapping["MaximumLatitude"], 90.0);
  ASSERT_EQ((double)mapping["MinimumLongitude"], 0.0);
  ASSERT_EQ((double)mapping["MaximumLongitude"], 360.0);
  ASSERT_EQ((double)mapping["UpperLeftCornerX"], -5458204.8);
  ASSERT_EQ((double)mapping["UpperLeftCornerY"], 2729102.4);
  ASSERT_EQ((double)mapping["PixelResolution"], 7580.84);
  ASSERT_EQ((double)mapping["Scale"], 4.0);

  std::unique_ptr<Histogram> hist (outCube.histogram());
  ASSERT_NEAR(hist->Average(), 1.7375e+06, 10);
  ASSERT_NEAR(hist->Sum(), 2.50026e+09, 1e3);
  ASSERT_EQ(hist->ValidPixels(), 1439);
  ASSERT_NEAR(hist->StandardDeviation(), 9187.96, .0001);
}

TEST(Pds2Isis, Pds2isisTestProjection) {
  Pvl appLog;
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/pds2isis_out.cub";
  QVector<QString> args = { "from=data/pds2isis/ff17_cropped.lbl",
                            "to=" + cubeFileName };
  UserInterface options(APP_XML, args);

  try {
    pds2isis(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest file: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(cubeFileName);
  Pvl *outLabel = outCube.label();

  PvlGroup mapping = outLabel->findGroup("Mapping", Pvl::Traverse);
  ASSERT_EQ(mapping["ProjectionName"][0].toStdString(), "Sinusoidal");
  ASSERT_EQ((double)mapping["CenterLongitude"], 325.3829);
  ASSERT_EQ(mapping["TargetName"][0].toStdString(), "Venus");
  ASSERT_EQ((double)mapping["EquatorialRadius"], 6051920.0);
  ASSERT_EQ((double)mapping["PolarRadius"], 6051920.0);
  ASSERT_EQ(mapping["LatitudeType"][0].toStdString(), "Planetocentric");
  ASSERT_EQ(mapping["LongitudeDirection"][0].toStdString(), "PositiveEast");
  ASSERT_EQ((int)mapping["LongitudeDomain"], 360);
  ASSERT_EQ((double)mapping["MinimumLatitude"], -29.6357);
  ASSERT_EQ((double)mapping["MaximumLatitude"], -28.9092);
  ASSERT_EQ((double)mapping["MinimumLongitude"], 322.0367);
  ASSERT_EQ((double)mapping["MaximumLongitude"], 322.8903);
  ASSERT_EQ((double)mapping["UpperLeftCornerX"], -307162.5);
  ASSERT_EQ((double)mapping["UpperLeftCornerY"], -3053025.0);
  ASSERT_EQ((double)mapping["PixelResolution"], 75.0);
  ASSERT_EQ((double)mapping["Scale"], 1407.4);

  std::unique_ptr<Histogram> hist (outCube.histogram());
  ASSERT_NEAR(hist->Average(), 67.7978515625, .00001);
  ASSERT_EQ(hist->Sum(), 69425);
  ASSERT_EQ(hist->ValidPixels(), 1024);
  ASSERT_NEAR(hist->StandardDeviation(), 26.0079, .0001);
}

TEST(Pds2Isis, Pds2isisTestSpecialPixels) {
  Pvl appLog;
  QTemporaryDir prefix;
  QVector<QString> args;
  QString cubeFileName = prefix.path() + "/pds2isis_out.cub";

  // test setnullrange
  args = { "from=data/pds2isis/ab102401_cropped.img",
           "to=" + cubeFileName,
           "setnullrange=yes", "nullmin=15.0", "nullmax=45.0" };
  UserInterface options_nullrange(APP_XML, args);
  try {
    pds2isis(options_nullrange, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Failed for setnullrange=yes on file: " << e.toString().toStdString().c_str() << std::endl;
  }

  // test sethrsrange
  args = { "from=data/pds2isis/ab102401_cropped.img",
           "to=" + cubeFileName,
           "sethrsrange=yes", "hrsmin=220.0", "hrsmax=250.0"};
  UserInterface options_hrsrange(APP_XML, args);
  try {
    pds2isis(options_hrsrange, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Failed for sethrsrange=yes on file: " << e.toString().toStdString().c_str() << std::endl;
  }

  // test sethisrange
  args = { "from=data/pds2isis/ab102401_cropped.img",
           "to=" + cubeFileName,
           "sethisrange=yes", "hismin=190.0", "hismax=219.0"};
  UserInterface options_hisrange(APP_XML, args);
  try {
    pds2isis(options_hisrange, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Failed for sethisrange=yes on file: " << e.toString().toStdString().c_str() << std::endl;
  }

  // test setlrsrange
  args = { "from=data/pds2isis/ab102401_cropped.img",
           "to=" + cubeFileName,
           "setlrsrange=yes", "lrsmin=96.0", "lrsmax=125.0"};
  UserInterface options_lrsrange(APP_XML, args);
  try {
    pds2isis(options_lrsrange, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Failed for setlrsrange=yes on file: " << e.toString().toStdString().c_str() << std::endl;
  }

  // test setlisrange
  args = { "from=data/pds2isis/ab102401_cropped.img",
           "to=" + cubeFileName,
           "setlisrange=yes", "lismin=65.0", "lismax=95.0"};
  UserInterface options_lisrange(APP_XML, args);
  try {
    pds2isis(options_lisrange, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Failed for setlisrange=yes on file: " << e.toString().toStdString().c_str() << std::endl;
  }
}

TEST(Pds2Isis, Pds2isisTestBIL) {
  Pvl appLog;
  QTemporaryDir prefix;
  QVector<QString> args;
  QString cubeFileName = prefix.path() + "/pds2isis_BIL_out.cub";

  args = { "from=data/pds2isis/BILtestData_cropped.LBL",
           "to=" + cubeFileName};
  UserInterface options(APP_XML, args);
  try {
    pds2isis(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Failed to ingest file: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(cubeFileName);
  Pvl *outLabel = outCube.label();

  PvlGroup dimensions = outLabel->findGroup("Dimensions", Pvl::Traverse);
  ASSERT_EQ((int)dimensions["Lines"], 1);
  ASSERT_EQ((int)dimensions["Samples"], 304);
  ASSERT_EQ((int)dimensions["Bands"], 1);

  std::unique_ptr<Histogram> hist (outCube.histogram());
  ASSERT_NEAR(hist->Average(), 6.35692e+31, 1e25);
  ASSERT_NEAR(hist->Sum(), 1.9325e+34, 1e29);
  ASSERT_EQ(hist->ValidPixels(), 304);
  ASSERT_NEAR(hist->StandardDeviation(), 1.08618e+33, 1e28);
}

TEST(Pds2Isis, Pds2isisTestBIP) {
  Pvl appLog;
  QTemporaryDir prefix;
  QVector<QString> args;
  QString cubeFileName = prefix.path() + "/pds2isis_BIP_out.cub";

  args = { "from=data/pds2isis/BIPtestData_cropped.LBL",
           "to=" + cubeFileName};
  UserInterface options(APP_XML, args);
  try {
    pds2isis(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Failed to ingest file: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(cubeFileName);
  Pvl *outLabel = outCube.label();

  PvlGroup dimensions = outLabel->findGroup("Dimensions", Pvl::Traverse);
  ASSERT_EQ((int)dimensions["Lines"], 1);
  ASSERT_EQ((int)dimensions["Samples"], 304);
  ASSERT_EQ((int)dimensions["Bands"], 1);

  std::unique_ptr<Histogram> hist (outCube.histogram());
  ASSERT_NEAR(hist->Average(), 6.35692e+31, 1e25);
  ASSERT_NEAR(hist->Sum(), 1.9325e+34, 1e29);
  ASSERT_EQ(hist->ValidPixels(), 304);
  ASSERT_NEAR(hist->StandardDeviation(), 1.08618e+33, 1e28);
}

TEST(Pds2Isis, Pds2isisTestNIMSQub) {
  Pvl appLog;
  QTemporaryDir prefix;
  QVector<QString> args;
  QString cubeFileName = prefix.path() + "/pds2isis_QUB_out.cub";

  args = { "from=data/pds2isis/30i001ci_cropped.qub",
           "to=" + cubeFileName};
  UserInterface options(APP_XML, args);
  try {
    pds2isis(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Failed to ingest file: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(cubeFileName);
  Pvl *outLabel = outCube.label();

  PvlGroup dimensions = outLabel->findGroup("Dimensions", Pvl::Traverse);
  ASSERT_EQ((int)dimensions["Lines"], 46);
  ASSERT_EQ((int)dimensions["Samples"], 1);
  ASSERT_EQ((int)dimensions["Bands"], 12);

  std::unique_ptr<Histogram> hist (outCube.histogram());
  ASSERT_NEAR(hist->Average(), 1.64693e+30, 1e25);
  ASSERT_NEAR(hist->Sum(), 7.57588e+31, 1e26);
  ASSERT_EQ(hist->ValidPixels(), 46);
  ASSERT_NEAR(hist->StandardDeviation(), 1.117e+31, 1e26);
}
