#include "ClipperNacRollingShutterCamera.h"
#include "CameraFixtures.h"
#include "TestUtilities.h"

#include <gtest/gtest.h>

using namespace Isis;

TEST_F(ClipperNacRsCube, ClipperNacRsCameraUnitTest) {

  ClipperNacRollingShutterCamera *cam = (ClipperNacRollingShutterCamera *)testCube->camera();

  EXPECT_EQ(cam->CkFrameId(), -159000);
  EXPECT_EQ(cam->CkReferenceId(), 1);
  EXPECT_EQ(cam->SpkTargetId(), -159);
  EXPECT_EQ(cam->SpkReferenceId(), 1);

  EXPECT_NEAR(cam->FocalLength(), 150.402, 0.0001);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameLong(), "Europa Clipper");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameShort(), "Clipper");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameLong(), "Europa Imaging System Rolling Shutter Narrow Angle Camera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameShort(), "EIS-RSNAC");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentId(), "EIS-NAC-RS");


  EXPECT_TRUE(cam->SetImage(145, 161));
  EXPECT_NEAR(cam->UniversalLatitude(), 8.6601675738056922, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 253.94913698482958, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 145, 0.0001);
  EXPECT_NEAR(cam->Line(), 161, 0.0001);

  EXPECT_TRUE(cam->SetImage(3655, 157));
  EXPECT_NEAR(cam->UniversalLatitude(), 12.393863983217367, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 255.89185956199307, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 3655, 0.0001);
  EXPECT_NEAR(cam->Line(), 157, 0.0001);

  EXPECT_TRUE(cam->SetImage(289, 1767));
  EXPECT_NEAR(cam->UniversalLatitude(), 7.8819000470364564, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 255.75554569654594, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 289, 0.0001);
  EXPECT_NEAR(cam->Line(), 1767, 0.001);

  EXPECT_TRUE(cam->SetImage(3767, 1579));
  EXPECT_NEAR(cam->UniversalLatitude(), 11.788225243842827, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 257.62075252064386, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 3767, 0.0001);
  EXPECT_NEAR(cam->Line(), 1579, 0.001);
 }
