#include "Camera.h"
#include "CameraFactory.h"
#include "Cube.h"
#include "IException.h"
#include "TestUtilities.h"

#include <QString>

#include <gtest/gtest.h>

using namespace Isis;

/**
 * Test the Mars Express HRSC camera model.
 *
 * This replaces the legacy objs/HrscCamera unitTest. It reuses the existing
 * HRSC stereo-channel cube from the ISIS test data (no new test cube is added).
 */
TEST(HrscCameraTests, HrscCameraUnitTest) {
  Cube cube("$ISISTESTDATA/isis/src/mex/unitTestData/h2254_0000_s12.cub", "r");
  Camera *cam = CameraFactory::Create(cube);

  EXPECT_EQ(cam->CkFrameId(), -41001);
  EXPECT_EQ(cam->CkReferenceId(), 1);
  EXPECT_EQ(cam->SpkTargetId(), -41);
  EXPECT_EQ(cam->SpkReferenceId(), 1);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameLong(), "Mars Express");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameShort(), "MEX");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameLong(), "High Resolution Stereo Camera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameShort(), "HRSC");

  // The four corners: image -> ground -> image must be self-consistent.
  double nSamps = cam->Samples();
  double nLines = cam->Lines();
  double corners[4][2] = {{1.0, 1.0}, {nSamps, 1.0}, {1.0, nLines}, {nSamps, nLines}};
  for (int i = 0; i < 4; i++) {
    double sample = corners[i][0];
    double line = corners[i][1];
    ASSERT_TRUE(cam->SetImage(sample, line));
    ASSERT_TRUE(cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude()));
    EXPECT_NEAR(cam->Sample(), sample, 0.02);
    EXPECT_NEAR(cam->Line(), line, 0.02);
  }

  // Center pixel: known ground point.
  ASSERT_TRUE(cam->SetImage(nSamps / 2, nLines / 2));
  EXPECT_NEAR(cam->UniversalLatitude(), -62.4809504564515166, 2.7E-5);
  EXPECT_NEAR(cam->UniversalLongitude(), 48.4111258061244243, 2.1E-6);

  delete cam;
}

/**
 * Regression test for the line/time round trip of a variable-rate line scanner.
 *
 * HRSC does not scan at a constant rate: the line exposure time changes in a few
 * discrete steps partway down the image, so the camera uses
 * VariableLineScanCameraDetectorMap. Mapping a pixel to the ground and back,
 * image -> ground -> image, must return the original pixel.
 *
 * The bug was in the ground -> image direction. When the ephemeris time landed
 * shortly before one of the rate changes, SetDetector selected the wrong rate
 * section, so the round trip did not close. The sample and line below are picked
 * to fall in that window, just ahead of a rate change for this cube; there the
 * round trip was off by about a pixel before the fix and closes to a small
 * fraction of a pixel after it. The check is that the round trip closes, not any
 * absolute coordinate.
 */
TEST(HrscCameraTests, VariableLineRateRoundTrip) {
  Cube cube("$ISISTESTDATA/isis/src/mex/unitTestData/h2254_0000_s12.cub", "r");
  Camera *cam = CameraFactory::Create(cube);

  double sample = 1300.0;
  double line = 585.0;
  ASSERT_TRUE(cam->SetImage(sample, line));
  double lat = cam->UniversalLatitude();
  double lon = cam->UniversalLongitude();
  ASSERT_TRUE(cam->SetUniversalGround(lat, lon));
  // Require the round trip to close. Without the fix it missed by about a pixel
  // here; with it the error is a tiny fraction of a pixel. The tolerance is kept
  // loose enough to stay portable across platforms.
  EXPECT_NEAR(cam->Sample(), sample, 0.02);
  EXPECT_NEAR(cam->Line(), line, 0.02);

  delete cam;
}
