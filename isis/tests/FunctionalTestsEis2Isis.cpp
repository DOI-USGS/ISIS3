#include <QTemporaryDir>
#include <QFile>

#include "eis2isis.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace Isis;


static QString APP_XML = FileName("$ISISROOT/bin/xml/eis2isis.xml").expanded();

TEST(Eis2Isis, Eis2IsisTestDefault) {

  /*
  To test proper run with just the main image and an associated times file
	  $(APPNAME) $(TSTARGS) from=$(INPUT)/simulated_clipper_eis_nac_rolling_shutter.xml \
	  to=$(OUTPUT)/simulated_clipper_eis_nac_rolling_shutter2.cub \
	  mainreadout=$(INPUT)/simulated_clipper_eis_nac_rolling_shutter_times.csv;
  */

  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/eis2isisTEMP.cub";
  QVector<QString> args = { "from=data/eis2isis/simulated_clipper_eis_nac_rolling_shutter.xml",
                            "to=" + cubeFileName,
                            "mainreadout=data/eis2isis/simulated_clipper_eis_nac_rolling_shutter_times.csv" };

  UserInterface options(APP_XML, args);

  try {
    eis2isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest image: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions Group
  EXPECT_EQ(cube.sampleCount(), 4000);
  EXPECT_EQ(cube.lineCount(), 2000);
  EXPECT_EQ(cube.bandCount(), 1);

    // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_EQ(inst["SpacecraftName"][0].toStdString(), "Clipper");
  EXPECT_EQ(inst["InstrumentId"][0].toStdString(), "EIS-NAC-RS" );
  EXPECT_EQ(inst["TargetName"][0].toStdString(), "Europa" );
  EXPECT_EQ(inst["StartTime"][0].toStdString(), "2025-01-01T00:00:00.000" );
  EXPECT_EQ(inst["JitterLineCoefficients"][0].toStdString(), "0.0");
  EXPECT_EQ(inst["JitterLineCoefficients"][1].toStdString(), "0.0");
  EXPECT_EQ(inst["JitterLineCoefficients"][2].toStdString(), "0.0");
  EXPECT_EQ(inst["JitterSampleCoefficients"][0].toStdString(), "0.0");
  EXPECT_EQ(inst["JitterSampleCoefficients"][1].toStdString(), "0.0");
  EXPECT_EQ(inst["JitterSampleCoefficients"][2].toStdString(), "0.0");
  
}
