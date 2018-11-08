#include "Spectel.h" 
#include "Constants.h" 
#include "IException.h"
#include "Pixel.h"
#include "SpecialPixel.h"

#include <QString>
#include <QDebug> 

#include <gtest/gtest.h>

#define TEST_EPSILON 1e-8

TEST(SpectelTest, SimpleConstructor) {
  Isis::Spectel spec(1, 2, 100, 123.45, 0.1, 0.05);
  EXPECT_EQ(spec.sample(), 1);
  EXPECT_EQ(spec.line(), 2);
  EXPECT_EQ(spec.band(), 100);
  EXPECT_EQ(spec.DN(), 123.45);
  EXPECT_EQ(spec.centerWavelength(), 0.1);
  EXPECT_EQ(spec.filterWidth(), 0.05);
}

TEST(SpectelTest, CopyConstructor) {
  Isis::Spectel spec(1, 2, 100, 123.45, 0.1, 0.05);
  Isis::Spectel spec2(spec);
  EXPECT_EQ(spec2.sample(), 1);
  EXPECT_EQ(spec2.line(), 2);
  EXPECT_EQ(spec2.band(), 100);
  EXPECT_EQ(spec2.DN(), 123.45);
  EXPECT_EQ(spec2.centerWavelength(), 0.1);
  EXPECT_EQ(spec2.filterWidth(), 0.05);
}

TEST(SpectelTest, AssignmentOperator) {
  Isis::Spectel spec(1, 2, 100, 123.45, 0.1, 0.05);
  Isis::Spectel spec2(spec);
  spec2 = spec;
  EXPECT_EQ(spec2.sample(), 1);
  EXPECT_EQ(spec2.line(), 2);
  EXPECT_EQ(spec2.band(), 100);
  EXPECT_EQ(spec2.DN(), 123.45);
  EXPECT_EQ(spec2.centerWavelength(), 0.1);
  EXPECT_EQ(spec2.filterWidth(), 0.05);
}

TEST(SpectelTest, PixelConstructor) {
  Isis::Spectel spec(Isis::Pixel(1, 2, 3, 0.4), 0.5, 0.6);
  EXPECT_EQ(spec.sample(), 1);
  EXPECT_EQ(spec.line(), 2);
  EXPECT_EQ(spec.band(), 3);
  EXPECT_EQ(spec.DN(), 0.4);
  EXPECT_EQ(spec.centerWavelength(), 0.5);
  EXPECT_EQ(spec.filterWidth(), 0.6);
}


