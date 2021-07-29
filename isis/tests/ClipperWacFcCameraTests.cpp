#include <gtest/gtest.h>
#include "Fixtures.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "Cube.h"
#include "ClipperWacFcCamera.h"
#include "IException.h"
#include "iTime.h"
#include "TestUtilities.h"

using namespace Isis;

void TestLineSamp(Camera *cam, double samp, double line) {
  bool success = cam->SetImage(samp, line);
  EXPECT_TRUE(success);

  if(success) {
    success = cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude());
  }
  EXPECT_TRUE(success);

  if(success) {
    EXPECT_NEAR(samp, cam->Sample(), 1.1e-2);
    EXPECT_NEAR(line, cam->Line(), 1.0e-2);
  }
}

void TestImageToGroundToImage(Camera *cam, double samp, double line, double lat, double lon){
  ASSERT_TRUE(cam->SetImage(samp, line));
  EXPECT_DOUBLE_EQ(cam->UniversalLatitude(), lat);
  EXPECT_DOUBLE_EQ(cam->UniversalLongitude(), lon);
  ASSERT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
  EXPECT_NEAR(cam->Sample(), samp, 0.001);
  EXPECT_NEAR(cam->Line(), line, 0.001);
}


TEST_F(ClipperWacFcCube, ClipperWacFcCameraUnitTest) {
  ClipperWacFcCamera *cam;
  try {
    cam = (ClipperWacFcCamera *) CameraFactory::Create(*wacFcCube);
  } catch(IException &e) {
    FAIL() << "Unable to create ClipperWacFcCamera" << std::endl;
  }

  // Camera info
  //EXPECT_EQ(cam->instrumentRotation()->Frame(), -27002);    // from the defaultcube isd
  EXPECT_EQ(cam->CkFrameId(), -159000);
  EXPECT_EQ(cam->CkReferenceId(), 1);
  EXPECT_EQ(cam->SpkTargetId(), -159);
  EXPECT_EQ(cam->SpkReferenceId(), 1);
  EXPECT_NEAR(cam->FocalLength(), 150.402, 1e-4);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameLong(), "Europa Clipper");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameShort(), "Clipper");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameLong(), "Europa Imaging System Framing Wide Angle Camera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameShort(), "EIS-FWAC");

  // Check SetImage on corners
  int line, samp, nline, nsamp;
  line = 1.0;     samp = 1.0;
  nline = 1203.0; nsamp = 1055.0;
  TestLineSamp(cam, line, samp);
  TestLineSamp(cam, line, nsamp);
  TestLineSamp(cam, nline, nsamp);
  TestLineSamp(cam, nline, samp);

  TestImageToGroundToImage(cam, 145, 161, 1.0318106954644963, 79.845106263190786);
  TestImageToGroundToImage(cam, 3655, 157, -30.257524123457561, 68.491558949717344);
  TestImageToGroundToImage(cam, 289, 1767, 4.3522464978639448, 65.049067459499526);
  TestImageToGroundToImage(cam, 3767, 1579, -26.240527197710811, 54.405682822879818);

  // Simple test for ClipperWacFcCamera::ShutterOpenCloseTimes
  PvlGroup &inst = wacFcCube->label()->findObject("IsisCube").findGroup("Instrument", Pvl::Traverse);
  QString startTime = inst["StartTime"];
  iTime etStart(startTime);

  std::pair<iTime, iTime> startStop;
  startStop = cam->ShutterOpenCloseTimes(etStart.Et(), 0.00005);  // dummy value for exposure duration
  EXPECT_TRUE(startStop.first.Et() < startStop.second.Et());
}
