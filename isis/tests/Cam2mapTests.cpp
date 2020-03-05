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

#include "gmock/gmock.h"

using namespace Isis;

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
  std::cout << userMap << '\n';
  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  QVector<QString> args = {"to=/tmp/level2.cub", "matchmap=yes"};
  UserInterface ui(APP_XML, args);

  Pvl log;

  std::cout << *testCube->label() << '\n';
  cam2map(testCube, userMap, userGrp, ui, &log);
  Cube ocube("/tmp/level2.cub");
  std::cout << *ocube.label() << '\n';

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
