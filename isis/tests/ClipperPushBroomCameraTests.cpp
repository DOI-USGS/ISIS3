#include "ClipperPushBroomCamera.h"
#include "Fixtures.h"
#include "TestUtilities.h"

#include <gtest/gtest.h>

using namespace Isis;

TEST_F(ClipperPbCube, ClipperPushBroomCameraUnitTest) {

  ClipperPushBroomCamera *cam = (ClipperPushBroomCamera *)testCube->camera();

  EXPECT_EQ(cam->CkFrameId(), -159121);
  EXPECT_EQ(cam->CkReferenceId(), -159010);
  EXPECT_EQ(cam->SpkTargetId(), -159);
  EXPECT_EQ(cam->SpkReferenceId(), 1);

  EXPECT_NEAR(cam->FocalLength(), 150.402, 0.0001);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameLong(), "Europa Clipper");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameShort(), "Clipper");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameLong(), "Europa Imaging System Push Broom Narrow Angle Camera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameShort(), "EIS-PBNAC");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentId(), "EIS-NAC-PB");

  EXPECT_TRUE(cam->SetImage(5, 5));
  EXPECT_NEAR(cam->UniversalLatitude(), 8.6088673336344534, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 253.69764361013753, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 5, 0.0001);
  //EXPECT_NEAR(cam->Line(), 5, 0.0001);
}
