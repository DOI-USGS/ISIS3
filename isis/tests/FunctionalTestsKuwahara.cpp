#include <QString>
#include <QVector>

#include "Brick.h"
#include "Cube.h"
#include "CubeFixtures.h"
#include "FileName.h"
#include "IException.h"
#include "LineManager.h"
#include "Statistics.h"
#include "TempFixtures.h"
#include "kuwahara.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/kuwahara.xml").expanded();

/**
 * Read a single pixel (sample, line, band) from a cube.
 */
static double readPixel(Cube &cube, int sample, int line, int band = 1) {
  Brick b(1, 1, 1, cube.pixelType());
  b.SetBasePosition(sample, line, band);
  cube.read(b);
  return b[0];
}

/**
 * KuwaharaConstant
 *
 * Feature: a uniform region must be left unchanged by the filter, and the
 * output dimensions/bands must match the input.
 *
 * A 10x10 constant cube (DN = 100) is filtered with the default 5x5 boxcar.
 * Every quadrant has zero variance and an average of 100, so the output is
 * 100 everywhere. This also covers dimension and band preservation.
 */
TEST_F(TempTestingFiles, FunctionalTestKuwaharaConstant) {
  QString inPath = tempDir.path() + "/constant.cub";
  createLinearPatternCube(inPath, 10, 10, 1, /*base*/100.0, 0.0, 0.0, 0.0);

  QString outPath = tempDir.path() + "/constantOut.cub";
  QVector<QString> args = {"from=" + inPath, "to=" + outPath};
  UserInterface ui(APP_XML, args);

  try {
    kuwahara(ui);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(outPath);
  EXPECT_EQ(outCube.sampleCount(), 10);
  EXPECT_EQ(outCube.lineCount(), 10);
  EXPECT_EQ(outCube.bandCount(), 1);

  std::unique_ptr<Statistics> stats(outCube.statistics());
  EXPECT_EQ(stats->ValidPixels(), 100);
  EXPECT_DOUBLE_EQ(stats->Minimum(), 100.0);
  EXPECT_DOUBLE_EQ(stats->Maximum(), 100.0);
  EXPECT_DOUBLE_EQ(stats->Average(), 100.0);
  EXPECT_DOUBLE_EQ(stats->StandardDeviation(), 0.0);
}

/**
 * KuwaharaColumnRamp
 *
 * Feature: the filter outputs the mean of the minimum-variance quadrant, and
 * the documented tie-break (first quadrant encountered wins) is honored.
 *
 * Input DN = sample (constant per column). With a 3x3 boxcar each pixel's four
 * overlapping 2x2 quadrants are, for an interior pixel at (s, l):
 *   upper-left  = {s-1, s}   over two lines -> mean s-0.5
 *   upper-right = {s, s+1}   over two lines -> mean s+0.5
 *   lower-left  = {s-1, s}   over two lines -> mean s-0.5
 *   lower-right = {s, s+1}   over two lines -> mean s+0.5
 * All four have identical variance (two values differing by 1). The kuwahara
 * XML documents that on a variance tie the first quadrant encountered
 * (upper-left) is used, so the expected output is s-0.5 for every line.
 * The expectation s-0.5 is derived from the column-ramp definition and the 2x2
 * quadrant geometry, independent of the implementation.
 */
TEST_F(TempTestingFiles, FunctionalTestKuwaharaColumnRamp) {
  QString inPath = tempDir.path() + "/colramp.cub";
  createLinearPatternCube(inPath, 10, 10, 1, 0.0, /*lineMult*/0.0, /*sampMult*/1.0, 0.0);

  QString outPath = tempDir.path() + "/colrampOut.cub";
  QVector<QString> args = {"from=" + inPath, "to=" + outPath,
                           "samples=3", "lines=3"};
  UserInterface ui(APP_XML, args);

  try {
    kuwahara(ui);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(outPath);
  // Interior pixels (boxcar fully inside the cube): result = sample - 0.5,
  // identical for every line (tie-break selects the upper-left quadrant).
  for (int sample = 2; sample <= 9; sample++) {
    for (int line = 2; line <= 9; line++) {
      EXPECT_DOUBLE_EQ(readPixel(outCube, sample, line), sample - 0.5)
          << "at sample " << sample << ", line " << line;
    }
  }
}

/**
 * KuwaharaRowRamp
 *
 * Feature: line-direction behavior is symmetric to the sample direction, and
 * the same documented tie-break applies.
 *
 * Input DN = line (constant per row). By the same 2x2 quadrant geometry as the
 * column-ramp case (transposed), all four quadrant variances tie and the
 * upper-left quadrant wins, giving line - 0.5 for every interior pixel
 * independent of sample.
 */
TEST_F(TempTestingFiles, FunctionalTestKuwaharaRowRamp) {
  QString inPath = tempDir.path() + "/rowramp.cub";
  createLinearPatternCube(inPath, 10, 10, 1, 0.0, /*lineMult*/1.0, /*sampMult*/0.0, 0.0);

  QString outPath = tempDir.path() + "/rowrampOut.cub";
  QVector<QString> args = {"from=" + inPath, "to=" + outPath,
                           "samples=3", "lines=3"};
  UserInterface ui(APP_XML, args);

  try {
    kuwahara(ui);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(outPath);
  for (int sample = 2; sample <= 9; sample++) {
    for (int line = 2; line <= 9; line++) {
      EXPECT_DOUBLE_EQ(readPixel(outCube, sample, line), line - 0.5)
          << "at sample " << sample << ", line " << line;
    }
  }
}

/**
 * KuwaharaMinVarianceQuadrant
 *
 * Feature: the filter selects the minimum-variance quadrant, NOT simply the
 * first (upper-left) quadrant. This distinguishes genuine min-variance
 * selection from the tie-break path and uses an independently derived
 * expectation with no variance tie.
 *
 * A 5x5 cube is constructed so that, for the center pixel (3,3) with a 3x3
 * boxcar, the four overlapping 2x2 quadrants are:
 *   upper-left  = {1, 2, 3, 8} -> variance > 0,  mean 3.5
 *   upper-right = {2, 2, 8, 8} -> variance > 0,  mean 5.0
 *   lower-left  = {3, 8, 3, 8} -> variance > 0,  mean 5.5
 *   lower-right = {8, 8, 8, 8} -> variance = 0,  mean 8.0  (minimum)
 * The lower-right quadrant is uniform, so it has the strictly smallest
 * variance and its mean (8) must be selected even though it is not the first
 * quadrant. The expected output at (3,3) is therefore exactly 8.
 *
 * Neighborhood values (samples 1-5, lines 1-5; only samples 2-4 / lines 2-4
 * enter the (3,3) boxcar):
 *   line 2:  0  1  2  2  0
 *   line 3:  0  3  8  8  0
 *   line 4:  0  3  8  8  0
 */
TEST_F(TempTestingFiles, FunctionalTestKuwaharaMinVarianceQuadrant) {
  QString inPath = tempDir.path() + "/minvar.cub";
  Cube inCube;
  inCube.setDimensions(5, 5, 1);
  inCube.create(inPath);

  double rows[5][5] = {
    {0, 0, 0, 0, 0},
    {0, 1, 2, 2, 0},
    {0, 3, 8, 8, 0},
    {0, 3, 8, 8, 0},
    {0, 0, 0, 0, 0}
  };

  LineManager line(inCube);
  int l = 0;
  for (line.begin(); !line.end(); line++, l++) {
    for (int i = 0; i < line.size(); i++) {
      line[i] = rows[l][i];
    }
    inCube.write(line);
  }
  inCube.close();

  QString outPath = tempDir.path() + "/minvarOut.cub";
  QVector<QString> args = {"from=" + inPath, "to=" + outPath,
                           "samples=3", "lines=3"};
  UserInterface ui(APP_XML, args);

  try {
    kuwahara(ui);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(outPath);
  // Lower-right quadrant (variance 0) wins over the first quadrant.
  EXPECT_DOUBLE_EQ(readPixel(outCube, 3, 3), 8.0);
}

/**
 * KuwaharaNonSquareBoxcar
 *
 * Feature: non-default and non-square boxcar dimensions (SAMPLES != LINES). The
 * legacy Makefile test only exercised the default 5x5 box; this covers a
 * user-specified 5x3 box.
 *
 * Input DN = sample. With SAMPLES=5, LINES=3 the subunit is 3 wide x 2 tall.
 * For an interior pixel at sample s the upper-left quadrant spans columns
 * {s-2, s-1, s} -> mean s-1. As with the symmetric ramp all quadrant variances
 * tie, so the upper-left quadrant is selected and the expected output is s-1,
 * independent of line. Expectation derived from ramp definition + geometry.
 */
TEST_F(TempTestingFiles, FunctionalTestKuwaharaNonSquareBoxcar) {
  QString inPath = tempDir.path() + "/nonsquare.cub";
  createLinearPatternCube(inPath, 10, 10, 1, 0.0, 0.0, /*sampMult*/1.0, 0.0);

  QString outPath = tempDir.path() + "/nonsquareOut.cub";
  QVector<QString> args = {"from=" + inPath, "to=" + outPath,
                           "samples=5", "lines=3"};
  UserInterface ui(APP_XML, args);

  try {
    kuwahara(ui);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(outPath);
  // Interior pixels with the full 5-wide / 3-tall box inside the cube.
  for (int sample = 3; sample <= 8; sample++) {
    for (int line = 2; line <= 9; line++) {
      EXPECT_DOUBLE_EQ(readPixel(outCube, sample, line), sample - 1.0)
          << "at sample " << sample << ", line " << line;
    }
  }
}

/**
 * KuwaharaBoxSizeReset
 *
 * Feature: the boxcar dimensions are reset on every invocation; sequential
 * calls with different SAMPLES/LINES in the same process must not reuse stale
 * state.
 *
 * The same column-ramp input is filtered first with a 3x3 box (expected
 * interior result sample-0.5) and then with a 5x3 box (expected interior
 * result sample-1). Each output must match its own box size's expectation,
 * proving the second call did not inherit the first call's dimensions.
 */
TEST_F(TempTestingFiles, FunctionalTestKuwaharaBoxSizeReset) {
  QString inPath = tempDir.path() + "/resetramp.cub";
  createLinearPatternCube(inPath, 10, 10, 1, 0.0, 0.0, /*sampMult*/1.0, 0.0);

  QString out3x3 = tempDir.path() + "/reset3x3.cub";
  QVector<QString> args3 = {"from=" + inPath, "to=" + out3x3,
                            "samples=3", "lines=3"};
  UserInterface ui3(APP_XML, args3);

  QString out5x3 = tempDir.path() + "/reset5x3.cub";
  QVector<QString> args5 = {"from=" + inPath, "to=" + out5x3,
                            "samples=5", "lines=3"};
  UserInterface ui5(APP_XML, args5);

  try {
    kuwahara(ui3);
    kuwahara(ui5);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube3(out3x3);
  Cube cube5(out5x3);
  for (int sample = 3; sample <= 8; sample++) {
    EXPECT_DOUBLE_EQ(readPixel(cube3, sample, 5), sample - 0.5)
        << "3x3 at sample " << sample;
    EXPECT_DOUBLE_EQ(readPixel(cube5, sample, 5), sample - 1.0)
        << "5x3 at sample " << sample;
  }
}

/**
 * KuwaharaEdgePreservation
 *
 * Feature: the defining property of the Kuwahara filter - edges are preserved
 * rather than blurred.
 *
 * Input is a vertical step edge (samples 1-5 = 10, samples 6-10 = 20). A
 * mean filter would produce intermediate values (e.g. 15) across the edge.
 * The Kuwahara filter always finds a zero-variance quadrant lying entirely in
 * one flat region, so every output pixel is exactly 10 or 20 - no blurring.
 * Every pixel in the output is checked, not just the set of unique values.
 */
TEST_F(TempTestingFiles, FunctionalTestKuwaharaEdgePreservation) {
  QString inPath = tempDir.path() + "/step.cub";
  Cube inCube;
  inCube.setDimensions(10, 10, 1);
  inCube.create(inPath);
  LineManager line(inCube);
  for (line.begin(); !line.end(); line++) {
    for (int i = 0; i < line.size(); i++) {
      line[i] = (i < 5) ? 10.0 : 20.0;   // sample index 0-4 -> 10, 5-9 -> 20
    }
    inCube.write(line);
  }
  inCube.close();

  QString outPath = tempDir.path() + "/stepOut.cub";
  QVector<QString> args = {"from=" + inPath, "to=" + outPath,
                           "samples=3", "lines=3"};
  UserInterface ui(APP_XML, args);

  try {
    kuwahara(ui);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(outPath);
  std::unique_ptr<Statistics> stats(outCube.statistics());

  // Edge is preserved: only the two original DNs survive, no blurred values.
  EXPECT_EQ(stats->ValidPixels(), 100);
  EXPECT_DOUBLE_EQ(stats->Minimum(), 10.0);
  EXPECT_DOUBLE_EQ(stats->Maximum(), 20.0);

  for (int lineNumber = 1; lineNumber <= 10; lineNumber++) {
    for (int sample = 1; sample <= 10; sample++) {
      double dn = readPixel(outCube, sample, lineNumber);
      bool flat = (dn == 10.0 || dn == 20.0);
      EXPECT_TRUE(flat) << "blurred value " << dn
                        << " at sample " << sample << ", line " << lineNumber;
    }
  }
}

/**
 * KuwaharaMultiBand
 *
 * Feature: each band is filtered independently and the band count is
 * preserved.
 *
 * Input is a 3-band cube with constant DN per band (band * 10). The filter
 * leaves each uniform band unchanged, so band b is uniformly b*10 in the
 * output. Each band is checked with full-band statistics and a representative
 * interior pixel value.
 */
TEST_F(TempTestingFiles, FunctionalTestKuwaharaMultiBand) {
  QString inPath = tempDir.path() + "/bands.cub";
  createLinearPatternCube(inPath, 10, 10, 3, 0.0, 0.0, 0.0, /*bandMult*/10.0);

  QString outPath = tempDir.path() + "/bandsOut.cub";
  QVector<QString> args = {"from=" + inPath, "to=" + outPath};
  UserInterface ui(APP_XML, args);

  try {
    kuwahara(ui);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(outPath);
  EXPECT_EQ(outCube.bandCount(), 3);

  for (int band = 1; band <= 3; band++) {
    std::unique_ptr<Statistics> stats(outCube.statistics(band));
    EXPECT_EQ(stats->ValidPixels(), 100);
    EXPECT_DOUBLE_EQ(stats->Average(), band * 10.0) << "band " << band;
    EXPECT_DOUBLE_EQ(stats->StandardDeviation(), 0.0) << "band " << band;
    // Representative interior pixel in each band.
    EXPECT_DOUBLE_EQ(readPixel(outCube, 5, 5, band), band * 10.0)
        << "band " << band;
  }
}

/**
 * KuwaharaCubeOverloadEquivalence
 *
 * Feature: the kuwahara(Cube *, UserInterface &) overload produces the same
 * result as kuwahara(UserInterface &), confirming main.cpp's thin wrapper and
 * the cube-based test entry point are equivalent.
 */
TEST_F(TempTestingFiles, FunctionalTestKuwaharaCubeOverloadEquivalence) {
  QString inPath = tempDir.path() + "/overload.cub";
  createLinearPatternCube(inPath, 10, 10, 1, 0.0, 0.0, /*sampMult*/1.0, 0.0);

  // Filename overload (opens FROM internally).
  QString outUi = tempDir.path() + "/overloadUi.cub";
  QVector<QString> argsUi = {"from=" + inPath, "to=" + outUi,
                             "samples=3", "lines=3"};
  UserInterface uiA(APP_XML, argsUi);

  // Cube * overload (caller opens the cube).
  QString outCubePtr = tempDir.path() + "/overloadCube.cub";
  QVector<QString> argsCube = {"from=" + inPath, "to=" + outCubePtr,
                               "samples=3", "lines=3"};
  UserInterface uiB(APP_XML, argsCube);
  Cube icube;
  icube.open(inPath);

  try {
    kuwahara(uiA);
    kuwahara(&icube, uiB);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cubeA(outUi);
  Cube cubeB(outCubePtr);
  ASSERT_EQ(cubeA.sampleCount(), cubeB.sampleCount());
  ASSERT_EQ(cubeA.lineCount(), cubeB.lineCount());
  for (int line = 1; line <= cubeA.lineCount(); line++) {
    for (int sample = 1; sample <= cubeA.sampleCount(); sample++) {
      EXPECT_DOUBLE_EQ(readPixel(cubeA, sample, line),
                       readPixel(cubeB, sample, line))
          << "mismatch at sample " << sample << ", line " << line;
    }
  }
}
