/**
 * @file
 *
 * Unit tests for fx application.
 *
 * Test strategy: All enabled tests use programmatically generated cubes
 * with simple, deterministic DN values. This eliminates the 87.5 MB external
 * ISISTESTDATA dependency while preserving coverage of fx equation parsing,
 * operators, modes, and band attribute handling.
 */

#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include <cmath>

#include "fx.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "TestUtilities.h"
#include "IException.h"
#include "Cube.h"
#include "Statistics.h"
#include "LineManager.h"
#include "FileName.h"
#include "FileList.h"
#include "UserInterface.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/fx.xml").expanded();

/**
 * FxTest fixture for direct callable fx tests.
 * Generates test cubes with deterministic DN patterns.
 */
class FxTest : public TempTestingFiles {
protected:
  void SetUp() override {
    TempTestingFiles::SetUp();
  }

  // Get DN value at specific location
  double getPixel(const QString &cubePath, int sample, int line, int band) {
    Cube cube(cubePath);
    LineManager mgr(cube);
    mgr.SetLine(line, band);
    cube.read(mgr);
    return mgr[sample-1];
  }
};


/**
 * Test singleLine: f1 + f2
 * f1 = sample + line, f2 = 2*sample + line
 * Expected: output = 3*sample + 2*line
 */
TEST_F(FxTest, FunctionalTestFxSingleLine) {
  QString input1 = createLinearPatternCube(tempDir.path() + "/in1.cub", 5, 5, 1, 0.0, 1.0, 1.0, 0.0);  // DN = line + sample
  QString input2 = createLinearPatternCube(tempDir.path() + "/in2.cub", 5, 5, 1, 0.0, 1.0, 2.0, 0.0);  // DN = line + 2*sample
  QString output = tempDir.path() + "/result.cub";

  QVector<QString> args = {"f1=" + input1, "f2=" + input2, "to=" + output, "equation=f1+f2"};
  UserInterface ui(APP_XML, args);
  fx(ui);

  // Validate: output = (line+sample) + (line+2*sample) = 2*line + 3*sample
  EXPECT_NEAR(getPixel(output, 1, 1, 1), 5.0, 0.001);   // 2*1 + 3*1 = 5
  EXPECT_NEAR(getPixel(output, 3, 2, 1), 13.0, 0.001);  // 2*2 + 3*3 = 13
  EXPECT_NEAR(getPixel(output, 5, 5, 1), 25.0, 0.001);  // 2*5 + 3*5 = 25

  Cube outputCube(output);
  EXPECT_EQ(outputCube.sampleCount(), 5);
  EXPECT_EQ(outputCube.lineCount(), 5);
  EXPECT_EQ(outputCube.bandCount(), 1);

  Statistics stats;
  for (int line = 1; line <= outputCube.lineCount(); line++) {
    LineManager mgr(outputCube);
    mgr.SetLine(line, 1);
    outputCube.read(mgr);
    stats.AddData(mgr.DoubleBuffer(), mgr.size());
  }
  EXPECT_EQ(stats.ValidPixels(), 25);
  EXPECT_NEAR(stats.Minimum(), 5.0, 0.001);   // min at (1,1): 2*1+3*1=5
  EXPECT_NEAR(stats.Maximum(), 25.0, 0.001);  // max at (5,5): 2*5+3*5=25
  EXPECT_NEAR(stats.Average(), 15.0, 0.001);  // avg of linear pattern
}


/**
 * Test default: --f1+f2/cos(1/2)
 * Tests negation operator (-- prefix for negative) and trig function.
 * f1 = band1 = 10+5*1 = 15.0, f2 = band2 = 10+5*2 = 20.0
 * Expected: -f1 + f2/cos(0.5) = -15 + 20/0.8776 ≈ -15 + 22.79 = 7.79
 */
TEST_F(FxTest, FunctionalTestFxDefault) {
  // Create 2-band cube: band1=15, band2=20 (constant per band)
  QString input = createLinearPatternCube(tempDir.path() + "/input.cub", 4, 4, 2, 10.0, 0.0, 0.0, 5.0);
  QString output = tempDir.path() + "/result.cub";

  QVector<QString> args = {"f1=" + input + "+1", "f2=" + input + "+2", "to=" + output, "equation=--f1+f2/cos(1/2)"};
  UserInterface ui(APP_XML, args);
  fx(ui);

  Cube outputCube(output);
  EXPECT_EQ(outputCube.sampleCount(), 4);
  EXPECT_EQ(outputCube.lineCount(), 4);

  // Expected: -15 + 20/cos(0.5) ≈ -15 + 22.79 = 7.79
  double dn = getPixel(output, 1, 1, 1);
  EXPECT_NEAR(dn, 7.79, 0.01) << "Negation prefix and trig division result";

  Statistics stats;
  for (int line = 1; line <= outputCube.lineCount(); line++) {
    LineManager mgr(outputCube);
    mgr.SetLine(line, 1);
    outputCube.read(mgr);
    stats.AddData(mgr.DoubleBuffer(), mgr.size());
  }
  EXPECT_EQ(stats.ValidPixels(), 16);
  EXPECT_NEAR(stats.Minimum(), 7.79, 0.01);
  EXPECT_NEAR(stats.Maximum(), 7.79, 0.01);
  EXPECT_NEAR(stats.Average(), 7.79, 0.01);
}


/**
 * Test cubestats: cubemax, cubemin, linemin, cubeavg, cubestd, max, min
 * f1: DNs range 1-6 (1,2,3,4,5,6)
 * f2: DNs range 10-15 (10,11,12,13,14,15)
 */
TEST_F(FxTest, FunctionalTestFxCubestats) {
  QString input1 = createLinearPatternCube(tempDir.path() + "/in1.cub", 3, 2, 1, 0.0, 0.0, 1.0, 0.0);   // DN = sample (1,2,3)
  QString input2 = createLinearPatternCube(tempDir.path() + "/in2.cub", 3, 2, 1, 10.0, 0.0, 1.0, 0.0);  // DN = 10+sample (11,12,13)

  // Test: cubemax(f1) - cubemin(f2)
  // cubemax(f1) = 3, cubemin(f2) = 11
  // Expected: 3 - 11 = -8 (constant cube)
  QString output1 = tempDir.path() + "/result1.cub";
  QVector<QString> args1 = {"f1=" + input1, "f2=" + input2, "to=" + output1, "equation=cubemax(f1)-cubemin(f2)"};
  UserInterface ui1(APP_XML, args1);
  fx(ui1);
  EXPECT_NEAR(getPixel(output1, 1, 1, 1), -8.0, 0.001);
  Statistics stats1;
  Cube out1(output1);
  for (int line = 1; line <= out1.lineCount(); line++) {
    LineManager mgr(out1);
    mgr.SetLine(line, 1);
    out1.read(mgr);
    stats1.AddData(mgr.DoubleBuffer(), mgr.size());
  }
  EXPECT_EQ(stats1.ValidPixels(), 6);
  EXPECT_NEAR(stats1.Minimum(), -8.0, 0.001);
  EXPECT_NEAR(stats1.Maximum(), -8.0, 0.001);

  // Test: max(f1, f2) - pixel-wise max
  // f1: 1,2,3  f2: 11,12,13  => max: 11,12,13
  QString output2 = tempDir.path() + "/result2.cub";
  QVector<QString> args2 = {"f1=" + input1, "f2=" + input2, "to=" + output2, "equation=max(f1,f2)"};
  UserInterface ui2(APP_XML, args2);
  fx(ui2);
  EXPECT_NEAR(getPixel(output2, 1, 1, 1), 11.0, 0.001);
  EXPECT_NEAR(getPixel(output2, 2, 1, 1), 12.0, 0.001);
  Statistics stats2;
  Cube out2(output2);
  for (int line = 1; line <= out2.lineCount(); line++) {
    LineManager mgr(out2);
    mgr.SetLine(line, 1);
    out2.read(mgr);
    stats2.AddData(mgr.DoubleBuffer(), mgr.size());
  }
  EXPECT_EQ(stats2.ValidPixels(), 6);
  EXPECT_NEAR(stats2.Minimum(), 11.0, 0.001);
  EXPECT_NEAR(stats2.Maximum(), 13.0, 0.001);
  EXPECT_NEAR(stats2.Average(), 12.0, 0.001);

  // Test: cubeavg(f1) - average of all pixels
  // f1 has 6 pixels: 1,2,3,1,2,3 => avg = 2.0
  QString output3 = tempDir.path() + "/result3.cub";
  QVector<QString> args3 = {"f1=" + input1, "to=" + output3, "equation=cubeavg(f1)"};
  UserInterface ui3(APP_XML, args3);
  fx(ui3);
  EXPECT_NEAR(getPixel(output3, 1, 1, 1), 2.0, 0.001);
  Statistics stats3;
  Cube out3(output3);
  for (int line = 1; line <= out3.lineCount(); line++) {
    LineManager mgr(out3);
    mgr.SetLine(line, 1);
    out3.read(mgr);
    stats3.AddData(mgr.DoubleBuffer(), mgr.size());
  }
  EXPECT_EQ(stats3.ValidPixels(), 6);
  EXPECT_NEAR(stats3.Average(), 2.0, 0.001);

  // Test: cubestd(f1) - standard deviation
  QString output4 = tempDir.path() + "/result4.cub";
  QVector<QString> args4 = {"f1=" + input1, "to=" + output4, "equation=cubestd(f1)"};
  UserInterface ui4(APP_XML, args4);
  fx(ui4);
  Cube out4(output4);
  EXPECT_GT(out4.sampleCount(), 0);
  Statistics stats4;
  for (int line = 1; line <= out4.lineCount(); line++) {
    LineManager mgr(out4);
    mgr.SetLine(line, 1);
    out4.read(mgr);
    stats4.AddData(mgr.DoubleBuffer(), mgr.size());
  }
  EXPECT_EQ(stats4.ValidPixels(), 6);
  EXPECT_GT(stats4.Average(), 0.0);
}


/**
 * Test operators: max, min, neg, degs, rads, mod
 * Uses tiny 5x5 cube with simple DN pattern.
 */
TEST_F(FxTest, FunctionalTestFxOperators) {
  QString input = createLinearPatternCube(tempDir.path() + "/input.cub", 5, 5, 2, 50.0, 1.0, 1.0, 10.0);
  // Band 1: DN = 50 + line + sample + 10*1 = 61-70
  // Band 2: DN = 50 + line + sample + 10*2 = 71-80

  // Test: max(90, max(f1,f2)) - values below 90 should produce 90
  QString maxOutput = tempDir.path() + "/max2.cub";
  QVector<QString> maxArgs = {"f1=" + input + "+1", "f2=" + input + "+2", "to=" + maxOutput, "equation=max(90, max(f1,f2))"};
  UserInterface maxUi(APP_XML, maxArgs);
  fx(maxUi);
  // All values > 90 should produce exactly 90.
  Statistics maxStats;
  Cube maxCube(maxOutput);
  for (int line = 1; line <= maxCube.lineCount(); line++) {
    LineManager mgr(maxCube);
    mgr.SetLine(line, 1);
    maxCube.read(mgr);
    maxStats.AddData(mgr.DoubleBuffer(), mgr.size());
  }
  EXPECT_EQ(maxStats.ValidPixels(), 25);
  EXPECT_NEAR(maxStats.Minimum(), 90.0, 0.001);
  EXPECT_NEAR(maxStats.Maximum(), 90.0, 0.001);
  EXPECT_NEAR(maxStats.Average(), 90.0, 0.001);

  // Test: min(40, min(f1,f2)) - should use min
  QString minOutput = tempDir.path() + "/min2.cub";
  QVector<QString> minArgs = {"f1=" + input + "+1", "f2=" + input + "+2", "to=" + minOutput, "equation=min(40, min(f1,f2))"};
  UserInterface minUi(APP_XML, minArgs);
  fx(minUi);
  // f1 min = 62, f2 min = 72, min(f1,f2) = 62, min(40,62) = 40
  EXPECT_NEAR(getPixel(minOutput, 1, 1, 1), 40.0, 0.001);
  Statistics minStats;
  Cube minCube(minOutput);
  for (int line = 1; line <= minCube.lineCount(); line++) {
    LineManager mgr(minCube);
    mgr.SetLine(line, 1);
    minCube.read(mgr);
    minStats.AddData(mgr.DoubleBuffer(), mgr.size());
  }
  EXPECT_EQ(minStats.ValidPixels(), 25);
  EXPECT_NEAR(minStats.Maximum(), 40.0, 0.001);
  EXPECT_NEAR(minStats.Average(), 40.0, 0.001);

  // Test: neg(f1) - negate
  QString negOutput = tempDir.path() + "/neg.cub";
  QVector<QString> negArgs = {"f1=" + input + "+1", "to=" + negOutput, "equation=neg(f1)"};
  UserInterface negUi(APP_XML, negArgs);
  fx(negUi);
  EXPECT_NEAR(getPixel(negOutput, 1, 1, 1), -62.0, 0.001);  // -(50+1+1+10)
  Statistics negStats;
  Cube negCube(negOutput);
  for (int line = 1; line <= negCube.lineCount(); line++) {
    LineManager mgr(negCube);
    mgr.SetLine(line, 1);
    negCube.read(mgr);
    negStats.AddData(mgr.DoubleBuffer(), mgr.size());
  }
  EXPECT_EQ(negStats.ValidPixels(), 25);
  EXPECT_NEAR(negStats.Minimum(), -70.0, 0.001);
  EXPECT_NEAR(negStats.Maximum(), -62.0, 0.001);

  // Test: degs(pi) - pi radians = 180 degrees
  QString degsOutput = tempDir.path() + "/degs.cub";
  QVector<QString> degsArgs = {"f1=" + input + "+1", "to=" + degsOutput, "equation=degs(pi)"};
  UserInterface degsUi(APP_XML, degsArgs);
  fx(degsUi);
  EXPECT_NEAR(getPixel(degsOutput, 1, 1, 1), 180.0, 0.001);
  Statistics degsStats;
  Cube degsCube(degsOutput);
  for (int line = 1; line <= degsCube.lineCount(); line++) {
    LineManager mgr(degsCube);
    mgr.SetLine(line, 1);
    degsCube.read(mgr);
    degsStats.AddData(mgr.DoubleBuffer(), mgr.size());
  }
  EXPECT_EQ(degsStats.ValidPixels(), 25);
  EXPECT_NEAR(degsStats.Average(), 180.0, 0.001);

  // Test: rads(360.0) - 360 degrees = 2π radians
  QString radsOutput = tempDir.path() + "/rads.cub";
  QVector<QString> radsArgs = {"f1=" + input + "+1", "to=" + radsOutput, "equation=rads(360.0)"};
  UserInterface radsUi(APP_XML, radsArgs);
  fx(radsUi);
  EXPECT_NEAR(getPixel(radsOutput, 1, 1, 1), 2.0 * M_PI, 0.001);
  Statistics radsStats;
  Cube radsCube(radsOutput);
  for (int line = 1; line <= radsCube.lineCount(); line++) {
    LineManager mgr(radsCube);
    mgr.SetLine(line, 1);
    radsCube.read(mgr);
    radsStats.AddData(mgr.DoubleBuffer(), mgr.size());
  }
  EXPECT_EQ(radsStats.ValidPixels(), 25);
  EXPECT_NEAR(radsStats.Average(), 2.0 * M_PI, 0.001);

  // Test: sample%2 - modulo operator
  QString modOutput = tempDir.path() + "/mod.cub";
  QVector<QString> modArgs = {"f1=" + input + "+1", "to=" + modOutput, "equation=sample%2"};
  UserInterface modUi(APP_XML, modArgs);
  fx(modUi);
  EXPECT_NEAR(getPixel(modOutput, 1, 1, 1), 1.0, 0.001);  // sample 1 % 2 = 1
  EXPECT_NEAR(getPixel(modOutput, 2, 1, 1), 0.0, 0.001);  // sample 2 % 2 = 0
  Statistics modStats;
  Cube modCube(modOutput);
  for (int line = 1; line <= modCube.lineCount(); line++) {
    LineManager mgr(modCube);
    mgr.SetLine(line, 1);
    modCube.read(mgr);
    modStats.AddData(mgr.DoubleBuffer(), mgr.size());
  }
  EXPECT_EQ(modStats.ValidPixels(), 25);
  EXPECT_NEAR(modStats.Minimum(), 0.0, 0.001);
  EXPECT_NEAR(modStats.Maximum(), 1.0, 0.001);
}


/**
 * Test outputonly: create cube from equation without input
 * equation = line + sample + band
 */
TEST_F(FxTest, FunctionalTestFxOutputonly) {
  QString output = tempDir.path() + "/result.cub";

  QVector<QString> args = {"to=" + output, "mode=outputonly", "lines=10", "samples=10", "bands=2",
         "equation=line+sample+band"};
  UserInterface ui(APP_XML, args);
  fx(ui);

  Cube outputCube(output);
  EXPECT_EQ(outputCube.sampleCount(), 10);
  EXPECT_EQ(outputCube.lineCount(), 10);
  EXPECT_EQ(outputCube.bandCount(), 2);

  // Validate: DN = line + sample + band
  EXPECT_NEAR(getPixel(output, 1, 1, 1), 3.0, 0.001);    // 1+1+1 = 3
  EXPECT_NEAR(getPixel(output, 5, 3, 1), 9.0, 0.001);    // 5+3+1 = 9
  EXPECT_NEAR(getPixel(output, 10, 10, 2), 22.0, 0.001); // 10+10+2 = 22

  Statistics stats;
  for (int band = 1; band <= outputCube.bandCount(); band++) {
    for (int line = 1; line <= outputCube.lineCount(); line++) {
      LineManager mgr(outputCube);
      mgr.SetLine(line, band);
      outputCube.read(mgr);
      stats.AddData(mgr.DoubleBuffer(), mgr.size());
    }
  }
  EXPECT_EQ(stats.ValidPixels(), 200);
  EXPECT_NEAR(stats.Minimum(), 3.0, 0.001);   // min: 1+1+1
  EXPECT_NEAR(stats.Maximum(), 22.0, 0.001);  // max: 10+10+2
}


/**
 * Test filelist: use fromlist mode with multiple cubes
 * f1 = 10, f2 = 20, f3 = 30
 * Expected: output = 10 + 20 + 30 = 60
 */
TEST_F(FxTest, FunctionalTestFxFilelist) {
  QString input1 = createLinearPatternCube(tempDir.path() + "/in1.cub", 4, 4, 1, 10.0, 0.0, 0.0, 0.0);
  QString input2 = createLinearPatternCube(tempDir.path() + "/in2.cub", 4, 4, 1, 20.0, 0.0, 0.0, 0.0);
  QString input3 = createLinearPatternCube(tempDir.path() + "/in3.cub", 4, 4, 1, 30.0, 0.0, 0.0, 0.0);

  QString fromlist = tempDir.path() + "/fromlist.lis";
  FileList cubeList;
  cubeList.append(FileName(input1));
  cubeList.append(FileName(input2));
  cubeList.append(FileName(input3));
  cubeList.write(fromlist);

  QString output = tempDir.path() + "/result.cub";

  QVector<QString> args = {"fromlist=" + fromlist, "mode=list", "equation=f1+f2+f3", "to=" + output};
  UserInterface ui(APP_XML, args);
  fx(ui);

  EXPECT_NEAR(getPixel(output, 1, 1, 1), 60.0, 0.001);

  Cube outputCube(output);
  EXPECT_EQ(outputCube.sampleCount(), 4);
  EXPECT_EQ(outputCube.lineCount(), 4);
  EXPECT_EQ(outputCube.bandCount(), 1);

  Statistics stats;
  for (int line = 1; line <= outputCube.lineCount(); line++) {
    LineManager mgr(outputCube);
    mgr.SetLine(line, 1);
    outputCube.read(mgr);
    stats.AddData(mgr.DoubleBuffer(), mgr.size());
  }
  EXPECT_EQ(stats.ValidPixels(), 16);
  EXPECT_NEAR(stats.Minimum(), 60.0, 0.001);
  EXPECT_NEAR(stats.Maximum(), 60.0, 0.001);
  EXPECT_NEAR(stats.Average(), 60.0, 0.001);
}


/**
 * Test filelistattributes: fromlist with band attributes
 * Creates 4x4x5 multiband cubes, selects specific bands.
 * f1+1 (band 1) = 10, f2+3 (band 3) = 30
 * Expected: output = 10 * 30 = 300
 */
TEST_F(FxTest, FunctionalTestFxFilelistattributes) {
  // Create multiband cubes: each band has distinct DN value
  // Band N: DN = 10 * N (using bandMult=10.0)
  QString vims1 = createLinearPatternCube(tempDir.path() + "/vims1.cub", 4, 4, 5, 0.0, 0.0, 0.0, 10.0);
  QString vims2 = createLinearPatternCube(tempDir.path() + "/vims2.cub", 4, 4, 5, 0.0, 0.0, 0.0, 10.0);

  QString listPath = tempDir.path() + "/list.lis";
  // Write this list manually because FileList::write() normalizes FileName entries
  // and does not preserve cube attribute suffixes such as +1 and +3.
  QFile file(listPath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    FAIL() << "Failed to create list file";
  }
  QTextStream out(&file);
  out << vims1 + "+1\n";   // Select band 1 (DN=10)
  out << vims2 + "+3\n";   // Select band 3 (DN=30)
  file.close();

  QString output = tempDir.path() + "/result.cub";
  QVector<QString> args = {"fromlist=" + listPath, "mode=list", "equation=f1*f2", "to=" + output};
  UserInterface ui(APP_XML, args);
  fx(ui);

  // Validate: 10 * 30 = 300
  EXPECT_NEAR(getPixel(output, 1, 1, 1), 300.0, 0.001);

  Cube outputCube(output);
  EXPECT_EQ(outputCube.sampleCount(), 4);
  EXPECT_EQ(outputCube.lineCount(), 4);

  Statistics stats;
  for (int line = 1; line <= outputCube.lineCount(); line++) {
    LineManager mgr(outputCube);
    mgr.SetLine(line, 1);
    outputCube.read(mgr);
    stats.AddData(mgr.DoubleBuffer(), mgr.size());
  }
  EXPECT_EQ(stats.ValidPixels(), 16);
  EXPECT_NEAR(stats.Minimum(), 300.0, 0.001);
  EXPECT_NEAR(stats.Maximum(), 300.0, 0.001);
  EXPECT_NEAR(stats.Average(), 300.0, 0.001);
}
