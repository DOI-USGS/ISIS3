#include <iostream>
#include <QTemporaryFile>

#include "cam2map.h"

#include "Cube.h"
#include "CubeAttribute.h"
#include "IException.h"
#include "LineManager.h"
#include "PixelType.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "TestUtilities.h"
#include "FileName.h"
#include "ProjectionFactory.h"
#include "CameraFixtures.h"
#include "Mocks.h"

using namespace Isis;
using ::testing::Return;
using ::testing::AtLeast;

static QString APP_XML = FileName("$ISISROOT/bin/xml/cam2map.xml").expanded();

// Create a height-above-datum DEM .cub covering a given lat/lon extent.
// The mock Viking Orbiter camera's body-fixed position is at lon ~254, lat ~10.
// Pixels are filled with smooth terrain (30-60 m above datum).
QString createHeightDem(const QString &dir,
                        double minLat, double maxLat,
                        double minLon, double maxLon,
                        double centerLon) {
  Pvl demLabel;
  std::ifstream cubeLabel("data/defaultImage/demCube.pvl");
  cubeLabel >> demLabel;

  PvlGroup &demMap = demLabel.findObject("IsisCube").findGroup("Mapping");
  demMap["MinimumLatitude"]  = toString(minLat);
  demMap["MaximumLatitude"]  = toString(maxLat);
  demMap["MinimumLongitude"] = toString(minLon);
  demMap["MaximumLongitude"] = toString(maxLon);
  demMap["CenterLongitude"]  = toString(centerLon);

  double eqRad = 3396190.0;
  double deg2m = eqRad * M_PI / 180.0;
  double demUlx = (minLon - centerLon) * deg2m;
  double demUly = maxLat * deg2m;
  demMap["UpperLeftCornerX"] = toString(demUlx);
  demMap["UpperLeftCornerY"] = toString(demUly);
  // 100x100 cube, so pixel size = extent / 100. Must be square.
  double pixRes = (maxLat - minLat) * deg2m / 100.0;
  demMap["PixelResolution"] = toString(pixRes);
  demMap["Scale"] = toString(1.0 / (pixRes / deg2m));

  demLabel.findObject("IsisCube").findObject("Core")
    .findGroup("Pixels")["Type"] = "Real";
  demLabel.findObject("IsisCube").findObject("Core")
    .findGroup("Pixels")["Base"] = "0.0";
  demLabel.findObject("IsisCube").findObject("Core")
    .findGroup("Pixels")["Multiplier"] = "1.0";

  QString path = dir + "/heightDem.cub";
  Cube heightDem;
  heightDem.fromLabel(path, demLabel, "rw");

  int xCenter = heightDem.lineCount() / 2;
  int yCenter = heightDem.sampleCount() / 2;
  double radius = std::min(xCenter, yCenter);
  double depth = 30.0;
  LineManager line(heightDem);
  double xPos = 0.0;
  for (line.begin(); !line.end(); line++) {
    for (int yPos = 0; yPos < line.size(); yPos++) {
      double pointRadius = sqrt(pow(xPos - xCenter, 2) +
                                pow(yPos - yCenter, 2));
      if (pointRadius < radius)
        line[yPos] = (sin((M_PI * pointRadius) / (2 * radius)) * depth)
                     + depth;
      else
        line[yPos] = depth * 2;
    }
    xPos++;
    heightDem.write(line);
  }
  heightDem.reopen("rw");
  heightDem.close();
  return path;
}

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
      MaximumLatitude    = 5 <degrees>
      MinimumLongitude   = 0 <degrees>
      MaximumLongitude   = 5 <degrees>

      PixelResolution    = 100000 <meters/pixel>
      Scale              = 512.0 <pixels/degree>
    End_Group
  )");

  Pvl userMap;
  labelStrm >> userMap;
  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  QVector<QString> args = {"to="+tempDir.path()+"/level2.cub", "pixres=map"};
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

TEST_F(DefaultCube, FunctionalTestCam2mapMismatch) {
  std::istringstream labelStrm(R"(
    Group = Mapping
      ProjectionName  = Sinusoidal
      CenterLongitude = 0.0 <degrees>

      TargetName         = Moon
      EquatorialRadius   = 3396190.0 <meters>
      PolarRadius        = 3376200.0 <meters>

      LatitudeType       = Planetocentric
      LongitudeDirection = PositiveEast
      LongitudeDomain    = 360 <degrees>

      MinimumLatitude    = 0 <degrees>
      MaximumLatitude    = 5 <degrees>
      MinimumLongitude   = 0 <degrees>
      MaximumLongitude   = 5 <degrees>

      PixelResolution    = 100000 <meters/pixel>
      Scale              = 512.0 <pixels/degree>
    End_Group
  )");

  Pvl userMap;
  labelStrm >> userMap;
  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  QVector<QString> args = {"to="+tempDir.path()+"/level2.cub", "pixres=map"};
  UserInterface ui(APP_XML, args);

  Pvl log;
  try {
    cam2map(testCube, userMap, userGrp, ui, &log);
  }
  catch(IException &e) {
    ASSERT_EQ(e.errorType(), 2);
  }
}

TEST_F(DefaultCube, FunctionalTestCam2mapUserLatlon) {
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
      MaximumLatitude    = 5 <degrees>
      MinimumLongitude   = 0 <degrees>
      MaximumLongitude   = 5 <degrees>

      PixelResolution    = 100000 <meters/pixel>
      Scale              = 512.0 <pixels/degree>
    End_Group
  )");

  Pvl userMap;
  labelStrm >> userMap;
  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  QVector<QString> args = {"to="+tempDir.path()+"/level2.cub", "matchmap=no", "minlon=0",
                           "maxlon=10", "minlat=0", "maxlat=10", "defaultrange=camera",
                           "pixres=map"};
  UserInterface ui(APP_XML, args);

  Pvl log;

  cam2map(testCube, userMap, userGrp, ui, &log);
  Cube ocube(tempDir.path()+"/level2.cub");

  ASSERT_EQ(userGrp.findKeyword("PixelResolution")[0], "100000.0");
  ASSERT_EQ(userGrp.findKeyword("Scale")[0], "0.59274697523306");

  ASSERT_EQ(userGrp.findKeyword("MinimumLongitude")[0], "0.0");
  ASSERT_EQ(userGrp.findKeyword("MaximumLongitude")[0], "10.0");
  ASSERT_EQ(userGrp.findKeyword("MinimumLatitude")[0], "0.0");
  ASSERT_EQ(userGrp.findKeyword("MaximumLatitude")[0], "10.0");

  ASSERT_EQ(userGrp.findKeyword("UpperLeftCornerX")[0], "0.0");
  ASSERT_EQ(userGrp.findKeyword("UpperLeftCornerY")[0], "600000.0");
}

TEST_F(LineScannerCube, FunctionalTestCam2mapMapLatlon) {
  std::istringstream labelStrm(R"(
    Group = Mapping
      ProjectionName  = Sinusoidal
      CenterLongitude = 0.0 <degrees>

      TargetName         = MOON
      EquatorialRadius   = 3396190.0 <meters>
      PolarRadius        = 3376200.0 <meters>

      LatitudeType       = Planetocentric
      LongitudeDirection = PositiveEast
      LongitudeDomain    = 360 <degrees>

      MinimumLatitude    = 0 <degrees>
      MaximumLatitude    = 1 <degrees>
      MinimumLongitude   = 0 <degrees>
      MaximumLongitude   = 2 <degrees>

      PixelResolution    = 100000 <meters/pixel>
      Scale              = 512.0 <pixels/degree>
    End_Group
  )");
  Pvl userMap;
  labelStrm >> userMap;
  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  QVector<QString> args = {"to="+tempDir.path()+"/level2.cub", "matchmap=no",
                           "defaultrange=map", "pixres=camera"};
  UserInterface ui(APP_XML, args);

  Pvl log;

  cam2map(testCube, userMap, userGrp, ui, &log);
  Cube ocube(tempDir.path()+"/level2.cub");

  // Check for equality of floats, not of strings, because QString values
  // show up in an unreadable encoding when printed to the console when tests fail.
  auto res = userGrp.findKeyword("PixelResolution")[0];
  ASSERT_EQ(atof(res.toStdString().c_str()), 9.0084340942885994);
  auto scale = userGrp.findKeyword("Scale")[0];
  ASSERT_EQ(atof(scale.toStdString().c_str()), 6579.9113256417004);

  ASSERT_EQ(userGrp.findKeyword("MinimumLongitude")[0], "0");
  ASSERT_EQ(userGrp.findKeyword("MaximumLongitude")[0], "2");
  ASSERT_EQ(userGrp.findKeyword("MinimumLatitude")[0], "0");
  ASSERT_EQ(userGrp.findKeyword("MaximumLatitude")[0], "1");

  // Test floats, not QString, as those are printed as unreadable junk
  // when a test fails.
  auto cornerX = userGrp.findKeyword("UpperLeftCornerX")[0];
  ASSERT_EQ(atof(cornerX.toStdString().c_str()), 0);
  auto cornerY = userGrp.findKeyword("UpperLeftCornerY")[0];
  ASSERT_EQ(atof(cornerY.toStdString().c_str()), 59275.496340418998);
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

TEST_F(DefaultCube, FunctionalTestCam2mapFramerMock) {
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

  QVector<QString> args = {"to=" + tempDir.path() + "/level2.cub", "matchmap=yes"};
  UserInterface ui(APP_XML, args);

  Pvl log;
  MockProcessRubberSheet rs;
  FileName fn(tempDir.path() + "/level2.cub");
  CubeAttributeOutput  outputAttr(fn);
  Cube outputCube;
  outputCube.setDimensions(1, 1, 1);
  outputCube.create(fn.expanded(), outputAttr);
  outputCube.reopen("rw");

  EXPECT_CALL(rs, SetInputCube(testCube, 0)).Times(AtLeast(1));
  EXPECT_CALL(rs, SetOutputCube).Times(AtLeast(1)).WillOnce(Return(&outputCube));
  EXPECT_CALL(rs, SetTiling(4,4)).Times(AtLeast(1));
  EXPECT_CALL(rs, StartProcess).Times(AtLeast(1));
  EXPECT_CALL(rs, EndProcess).Times(AtLeast(1));
  cam2map(testCube, userMap, userGrp, rs, ui, &log);
}

TEST_F(LineScannerCube, FunctionalTestCam2mapLineScanMock){

  std::istringstream labelStrm(R"(
    Group = Mapping
      ProjectionName     = Sinusoidal
      CenterLongitude    = 338.43365399713
      TargetName         = MOON
      EquatorialRadius   = 1737400.0 <meters>
      PolarRadius        = 1737400.0 <meters>
      LatitudeType       = Planetocentric
      LongitudeDirection = PositiveEast
      LongitudeDomain    = 360
      MinimumLatitude    = 11.463745149835
      MaximumLatitude    = 11.476785565832
      MinimumLongitude   = 337.81781569041
      MaximumLongitude   = 339.04949230384
      UpperLeftCornerX   = -18307.842628129 <meters>
      UpperLeftCornerY   = 348018.60964676 <meters>
      PixelResolution    = 8.926300647552 <meters/pixel>
      Scale              = 3397.0792180819 <pixels/degree>
    End_Group
  )");

  Pvl userMap;
  labelStrm >> userMap;
  PvlGroup &userGrp = userMap.findGroup("Mapping", Pvl::Traverse);

  QVector<QString> args = {"to=" + tempDir.path() + "/level2.cub", "matchmap=yes"};

  UserInterface ui(APP_XML, args);

  Pvl log;
  MockProcessRubberSheet rs;
  FileName fn(tempDir.path() + "/level2.cub");
  CubeAttributeOutput outputAttr(fn);
  Cube outputCube;
  outputCube.setDimensions(1, 1, 1);
  outputCube.create(fn.expanded(), outputAttr);
  outputCube.reopen("rw");

  EXPECT_CALL(rs, SetInputCube(testCube, 0)).Times(AtLeast(1));
  EXPECT_CALL(rs, SetOutputCube).Times(AtLeast(1)).WillOnce(Return(&outputCube));
  EXPECT_CALL(rs, processPatchTransform).Times(AtLeast(1));
  EXPECT_CALL(rs, EndProcess).Times(AtLeast(1));

  cam2map(testCube, userMap, userGrp, rs, ui, &log);
}

TEST_F(DefaultCube, FunctionalTestCam2mapForwardMock) {
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

  QVector<QString> args = {"to=" + tempDir.path()+ "/level2.cub",
                          "matchmap=yes",
                          "warpalgorithm=forwardpatch",
                          "patchsize=0"};
  UserInterface ui(APP_XML, args);

  Pvl log;
  MockProcessRubberSheet rs;
  FileName fn(tempDir.path() + "/level2.cub");
  CubeAttributeOutput  outputAttr(fn);
  Cube outputCube;
  outputCube.setDimensions(1, 1, 1);
  outputCube.create(fn.expanded(), outputAttr);
  outputCube.reopen("rw");

  EXPECT_CALL(rs, SetInputCube(testCube, 0)).Times(AtLeast(1));
  EXPECT_CALL(rs, SetOutputCube).Times(AtLeast(1)).WillOnce(Return(&outputCube));
  EXPECT_CALL(rs, setPatchParameters(1, 1, 3, 3, 2, 2)).Times(AtLeast(1));
  EXPECT_CALL(rs, processPatchTransform).Times(AtLeast(1));
  EXPECT_CALL(rs, EndProcess).Times(AtLeast(1));
  cam2map(testCube, userMap, userGrp, rs, ui, &log);
}

// Test the ASP_MAP per-pixel projection code path.
// Uses the DemCube fixture which creates a synthetic DEM with
// ShapeModelStatistics table (demprep'd). Verifies that:
// 1. Output has a Mapping group with Equirectangular projection
// 2. Output has an AspMapproject metadata group with expected keywords
// 3. Grid is snapped (UpperLeftCornerX/Y are multiples of half-pixel)
// 4. Output has non-zero dimensions
TEST_F(DemCube, FunctionalTestCam2mapAspMap) {
  // Create a height-above-datum DEM covering the mock camera's footprint.
  // Camera body-fixed position is at lon ~254, lat ~10.
  QString heightDemPath = createHeightDem(tempDir.path(),
                                          -35.0, 55.0, 200.0, 290.0, 245.0);
  QString outPath = tempDir.path() + "/aspmap_out.cub";

  QVector<QString> args = {"from=" + testCube->fileName(),
                           "to=" + outPath,
                           "asp_map=true",
                           "dem=" + heightDemPath,
                           "pixres=mpp",
                           "resolution=500"};
  UserInterface ui(APP_XML, args);

  Pvl log;
  cam2map(ui, &log);

  Cube ocube(outPath);
  Pvl *outLabel = ocube.label();

  // Check Mapping group exists and has expected projection
  PvlGroup mapGrp = outLabel->findGroup("Mapping", Pvl::Traverse);
  ASSERT_EQ(mapGrp.findKeyword("ProjectionName")[0], "Equirectangular");

  // Check grid snapping: UL corner should be at center - pixres/2,
  // i.e., a multiple of pixres offset by half a pixel
  double ulx = toDouble(mapGrp.findKeyword("UpperLeftCornerX")[0]);
  double uly = toDouble(mapGrp.findKeyword("UpperLeftCornerY")[0]);
  double pixres = 500.0;
  double cx = ulx + pixres / 2.0;
  double cy = uly - pixres / 2.0;
  // First pixel center should be at an integer multiple of pixres
  ASSERT_NEAR(fmod(fabs(cx), pixres), 0.0, 1.0);
  ASSERT_NEAR(fmod(fabs(cy), pixres), 0.0, 1.0);

  // Check output has nonzero dimensions
  ASSERT_GT(ocube.sampleCount(), 0);
  ASSERT_GT(ocube.lineCount(), 0);

  // Check AspMapproject metadata group
  PvlGroup aspGrp = outLabel->findGroup("AspMapproject", Pvl::Traverse);
  ASSERT_EQ(aspGrp.findKeyword("CAMERA_MODEL_TYPE")[0], "isis");
  ASSERT_EQ(aspGrp.findKeyword("BUNDLE_ADJUST_PREFIX")[0], "NONE");
  ASSERT_TRUE(aspGrp.hasKeyword("DEM_FILE"));
  ASSERT_TRUE(aspGrp.hasKeyword("INPUT_IMAGE_FILE"));
  ASSERT_TRUE(aspGrp.hasKeyword("CAMERA_FILE"));
}

TEST_F(DefaultCube, FunctionalTestCam2mapReverseMock) {
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

  QVector<QString> args = {"to=" + tempDir.path() + "/level2.cub",
                          "matchmap=yes",
                          "warpalgorithm=reversepatch",
                          "patchsize=3"};
  UserInterface ui(APP_XML, args);

  Pvl log;
  MockProcessRubberSheet rs;
  FileName fn(tempDir.path() + "/level2.cub");
  CubeAttributeOutput  outputAttr(fn);
  Cube outputCube;
  outputCube.setDimensions(1, 1, 1);
  outputCube.create(fn.expanded(), outputAttr);
  outputCube.reopen("rw");

  EXPECT_CALL(rs, SetInputCube(testCube, 0)).Times(AtLeast(1));
  EXPECT_CALL(rs, SetOutputCube).Times(AtLeast(1)).WillOnce(Return(&outputCube));
  EXPECT_CALL(rs, SetTiling(4, 4)).Times(AtLeast(1));
  EXPECT_CALL(rs, StartProcess).Times(AtLeast(1));
  EXPECT_CALL(rs, EndProcess).Times(AtLeast(1));
  cam2map(testCube, userMap, userGrp, rs, ui, &log);
}
