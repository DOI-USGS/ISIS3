#include "BulletDskShape.h"
#include "IException.h"
#include "SpecialPixel.h"

#include <gtest/gtest.h>

using namespace Isis;

TEST(BulletDskShapeTests, DefaultConstructor) {
  BulletDskShape defaultDskShape;
  EXPECT_EQ(defaultDskShape.name(), "");
  EXPECT_FALSE((bool) defaultDskShape.body());
  EXPECT_EQ(defaultDskShape.getNumTriangles(), 0);
  EXPECT_EQ(defaultDskShape.getNumVertices(), 0);
}

TEST(BulletDskShapeTests, SingleSegment) {
  QString dskfile("$base/testData/hay_a_amica_5_itokawashape_v1_0_64q.bds");

  BulletDskShape itokawaShape(dskfile);
  EXPECT_EQ(itokawaShape.name(), "");
  EXPECT_DOUBLE_EQ(itokawaShape.maximumDistance(), 0.68395571742620886);
  EXPECT_TRUE((bool) itokawaShape.body());
  EXPECT_EQ(itokawaShape.getNumTriangles(), 49152);
  EXPECT_EQ(itokawaShape.getNumVertices(), 25350);

  btMatrix3x3 truthTriangle(-0.15153, 0.08183, 0.07523,
                            -0.05653, 0.08832, 0.08776,
                             0.08183, 0.07523, -0.14726);
  btMatrix3x3 triangle = itokawaShape.getTriangle(0);
  EXPECT_TRUE(triangle == truthTriangle);

  btVector3 truthNormal(-0.0013612620999999992,
                         0.024060550800000004,
                        -0.0021415063999999989);
  btVector3 normal = itokawaShape.getNormal(0);
  EXPECT_TRUE(normal == truthNormal);
}

TEST(BulletDskShapeTests, BadFile) {
  EXPECT_THROW(BulletDskShape("not_a_file"), IException);
}


TEST(BulletDskShapeTests, MutiSegment) {
  QString dskfile("$base/testData/test_shape.bds");

  BulletDskShape multiseg(dskfile);
  EXPECT_EQ(multiseg.name(), "");
  EXPECT_DOUBLE_EQ(multiseg.maximumDistance(), 7.3484692283495345);
  EXPECT_TRUE((bool) multiseg.body());
  EXPECT_EQ(multiseg.getNumTriangles(), 14);
  EXPECT_EQ(multiseg.getNumVertices(), 13);

  btMatrix3x3 truthTriangle(0, 0, 6,
                            0, 6, 1,
                            0, 5, 2);
  btMatrix3x3 triangle = multiseg.getTriangle(0);
  EXPECT_TRUE(triangle == truthTriangle);


  btVector3 truthNormal(1, 0, 0);
  btVector3 normal = multiseg.getNormal(0);
  EXPECT_TRUE(normal == truthNormal);
}
