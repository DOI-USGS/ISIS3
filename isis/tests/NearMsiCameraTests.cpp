#include "Camera.h"
#include "CameraFixtures.h"
#include "TestUtilities.h"

#include <gtest/gtest.h>

using namespace Isis;

TEST_F(NearMsiCameraCube, NearMsiCameraTest) {

  Camera *cam = testCube->camera();

  EXPECT_EQ(cam->CkFrameId(), -93000);
  EXPECT_EQ(cam->CkReferenceId(), 1);
  EXPECT_EQ(cam->SpkTargetId(), -93);
  EXPECT_EQ(cam->SpkReferenceId(), 1);

  EXPECT_NEAR(cam->FocalLength(), 166.85, 0.0001);
  EXPECT_NEAR(cam->PixelPitch(), 0.016, 0.0001);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameLong(), "Near Earth Asteroid Rendezvous");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameShort(), "NEAR");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameLong(), "Multi-Spectral Imager");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameShort(), "MSI");

  // Upper Left
  EXPECT_TRUE(cam->SetImage(34, 34));
  EXPECT_NEAR(cam->UniversalLatitude(),  -17.686235689292037, 1e-10);
  EXPECT_NEAR(cam->UniversalLongitude(),  36.717548917904146, 1e-10);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 34, 0.001);
  EXPECT_NEAR(cam->Line(), 34, 0.001);

  // Upper Right
  EXPECT_TRUE(cam->SetImage(504, 34));
  EXPECT_NEAR(cam->UniversalLatitude(), -25.741437596768307, 1e-10);
  EXPECT_NEAR(cam->UniversalLongitude(), 34.216663871981211, 1e-10);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 504, 0.001);
  EXPECT_NEAR(cam->Line(), 34, 0.001);

  // Lower Right
  EXPECT_TRUE(cam->SetImage(504, 379));
  EXPECT_NEAR(cam->UniversalLatitude(), -29.649628167745398, 1e-10);
  EXPECT_NEAR(cam->UniversalLongitude(), 38.194196619380435, 1e-10);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 504, 0.001);
  EXPECT_NEAR(cam->Line(), 379, 0.001);

  // Lower Left
  EXPECT_TRUE(cam->SetImage(34, 379));
  EXPECT_NEAR(cam->UniversalLatitude(), -20.18045520120209, 1e-10);
  EXPECT_NEAR(cam->UniversalLongitude(), 40.577715588340105, 1e-10);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 34, 0.001);
  EXPECT_NEAR(cam->Line(), 379, 0.001);

// Lower Left
  EXPECT_TRUE(cam->SetImage(268.5, 206.0));
  EXPECT_NEAR(cam->UniversalLatitude(), -22.852443468381061, 1e-10);
  EXPECT_NEAR(cam->UniversalLongitude(), 37.504660702426833, 1e-10);
  EXPECT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), 268.5, 0.001);
  EXPECT_NEAR(cam->Line(), 206, 0.001);
}

