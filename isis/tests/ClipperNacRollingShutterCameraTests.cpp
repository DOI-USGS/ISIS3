#include "ClipperNacRollingShutterCamera.h"
#include "Fixtures.h"
#include "TestUtilities.h"

#include <gtest/gtest.h>

using namespace Isis;

TEST_F(ClipperNacRsCube, ClipperNacRsCameraUnitTest) {

  ClipperNacRollingShutterCamera *cam = (ClipperNacRollingShutterCamera *)testCube->camera();

  EXPECT_EQ(cam->CkFrameId(), -159011);
  EXPECT_EQ(cam->CkReferenceId(), -159010);
  EXPECT_EQ(cam->SpkTargetId(), -159);
  EXPECT_EQ(cam->SpkReferenceId(), 1);

  EXPECT_NEAR(cam->FocalLength(), 150.402, 0.0001);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameLong(), "Europa Clipper");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameShort(), "Clipper");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameLong(), "Europa Imaging System Rolling Shutter Narrow Angle Camera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameShort(), "EIS-RSNAC");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentId(), "EIS-NAC-RS");


  EXPECT_TRUE(cam->SetImage(145, 161));
  EXPECT_NEAR(cam->UniversalLatitude(), 10.248688804675723, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 256.15486019464515, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 145, 0.0001);
  EXPECT_NEAR(cam->Line(), 161, 0.0001);

  EXPECT_TRUE(cam->SetImage(3655, 157));
  EXPECT_NEAR(cam->UniversalLatitude(), 14.250132025597672, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 258.44639101206752, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 3655, 0.0001);
  EXPECT_NEAR(cam->Line(), 157, 0.0001);

  EXPECT_TRUE(cam->SetImage(289, 1767));
  EXPECT_NEAR(cam->UniversalLatitude(), 9.4776705775142567, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 258.17321108594587, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 289, 0.0001);
  EXPECT_NEAR(cam->Line(), 1767, 0.001);

  EXPECT_TRUE(cam->SetImage(3767, 1579));
  EXPECT_NEAR(cam->UniversalLatitude(), 13.641265011679993, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 260.38986965108342, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 3767, 0.0001);
  EXPECT_NEAR(cam->Line(), 1579, 0.001);
 }
