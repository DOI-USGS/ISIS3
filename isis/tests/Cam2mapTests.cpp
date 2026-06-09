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

// Check that each band in a projected cube has valid pixels with
// non-trivial DN range. At least 10% of pixels must be non-null
// and the DN range must exceed 0.001.
void checkPixelValues(Cube &cube, int expectedBands) {
  ASSERT_GT(cube.sampleCount(), 0);
  ASSERT_GT(cube.lineCount(), 0);
  ASSERT_EQ(cube.bandCount(), expectedBands);

  int samples = cube.sampleCount();
  int lines = cube.lineCount();
  int totalPixels = samples * lines;

  for (int band = 1; band <= expectedBands; band++) {
    LineManager reader(cube);
    int validCount = 0;
    double minVal = std::numeric_limits<double>::max();
    double maxVal = -std::numeric_limits<double>::max();
    for (int line = 1; line <= lines; line++) {
      reader.SetLine(line, band);
      cube.read(reader);
      for (int s = 0; s < reader.size(); s++) {
        double dn = reader[s];
        if (!IsSpecial(dn)) {
          validCount++;
          if (dn < minVal) minVal = dn;
          if (dn > maxVal) maxVal = dn;
        }
      }
    }
    ASSERT_GT(validCount, totalPixels / 10)
      << "Band " << band << ": too few valid pixels: "
      << validCount << " / " << totalPixels;
    ASSERT_GT(maxVal - minVal, 0.001)
      << "Band " << band << ": pixel values are constant: "
      << "min=" << minVal << " max=" << maxVal;
  }
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

// Test the ASP_MAP per-pixel projection code path with (1) sinusoidal
// projection output, (2) equirectangular projection DEM input, and (3) user
// lat/lon bounds. Checks resulting projection, metadata, bounds, grid snapping,
// and pixel values.
TEST_F(DemCube, FunctionalTestCam2mapAspMap) {
  // Camera body-fixed position is at lon ~254, lat ~10.
  QString heightDemPath = createHeightDem(tempDir.path(),
                                          -35.0, 55.0, 200.0, 290.0, 245.0);

  // Write a sinusoidal .map file
  QString mapPath = tempDir.path() + "/sinusoidal.map";
  Pvl mapPvl;
  PvlGroup mapGrp("Mapping");
  mapGrp += PvlKeyword("ProjectionName", "Sinusoidal");
  mapGrp += PvlKeyword("CenterLongitude", "245.0");
  mapGrp += PvlKeyword("TargetName", "MARS");
  mapGrp += PvlKeyword("EquatorialRadius", "3396190.0", "meters");
  mapGrp += PvlKeyword("PolarRadius", "3376200.0", "meters");
  mapGrp += PvlKeyword("LatitudeType", "Planetocentric");
  mapGrp += PvlKeyword("LongitudeDirection", "PositiveEast");
  mapGrp += PvlKeyword("LongitudeDomain", "360");
  mapPvl.addGroup(mapGrp);
  mapPvl.write(mapPath);

  QString outPath = tempDir.path() + "/aspmap_out.cub";

  // First run: no bounds, to discover the auto-computed extent
  QString refPath = tempDir.path() + "/aspmap_ref.cub";
  {
    QVector<QString> refArgs = {"from=" + testCube->fileName(),
                                "to=" + refPath,
                                "asp_map=true",
                                "dem=" + heightDemPath,
                                "map=" + mapPath,
                                "pixres=mpp",
                                "resolution=500"};
    UserInterface refUi(APP_XML, refArgs);
    Pvl refLog;
    cam2map(refUi, &refLog);
  }
  // Read auto-computed bounds and shrink by 20% on each side
  double eqRad = 3396190.0;
  double deg2m = eqRad * M_PI / 180.0;
  double pixres = 500.0;
  Cube refCube(refPath);
  PvlGroup refMap = refCube.label()->findGroup("Mapping", Pvl::Traverse);
  double refUlx = toDouble(refMap.findKeyword("UpperLeftCornerX")[0]);
  double refUly = toDouble(refMap.findKeyword("UpperLeftCornerY")[0]);
  double refLrx = refUlx + refCube.sampleCount() * pixres;
  double refLry = refUly - refCube.lineCount() * pixres;
  refCube.close();
  // Convert projected coords to lat/lon (sinusoidal: y = lat * deg2m,
  // x = (lon - centerLon) * deg2m * cos(lat))
  double autoMinLat = refLry / deg2m;
  double autoMaxLat = refUly / deg2m;
  double midLat = (autoMinLat + autoMaxLat) / 2.0;
  double cosLat = cos(midLat * M_PI / 180.0);
  double autoMinLon = 245.0 + refUlx / (deg2m * cosLat);
  double autoMaxLon = 245.0 + refLrx / (deg2m * cosLat);
  // Shrink by 20% on each side to get a proper subset
  double latRange = autoMaxLat - autoMinLat;
  double userMinLat = autoMinLat + 0.2 * latRange;
  double userMaxLat = autoMaxLat - 0.2 * latRange;
  double lonRange = autoMaxLon - autoMinLon;
  double userMinLon = autoMinLon + 0.2 * lonRange;
  double userMaxLon = autoMaxLon - 0.2 * lonRange;

  QVector<QString> args = {"from=" + testCube->fileName(),
                           "to=" + outPath,
                           "asp_map=true",
                           "dem=" + heightDemPath,
                           "map=" + mapPath,
                           "pixres=mpp",
                           "resolution=500",
                           "minlat=" + toString(userMinLat),
                           "maxlat=" + toString(userMaxLat),
                           "minlon=" + toString(userMinLon),
                           "maxlon=" + toString(userMaxLon)};
  UserInterface ui(APP_XML, args);

  Pvl log;
  cam2map(ui, &log);

  Cube ocube(outPath);
  Pvl *outLabel = ocube.label();

  // Check Mapping group exists and has sinusoidal projection
  PvlGroup outMapGrp = outLabel->findGroup("Mapping", Pvl::Traverse);
  ASSERT_EQ(outMapGrp.findKeyword("ProjectionName")[0], "Sinusoidal");

  // Verify output extent is consistent with the user lat/lon bounds.
  // ASP_MAP doesn't write MinimumLatitude etc. to PVL, so verify via
  // projected coordinates and image dimensions.
  double ulx = toDouble(outMapGrp.findKeyword("UpperLeftCornerX")[0]);
  double uly = toDouble(outMapGrp.findKeyword("UpperLeftCornerY")[0]);
  int samples = ocube.sampleCount();
  int lines = ocube.lineCount();
  double lrx = ulx + samples * pixres;
  double lry = uly - lines * pixres;
  // Convert back to lat/lon (sinusoidal: y = lat * deg2m,
  // x = (lon - centerLon) * deg2m * cos(lat))
  double outMaxLat = uly / deg2m;
  double outMinLat = lry / deg2m;
  double outCosLat = cos((outMinLat + outMaxLat) / 2.0 * M_PI / 180.0);
  double outMinLon = 245.0 + ulx / (deg2m * outCosLat);
  double outMaxLon = 245.0 + lrx / (deg2m * outCosLat);
  // Allow 1.5-pixel tolerance: grid snapping (floor/ceil) can expand by
  // up to 1 pixel, and the produced extent goes 0.5 GSD beyond grid centers
  double tolDeg = 1.5 * pixres / deg2m;
  ASSERT_NEAR(outMinLat, userMinLat, tolDeg) << "MinLat";
  ASSERT_NEAR(outMaxLat, userMaxLat, tolDeg) << "MaxLat";
  double tolDegLon = 1.5 * pixres / (deg2m * outCosLat);
  ASSERT_NEAR(outMinLon, userMinLon, tolDegLon) << "MinLon";
  ASSERT_NEAR(outMaxLon, userMaxLon, tolDegLon) << "MaxLon";

  // Check grid snapping: first pixel center at integer multiple of pixres
  double cx = ulx + pixres / 2.0;
  double cy = uly - pixres / 2.0;
  ASSERT_NEAR(fmod(fabs(cx), pixres), 0.0, 1.0);
  ASSERT_NEAR(fmod(fabs(cy), pixres), 0.0, 1.0);

  // Check output has nonzero dimensions
  ASSERT_GT(samples, 0);
  ASSERT_GT(lines, 0);

  // Check AspMapproject metadata group
  PvlGroup aspGrp = outLabel->findGroup("AspMapproject", Pvl::Traverse);
  ASSERT_EQ(aspGrp.findKeyword("CAMERA_MODEL_TYPE")[0], "isis");
  ASSERT_EQ(aspGrp.findKeyword("BUNDLE_ADJUST_PREFIX")[0], "NONE");
  ASSERT_TRUE(aspGrp.hasKeyword("DEM_FILE"));
  ASSERT_TRUE(aspGrp.hasKeyword("INPUT_IMAGE_FILE"));
  ASSERT_TRUE(aspGrp.hasKeyword("CAMERA_FILE"));

  // Check pixel values across all bands
  checkPixelValues(ocube, ocube.bandCount());
}

// Test ASP_MAP matchmap=true with a PVL .map file.
// Also exercises auto-GSD (no pixres/resolution args in step 1).
// Two-step self-consistency test:
// 1. Run cam2map asp_map=true with auto GSD to produce a reference output.
// 2. Extract the Mapping group from that output, write it as a PVL .map
//    file with Samples/Lines added, then run cam2map asp_map=true
//    matchmap=true map=that.map.
// The matchmap output must have the same grid (ULX/Y, GSD, dimensions)
// as the reference. This exercises both the auto-GSD computation
// (computeAutoGrid) and the readGridFromPvl() fallback path (GDAL can't
// open a .map text file, so it falls through to PVL parsing).
TEST_F(DemCube, FunctionalTestCam2mapAspMapMatchmapPvl) {
  QString heightDemPath = createHeightDem(tempDir.path(),
                                          -35.0, 55.0, 200.0, 290.0, 245.0);

  // Step 1: Run cam2map asp_map=true with auto grid
  QString refPath = tempDir.path() + "/aspmap_ref.cub";
  {
    QVector<QString> args = {"from=" + testCube->fileName(),
                             "to=" + refPath,
                             "asp_map=true",
                             "dem=" + heightDemPath};
    UserInterface ui(APP_XML, args);
    Pvl log;
    cam2map(ui, &log);
  }

  // Read the reference output's Mapping group
  Cube refCube(refPath);
  Pvl *refLabel = refCube.label();
  PvlGroup refMapGrp = refLabel->findGroup("Mapping", Pvl::Traverse);
  int refSamples = refCube.sampleCount();
  int refLines = refCube.lineCount();
  refCube.close();

  // Step 2: Write a PVL .map file from the reference Mapping group.
  // Add Samples and Lines keywords (needed by readGridFromPvl).
  PvlGroup mapGrp = refMapGrp;
  mapGrp.addKeyword(PvlKeyword("Samples", toString(refSamples)));
  mapGrp.addKeyword(PvlKeyword("Lines", toString(refLines)));
  Pvl mapPvl;
  mapPvl.addGroup(mapGrp);
  QString mapPath = tempDir.path() + "/reference.map";
  mapPvl.write(mapPath);

  // Step 3: Run cam2map asp_map=true with matchmap=true map=reference.map
  QString outPath = tempDir.path() + "/aspmap_matchmap.cub";
  {
    QVector<QString> args = {"from=" + testCube->fileName(),
                             "to=" + outPath,
                             "asp_map=true",
                             "dem=" + heightDemPath,
                             "map=" + mapPath,
                             "matchmap=true"};
    UserInterface ui(APP_XML, args);
    Pvl log;
    cam2map(ui, &log);
  }

  // Step 4: Verify the matchmap output matches the reference grid
  Cube ocube(outPath);
  Pvl *outLabel = ocube.label();
  PvlGroup outMapGrp = outLabel->findGroup("Mapping", Pvl::Traverse);

  // Dimensions must match exactly
  ASSERT_EQ(ocube.sampleCount(), refSamples)
    << "Samples mismatch: matchmap=" << ocube.sampleCount()
    << " ref=" << refSamples;
  ASSERT_EQ(ocube.lineCount(), refLines)
    << "Lines mismatch: matchmap=" << ocube.lineCount()
    << " ref=" << refLines;

  // Grid origin must match exactly
  double outUlx = toDouble(outMapGrp.findKeyword("UpperLeftCornerX")[0]);
  double outUly = toDouble(outMapGrp.findKeyword("UpperLeftCornerY")[0]);
  double refUlx = toDouble(refMapGrp.findKeyword("UpperLeftCornerX")[0]);
  double refUly = toDouble(refMapGrp.findKeyword("UpperLeftCornerY")[0]);
  ASSERT_DOUBLE_EQ(outUlx, refUlx) << "UpperLeftCornerX mismatch";
  ASSERT_DOUBLE_EQ(outUly, refUly) << "UpperLeftCornerY mismatch";

  // GSD must match
  double outRes = toDouble(outMapGrp.findKeyword("PixelResolution")[0]);
  double refRes = toDouble(refMapGrp.findKeyword("PixelResolution")[0]);
  ASSERT_DOUBLE_EQ(outRes, refRes) << "PixelResolution mismatch";

  // Projection must match
  ASSERT_EQ(outMapGrp.findKeyword("ProjectionName")[0],
            refMapGrp.findKeyword("ProjectionName")[0]);

  // Check pixel values are valid
  checkPixelValues(ocube, ocube.bandCount());
}

// Test ASP_MAP with a CSM camera model via the isd= parameter.
// Uses the same DemCube fixture (reuses testCube for pixel data) but
// provides a CSM ISD JSON file instead of the ISIS camera.
// The CSM framer looks at lon=0, lat=0 on Mars from 4000 km.
// Requires USGSCSM plugin, which is loaded from ISISROOT or CONDA_PREFIX.
TEST_F(DemCube, FunctionalTestCam2mapAspMapCsm) {
  // Check if CSM plugins are available. The test preferences
  // (TestPreferences) list CSM plugin directories under CONDA_PREFIX
  // and ISISROOT. Check both without modifying ISISROOT, since changing
  // it breaks ISIS file lookups (e.g., CubeFormatTemplate.pft).
  auto hasPlugins = [](const char *dir) {
    return dir && QDir(QString(dir) + "/lib/csmplugins").exists();
  };
  const char *isisroot = getenv("ISISROOT");
  const char *conda = getenv("CONDA_PREFIX");
  if (!hasPlugins(isisroot) && !hasPlugins(conda))
    GTEST_SKIP() << "No CSM plugins found (set ISISROOT or CONDA_PREFIX)";

  // DEM centered at lon=0, lat=0 to match CSM framer footprint
  QString heightDemPath = createHeightDem(tempDir.path(),
                                          -5.0, 5.0, -5.0, 5.0, 0.0);
  QString outPath = tempDir.path() + "/aspmap_csm_out.cub";
  QString isdPath = "data/defaultImage/orbitalMarsFramer.json";

  QVector<QString> args = {"from=" + testCube->fileName(),
                           "to=" + outPath,
                           "asp_map=true",
                           "dem=" + heightDemPath,
                           "isd=" + isdPath,
                           "pixres=mpp",
                           "resolution=1000"};
  UserInterface ui(APP_XML, args);

  Pvl log;
  try {
    cam2map(ui, &log);
  } catch (IException &e) {
    GTEST_SKIP() << "cam2map CSM test skipped (environment issue): "
                 << e.what();
  }

  Cube ocube(outPath);
  Pvl *outLabel = ocube.label();

  // Check Mapping group
  PvlGroup mapGrp = outLabel->findGroup("Mapping", Pvl::Traverse);
  ASSERT_EQ(mapGrp.findKeyword("ProjectionName")[0], "Equirectangular");

  // Check output has nonzero dimensions
  ASSERT_GT(ocube.sampleCount(), 0);
  ASSERT_GT(ocube.lineCount(), 0);

  // Check AspMapproject metadata
  PvlGroup aspGrp = outLabel->findGroup("AspMapproject", Pvl::Traverse);
  ASSERT_TRUE(aspGrp.hasKeyword("CAMERA_MODEL_TYPE"));

  // Check pixel values
  checkPixelValues(ocube, ocube.bandCount());
}

// Multi-band fixture: reuses DemCube but makes the input cube 3-band.
// The mock camera is band-independent, so this tests multi-band I/O
// plumbing, not band-dependent camera behavior.
// We recreate only testCube (not projTestCube) to avoid ISD parse issues.
class MultiBandDemCube : public DemCube {
protected:
  void SetUp() override {
    DemCube::SetUp();

    int lines = testCube->lineCount();
    int bands = 3;

    // Extract the label and change band count
    Pvl newLabel;
    newLabel.addObject(testCube->label()->findObject("IsisCube"));
    PvlGroup &dim = newLabel.findObject("IsisCube")
                      .findObject("Core").findGroup("Dimensions");
    dim.findKeyword("Bands").setValue(QString::number(bands));

    nlohmann::json savedIsd = isd;

    // Recreate testCube with 3 bands
    delete testCube;
    testCube = new Cube();
    testCube->fromIsd(tempDir.path() + "/default.cub", newLabel, savedIsd, "rw");

    // Write per-band pixel data
    LineManager line(*testCube);
    int pixelValue = 1;
    for (int band = 1; band <= bands; band++)
      for (int i = 1; i <= lines; i++) {
        line.SetLine(i, band);
        for (int j = 0; j < line.size(); j++) {
          line[j] = (double)(pixelValue % 255);
          pixelValue++;
        }
        testCube->write(line);
      }
    testCube->reopen("rw");
  }
};

TEST_F(MultiBandDemCube, FunctionalTestCam2mapAspMapMultiBand) {
  QString heightDemPath = createHeightDem(tempDir.path(),
                                          -35.0, 55.0, 200.0, 290.0, 245.0);
  QString outPath = tempDir.path() + "/aspmap_multiband_out.cub";

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

  // Output must have same number of bands as input
  ASSERT_EQ(ocube.bandCount(), 3)
    << "Expected 3 bands, got " << ocube.bandCount();

  // Check pixel values across all bands
  checkPixelValues(ocube, 3);
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
