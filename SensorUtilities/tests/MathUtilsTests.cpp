#include <gtest/gtest.h>

#include <cmath>

#include "MathUtils.h"

using namespace SensorUtilities;

// Check to make sure we can create an image point without a band and it defaults to 0.
TEST(ImagePt, NoBandInit) {
  ImagePt testPt = {1.0, 2.0};
  EXPECT_EQ(testPt.band, 0.0);
}

TEST(Magnitude, Unit) {
  EXPECT_DOUBLE_EQ(magnitude({1.0, 0.0, 0.0}), 1.0);
  EXPECT_DOUBLE_EQ(magnitude({0.0, 1.0, 0.0}), 1.0);
  EXPECT_DOUBLE_EQ(magnitude({0.0, 0.0, 1.0}), 1.0);
}

TEST(Magnitude, NonUnit) {
  EXPECT_DOUBLE_EQ(magnitude({0.0, 0.0, 0.0}), 0.0);
  EXPECT_DOUBLE_EQ(magnitude({2.0, 1.0, 4.0}), sqrt(21.0));
  EXPECT_DOUBLE_EQ(magnitude({0.2, 0.1, 0.4}), sqrt(0.21));
}

TEST(Difference, vec) {
  Vec start = {1.0, 2.0, 3.0};
  Vec stop = {6.0, 5.0, 4.0};
  Vec result = {5.0, 3.0, 1.0};
  EXPECT_EQ(stop - start, result);
}

TEST(SepAngle, RightAngle) {
  EXPECT_DOUBLE_EQ(sepAngle({1, 0, 0}, {0, 1, 0}) , M_PI / 2.0);
}

TEST(SepAngle, AcuteAngle) {
  EXPECT_DOUBLE_EQ(sepAngle({1, 0, 0}, {1, 1, 0}) , M_PI / 4.0);
}

TEST(SepAngle, ObtuseAngle) {
  EXPECT_DOUBLE_EQ(sepAngle({1, 0, 0}, {-1, 1, 0}) , 3.0 * M_PI / 4.0);
}

TEST(SepAngle, Normalization) {
  EXPECT_DOUBLE_EQ(
      sepAngle({1, 0, 0}, {1, 1, 0}),
      sepAngle({100, 0, 0}, {100, 100, 0}));
}

TEST(SphericalToRect, Axes) {
  Vec result = sphericalToRect({0.0, 0.0, 1000.0});
  EXPECT_NEAR(result.x, 1000.0, 1e-12);
  EXPECT_NEAR(result.y, 0.0, 1e-12);
  EXPECT_NEAR(result.z, 0.0, 1e-12);

  result = sphericalToRect({0.0, M_PI, 1000.0});
  EXPECT_NEAR(result.x, -1000.0, 1e-12);
  EXPECT_NEAR(result.y, 0.0, 1e-12);
  EXPECT_NEAR(result.z, 0.0, 1e-12);

  result = sphericalToRect({M_PI / 2.0, 0.0, 1000.0});
  EXPECT_NEAR(result.x, 0.0, 1e-12);
  EXPECT_NEAR(result.y, 0.0, 1e-12);
  EXPECT_NEAR(result.z, 1000.0, 1e-12);

  result = sphericalToRect({M_PI / -2.0, 0.0, 1000.0});
  EXPECT_NEAR(result.x, 0.0, 1e-12);
  EXPECT_NEAR(result.y, 0.0, 1e-12);
  EXPECT_NEAR(result.z, -1000.0, 1e-12);
}

TEST(RectToSpherical, Axes) {
  GroundPt3D result = rectToSpherical({1000.0, 0.0, 0.0});
  EXPECT_DOUBLE_EQ(result.lat, 0.0);
  EXPECT_DOUBLE_EQ(result.lon, 0.0);
  EXPECT_DOUBLE_EQ(result.radius, 1000.0);

  result = rectToSpherical({-1000.0, 0.0, 0.0});
  EXPECT_DOUBLE_EQ(result.lat, 0.0);
  EXPECT_DOUBLE_EQ(result.lon, M_PI);
  EXPECT_DOUBLE_EQ(result.radius, 1000.0);

  result = rectToSpherical({0.0, 0.0, 1000.0});
  EXPECT_DOUBLE_EQ(result.lat, M_PI / 2.0);
  EXPECT_DOUBLE_EQ(result.lon, 0.0);
  EXPECT_DOUBLE_EQ(result.radius, 1000.0);

  result = rectToSpherical({0.0, 0.0, -1000.0});
  EXPECT_DOUBLE_EQ(result.lat, M_PI / -2.0);
  EXPECT_DOUBLE_EQ(result.lon, 0.0);
  EXPECT_DOUBLE_EQ(result.radius, 1000.0);
}
