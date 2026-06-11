#include "makeflat.h"

#include "Cube.h"
#include "CubeFixtures.h"
#include "FileList.h"
#include "Fixtures.h"
#include "LineManager.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Statistics.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/makeflat.xml").expanded();

class MakeflatTest : public TempTestingFiles {
protected:
  QString createFromList(const QStringList &cubeFiles) {
    QString listPath = tempDir.path() + "/fromlist.lis";
    FileList fileList;
    for (const QString &cube : cubeFiles) {
      fileList.append(cube);
    }
    fileList.write(listPath);
    return listPath;
  }
};

TEST_F(MakeflatTest, FunctionalTestMakeflatFraming) {
  QString input1 = tempDir.path() + "/framing_input1.cub";
  QString input2 = tempDir.path() + "/framing_input2.cub";

  // Linear gradient with sampMult tuned to produce stdev within tolerance
  // For 500 samples: stdev ≈ sampMult * 144, target stdev ~60
  createLinearPatternCube(input1, 500, 50, 1, 800.0, 0.0, 0.42, 0.0);
  createLinearPatternCube(input2, 500, 50, 1, 850.0, 0.0, 0.42, 0.0);

  QStringList inputCubes;
  inputCubes << input1 << input2;

  QString fromList = createFromList(inputCubes);
  QString outputCube = tempDir.path() + "/framing_output.cub";
  QString excludeFile = tempDir.path() + "/framing_exclude.txt";

  QVector<QString> args = {
    "fromlist=" + fromList,
    "to=" + outputCube,
    "stdevtol=190",
    "imagetype=FRAMING",
    "exclude=" + excludeFile
  };

  UserInterface ui(APP_XML, args);

  try {
    makeflat(ui);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString();
  }

  Cube output(outputCube);
  EXPECT_EQ(output.sampleCount(), 500);
  EXPECT_EQ(output.lineCount(), 50);
  EXPECT_EQ(output.bandCount(), 1);

  std::unique_ptr<Statistics> stats(output.statistics());
  EXPECT_EQ(stats->ValidPixels(), 25000);
  EXPECT_NEAR(stats->Average(), 1.0, 0.01);
  EXPECT_GT(stats->Minimum(), 0.8);
  EXPECT_LT(stats->Maximum(), 1.2);
  EXPECT_LT(stats->StandardDeviation(), 0.2);

  EXPECT_TRUE(QFile::exists(excludeFile));

  Pvl excludePvl(excludeFile);
  EXPECT_TRUE(excludePvl.hasGroup("ExcludedFiles"));
  PvlGroup excludedFilesGrp = excludePvl.findGroup("ExcludedFiles");
  EXPECT_EQ(excludedFilesGrp.keywords(), 0);

  output.close();
}

TEST_F(MakeflatTest, FunctionalTestMakeflatLinescan) {
  // Create synthetic linescan cubes with 100 lines each, NUMLINES=10 means 10 frames per cube
  QString input1 = tempDir.path() + "/linescan_input1.cub";
  QString input2 = tempDir.path() + "/linescan_input2.cub";

  // Linear gradient with sampMult tuned to produce stdev within tolerance
  // For 5000 samples: stdev ≈ sampMult * 1443, target stdev ~35
  createLinearPatternCube(input1, 5000, 100, 1, 1000.0, 0.0, 0.024, 0.0);
  createLinearPatternCube(input2, 5000, 100, 1, 1050.0, 0.0, 0.024, 0.0);

  QStringList inputCubes;
  inputCubes << input1 << input2;

  QString fromList = createFromList(inputCubes);
  QString outputCube = tempDir.path() + "/linescan_output.cub";
  QString excludeFile = tempDir.path() + "/linescan_exclude.txt";

  QVector<QString> args = {
    "fromlist=" + fromList,
    "to=" + outputCube,
    "stdevtol=190",
    "imagetype=LINESCAN",
    "numlines=10",
    "exclude=" + excludeFile
  };

  UserInterface ui(APP_XML, args);

  try {
    makeflat(ui);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString();
  }

  Cube output(outputCube);
  EXPECT_EQ(output.sampleCount(), 5000);
  EXPECT_EQ(output.lineCount(), 1);
  EXPECT_EQ(output.bandCount(), 1);

  std::unique_ptr<Statistics> stats(output.statistics());
  EXPECT_EQ(stats->ValidPixels(), 5000);
  EXPECT_NEAR(stats->Average(), 1.0, 0.01);
  EXPECT_GT(stats->Minimum(), 0.9);
  EXPECT_LT(stats->Maximum(), 1.1);
  EXPECT_LT(stats->StandardDeviation(), 0.1);

  EXPECT_TRUE(QFile::exists(excludeFile));

  Pvl excludePvl(excludeFile);
  EXPECT_TRUE(excludePvl.hasGroup("ExcludedFiles"));
  PvlGroup excludedFilesGrp = excludePvl.findGroup("ExcludedFiles");
  EXPECT_EQ(excludedFilesGrp.keywords(), 0);

  output.close();
}

TEST_F(MakeflatTest, FunctionalTestMakeflatPushframe) {
  // Create synthetic pushframe cubes with 100 lines each (2 framelets of height 50)
  // First two cubes are within tolerance, third has excessive variance to test exclusion
  QString input1 = tempDir.path() + "/pushframe_input1.cub";
  QString input2 = tempDir.path() + "/pushframe_input2.cub";
  QString input3 = tempDir.path() + "/pushframe_input3.cub";

  // Linear gradient with sampMult tuned: first two within tolerance, third exceeds STDEVTOL=150
  // For 5000 samples: stdev ≈ sampMult * 1443
  createLinearPatternCube(input1, 5000, 100, 1, 900.0, 0.0, 0.055, 0.0);   // stdev ~79
  createLinearPatternCube(input2, 5000, 100, 1, 950.0, 0.0, 0.055, 0.0);   // stdev ~79
  createLinearPatternCube(input3, 5000, 100, 1, 1000.0, 0.0, 0.108, 0.0);  // stdev ~156, exceeds tolerance

  QStringList inputCubes;
  inputCubes << input1 << input2 << input3;

  QString fromList = createFromList(inputCubes);
  QString outputCube = tempDir.path() + "/pushframe_output.cub";
  QString excludeFile = tempDir.path() + "/pushframe_exclude.pvl";

  QVector<QString> args = {
    "fromlist=" + fromList,
    "to=" + outputCube,
    "stdevtol=150",
    "imagetype=PUSHFRAME",
    "frameletheight=50",
    "exclude=" + excludeFile
  };

  UserInterface ui(APP_XML, args);

  try {
    makeflat(ui);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString();
  }

  Cube output(outputCube);
  EXPECT_EQ(output.sampleCount(), 5000);
  EXPECT_EQ(output.lineCount(), 50);
  EXPECT_EQ(output.bandCount(), 1);

  std::unique_ptr<Statistics> stats(output.statistics());
  EXPECT_EQ(stats->ValidPixels(), 250000);
  EXPECT_NEAR(stats->Average(), 1.0, 0.01);
  EXPECT_GT(stats->Minimum(), 0.85);
  EXPECT_LT(stats->Maximum(), 1.15);
  EXPECT_LT(stats->StandardDeviation(), 0.15);

  EXPECT_TRUE(QFile::exists(excludeFile));

  Pvl excludePvl(excludeFile);
  EXPECT_TRUE(excludePvl.hasGroup("ExcludedFiles"));
  PvlGroup excludedFilesGrp = excludePvl.findGroup("ExcludedFiles");
  EXPECT_EQ(excludedFilesGrp.keywords(), 1);
  EXPECT_TRUE(excludedFilesGrp.hasKeyword("File"));
  EXPECT_TRUE(excludedFilesGrp["File"][0].contains("pushframe_input3.cub"));

  output.close();
}
