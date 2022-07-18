#include <QTextStream>
#include <QStringList>
#include <QTemporaryFile>

#include "camstats.h"
#include "CameraFixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"


using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/camstats.xml").expanded();

TEST_F(DefaultCube, FunctionalTestCamstatsDefaultParameters) {
  QVector<QString> args = {};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  camstats(testCube, options, &appLog);

  PvlGroup group = appLog.findGroup("User Parameters");
  EXPECT_DOUBLE_EQ((double) group.findKeyword("Linc"), 1.0);
  EXPECT_DOUBLE_EQ((double) group.findKeyword("Sinc"), 1.0);

  group = appLog.findGroup("Latitude");
  EXPECT_NEAR( (double) group.findKeyword("LatitudeMinimum"), 9.928647987, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LatitudeMaximum"), 10.434709753, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LatitudeAverage"), 10.181983206, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LatitudeStandardDeviation"), 0.110841027, 1e-8);

  group = appLog.findGroup("Longitude");
  EXPECT_NEAR( (double) group.findKeyword("LongitudeMinimum"), 255.645548718, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LongitudeMaximum"), 256.146069525, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LongitudeAverage"), 255.893904910, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LongitudeStandardDeviation"), 0.106583304, 1e-8);

  group = appLog.findGroup("SampleResolution");
  EXPECT_NEAR( (double) group.findKeyword("SampleResolutionMinimum"), 18.840683425, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("SampleResolutionMaximum"), 18.985953877, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("SampleResolutionAverage"), 18.908165593, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("SampleResolutionStandardDeviation"), 0.038060007, 1e-8);

  group = appLog.findGroup("LineResolution");
  EXPECT_NEAR( (double) group.findKeyword("LineResolutionMinimum"), 18.840683425, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LineResolutionMaximum"), 18.985953877, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LineResolutionAverage"), 18.908165593, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LineResolutionStandardDeviation"), 0.038060007, 1e-8);

  group = appLog.findGroup("Resolution");
  EXPECT_NEAR( (double) group.findKeyword("ResolutionMinimum"), 18.840683425, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("ResolutionMaximum"), 18.985953877, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("ResolutionAverage"), 18.908165593, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("ResolutionStandardDeviation"), 0.038060007, 1e-8);

  group = appLog.findGroup("ObliqueSampleResolution");
  EXPECT_NEAR( (double) group.findKeyword("ObliqueSampleResolutionMinimum"), 18.967781671350998, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("ObliqueSampleResolutionMaximum"), 21.179434547755999, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("ObliqueSampleResolutionAverage"), 19.550786846366002, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("ObliqueSampleResolutionStandardDeviation"), 0.21126188466418, 1e-8);

  group = appLog.findGroup("ObliqueLineResolution");
  EXPECT_NEAR( (double) group.findKeyword("ObliqueLineResolutionMinimum"), 18.967781671350998, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("ObliqueLineResolutionMaximum"), 21.179434547755999, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("ObliqueLineResolutionAverage"), 19.550786846366002, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("ObliqueLineResolutionStandardDeviation"), 0.21126188466418, 1e-8);

  group = appLog.findGroup("ObliqueResolution");
  EXPECT_NEAR( (double) group.findKeyword("ObliqueResolutionMinimum"), 18.967781671350998, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("ObliqueResolutionMaximum"), 21.179434547755999, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("ObliqueResolutionAverage"), 19.550786846366002, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("ObliqueResolutionStandardDeviation"), 0.21126188466418, 1e-8);

  group = appLog.findGroup("AspectRatio");
  EXPECT_DOUBLE_EQ( (double) group.findKeyword("AspectRatioMinimum"), 1.0);
  // Maximum spelled incorrectly to match misspelling in CameraStatistics.cpp
  EXPECT_DOUBLE_EQ( (double) group.findKeyword("AspectRatioMaximun"), 1.0);
  EXPECT_DOUBLE_EQ( (double) group.findKeyword("AspectRatioAverage"), 1.0);
  EXPECT_DOUBLE_EQ( (double) group.findKeyword("AspectRatioStandardDeviation"), 0.0);

  group = appLog.findGroup("PhaseAngle");
  EXPECT_NEAR( (double) group.findKeyword("PhaseMinimum"), 79.756143590, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("PhaseMaximum"), 81.304900313, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("PhaseAverage"), 80.529097153, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("PhaseStandardDeviation"), 0.444208612, 1e-8);

  group = appLog.findGroup("EmissionAngle");
  EXPECT_NEAR( (double) group.findKeyword("EmissionMinimum"), 6.5875955784639002, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("EmissionMaximum"), 26.933702102375999, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("EmissionAverage"), 14.577804851994999, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("EmissionStandardDeviation"), 1.9856896435092, 1e-8);

  group = appLog.findGroup("IncidenceAngle");
  EXPECT_NEAR( (double) group.findKeyword("IncidenceMinimum"), 53.332095294516002, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("IncidenceMaximum"), 73.850710962080996, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("IncidenceAverage"), 66.178552657137004, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("IncidenceStandardDeviation"), 1.7434735102028001, 1e-8);

  group = appLog.findGroup("LocalSolarTime");
  EXPECT_NEAR( (double) group.findKeyword("LocalSolarTimeMinimum"), 7.7698055422, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LocalSolarTimeMaximum"), 7.8031735959, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LocalSolarTimeAverage"), 7.7863626216, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LocalSolarTimeStandardDeviation"), 0.007105554, 1e-8);

  group = appLog.findGroup("LocalRadius");
  EXPECT_NEAR( (double) group.findKeyword("LocalRadiusMinimum"), 3410663.3374636001, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LocalRadiusMaximum"), 3413492.0662691998, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LocalRadiusAverage"), 3412205.8144924999, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("LocalRadiusStandardDeviation"), 648.5763070953, 1e-5);

  group = appLog.findGroup("NorthAzimuth");
  EXPECT_NEAR( (double) group.findKeyword("NorthAzimuthMinimum"), 312.299406589, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("NorthAzimuthMaximum"), 350.597812506, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("NorthAzimuthAverage"), 332.967661510, 1e-8);
  EXPECT_NEAR( (double) group.findKeyword("NorthAzimuthStandardDeviation"), 0.673831891, 1e-8);
}

TEST_F(DefaultCube, FunctionalTestCamstatsAttach) {
  QVector<QString> args = {"attach=true", "linc=100", "sinc=100"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  QString inPath = testCube->fileName();

  camstats(testCube, options, &appLog);

  testCube->open(inPath, "r");
  EXPECT_TRUE(testCube->hasTable("CameraStatistics"));
}

TEST_F(DefaultCube, FunctionalTestCamstatsFlat) {
  QTemporaryFile flatFile;
  flatFile.open();

  QVector<QString> args = {"to=" + flatFile.fileName(), "format=flat", "linc=100", "sinc=100"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  camstats(testCube, options, &appLog);

  int lineNumber = 0;
  QTextStream flatStream(&flatFile);
  while(!flatStream.atEnd()) {
    QString line = flatStream.readLine();
    QStringList fields = line.split(",");

    if(lineNumber == 0) {
      EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(1), "LatitudeMinimum");
      EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(2), "LatitudeMaximum");
    }
    else if(lineNumber == 1) {
      EXPECT_NEAR(fields.value(2).toDouble(), 10.434709753, 1e-8);
      EXPECT_NEAR(fields.value(1).toDouble(), 9.928647987, 1e-8);
    }
    lineNumber++;
  }
}
