#include "ClipperPushBroomCamera.h"
#include "Fixtures.h"
#include "TestUtilities.h"

#include <gtest/gtest.h>

using namespace Isis;

TEST_F(MroHiriseCube, ClipperPushBroomCameraUnitTest) {

  PvlGroup &kernels = testCube->label()->findObject("IsisCube").findGroup("Kernels");
  kernels.findKeyword("NaifFrameCode").setValue("-159011");

  PvlGroup &inst = testCube->label()->findObject("IsisCube").findGroup("Instrument");
  std::istringstream iss(R"(
    Group = Instrument
      SpacecraftName            = Clipper
      InstrumentId              = EIS-NAC-PB
      TargetName                = Europa
      StartTime                 = 2025-01-01T00:00:00.000
      LineExposureDuration      = 0.337600
    End_Group
  )");

  PvlGroup newInstGroup;
  iss >> newInstGroup;
  inst = newInstGroup;

  PvlObject &naifKeywords = testCube->label()->findObject("NaifKeywords");
  std::istringstream nk(R"(
    Object = NaifKeywords
      BODY_CODE               = 502
      BODY502_RADII           = (1562.6, 1560.3, 1559.5)
      BODY_FRAME_CODE         = 10024
      INS-159011_FOCAL_LENGTH = 150.40199
      INS-159011_PIXEL_PITCH  = 0.014
      INS-159011_TRANSX       = (0.0, 0.014004651, 0.0)
      INS-159011_TRANSY       = (0.0, 0.0, 0.01399535)
      INS-159011_ITRANSS      = (0.0, 71.404849, 0.0)
      INS-159011_ITRANSL      = (0.0, 0.0, 71.4523)
      INS-159011_OD_K         = (0.0, 0.0, 0.0)
    End_Object
  )");

  PvlObject newNaifKeywords;
  nk >> newNaifKeywords;
  naifKeywords = newNaifKeywords;

  ClipperPushBroomCamera *cam = (ClipperPushBroomCamera *)testCube->camera();

  EXPECT_EQ(cam->CkFrameId(), -159011);
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
  EXPECT_NEAR(cam->Line(), -2336971768909.2378, 0.0001);
}
