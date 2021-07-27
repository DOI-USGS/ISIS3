#include "ClipperPushBroomCamera.h"
#include "Fixtures.h"
#include "TestUtilities.h"

#include <gtest/gtest.h>

using namespace Isis;

TEST_F(ClipperPbCube, ClipperPushBroomCameraNacTest) {
  setInstrument("-159101", "EIS-NAC-PB");

  ClipperPushBroomCamera *cam = (ClipperPushBroomCamera *)testCube->camera();

  EXPECT_EQ(cam->CkFrameId(), -159000);
  EXPECT_EQ(cam->CkReferenceId(), 1);
  EXPECT_EQ(cam->SpkTargetId(), -159);
  EXPECT_EQ(cam->SpkReferenceId(), 1);

  EXPECT_NEAR(cam->FocalLength(), 150.402, 0.0001);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameLong(), "Europa Clipper");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameShort(), "Clipper");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameLong(), "Europa Imaging System Push Broom Narrow Angle Camera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameShort(), "EIS-PBNAC");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentId(), "EIS-NAC-PB");

  EXPECT_TRUE(cam->SetImage(1, 1));
  EXPECT_NEAR(cam->UniversalLatitude(), 1.2968475577607894, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 80.39050360283612, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 1, 0.01);
  EXPECT_NEAR(cam->Line(), 1, 0.01);

  EXPECT_TRUE(cam->SetImage(1, 2048));
  EXPECT_NEAR(cam->UniversalLatitude(), 1.2968475577607894, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 80.39050360283612, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 1, 0.01);
  EXPECT_NEAR(cam->Line(), 2048, 0.01);

  EXPECT_TRUE(cam->SetImage(4096, 2048));
  EXPECT_NEAR(cam->UniversalLatitude(), -34.89353443307678, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 67.50276195879313, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 4096, 0.01);
  EXPECT_NEAR(cam->Line(), 2048, 0.01);

  EXPECT_TRUE(cam->SetImage(4096, 1));
  EXPECT_NEAR(cam->UniversalLatitude(), -34.89353443307678, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 67.50276195879313, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 4096, 0.01);
  EXPECT_NEAR(cam->Line(), 1, 0.01);
}

TEST_F(ClipperPbCube, ClipperPushBroomCameraWacTest) {
  setInstrument("-159102", "EIS-WAC-PB");

  ClipperPushBroomCamera *cam = (ClipperPushBroomCamera *)testCube->camera();

  EXPECT_EQ(cam->CkFrameId(), -159000);
  EXPECT_EQ(cam->CkReferenceId(), 1);
  EXPECT_EQ(cam->SpkTargetId(), -159);
  EXPECT_EQ(cam->SpkReferenceId(), 1);

  EXPECT_NEAR(cam->FocalLength(), 150.402, 0.0001);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameLong(), "Europa Clipper");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameShort(), "Clipper");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameLong(), "Europa Imaging System Push Broom Wide Angle Camera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameShort(), "EIS-PBWAC");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentId(), "EIS-WAC-PB");

  EXPECT_TRUE(cam->SetImage(1, 1));
  EXPECT_NEAR(cam->UniversalLatitude(), 1.5088267433142744, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 81.378861952389798, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 1, 0.01);
  EXPECT_NEAR(cam->Line(), 1, 0.01);

  EXPECT_TRUE(cam->SetImage(1, 2048));
  EXPECT_NEAR(cam->UniversalLatitude(), 1.5088267433142744, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 81.378861952389798, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 1, 0.01);
  EXPECT_NEAR(cam->Line(), 2048, 0.01);

  EXPECT_TRUE(cam->SetImage(4096, 2048));
  EXPECT_NEAR(cam->UniversalLatitude(), -34.782957391682949, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 68.590363919992924, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 4096, 0.01);
  EXPECT_NEAR(cam->Line(), 2048, 0.01);

  EXPECT_TRUE(cam->SetImage(4096, 1));
  EXPECT_NEAR(cam->UniversalLatitude(), -34.782957391682949, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 68.590363919992924, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 4096, 0.01);
  EXPECT_NEAR(cam->Line(), 1, 0.01);
}
