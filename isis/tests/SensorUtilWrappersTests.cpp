#include "gmock/gmock.h"

#include "Mocks.h"

#include "IsisIlluminator.h"

TEST(IsisIlluminator, PositionNewTime) {
  double testTime = 10.0;
  double oldTime = testTime - 1;
  std::vector<double> testPosKm = {-1.0, 1.0, 2.0};
  SensorUtilities::Vec testPosM = {-1000.0, 1000.0, 2000.0};

  MockSpicePosition mockSpice(0, 1);
  EXPECT_CALL(mockSpice, EphemerisTime)
        .WillRepeatedly(::testing::Return(oldTime));

  EXPECT_CALL(mockSpice, SetEphemerisTime(testTime))
        .WillRepeatedly(::testing::ReturnRef(testPosKm));

  EXPECT_CALL(mockSpice, Coordinate)
        .WillRepeatedly(::testing::ReturnRef(testPosKm));

  EXPECT_CALL(mockSpice, SetEphemerisTime(oldTime))
        .WillRepeatedly(::testing::ReturnRef(testPosKm));

  IsisIlluminator testIlluminator(&mockSpice);

  EXPECT_EQ(testIlluminator.position(testTime), testPosM);
}

TEST(IsisIlluminator, PositionOldTime) {
  double testTime = 10.0;
  std::vector<double> testPosKm = {-1.0, 1.0, 2.0};
  SensorUtilities::Vec testPosM = {-1000.0, 1000.0, 2000.0};

  MockSpicePosition mockSpice(0, 1);
  EXPECT_CALL(mockSpice, EphemerisTime)
        .WillRepeatedly(::testing::Return(testTime));

  EXPECT_CALL(mockSpice, Coordinate)
        .WillRepeatedly(::testing::ReturnRef(testPosKm));

  IsisIlluminator testIlluminator(&mockSpice);

  EXPECT_EQ(testIlluminator.position(testTime), testPosM);
}