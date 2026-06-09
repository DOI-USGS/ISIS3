/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "TempFixtures.h"
#include "Cube.h"
#include "Pvl.h"
#include "FileName.h"
#include "Histogram.h"
#include "LineManager.h"
#include "handmos.h"
#include "UserInterface.h"
#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/handmos.xml").expanded();

/**
 * Fixture class for handmos tests
 */
class HandmosTest : public TempTestingFiles {
protected:
  QString testDataDir;

  void SetUp() override {
    TempTestingFiles::SetUp();
    testDataDir = QString(_SOURCE_PREFIX) + "/data";
  }

  QString testDataPath(const QString &fileName) const {
    return testDataDir + "/" + fileName;
  }

  void createTestCube(const QString &filename, int samples, int lines, int bands, double fillValue = 1.0) {
    Cube cube;
    cube.setDimensions(samples, lines, bands);
    cube.create(filename);

    for (int band = 1; band <= bands; band++) {
      for (int line = 1; line <= lines; line++) {
        LineManager lineBuffer(cube);
        lineBuffer.SetLine(line, band);
        for (int i = 0; i < lineBuffer.size(); i++) {
          lineBuffer[i] = fillValue * band;
        }
        cube.write(lineBuffer);
      }
    }
    cube.close();
  }
};

TEST_F(HandmosTest, FunctionalTestHandmosOntop) {
  QString inputCube = tempDir.path() + "/isisTruth.cub";
  QString mosaicCube = tempDir.path() + "/handmosTruth1.cub";
  QString trackingCube = tempDir.path() + "/handmosTruth1_tracking.cub";

  createTestCube(inputCube, 128, 128, 1, 100.0);

  QVector<QString> args = {
    "from=" + inputCube,
    "mosaic=" + mosaicCube,
    "priority=ontop",
    "outsample=1",
    "outline=2",
    "outband=1",
    "create=yes",
    "track=yes",
    "propagate=false",
    "nsamples=10",
    "nlines=20",
    "nbands=1"
  };

  UserInterface ui(APP_XML, args);
  handmos(ui);

  EXPECT_TRUE(QFile::exists(mosaicCube));

  Cube output(mosaicCube);
  EXPECT_EQ(output.sampleCount(), 10);
  EXPECT_EQ(output.lineCount(), 20);
  EXPECT_EQ(output.bandCount(), 1);
  output.close();

  EXPECT_TRUE(QFile::exists(trackingCube));
}

TEST_F(HandmosTest, FunctionalTestHandmosBeneath) {
  QString inputCube = tempDir.path() + "/isisTruth.cub";
  QString mosaicCube = tempDir.path() + "/handmosTruth2.cub";

  createTestCube(inputCube, 128, 128, 5, 50.0);

  QVector<QString> args = {
    "from=" + inputCube,
    "mosaic=" + mosaicCube,
    "priority=beneath",
    "insample=1",
    "inline=1",
    "inband=1",
    "outsample=1",
    "outline=2",
    "outband=2",
    "nlines=10",
    "nsamples=10",
    "nbands=5",
    "track=no",
    "propagate=false",
    "create=yes"
  };

  UserInterface ui(APP_XML, args);
  handmos(ui);

  EXPECT_TRUE(QFile::exists(mosaicCube));

  Cube output(mosaicCube);
  EXPECT_EQ(output.sampleCount(), 10);
  EXPECT_EQ(output.lineCount(), 10);
  EXPECT_EQ(output.bandCount(), 5);
  output.close();
}

TEST_F(HandmosTest, FunctionalTestHandmosBand) {
  QString inputCube = testDataPath("equalizer/MVA_2B2_01_02362S119E3542.lev2.cub");
  QString mosaicCube = tempDir.path() + "/handmosTruth2.cub";

  if (!QFile::exists(inputCube)) {
    GTEST_SKIP() << "Test data not available";
  }

  QVector<QString> args = {
    "from=" + inputCube,
    "mosaic=" + mosaicCube,
    "priority=BAND",
    "insample=1",
    "inline=1",
    "inband=1",
    "outsample=1",
    "outline=1",
    "outband=1",
    "nlines=400",
    "nsamples=895",
    "nbands=5",
    "create=yes",
    "propagate=false",
    "track=true",
    "type=KEYWORD",
    "keyname=FilterName",
    "keyvalue=MV1",
    "criteria=LESSER",
    "highsaturation=false",
    "lowsaturation=false",
    "null=false"
  };

  UserInterface ui(APP_XML, args);
  handmos(ui);

  EXPECT_TRUE(QFile::exists(mosaicCube));

  Cube output(mosaicCube);
  EXPECT_EQ(output.sampleCount(), 895);
  EXPECT_EQ(output.lineCount(), 400);
  EXPECT_EQ(output.bandCount(), 5);
  output.close();
}

TEST_F(HandmosTest, FunctionalTestHandmosAverage) {
  QString inputCube = tempDir.path() + "/isisTruth.cub";
  QString mosaicCube = tempDir.path() + "/handmosTruth1.cub";

  createTestCube(inputCube, 128, 128, 1, 100.0);

  QVector<QString> args1 = {
    "from=" + inputCube,
    "mosaic=" + mosaicCube,
    "priority=average",
    "outsample=1",
    "outline=2",
    "outband=1",
    "create=yes",
    "propagate=false",
    "nsamples=10",
    "nlines=20",
    "nbands=1"
  };

  UserInterface ui(APP_XML, args1);
  handmos(ui);

  EXPECT_TRUE(QFile::exists(mosaicCube));

  Cube output(mosaicCube);
  EXPECT_EQ(output.sampleCount(), 10);
  EXPECT_EQ(output.lineCount(), 20);
  EXPECT_EQ(output.bandCount(), 2);  // Average creates 2x bands
  output.close();
}

TEST_F(HandmosTest, FunctionalTestHandmosOverlay) {
  // Create base mosaic 800x800 with DN=100
  QString baseCube = tempDir.path() + "/base_mosaic.cub";
  createTestCube(baseCube, 800, 800, 1, 100.0);

  // Create overlay 400x400 with DN=200
  QString overlayCube = tempDir.path() + "/overlay.cub";
  createTestCube(overlayCube, 400, 400, 1, 200.0);

  // Overlay at position (300,300), testing:
  // - create=no (preserves existing mosaic)
  // - Non-origin positioning
  // - Correct overlay placement
  QVector<QString> args = {
    "from=" + overlayCube,
    "mosaic=" + baseCube,
    "priority=ontop",
    "outsample=300",
    "outline=300",
    "outband=1",
    "create=no"
  };

  UserInterface ui(APP_XML, args);
  handmos(ui);

  EXPECT_TRUE(QFile::exists(baseCube));

  Cube output(baseCube);

  // Base dimensions should be preserved
  EXPECT_EQ(output.sampleCount(), 800);
  EXPECT_EQ(output.lineCount(), 800);
  EXPECT_EQ(output.bandCount(), 1);

  // Validate histogram shows mixed DN values
  std::unique_ptr<Histogram> hist(output.histogram());
  EXPECT_GT(hist->Average(), 100.0);  // Increased by overlay
  EXPECT_LT(hist->Average(), 200.0);  // But not fully replaced

  // Verify pixel values at specific locations
  LineManager line(output);

  line.SetLine(500, 1);
  output.read(line);
  EXPECT_NEAR(line[450], 200.0, 1.0);  // Sample 450 should be overlay DN

  line.SetLine(100, 1);
  output.read(line);
  EXPECT_NEAR(line[100], 100.0, 1.0);  // Should be original base DN

  line.SetLine(300, 1);
  output.read(line);
  EXPECT_NEAR(line[100], 100.0, 1.0);  // Base DN before overlay starts
  EXPECT_NEAR(line[500], 200.0, 1.0);  // Overlay DN in middle

  output.close();
}

TEST_F(HandmosTest, FunctionalTestHandmosApplog) {
  QString inputCube = testDataPath("equalizer/MVA_2B2_01_02362S119E3542.lev2.cub");
  QString mosaicCube = tempDir.path() + "/handmosTruth2.cub";

  if (!QFile::exists(inputCube)) {
    GTEST_SKIP() << "Test data not available";
  }

  QVector<QString> args = {
    "from=" + inputCube,
    "mosaic=" + mosaicCube,
    "priority=BAND",
    "insample=1",
    "inline=1",
    "inband=1",
    "outsample=1",
    "outline=1",
    "outband=1",
    "nlines=400",
    "nsamples=895",
    "nbands=5",
    "create=yes",
    "propagate=false",
    "track=true",
    "type=KEYWORD",
    "keyname=FilterName",
    "keyvalue=MV2",
    "criteria=LESSER",
    "highsaturation=false",
    "lowsaturation=false",
    "null=false"
  };

  UserInterface ui(APP_XML, args);
  handmos(ui);

  EXPECT_TRUE(QFile::exists(mosaicCube));

  // Verify mosaic and tracking cube were created successfully
  Cube output(mosaicCube);
  EXPECT_EQ(output.sampleCount(), 895);
  EXPECT_EQ(output.lineCount(), 400);
  EXPECT_EQ(output.bandCount(), 5);
  output.close();
}

TEST_F(HandmosTest, FunctionalTestHandmosMultipleInputsCreateMosaic) {
  // Test multi-step workflow: create mosaic from first input, then add second input
  // This validates that handmos correctly handles multiple source cubes added
  // sequentially to build up a mosaic, with proper handling of overlap regions

  QString input1 = tempDir.path() + "/input1.cub";
  QString input2 = tempDir.path() + "/input2.cub";
  QString mosaicCube = tempDir.path() + "/multi_input_mosaic.cub";

  // Create first input cube 100x100 with DN=100
  createTestCube(input1, 100, 100, 1, 100.0);

  // Create second input cube 100x100 with DN=200
  createTestCube(input2, 100, 100, 1, 200.0);

  // First handmos: create mosaic from input1 at position (10,10)
  QVector<QString> args1 = {
    "from=" + input1,
    "mosaic=" + mosaicCube,
    "priority=ontop",
    "outsample=10",
    "outline=10",
    "outband=1",
    "create=yes",
    "propagate=false",
    "nsamples=160",
    "nlines=160",
    "nbands=1"
  };

  UserInterface ui1(APP_XML, args1);
  handmos(ui1);

  EXPECT_TRUE(QFile::exists(mosaicCube));

  // Second handmos: add input2 to existing mosaic at position (60,60)
  // This creates an overlap region from (60,60) to (109,109)
  QVector<QString> args2 = {
    "from=" + input2,
    "mosaic=" + mosaicCube,
    "priority=ontop",
    "outsample=60",
    "outline=60",
    "outband=1",
    "create=no"
  };

  UserInterface ui2(APP_XML, args2);
  handmos(ui2);

  // Validate the final mosaic
  Cube output(mosaicCube);
  EXPECT_EQ(output.sampleCount(), 160);
  EXPECT_EQ(output.lineCount(), 160);
  EXPECT_EQ(output.bandCount(), 1);

  // Verify statistics: two 100x100 inputs with 50x50 overlap
  // Expected: 7.5k pixels at DN=100, 10k at DN=200, 8.1k NULL
  std::unique_ptr<Histogram> hist(output.histogram());
  EXPECT_EQ(hist->ValidPixels(), 17500);
  EXPECT_EQ(hist->NullPixels(), 8100);
  EXPECT_NEAR(hist->Average(), 157.14, 0.1);
  EXPECT_DOUBLE_EQ(hist->Minimum(), 100.0);
  EXPECT_DOUBLE_EQ(hist->Maximum(), 200.0);
  EXPECT_NEAR(hist->Sum(), 2750000.0, 1.0);

  LineManager line(output);

  // Input1-only region
  line.SetLine(30, 1);
  output.read(line);
  EXPECT_NEAR(line[30], 100.0, 1.0);

  // Input2-only region
  line.SetLine(130, 1);
  output.read(line);
  EXPECT_NEAR(line[130], 200.0, 1.0);

  // Overlap region (priority=ontop)
  line.SetLine(80, 1);
  output.read(line);
  EXPECT_NEAR(line[80], 200.0, 1.0);

  // NULL region (outside both inputs)
  line.SetLine(5, 1);
  output.read(line);
  EXPECT_TRUE(IsSpecial(line[5]));

  output.close();
}
