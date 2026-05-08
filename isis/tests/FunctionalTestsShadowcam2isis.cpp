#include <QTemporaryDir>
#include <QFile>

#include "shadowcam2isis.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"
#include "OriginalLabel.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace Isis;
using ::testing::HasSubstr;

static QString APP_XML = FileName("$ISISROOT/bin/xml/shadowcam2isis.xml").expanded();

TEST(Shadowcam2isis, Shadowcam2IsisTestDefault) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/Shadowcam2isisTEMP.cub";
  QVector<QString> args = {"from=data/shadowcam/M013049982SE.crop.cub",
                           "to=" + cubeFileName };

  UserInterface options(APP_XML, args);
  try {
    shadowcam2isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest shadowcam image: " << e.toString().toStdString().c_str() << std::endl;
  }
  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions Group
  EXPECT_EQ(cube.sampleCount(), 3144);
  EXPECT_EQ(cube.lineCount(), 1);
  EXPECT_EQ(cube.bandCount(), 1);

  // Pixels Group
  EXPECT_EQ(PixelTypeName(cube.pixelType()), "Real");
  EXPECT_EQ(ByteOrderName(cube.byteOrder()), "Lsb");
  EXPECT_DOUBLE_EQ(cube.base(), 0.0);
  EXPECT_DOUBLE_EQ(cube.multiplier(), 1.0);

  std::istringstream instString(R"(
  Group = Instrument
    SpacecraftName          = "Korea Pathfinder Lunar Orbiter"
    InstrumentId            = ShadowCam
    TargetName              = MOON
    LineRate                = 1.1987 <ms>
    LineRateCode            = 360
    PrerollLines            = 1024
    NumberOfKLines          = 83
    ExecutionTime           = 2023-01-03T00:07:41.000000Z
    ExecutionSpacecraftTime = 1200:1731688
    StartTimeOffset         = 2.0272244
    PrerollTime             = 2023-01-03T00:07:41.799755Z
    StartTime               = 2023-01-03T00:07:43.027224Z
    StopTime                = 2023-01-03T00:09:23.679666Z
    TemperatureFPAA         = 0.45
    TemperatureFPAB         = 4.95
    TemperatureFPGA         = -0.6
    TemperatureTelescope    = -2.34
    Bterm1                  = 0
    Bterm2                  = 8
    Bterm3                  = 25
    Bterm4                  = 59
    Bterm5                  = 128
    Xterm0                  = 0
    Xterm1                  = 32
    Xterm2                  = 136
    Xterm3                  = 544
    Xterm4                  = 2208
    ModeTest                = false
    TDIDirection            = A
    GainCh0                 = 48
    GainCh1                 = 48
    GainCh2                 = 48
    GainCh3                 = 38
    GainCh4                 = 38
    GainCh5                 = 38
    OffsetCh0               = 5
    OffsetCh1               = 0
    OffsetCh2               = 1
    OffsetCh3               = 8
    OffsetCh4               = 4
    OffsetCh5               = 1
    SampleFirstPixel        = 0
    BiasSpread              = 4059
    BiasSpreadCh0           = 4079
    BiasSpreadCh1           = 4079
    BiasSpreadCh2           = 4079
    BiasSpreadCh3           = 4079
    BiasSpreadCh4           = 4079
    BiasSpreadCh5           = 4079
  End_Group
  )");
  PvlGroup truthInstGroup;
  instString >> truthInstGroup;

  PvlGroup &instGroup = isisLabel->findGroup("Instrument", Pvl::Traverse);

  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, instGroup, truthInstGroup);


  std::istringstream archiveString(R"(
  Group = Archive
    ProducerId              = KPLO_SHADOWCAM_TEAM
    ProducerInstitutionName = "ARIZONA STATE UNIVERSITY"
    PDSIdentifier           = M013049982SE
    ProductVersion          = v1.1
    MissionPhase            = commissioning
    DataQualityId           = 68
    DQIFpaOutOfBounds       = false
    DQIUnderExposed         = true
    DQIMissingData          = false
    DQIMissingSpice         = false
    DQIUncalibratable       = false
    DQICorruptionDetected   = true
    CreationTime            = 2024-08-09T22:29:40.949674Z
  End_Group

  )");
  PvlGroup truthArchiveGroup;
  archiveString >> truthArchiveGroup;

  PvlGroup &archiveGroup = isisLabel->findGroup("Archive", Pvl::Traverse);

  EXPECT_PRED_FORMAT2(AssertPvlGroupEqual, archiveGroup, truthArchiveGroup);

  // BandBin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  EXPECT_EQ(bandbin["FilterName"][0], "BroadBand");
  EXPECT_DOUBLE_EQ(bandbin["Center"], 600.0);
  EXPECT_DOUBLE_EQ(bandbin["Width"], 300.0);

  // Kernels Group
  PvlGroup &kernel = isisLabel->findGroup("Kernels", Pvl::Traverse);
  EXPECT_EQ(int(kernel["NaifFrameCode"]), -155151);

  std::unique_ptr<Histogram> hist (cube.histogram());

  EXPECT_NEAR(hist->Average(), 498.63390585241729, .0001);
  EXPECT_DOUBLE_EQ(hist->Sum(), 1567705);
  EXPECT_EQ(hist->ValidPixels(), 3144);
  EXPECT_NEAR(hist->StandardDeviation(), 1212.7900885145461, .00001);
}

TEST(Shadowcam2isis, Shadowcam2IsisTestCalibrated) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/Shadowcam2isisTEMP.cub";
  QVector<QString> args = {"from=data/shadowcam/M013049982SC.crop.cub",
                           "to=" + cubeFileName };
  UserInterface options(APP_XML, args);

  try {
    shadowcam2isis(options);
    FAIL() << "Should throw an exception" << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("is calibrated, no need to run shadowcam2isis."));
  }
}