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
 * Regression test for the VariableLineScanCameraDetectorMap round-trip near a
 * line-rate change. HRSC scans with a variable line rate, so the camera uses
 * VariableLineScanCameraDetectorMap. SetDetector (time -> line) used to select
 * the rate section by comparing the ephemeris time against GetStartEt() - 0.5,
 * where the 0.5 is the half-pixel offset of the line convention, not a time. In
 * the window just before a rate change this mis-selected the section and broke
 * the image <-> ground round trip.
 *
 * In this cube the first line-rate change is at line 633; line 585 is inside
 * that window and round-tripped about 1 pixel off before the fix.
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
  // The round trip is ~1e-5 px with the fix and was ~1 px off without it, so
  // this tolerance is generous for portability while still catching the bug.
  EXPECT_NEAR(cam->Sample(), sample, 0.02);
  EXPECT_NEAR(cam->Line(), line, 0.02);

  delete cam;
}
