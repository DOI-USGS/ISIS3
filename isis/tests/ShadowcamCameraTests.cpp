#include "ShadowCamCamera.h"
#include "CameraFixtures.h"
#include "TestUtilities.h"

#include <gtest/gtest.h>

using namespace Isis;

TEST_F(ShadowCamCube, ShadowCamCameraUnitTest) {

  ShadowCamCamera *cam = (ShadowCamCamera *)testCube->camera();

  EXPECT_EQ(cam->CkFrameId(), -155151);
  EXPECT_EQ(cam->CkReferenceId(), 1);
  EXPECT_EQ(cam->SpkTargetId(), -155);
  EXPECT_EQ(cam->SpkReferenceId(), 1);

  EXPECT_NEAR(cam->FocalLength(), 699.275, 0.0001);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameLong(), "KOREA PATHFINDER LUNAR ORBITER");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameShort(), "KPLO");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameLong(), "KOREA PATHFINDER LUNAR ORBITER SHADOWCAM");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameShort(), "KPLO SHADOWCAM");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentId(), "ShadowCam");


  EXPECT_TRUE(cam->SetImage(100, 1));
  EXPECT_NEAR(cam->UniversalLatitude(), -87.761413345430555, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 279.37204310540994, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 100, 0.0001);
  EXPECT_NEAR(cam->Line(), 1, 0.0001);

  EXPECT_TRUE(cam->SetImage(3000, 1));
  EXPECT_NEAR(cam->UniversalLatitude(), -87.702202706595187, 0.0001);
  EXPECT_NEAR(cam->UniversalLongitude(), 283.49470753590305, 0.0001);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 3000, 0.0001);
  EXPECT_NEAR(cam->Line(), 1, 0.0001);
 }