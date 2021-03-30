#include "noproj.h"

#include <QTemporaryFile>
#include <QTextStream>
#include <QStringList>
  
#include "Fixtures.h"
#include "LineManager.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/noproj.xml").expanded();

TEST_F(DefaultCube, FunctionalTestNoprojDefault) {
  // QString cubeFileName = tempDir.path() + "/output.cub";
  QString cubeFileName = "/Users/kdlee/Desktop/output.cub";
  QVector<QString> args = {"to=" + cubeFileName};
  UserInterface options(APP_XML, args);

  // Empty match cube
  noproj(testCube, NULL, options);

  Cube oCube(cubeFileName);
  Pvl *isisLabel = oCube.label();
  PvlGroup instGroup = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("SpacecraftName"), "IdealSpacecraft");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentId"), "IdealCamera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("TargetName"), "MARS");
  EXPECT_EQ((int) instGroup.findKeyword("SampleDetectors"), 1250);
  EXPECT_EQ((int) instGroup.findKeyword("LineDetectors"), 1150);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("InstrumentType"), "FRAMING");
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("EphemerisTime"), -709401200.26114);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, instGroup.findKeyword("StartTime"), "1977-07-09T20:05:51");
  EXPECT_EQ((int) instGroup.findKeyword("FocalPlaneXDependency"), 1);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransX"), 1.0);
  EXPECT_DOUBLE_EQ((double) instGroup.findKeyword("TransY"), 1.0);

  FileName matchedCube(instGroup.findKeyword("MatchedCube"));
  FileName defaultCubeName(testCube->fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, matchedCube.name(), defaultCubeName.name());

  PvlGroup origInst = isisLabel->findGroup("OriginalInstrument", Pvl::Traverse);
  PvlGroup testCubeInst = testCube->label()->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertPvlGroupKeywordsEqual, origInst, testCubeInst); //REFACTOR

  //check histogram

}
