#include <iostream>
#include <QTemporaryFile>

#include "cam2map.h"

#include "Cube.h"
#include "CubeAttribute.h"
#include "PixelType.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "TestUtilities.h"
#include "FileName.h"

#include "Fixtures.h"
#include "Mocks.h"

using namespace Isis;
using ::testing::Return;

static QString APP_XML = FileName("$ISISROOT/bin/xml/cam2map.xml").expanded();

TEST_F(DefaultCube, FunctionalTestCam2mapDefault) {
  std::istringstream labelStrm(R"(
    Group = Mapping
      ProjectionName  = Sinusoidal
      CenterLongitude = 0.0 <degrees>

      TargetName         = MARS
      EquatorialRadius   = 3396190.0 <meters>
      PolarRadius        = 3376200.0 <meters>

      LatitudeType       = Planetocentric
      LongitudeDirection = PositiveEast
      LongitudeDomain    = 360 <degrees>

      MinimumLatitude    = 0 <degrees>
      MaximumLatitude    = 10 <degrees>
      MinimumLongitude   = 0 <degrees>
      MaximumLongitude   = 10 <degrees>

      PixelResolution    = 100000 <meters/pixel>
      Scale              = 512.0 <pixels/degree>
    End_Group
  )");

  Pvl userMap;
  labelStrm >> userMap;
  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  QVector<QString> args = {"to="+tempDir.path()+"/level2.cub", "matchmap=yes"};
  UserInterface ui(APP_XML, args);

  Pvl log;

  cam2map(testCube, userMap, userGrp, ui, &log);
  Cube ocube(tempDir.path()+"/level2.cub");

  PvlGroup cubeMapGroup = ocube.label()->findGroup("Mapping", Pvl::Traverse);

  ASSERT_EQ(cubeMapGroup.findKeyword("ProjectionName"), userGrp.findKeyword("ProjectionName"));
  ASSERT_EQ(cubeMapGroup.findKeyword("CenterLongitude"), userGrp.findKeyword("CenterLongitude"));

  ASSERT_EQ(cubeMapGroup.findKeyword("TargetName"), userGrp.findKeyword("TargetName"));
  ASSERT_EQ(cubeMapGroup.findKeyword("EquatorialRadius"), userGrp.findKeyword("EquatorialRadius"));
  ASSERT_EQ(cubeMapGroup.findKeyword("PolarRadius"), userGrp.findKeyword("PolarRadius"));

  ASSERT_EQ(cubeMapGroup.findKeyword("LatitudeType"), userGrp.findKeyword("LatitudeType"));
  ASSERT_EQ(cubeMapGroup.findKeyword("LongitudeDirection"), userGrp.findKeyword("LongitudeDirection"));
  ASSERT_EQ(cubeMapGroup.findKeyword("LongitudeDomain"), userGrp.findKeyword("LongitudeDomain"));

  ASSERT_EQ(cubeMapGroup.findKeyword("MinimumLatitude"), userGrp.findKeyword("MinimumLatitude"));
  ASSERT_EQ(cubeMapGroup.findKeyword("MaximumLatitude"), userGrp.findKeyword("MaximumLatitude"));
  ASSERT_EQ(cubeMapGroup.findKeyword("MinimumLongitude"), userGrp.findKeyword("MinimumLongitude"));
  ASSERT_EQ(cubeMapGroup.findKeyword("MaximumLongitude"), userGrp.findKeyword("MaximumLongitude"));

  ASSERT_EQ(cubeMapGroup.findKeyword("PixelResolution"), userGrp.findKeyword("PixelResolution"));
  ASSERT_EQ(cubeMapGroup.findKeyword("Scale"), userGrp.findKeyword("Scale"));
}

TEST_F(DefaultCube, ReverseXformUnitTestCam2map) {
  MockCamera camera(*testCube);
  MockTProjection outmap(projLabel);

  Transform *transform;
  transform = new cam2mapReverse(100, 100, &camera, 200, 200, &outmap, 1, 1);

  EXPECT_CALL(outmap, SetWorld(1.0, 1.0)).WillOnce(Return(1));
  EXPECT_CALL(outmap, HasGroundRange()).WillOnce(Return(1));
  EXPECT_CALL(outmap, Latitude()).Times(2).WillRepeatedly(Return(2));
  EXPECT_CALL(outmap, MinimumLatitude()).WillOnce(Return(1));
  EXPECT_CALL(outmap, MaximumLatitude()).WillOnce(Return(10));
  EXPECT_CALL(outmap, Longitude()).Times(2).WillRepeatedly(Return(2));
  EXPECT_CALL(outmap, MinimumLongitude()).WillOnce(Return(1));
  EXPECT_CALL(outmap, MaximumLongitude()).WillOnce(Return(10));
  EXPECT_CALL(outmap, UniversalLatitude()).WillOnce(Return(2));
  EXPECT_CALL(outmap, UniversalLongitude()).WillOnce(Return(2));
  EXPECT_CALL(camera, SetUniversalGround(2, 2)).WillOnce(Return(1));
  EXPECT_CALL(camera, Sample()).Times(3).WillRepeatedly(Return(10.0));
  EXPECT_CALL(camera, Line()).Times(3).WillRepeatedly(Return(10.0));
  EXPECT_CALL(camera, SetImage(10.0, 10.0)).WillRepeatedly(Return(1.0));
  EXPECT_CALL(camera, UniversalLongitude()).WillOnce(Return(2));
  EXPECT_CALL(camera, UniversalLatitude()).WillOnce(Return(2));

  double inSample = 1.0;
  double inLine = 1.0;

  double const outSample = 1.0;
  double const outLine = 1.0;

  transform->Xform(inSample, inLine, outSample, outLine);

  ASSERT_EQ(inSample, 10.0);
  ASSERT_EQ(inLine, 10.0);
}

TEST_F(DefaultCube, ForwardXformUnitTestCam2map) {
  MockCamera camera(*testCube);
  MockTProjection outmap(projLabel);

  Transform *transform;
  transform = new cam2mapForward(100, 100, &camera, 200, 200, &outmap, 1);

  EXPECT_CALL(camera, SetImage(1.0, 1.0)).WillOnce(Return(1.0));
  EXPECT_CALL(camera, UniversalLatitude()).WillOnce(Return(2.0));
  EXPECT_CALL(camera, UniversalLongitude()).WillOnce(Return(2.0));
  EXPECT_CALL(outmap, SetUniversalGround(2.0, 2.0)).WillOnce(Return(1));
  EXPECT_CALL(outmap, HasGroundRange()).WillOnce(Return(1));
  EXPECT_CALL(outmap, Latitude()).Times(2).WillRepeatedly(Return(2));
  EXPECT_CALL(outmap, MinimumLatitude()).WillOnce(Return(1));
  EXPECT_CALL(outmap, MaximumLatitude()).WillOnce(Return(10));
  EXPECT_CALL(outmap, Longitude()).Times(2).WillRepeatedly(Return(2));
  EXPECT_CALL(outmap, MinimumLongitude()).WillOnce(Return(1));
  EXPECT_CALL(outmap, MaximumLongitude()).WillOnce(Return(10));
  EXPECT_CALL(outmap, WorldX()).WillOnce(Return(10.0));
  EXPECT_CALL(outmap, WorldY()).WillOnce(Return(10.0));

  double const inSample = 1.0;
  double const inLine = 1.0;

  double outSample = 1.0;
  double outLine = 1.0;

  transform->Xform(outSample, outLine, inSample, inLine);

  ASSERT_EQ(outSample, 10.0);
  ASSERT_EQ(outLine, 10.0);
}
