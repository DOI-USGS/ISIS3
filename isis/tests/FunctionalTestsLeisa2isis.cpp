#include <QTemporaryDir>

#include "leisa2isis.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/leisa2isis.xml").expanded();

TEST(leisa2isisTest, leisa2isisTestDefault) {
   Pvl appLog;
   QTemporaryDir prefix;
   QString cubeFileName = prefix.path() + "/leisa2isisTEMP.cub";
   QVector<QString> args = {"from=data/leisa2isis/lsb_0034933739_0x53c_sci_1_cropped.fit", "to=" + cubeFileName };

  UserInterface options(APP_XML, args);
  try {
   leisa2isis(options, &appLog);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest LEISA image: " << e.toString().toStdString().c_str() << std::endl;
  }
  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions Group
  PvlGroup &dimensions = isisLabel->findGroup("Dimensions", Pvl::Traverse);
  ASSERT_EQ(int(dimensions["Samples"]), 256);
  ASSERT_EQ(int(dimensions["Lines"]), 3);
  ASSERT_EQ(int(dimensions["Bands"]), 25);

  // Pixels Group
  PvlGroup &pixels = isisLabel->findGroup("Pixels", Pvl::Traverse);
  ASSERT_EQ(pixels["Type"][0].toStdString(), "Real");
  ASSERT_EQ(pixels["ByteOrder"][0].toStdString(), "Lsb");
  ASSERT_EQ(double(pixels["Base"]), 0.0);
  ASSERT_EQ(double(pixels["Multiplier"]), 1.0);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  ASSERT_EQ(inst["SpacecraftName"][0].toStdString(), "NEW HORIZONS");
  ASSERT_EQ(inst["InstrumentId"][0].toStdString(), "LEISA" );
  ASSERT_EQ(inst["TargetName"][0].toStdString(), "EUROPA" );
  ASSERT_EQ(inst["SpacecraftClockStartCount"][0].toStdString(), "1/0034931099:00000" );
  ASSERT_DOUBLE_EQ(double(inst["ExposureDuration"]), 0.676);
  ASSERT_EQ(inst["StartTime"][0].toStdString(), "2007-02-28T01:13:01.3882781" );
  ASSERT_EQ(inst["StopTime"][0].toStdString(), "2007-02-28T01:17:12.388278" );
  ASSERT_DOUBLE_EQ(double(inst["FrameRate"]), 1.47929);
  ASSERT_EQ(inst["FrameRate"].unit().toStdString(), "Hz");

  // Archive Group
  PvlGroup &archive = isisLabel->findGroup("Archive", Pvl::Traverse);
  ASSERT_DOUBLE_EQ(double(archive["MidObservationTime"]), 225897372.0736388);
  ASSERT_EQ(archive["MidObservationTime"].unit().toStdString(), "s past J2000");
  ASSERT_DOUBLE_EQ(double(archive["ObservationDuration"]), 251.0);
  ASSERT_EQ(archive["Detector"][0].toStdString(), "LEISA" );
  ASSERT_EQ(archive["ScanType"][0].toStdString(), "LEISA" );

  // BandBin Group
  // Check size, first, 2 middle, and last values? Enough?
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  ASSERT_EQ(bandbin["Center"].size(), 256);
  ASSERT_EQ(bandbin["Width"].size(), 256);
  ASSERT_EQ(bandbin["OriginalBand"].size(), 256);

  ASSERT_DOUBLE_EQ(bandbin["Center"][0].toDouble(), 2.4892);
  ASSERT_DOUBLE_EQ(bandbin["Center"][64].toDouble(), 1.9784);
  ASSERT_DOUBLE_EQ(bandbin["Center"][128].toDouble(), 1.572);
  ASSERT_DOUBLE_EQ(bandbin["Center"][255].toDouble(), 2.0898);

  ASSERT_DOUBLE_EQ(bandbin["Width"][0].toDouble(), 0.011228);
  ASSERT_DOUBLE_EQ(bandbin["Width"][64].toDouble(), 0.008924);
  ASSERT_DOUBLE_EQ(bandbin["Width"][128].toDouble(), 0.007091);
  ASSERT_DOUBLE_EQ(bandbin["Width"][255].toDouble(), 0.004915);

  ASSERT_DOUBLE_EQ(bandbin["OriginalBand"][0].toDouble(), 1);
  ASSERT_DOUBLE_EQ(bandbin["OriginalBand"][64].toDouble(), 65);
  ASSERT_DOUBLE_EQ(bandbin["OriginalBand"][128].toDouble(), 129);
  ASSERT_DOUBLE_EQ(bandbin["OriginalBand"][255].toDouble(), 256);

  // Kernels Group
  PvlGroup &kernel = isisLabel->findGroup("Kernels", Pvl::Traverse);
  ASSERT_EQ(int(kernel["NaifFrameCode"]), -98901);
  ASSERT_EQ(kernel["NaifFrameCode"].unit().toStdString(), "SPICE ID");
}
