#include "CSVReader.h"
#include "Cube.h"
#include "CubeFixtures.h"
#include "FileName.h"
#include "Histogram.h"
#include "IException.h"
#include "IString.h"
#include "LineManager.h"
#include "PvlGroup.h"
#include "SpecialPixel.h"
#include "Statistics.h"

#include "lineeq.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace Isis;
using ::testing::HasSubstr;

static QString APP_XML = FileName("$ISISROOT/bin/xml/lineeq.xml").expanded();

// All tests use generated cubes with deterministic DN patterns so the expected
// normalized values can be computed directly. lineeq normalizes each line by
// in * cubeAverage / smoothedLineAverage. With a boxcar size of 1 the line
// averages are not smoothed, so a cube whose DNs are constant within each line
// normalizes to a uniform cube equal to the cube-wide average of the line
// averages.

TEST_F(TempTestingFiles, FunctionalTestLineeqDnMath) {
  // Single band, 10 lines, DN = 10 * line (constant across samples).
  // Line averages: 10, 20, ..., 100. Cube average: 55.
  // With boxsize=1 every output pixel equals the cube average (55).
  QString inCubeFileName = tempDir.path() + "/in.cub";
  createLinearPatternCube(inCubeFileName, 10, 10, 1, 0.0, 10.0, 0.0, 0.0);

  QString outCubeFileName = tempDir.path() + "/out.cub";
  QVector<QString> args = {"from=" + inCubeFileName, "to=" + outCubeFileName,
                           "boxtype=absolute", "boxsize=1"};
  UserInterface ui(APP_XML, args);

  PvlGroup results;
  try {
    results = lineeq(ui);
  }
  catch (IException &e) {
    FAIL() << "Unable to run lineeq: " << e.what() << std::endl;
  }

  EXPECT_EQ(toInt(QString(results["BoxcarSize"])), 1);

  Cube oCube(outCubeFileName, "r");
  Statistics *stats = oCube.statistics();

  EXPECT_EQ(stats->ValidPixels(), 100);
  EXPECT_NEAR(stats->Average(), 55.0, 1e-9);
  EXPECT_NEAR(stats->Minimum(), 55.0, 1e-9);
  EXPECT_NEAR(stats->Maximum(), 55.0, 1e-9);
  EXPECT_NEAR(stats->StandardDeviation(), 0.0, 1e-9);

  delete stats;
}

TEST_F(TempTestingFiles, FunctionalTestLineeqMultiBand) {
  // Two bands, DN = 10 * line + 100 * band (constant across samples).
  // Band 1 line averages 110..200 -> cube average 155.
  // Band 2 line averages 210..300 -> cube average 255.
  // Each band normalizes (boxsize=1) to its own uniform cube average.
  QString inCubeFileName = tempDir.path() + "/in.cub";
  createLinearPatternCube(inCubeFileName, 10, 10, 2, 0.0, 10.0, 0.0, 100.0);

  QString outCubeFileName = tempDir.path() + "/out.cub";
  QVector<QString> args = {"from=" + inCubeFileName, "to=" + outCubeFileName,
                           "boxtype=absolute", "boxsize=1"};
  UserInterface ui(APP_XML, args);

  ASSERT_NO_THROW(lineeq(ui));

  Cube oCube(outCubeFileName, "r");

  Statistics *band1 = oCube.statistics(1);
  EXPECT_NEAR(band1->Average(), 155.0, 1e-9);
  EXPECT_NEAR(band1->StandardDeviation(), 0.0, 1e-9);
  delete band1;

  Statistics *band2 = oCube.statistics(2);
  EXPECT_NEAR(band2->Average(), 255.0, 1e-9);
  EXPECT_NEAR(band2->StandardDeviation(), 0.0, 1e-9);
  delete band2;
}

TEST_F(TempTestingFiles, FunctionalTestLineeqBoxcarSizeResolution) {
  // BoxcarSize is resolved from BOXTYPE/BOXSIZE and forced to be odd. The cube
  // has 10 lines so percentage and default sizes are easy to verify.
  QString inCubeFileName = tempDir.path() + "/in.cub";
  createLinearPatternCube(inCubeFileName, 10, 10, 1, 0.0, 10.0, 0.0, 0.0);

  auto boxcarSizeFor = [&](const QVector<QString> &settings) {
    // Use a unique output cube per invocation so repeated calls do not collide.
    QString suffix;
    for (const QString &setting : settings) {
      suffix += "_" + setting;
    }
    QString outCubeFileName = tempDir.path() + "/out" + suffix + ".cub";
    QVector<QString> args = {"from=" + inCubeFileName, "to=" + outCubeFileName};
    args += settings;
    UserInterface ui(APP_XML, args);
    PvlGroup results = lineeq(ui);
    return toInt(QString(results["BoxcarSize"]));
  };

  // NONE: 10% of 10 lines = 1 (already odd).
  EXPECT_EQ(boxcarSizeFor({"boxtype=none"}), 1);
  // ABSOLUTE odd value is used as-is.
  EXPECT_EQ(boxcarSizeFor({"boxtype=absolute", "boxsize=3"}), 3);
  // ABSOLUTE even value is rounded up to the next odd value.
  EXPECT_EQ(boxcarSizeFor({"boxtype=absolute", "boxsize=4"}), 5);
  // PERCENTAGE: 50% of 10 lines = 5.
  EXPECT_EQ(boxcarSizeFor({"boxtype=percentage", "boxsize=50"}), 5);
  // PERCENTAGE: 30% of 10 lines = 3.
  EXPECT_EQ(boxcarSizeFor({"boxtype=percentage", "boxsize=30"}), 3);
}

TEST_F(TempTestingFiles, FunctionalTestLineeqCsvOutput) {
  // AVERAGES=yes writes a CSV of the raw and smoothed line averages. With
  // boxsize=1 the smoothed average equals the raw average for every line.
  QString inCubeFileName = tempDir.path() + "/in.cub";
  createLinearPatternCube(inCubeFileName, 10, 10, 1, 0.0, 10.0, 0.0, 0.0);

  QString outCubeFileName = tempDir.path() + "/out.cub";
  QString csvFileName = tempDir.path() + "/averages.csv";
  QVector<QString> args = {"from=" + inCubeFileName, "to=" + outCubeFileName,
                           "boxtype=absolute", "boxsize=1",
                           "averages=yes", "csv=" + csvFileName};
  UserInterface ui(APP_XML, args);

  PvlGroup results;
  ASSERT_NO_THROW(results = lineeq(ui));
  EXPECT_EQ(toInt(QString(results["OutputCsv"])), 1);

  CSVReader csv(csvFileName, true, 0, ',', false, true);
  // One row per line of the single band.
  ASSERT_EQ(csv.rows(), 10);

  CSVReader::CSVAxis header = csv.getHeader();
  ASSERT_EQ(header.dim1(), 2);
  EXPECT_EQ(header[0], "Average");
  EXPECT_EQ(header[1], "SmoothedAvg");

  // Line averages run 10, 20, ..., 100 and are unchanged by a boxsize of 1.
  for (int line = 0; line < csv.rows(); line++) {
    CSVReader::CSVAxis row = csv.getRow(line);
    ASSERT_EQ(row.dim1(), 2);
    double expectedAverage = 10.0 * (line + 1);
    EXPECT_NEAR(toDouble(row[0]), expectedAverage, 1e-9);
    EXPECT_NEAR(toDouble(row[1]), expectedAverage, 1e-9);
  }
}

TEST_F(TempTestingFiles, FunctionalTestLineeqSpecialPixelPassthrough) {
  // Special pixels must pass through unchanged. Set the first line of an
  // otherwise valid cube to NULL; those pixels should remain NULL in the output
  // while the remaining valid lines are normalized.
  QString inCubeFileName = tempDir.path() + "/in.cub";
  createLinearPatternCube(inCubeFileName, 10, 10, 1, 0.0, 10.0, 0.0, 0.0);

  Cube inCube(inCubeFileName, "rw");
  LineManager line(inCube);
  line.SetLine(1, 1);
  for (int i = 0; i < line.size(); i++) {
    line[i] = NULL8;
  }
  inCube.write(line);
  inCube.close();

  QString outCubeFileName = tempDir.path() + "/out.cub";
  QVector<QString> args = {"from=" + inCubeFileName, "to=" + outCubeFileName,
                           "boxtype=absolute", "boxsize=1"};
  UserInterface ui(APP_XML, args);

  ASSERT_NO_THROW(lineeq(ui));

  Cube oCube(outCubeFileName, "r");

  // The NULL line passes through, so the output has exactly one NULL line.
  Histogram *hist = oCube.histogram();
  EXPECT_EQ(hist->NullPixels(), 10);
  EXPECT_EQ(hist->ValidPixels(), 90);
  delete hist;

  // The first output line should still be entirely NULL.
  LineManager outLine(oCube);
  outLine.SetLine(1, 1);
  oCube.read(outLine);
  for (int i = 0; i < outLine.size(); i++) {
    EXPECT_TRUE(IsSpecial(outLine[i]));
  }
}

TEST_F(NullPixelCube, FunctionalTestLineeqAllSpecialError) {
  // A cube with no valid data cannot be normalized and must raise a User error.
  testCube->reopen("r");

  QString outCubeFileName = tempDir.path() + "/out.cub";
  QVector<QString> args = {"from=" + testCube->fileName(), "to=" + outCubeFileName,
                           "boxtype=absolute", "boxsize=1"};
  UserInterface ui(APP_XML, args);

  try {
    lineeq(ui);
    FAIL() << "Expected an IException for an all-special cube." << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), HasSubstr("does not contain any valid data"));
  }
}
