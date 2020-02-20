#include "cnetstats.h"

#include "Fixtures.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/cnetstats.xml").expanded();

TEST_F(ThreeImageNetwork, FunctionalTestCnetstatsDefault) {
  QVector<QString> args = {};
  UserInterface options(APP_XML, args);
  Pvl log;
  QString serialNumList = cubeListTempPath.fileName();

  cnetstats(*network, serialNumList, options, &log);

  PvlGroup cnetSummary = log.findGroup("ControlNetSummary");
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("TotalImages"), 3);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("ImagesInControlNet"), 3);

  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("TotalPoints"), 16);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("ValidPoints"), 16);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("IgnoredPoints"), 0);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("FixedPoints"), 0);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("ConstrainedPoints"), 0);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("FreePoints"), 16);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("EditLockPoints"), 0);

  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("TotalMeasures"), 41);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("ValidMeasures"), 41);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("IgnoredMeasures"), 0);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("EditLockMeasures"), 0);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cnetSummary.findKeyword("AvgResidual"), "Null");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cnetSummary.findKeyword("MinResidual"), "Null");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cnetSummary.findKeyword("MaxResidual"), "Null");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cnetSummary.findKeyword("MinLineResidual"), "Null");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cnetSummary.findKeyword("MaxLineResidual"), "Null");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cnetSummary.findKeyword("MinSampleResidual"), "Null");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cnetSummary.findKeyword("MaxSampleResidual"), "Null");

  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("MinLineShift"), 0);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("MaxLineShift"), 0);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("MinSampleShift"), 0);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("MaxSampleShift"), 0);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("AvgPixelShift"), 0);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("MinPixelShift"), 0);
  EXPECT_DOUBLE_EQ( (double) cnetSummary.findKeyword("MaxPixelShift"), 0);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cnetSummary.findKeyword("MinGoodnessOfFit"), "NA");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cnetSummary.findKeyword("MaxGoodnessOfFit"), "NA");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cnetSummary.findKeyword("MinEccentricity"), "NA");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cnetSummary.findKeyword("MaxEccentricity"), "NA");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cnetSummary.findKeyword("MinPixelZScore"), "NA");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cnetSummary.findKeyword("MaxPixelZScore"), "NA");

  EXPECT_NEAR( (double) cnetSummary.findKeyword("MinConvexHullRatio"), 0.40388096067313, 1e-10);
  EXPECT_NEAR( (double) cnetSummary.findKeyword("MaxConvexHullRatio"), 0.60732301576372, 1e-10);
  EXPECT_NEAR( (double) cnetSummary.findKeyword("AvgConvexHullRatio"), 0.47889221036267, 1e-10);
}