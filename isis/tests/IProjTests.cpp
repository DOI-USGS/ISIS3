#include <QTemporaryFile>
#include <QString>
#include <iostream>
#include <iomanip>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "IProj.h"
#include "ProjectionFactory.h"
#include "Pvl.h"
#include "TProjection.h"

#include "TestUtilities.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace Isis;

class IProjMappingGroup : public ::testing::Test {

  protected:
    Pvl label;
    PvlGroup mappingGroup;
    IProj *projection;

    void SetUp() override {

        std::istringstream iss(R"(
            Group = Mapping
            TargetName              = Mars
            ProjStr                 = "+proj=eqc +lat_ts=0 +lat_0=0 +lon_0=-90 +over +x_0=0 +y_0=0 +R=1 +units=m +no_defs +type=crs"
            LatitudeType            = Planetocentric
            LongitudeDirection      = PositiveEast
            LongitudeDomain         = 180
            EquatorialRadius        = 1
            PolarRadius             = 1

            MinimumLatitude         = -90.0
            MaximumLatitude         = 90.0
            MinimumLongitude        = -180
            MaximumLongitude        = 180

            End_Group
        )");

        iss >> mappingGroup;
        label.addGroup(mappingGroup);

        projection = new IProj(label);
    }

    void TearDown() override {
      delete projection;
    }
  
};



TEST(IProjTest, TestIProjConstruction) {
    Pvl label;
    PvlGroup mapping("Mapping");
    QString projStr = "+proj=eqc +lat_ts=0 +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 +R=1 +units=m +no_defs +type=crs";
    mapping.addKeyword(PvlKeyword("ProjStr", projStr));
    mapping.addKeyword(PvlKeyword("EquatorialRadius", "10000"));
    mapping.addKeyword(PvlKeyword("PolarRadius", "10000"));
    mapping.addKeyword(PvlKeyword("LatitudeType", "Planetocentric"));
    mapping.addKeyword(PvlKeyword("LongitudeDirection", "PositiveEast"));
    mapping.addKeyword(PvlKeyword("LongitudeDomain", "180"));

    label.addGroup(mapping);

    IProj projection(label);
}

TEST_F(IProjMappingGroup, TestIProjPvlMapping) {
    PvlGroup returnMapping = projection->Mapping();

    EXPECT_TRUE(returnMapping == mappingGroup);
}

TEST_F(IProjMappingGroup, TestIProjSetGround) {
    bool success = projection->SetGround(-50.0, -75.0);
    ASSERT_TRUE(success);

    EXPECT_EQ(projection->WorldX(), 0.26179938779914935);
    EXPECT_EQ(projection->WorldY(), -0.87266462599716477);
}

TEST_F(IProjMappingGroup, TestIProjSetCoordinate) {
    bool success = projection->SetCoordinate(0.26179938779914935,-0.87266462599716477);
    ASSERT_TRUE(success);

    EXPECT_EQ(projection->Latitude(), -50);
    EXPECT_EQ(projection->Longitude(), -75);
}

TEST_F(IProjMappingGroup, TestIProjXYRange) {
    double minX = 0, minY = 0, maxX = 0, maxY = 0;
    bool rangeCheck = projection->XYRange(minX, maxX, minY, maxY);
    ASSERT_TRUE(rangeCheck);

    EXPECT_EQ(minX, -1.5707963267948966);
    EXPECT_EQ(maxX, 4.71238898038469);
    EXPECT_EQ(minY, -1.5707963267948966);
    EXPECT_EQ(maxY, 1.5707963267948966);
}

// IAU_2015:49920 declares its base geodetic CRS axes as (latitude, longitude) per ISO 19111,
// unlike a raw "+proj=" string which is always (longitude, latitude). This exercises
// proj_normalize_for_visualization() in IProj's constructor directly: without it,
// SetGround()/SetCoordinate() would silently swap latitude and longitude.
TEST(IProjTest, TestIProjLatLonAxisOrder) {
  PvlGroup mapping("Mapping");
  mapping.addKeyword(PvlKeyword("ProjStr", "IAU_2015:49920"));
  mapping.addKeyword(PvlKeyword("EquatorialRadius", "3396190"));
  mapping.addKeyword(PvlKeyword("PolarRadius", "3396190"));
  mapping.addKeyword(PvlKeyword("LatitudeType", "Planetographic"));
  mapping.addKeyword(PvlKeyword("LongitudeDirection", "PositiveEast"));
  mapping.addKeyword(PvlKeyword("LongitudeDomain", "180"));

  Pvl label;
  label.addGroup(mapping);

  IProj projection(label);

  bool setGroundSuccess = projection.SetGround(10.0, 20.0);
  ASSERT_TRUE(setGroundSuccess);
  EXPECT_EQ(projection.WorldX(), 1167483.6335681097);
  EXPECT_EQ(projection.WorldY(), 592746.97523306217);

  bool setCoordinateSuccess = projection.SetCoordinate(1167483.6335681097, 592746.97523306217);
  ASSERT_TRUE(setCoordinateSuccess);
  EXPECT_EQ(projection.Latitude(), 10.0);
  EXPECT_EQ(projection.Longitude(), 19.999999999999996);
}
