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

TEST(RadiansToDegrees, RadToDeg) {
  GroundPt2D radianLatLon = {0, M_PI};
  GroundPt2D result = radiansToDegrees(radianLatLon);
  EXPECT_DOUBLE_EQ(result.lat, 0);
  EXPECT_DOUBLE_EQ(result.lon, 180);

  radianLatLon = {0, -M_PI};
  result = radiansToDegrees(radianLatLon);
  EXPECT_DOUBLE_EQ(result.lat, 0);
  EXPECT_DOUBLE_EQ(result.lon, 180);

  radianLatLon = {M_PI, 0};
  result = radiansToDegrees(radianLatLon);
  EXPECT_DOUBLE_EQ(result.lat, 180);
  EXPECT_DOUBLE_EQ(result.lon, 0);

  radianLatLon = {-M_PI, 0};
  result = radiansToDegrees(radianLatLon);
  EXPECT_DOUBLE_EQ(result.lat, -180);
  EXPECT_DOUBLE_EQ(result.lon, 0);
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

TEST(GroundAzimuth, SubSolar) {
  GroundPt2D groundPt = {0, -180};
  GroundPt2D subSolarPt = {0, 90};

  EXPECT_DOUBLE_EQ(270.0, groundAzimuth(groundPt, subSolarPt));
}

TEST(PerpendicularVec, vec) {
Vec vecA = {6.0, 6.0, 6.0};
  Vec vecB = {2.0, 0.0, 0.0};
  Vec result = {0.0, 6.0, 6.0};
  EXPECT_EQ(perpendicularVec(vecA, vecB), result);
}

TEST(UnitVec, vec) {
  Vec vec = {5.0, 12.0, 0.0};
  Vec result = unitVector(vec);
  EXPECT_NEAR(result.x, 0.384615, 1e-6);
  EXPECT_NEAR(result.y, 0.923077, 1e-6);
  EXPECT_EQ(result.z, 0.0);
}

TEST(ScaleVec, vec) {
  Vec vec = {1.0, 2.0, -3.0};
  double scalar = 3.0;
  Vec result = {3.0, 6.0, -9.0};
  EXPECT_EQ(scaleVector(vec, scalar), result);
}

TEST(CrossProduct, vec) {
  Vec vecA = {6.0, 6.0, 6.0};
  Vec vecB = {2.0, 0.0, 0.0};
  Vec result = {0.0, 12.0, -12.0};
  EXPECT_EQ(result, crossProduct(vecA, vecB));
}

TEST(MatrixVecProduct, Vec) {
  Vec a = {0.0,  1.0,  0.0};
  Vec b = {-1.0,  0.0,  0.0};
  Vec c = {0.0,  0.0,  1.0};
  Matrix mat = {a, b, c};
  Vec vec = {1.0,  2.0,  3.0};

  Vec result = {2.0, -1.0, 3.0};
  EXPECT_EQ(result, matrixVecProduct(mat, vec));
}