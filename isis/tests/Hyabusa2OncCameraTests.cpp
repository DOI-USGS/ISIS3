#include <QDebug>

#include <iomanip>
#include <iostream>

#include "Hyb2OncCamera.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "IException.h"
#include "iTime.h"
#include "Preference.h"
#include "Pvl.h"
#include "TestUtilities.h"
#include "PvlGroup.h"
#include "Fixtures.h"

#include "gmock/gmock.h"

using namespace std;
using namespace Isis;

void testCamera(Cube &c, double knownLat, double knownLon,
                double s1, double l1, 
                double s2, double l2, 
                double s3, double l3, 
                double s4, double l4);

void testLineSamp(Camera *cam, double sample, double lne);

TEST_F(Hayabusa2OncW2Cube, Hayabusa2OncCameraW1CameraTest) {
  setInstrument("-37110", "ONC-W1", "HAYABUSA-2");
  Camera *cam = testCube->camera();
  EXPECT_EQ(cam->CkFrameId(), -37000);
  EXPECT_EQ(cam->CkReferenceId(), 1);
  EXPECT_EQ(cam->SpkTargetId(), -37);
  EXPECT_EQ(cam->SpkReferenceId(), 1);
   
  // Test name methods
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameLong(), "Hayabusa2");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual,cam->spacecraftNameShort(), "Hayabusa2");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual,cam->instrumentNameLong(), "Optical Navigation Camera - W1 Camera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual,cam->instrumentNameShort(), "ONC-W1");

  testCamera(*testCube, 11.215766294778371, 257.19997053715304, 51.0, 42.0, 173.0, 21.0, 54.0, 149.0, 174.0, 155.0);
}


TEST_F(Hayabusa2OncW2Cube, Hayabusa2OncCameraW2CameraTest) {
  setInstrument("-37120", "ONC-W2", "HAYABUSA-2");
   
  Camera *cam = testCube->camera();
  EXPECT_EQ(cam->CkFrameId(), -37000);
  EXPECT_EQ(cam->CkReferenceId(), 1);
  EXPECT_EQ(cam->SpkTargetId(), -37);
  EXPECT_EQ(cam->SpkReferenceId(), 1);
   
  // Test name methods
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameLong(), "Hayabusa2");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual,cam->spacecraftNameShort(), "Hayabusa2");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual,cam->instrumentNameLong(), "Optical Navigation Camera - W2 Camera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual,cam->instrumentNameShort(), "ONC-W2");

  testCamera(*testCube, 11.215766294778371, 257.19997053715304, 51.0, 42.0, 173.0, 21.0, 54.0, 149.0, 174.0, 155.0);

  
}

TEST_F(Hayabusa2OncW2Cube, Hayabusa2OncCameraTelecopicCameraTest) {
  setInstrument("-37100", "ONC-T", "HAYABUSA-2");

  Camera *cam = testCube->camera(); 
  EXPECT_EQ(cam->CkFrameId(), -37000);
  EXPECT_EQ(cam->CkReferenceId(), 1);
  EXPECT_EQ(cam->SpkTargetId(), -37);
  EXPECT_EQ(cam->SpkReferenceId(), 1);
   
  // Test name methods
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->spacecraftNameLong(), "Hayabusa2");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual,cam->spacecraftNameShort(), "Hayabusa2");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual,cam->instrumentNameLong(), "Optical Navigation Camera - Telescopic Camera");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual,cam->instrumentNameShort(), "ONC-T");

  testCamera(*testCube, 11.215766294778371, 257.19997053715304, 51.0, 42.0, 173.0, 21.0, 54.0, 149.0, 174.0, 155.0);
}

void testCamera(Cube &c,
                double knownLat, double knownLon,
                double s1, double l1, 
                double s2, double l2, 
                double s3, double l3, 
                double s4, double l4) {
  
  FramingCamera *cam = (FramingCamera*) CameraFactory::Create(c);
  
  // Test Shutter Open/Close 
  const PvlGroup &inst = c.label()->findGroup("Instrument", Pvl::Traverse);
  double exposureDuration = ((double) inst["ExposureDuration"])/1000;
  QString stime = inst["StartTime"];
  double et; // StartTime keyword is the center exposure time
  str2et_c(stime.toLatin1().data(), &et);
  pair <iTime, iTime> shuttertimes = cam->ShutterOpenCloseTimes(et, exposureDuration);
  
  EXPECT_DOUBLE_EQ(shuttertimes.first.Et(), 502399866.4151246);
  EXPECT_DOUBLE_EQ(shuttertimes.second.Et(), 502399866.41512734);

  // Test all four corners to make sure the conversions are right
  testLineSamp(cam, s1, l1);
  testLineSamp(cam, s2, l2);
  testLineSamp(cam, s3, l3);
  testLineSamp(cam, s4, l4);
  
  if (!cam->SetImage((cam->Samples()/2.0), (cam->Lines()/2.0))) {
    throw IException(IException::Unknown, "ERROR setting image to known position.", _FILEINFO_);
  }

  EXPECT_NEAR(cam->UniversalLatitude(), knownLat,  1E-10);
  EXPECT_NEAR(cam->UniversalLongitude(), knownLon,  1E-10);
  testLineSamp( cam, (cam->Samples()/2.0), (cam->Lines()/2.0) );
}


void testLineSamp(Camera *cam, double sample, double line) {
  bool success = cam->SetImage(sample, line);
  double lat;
  double lon;

  if (success) {
    lat = cam->UniversalLatitude();
    lon = cam->UniversalLongitude();
    success = cam->SetUniversalGround(lat, lon);
  }
  else {
    FAIL() << "Failed to set sample/line (Line: " << line << ", Sample: " << sample << ")." ;
  }

  if (success) {
    EXPECT_NEAR(sample, cam->Sample(), .001); 
    EXPECT_NEAR(line, cam->Line(), .001);
  }
  else {
    FAIL() << "Failed to set lat/lon (Lat: " << lat << ", Lon: " << lon << ")." ;
  }
}
