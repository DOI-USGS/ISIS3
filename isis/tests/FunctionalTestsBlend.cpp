/**
 * Functional tests for blend app
 */

#include "blend.h"

#include <QTemporaryDir>
#include <QString>

#include "Cube.h"
#include "FileName.h"
#include "FileList.h"
#include "Histogram.h"
#include "Pvl.h"
#include "TempFixtures.h"
#include "TestUtilities.h"
#include "UserInterface.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/blend.xml").expanded();

class BlendTest : public TempTestingFiles {
protected:
  QString testDataDir;

  void SetUp() override {
    TempTestingFiles::SetUp();
    // Reduced overlapping projected fixtures for tests needing
    // map-projected cubes with overlapping footprints.
    testDataDir = QString(_SOURCE_PREFIX) + "/data/overlapping_projected";
  }

  QString testDataPath(const QString &fileName) const {
    return testDataDir + "/" + fileName;
  }

  QString writeFileList(const QStringList &files, const QString &listName) {
    QString listPath = tempDir.path() + "/" + listName;
    FileList fileList;
    for (const QString &f : files) {
      fileList.append(FileName(f));
    }
    fileList.write(FileName(listPath));
    return listPath;
  }
};

TEST_F(BlendTest, FunctionalTestBlendDefault) {
  QString input1 = testDataPath("I00824006RDR.lev2.cub");
  QString input2 = testDataPath("I01523019RDR.lev2.cub");
  QString input3 = testDataPath("I02609002RDR.lev2.cub");

  ASSERT_TRUE(QFile::exists(input1)) << "Test cube 1 not found: " << input1.toStdString();
  ASSERT_TRUE(QFile::exists(input2)) << "Test cube 2 not found: " << input2.toStdString();
  ASSERT_TRUE(QFile::exists(input3)) << "Test cube 3 not found: " << input3.toStdString();

  QString output1 = tempDir.path() + "/I00824006RDR.lev2.blend.cub";
  QString output2 = tempDir.path() + "/I01523019RDR.lev2.blend.cub";
  QString output3 = tempDir.path() + "/I02609002RDR.lev2.blend.cub";

  QString fromList = writeFileList({input1, input2, input3}, "fromlist.lis");
  QString toList = writeFileList({output1, output2, output3}, "tolist.lis");

  QVector<QString> args = {
    "fromlist=" + fromList,
    "tolist=" + toList,
    "stop=200"
  };

  UserInterface ui(APP_XML, args);

  try {
    blend(ui);
  }
  catch (IException &e) {
    FAIL() << "blend threw exception: " << e.toString().toStdString();
  }

  ASSERT_TRUE(QFile::exists(output1)) << "Output 1 not created";
  ASSERT_TRUE(QFile::exists(output2)) << "Output 2 not created";
  ASSERT_TRUE(QFile::exists(output3)) << "Output 3 not created";

  Cube cube1(output1);
  EXPECT_EQ(cube1.sampleCount(), 82);
  EXPECT_EQ(cube1.lineCount(), 362);
  EXPECT_EQ(cube1.bandCount(), 10);

  Pvl *label1 = cube1.label();
  PvlGroup &mapping1 = label1->findGroup("Mapping", Pvl::Traverse);
  EXPECT_EQ(mapping1["ProjectionName"][0].toStdString(), "Sinusoidal");

  EXPECT_PRED_FORMAT5(AssertHistogramStats, cube1, 0.00036904116308585625, 4.2196166587236803, 11434, 4.0754605754043669e-05);
  cube1.close();

  Cube cube2(output2);
  EXPECT_EQ(cube2.sampleCount(), 82);
  EXPECT_EQ(cube2.lineCount(), 361);
  EXPECT_EQ(cube2.bandCount(), 10);

  EXPECT_PRED_FORMAT5(AssertHistogramStats, cube2, 0.00026874944683968313, 3.0704624301433796, 11425, 6.3434086914089142e-05);
  cube2.close();

  Cube cube3(output3);
  EXPECT_EQ(cube3.sampleCount(), 130);
  EXPECT_EQ(cube3.lineCount(), 718);
  EXPECT_EQ(cube3.bandCount(), 10);

  EXPECT_PRED_FORMAT5(AssertHistogramStats, cube3, 0.00017191059729849303, 3.0964536785404562, 18012, 5.3562699228936043e-05);
  cube3.close();
}

TEST_F(BlendTest, FunctionalTestBlendThemis) {
  QString input1 = testDataPath("I23851018_crop.cub");
  QString input2 = testDataPath("I01086005_crop.cub");

  ASSERT_TRUE(QFile::exists(input1)) << "Test cube 1 not found: " << input1.toStdString();
  ASSERT_TRUE(QFile::exists(input2)) << "Test cube 2 not found: " << input2.toStdString();

  QString output1 = tempDir.path() + "/I23851018_crop.blend.cub";
  QString output2 = tempDir.path() + "/I01086005_crop.blend.cub";

  QString fromList = writeFileList({input1, input2}, "fromlist.lis");
  QString toList = writeFileList({output1, output2}, "tolist.lis");

  QVector<QString> args = {
    "fromlist=" + fromList,
    "tolist=" + toList,
    "stop=200"
  };

  UserInterface ui(APP_XML, args);

  try {
    blend(ui);
  }
  catch (IException &e) {
    FAIL() << "blend threw exception: " << e.toString().toStdString();
  }

  ASSERT_TRUE(QFile::exists(output1)) << "Output 1 not created";
  ASSERT_TRUE(QFile::exists(output2)) << "Output 2 not created";

  Cube cube1(output1);
  EXPECT_EQ(cube1.sampleCount(), 200);
  EXPECT_EQ(cube1.lineCount(), 400);
  EXPECT_EQ(cube1.bandCount(), 1);

  Pvl *label1 = cube1.label();
  PvlGroup &mapping1 = label1->findGroup("Mapping", Pvl::Traverse);
  EXPECT_EQ(mapping1["ProjectionName"][0].toStdString(), "SimpleCylindrical");

  double minLat = mapping1["MinimumLatitude"];
  double maxLat = mapping1["MaximumLatitude"];
  EXPECT_NEAR(minLat, -0.498, 0.01);
  EXPECT_NEAR(maxLat, 0.177, 0.01);

  EXPECT_PRED_FORMAT5(AssertHistogramStats, cube1, 11.777417753459691, 942158.08802351495, 79997, 13.931134662682158);
  cube1.close();

  Cube cube2(output2);
  EXPECT_EQ(cube2.sampleCount(), 200);
  EXPECT_EQ(cube2.lineCount(), 400);
  EXPECT_EQ(cube2.bandCount(), 1);

  EXPECT_PRED_FORMAT5(AssertHistogramStats, cube2, 14.975943125451321, 1019472.3523219733, 68074, 11.242490621623574);
  cube2.close();
}
