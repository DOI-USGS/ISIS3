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
            ProjStr                 = "+proj=eqc +lat_ts=0 +lat_0=0 +lon_0=0 +x_0=0 +y_0=0 +R=1 +units=m +no_defs +type=crs"
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

    EXPECT_EQ(projection->WorldX(), -1.3089969389957472);
    EXPECT_EQ(projection->WorldY(), -0.87266462599716466);
}

TEST_F(IProjMappingGroup, TestIProjSetCoordinate) {
    bool success = projection->SetCoordinate(-1.3089969389957472,-0.87266462599716466);
    ASSERT_TRUE(success);

    EXPECT_NEAR(projection->Latitude(), -50, 1e-13);
    EXPECT_NEAR(projection->Longitude(), -75, 1e-13);
}

TEST_F(IProjMappingGroup, TestIProjXYRange) {
    double minX = 0, minY = 0, maxX = 0, maxY = 0;
    bool rangeCheck = projection->XYRange(minX, maxX, minY, maxY);
    ASSERT_TRUE(rangeCheck);

    EXPECT_NEAR(minX, -3.1415926535897931, 1e-13);
    EXPECT_NEAR(maxX, 3.1415926535897931, 1e-13);
    EXPECT_NEAR(minY, -1.5707963267948966, 1e-13);
    EXPECT_NEAR(maxY, 1.5707963267948966, 1e-13);
}