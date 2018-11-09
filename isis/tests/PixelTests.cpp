#include <gtest/gtest.h>

#include <functional>
#include <iostream>
#include <string>
// #include <tuple>
#include <utility>
#include <vector>

#include "Pixel.h"
#include "SpecialPixel.h"

using namespace Isis;

TEST(Pixel, DefaultConstructor) {
  Pixel p;
  EXPECT_EQ(0, p.sample());
  EXPECT_EQ(0, p.line());
  EXPECT_EQ(0, p.band());
  EXPECT_EQ(Isis::Null, p.DN());
}

TEST(Pixel, Constructor1) {
  Pixel p(0, 1, 2, 3.0);
  EXPECT_EQ(0, p.sample());
  EXPECT_EQ(1, p.line());
  EXPECT_EQ(2, p.band());
  EXPECT_DOUBLE_EQ(3.0, p.DN());
}

TEST(Pixel, CopyConstructor) {
  Pixel p(0, 1, 2, 3.0);
  // Note this is equivalent to Pixel copy(p);
  Pixel copy = p;
  EXPECT_EQ(0, copy.sample());
  EXPECT_EQ(1, copy.line());
  EXPECT_EQ(2, copy.band());
  EXPECT_DOUBLE_EQ(3.0, copy.DN());
}

TEST(Pixel, CopyAssignment) {
  Pixel p(0, 1, 2, 3.0);
  Pixel copy;
  copy = p;
  EXPECT_EQ(0, copy.sample());
  EXPECT_EQ(1, copy.line());
  EXPECT_EQ(2, copy.band());
  EXPECT_DOUBLE_EQ(3.0, copy.DN());
}

TEST(Pixel, To8Bit) {
  // Zero test
  EXPECT_EQ(Isis::NULL1, Pixel::To8Bit(0.0));
  // Negative test
  EXPECT_EQ(Isis::LOW_REPR_SAT1, Pixel::To8Bit(-1.0));
  // Trivial positive test
  EXPECT_EQ(1, Pixel::To8Bit(1.0));
  // Minimum valid input // Isis::ValidMinimum becomes \0
  EXPECT_EQ(Isis::LOW_REPR_SAT1, Pixel::To8Bit(Isis::ValidMinimum));
  // Maximum valid input // Isis::ValidMaximum becomes \xFF (255)
  EXPECT_EQ(Isis::HIGH_REPR_SAT1, Pixel::To8Bit(Isis::ValidMaximum));
  // "Null" pixel
  EXPECT_EQ(Isis::NULL1, Pixel::To8Bit(Isis::Null));
  // HRS
  EXPECT_EQ(Isis::HIGH_REPR_SAT1, Pixel::To8Bit(Isis::Hrs));
  // HIS
  EXPECT_EQ(Isis::HIGH_INSTR_SAT1, Pixel::To8Bit(Isis::His));
  // LRS
  EXPECT_EQ(Isis::LOW_REPR_SAT1, Pixel::To8Bit(Isis::Lrs));
  // LIS
  EXPECT_EQ(Isis::LOW_INSTR_SAT1, Pixel::To8Bit(Isis::Lis));

  /* @todo -- VALID MIN; VALID MAX */
}

TEST(Pixel, To16UBit) {
  // Zero test
  EXPECT_EQ(Isis::NULLU2, Pixel::To16UBit(0.0));
  // Negative test // -1.0 becomes HIGH_REPR_SATU2, not LOW_REPR_SATU2
  EXPECT_EQ(Isis::HIGH_REPR_SATU2, Pixel::To16UBit(-1.0));
  // Positive test
  EXPECT_EQ(1, Pixel::To16UBit(1.0));
  // Minimum valid input
  EXPECT_EQ(Isis::LOW_REPR_SATU2, Pixel::To16UBit(Isis::ValidMinimum));
  // Maximum valid input
  EXPECT_EQ(Isis::HIGH_REPR_SATU2, Pixel::To16UBit(Isis::ValidMaximum));
  // "Null" pixel
  EXPECT_EQ(Isis::NULLU2, Pixel::To16UBit(Isis::Null));
  // HRS
  EXPECT_EQ(Isis::HIGH_REPR_SATU2, Pixel::To16UBit(Isis::Hrs));
  // HIS
  EXPECT_EQ(Isis::HIGH_INSTR_SATU2, Pixel::To16UBit(Isis::His));
  // LRS
  EXPECT_EQ(Isis::LOW_REPR_SATU2, Pixel::To16UBit(Isis::Lrs));
  // LIS
  EXPECT_EQ(Isis::LOW_INSTR_SATU2, Pixel::To16UBit(Isis::Lis));
}

TEST(Pixel, To16Bit) {
  // Zero test
  EXPECT_EQ(0, Pixel::To16Bit(0.0));
  // Negative test
  EXPECT_EQ(-1, Pixel::To16Bit(-1.0));
  // Positive test
  EXPECT_EQ(1, Pixel::To16Bit(1.0));
  // Minimum valid input
  EXPECT_EQ(Isis::LOW_REPR_SAT2, Pixel::To16Bit(Isis::ValidMinimum));
  // Maximum valid input
  EXPECT_EQ(Isis::HIGH_REPR_SAT2, Pixel::To16Bit(Isis::ValidMaximum));
  // "Null" pixel
  EXPECT_EQ(Isis::NULL2, Pixel::To16Bit(Isis::Null));
  // HRS
  EXPECT_EQ(Isis::HIGH_REPR_SAT2, Pixel::To16Bit(Isis::Hrs));
  // HIS
  EXPECT_EQ(Isis::HIGH_INSTR_SAT2, Pixel::To16Bit(Isis::His));
  // LRS
  EXPECT_EQ(Isis::LOW_REPR_SAT2, Pixel::To16Bit(Isis::Lrs));
  // LIS
  EXPECT_EQ(Isis::LOW_INSTR_SAT2, Pixel::To16Bit(Isis::Lis));
}

TEST(Pixel, To32Bit) {
  // Zero test
  EXPECT_EQ(0, Pixel::To32Bit(0.0));
  // Negative test
  EXPECT_EQ(-1, Pixel::To32Bit(-1.0));
  // Positive test
  EXPECT_EQ(1, Pixel::To32Bit(1.0));
  // Minimum valid input // Isis::ValidMinimum becomes -inf (not ILOW_REPR_SAT4)
  EXPECT_FLOAT_EQ(Isis::LOW_REPR_SAT4, Pixel::To32Bit(Isis::ValidMinimum));
  // Maximum valid input // Isis::Maximum becomes inf (not IHIGH_REPR_SAT4)
  EXPECT_FLOAT_EQ(Isis::HIGH_REPR_SAT4, Pixel::To32Bit(Isis::ValidMaximum));
  // "Null" pixel
  EXPECT_FLOAT_EQ(Isis::NULL4, Pixel::To32Bit(Isis::Null));
  // HRS
  EXPECT_FLOAT_EQ(Isis::HIGH_REPR_SAT4, Pixel::To32Bit(Isis::Hrs));
  // HIS
  EXPECT_FLOAT_EQ(Isis::HIGH_INSTR_SAT4, Pixel::To32Bit(Isis::His));
  // LRS
  EXPECT_FLOAT_EQ(Isis::LOW_REPR_SAT4, Pixel::To32Bit(Isis::Lrs));
  // LIS
  EXPECT_FLOAT_EQ(Isis::LOW_INSTR_SAT4, Pixel::To32Bit(Isis::Lis));
}

TEST(Pixel, ToDouble) {
  // unsigned char
  unsigned char uc = 0;
  EXPECT_DOUBLE_EQ(Isis::Null, Pixel::ToDouble(uc));

  // short
  short s = 0.0;
  EXPECT_DOUBLE_EQ(0.0, Pixel::ToDouble(s));

  // unsigned short
  unsigned short us = 0.0;
  EXPECT_DOUBLE_EQ(Isis::Null, Pixel::ToDouble(us));

  // float
  float f = 0.0;
  EXPECT_DOUBLE_EQ(0.0, Pixel::ToDouble(f));

}

TEST(Pixel, ToFloat) {
  // unsigned char
  unsigned char uc = 0;
  EXPECT_DOUBLE_EQ(Isis::NULL4, Pixel::ToFloat(uc));

  // short
  short s = 0.0;
  EXPECT_DOUBLE_EQ(0.0, Pixel::ToFloat(s));

  // unsigned short
  unsigned short us = 0.0;
  EXPECT_DOUBLE_EQ(Isis::NULL4, Pixel::ToFloat(us));

  // float
  float f = 0.0;
  EXPECT_DOUBLE_EQ(0.0, Pixel::ToFloat(f));

}

class PixelSpecialFixture : public ::testing::TestWithParam< std::pair<bool, double> > {
protected:
  void PixelSetup(double pixelValue) {
    pixel = new Pixel(1, 2, 3, pixelValue);
  }
  void SetUp() override {}
  void TearDown() override {
    if (pixel)
      delete pixel;
    pixel = nullptr;
  }

  Pixel *pixel;
};

TEST_P(PixelSpecialFixture, static_IsSpecial) {
  EXPECT_EQ(GetParam().first, Pixel::IsSpecial(GetParam().second));
}
std::vector< std::pair<bool, double> > specialVector{
  std::make_pair(true, Isis::His),
  std::make_pair(true, Isis::Hrs),
  std::make_pair(true, Isis::Lis),
  std::make_pair(true, Isis::Lrs),
  std::make_pair(true, Isis::Null),
  std::make_pair(false, Isis::ValidMaximum),
  std::make_pair(false, Isis::ValidMinimum)
};
INSTANTIATE_TEST_CASE_P(static_IsSpecial,
                        PixelSpecialFixture,
                        ::testing::ValuesIn(specialVector));

TEST(Pixel, static_IsSpecial) {
  EXPECT_TRUE(Pixel::IsSpecial(Isis::His));
  EXPECT_TRUE(Pixel::IsSpecial(Isis::Hrs));
  EXPECT_TRUE(Pixel::IsSpecial(Isis::Lis));
  EXPECT_TRUE(Pixel::IsSpecial(Isis::Lrs));
  EXPECT_TRUE(Pixel::IsSpecial(Isis::Null));
  EXPECT_FALSE(Pixel::IsSpecial(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsSpecial(Isis::ValidMinimum));
}

TEST_P(PixelSpecialFixture, IsSpecial) {
  PixelSetup(GetParam().second);
  EXPECT_EQ(GetParam().first, pixel->IsSpecial());
}
INSTANTIATE_TEST_CASE_P(IsSpecial,
                        PixelSpecialFixture,
                        ::testing::ValuesIn(specialVector
                        ));

TEST(Pixel, IsValid) {
  EXPECT_FALSE(Pixel::IsValid(Isis::His));
  EXPECT_FALSE(Pixel::IsValid(Isis::Hrs));
  EXPECT_FALSE(Pixel::IsValid(Isis::Lis));
  EXPECT_FALSE(Pixel::IsValid(Isis::Lrs));
  EXPECT_FALSE(Pixel::IsValid(Isis::Null));
  EXPECT_TRUE(Pixel::IsValid(Isis::ValidMaximum));
  EXPECT_TRUE(Pixel::IsValid(Isis::ValidMinimum));
}

TEST(Pixel, IsNull) {
  EXPECT_FALSE(Pixel::IsNull(Isis::His));
  EXPECT_FALSE(Pixel::IsNull(Isis::Hrs));
  EXPECT_FALSE(Pixel::IsNull(Isis::Lis));
  EXPECT_FALSE(Pixel::IsNull(Isis::Lrs));
  EXPECT_TRUE(Pixel::IsNull(Isis::Null));
  EXPECT_FALSE(Pixel::IsNull(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsNull(Isis::ValidMinimum));
}

TEST(Pixel, IsHigh) {
  EXPECT_TRUE(Pixel::IsHigh(Isis::His));
  EXPECT_TRUE(Pixel::IsHigh(Isis::Hrs));
  EXPECT_FALSE(Pixel::IsHigh(Isis::Lis));
  EXPECT_FALSE(Pixel::IsHigh(Isis::Lrs));
  EXPECT_FALSE(Pixel::IsHigh(Isis::Null));
  EXPECT_FALSE(Pixel::IsHigh(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsHigh(Isis::ValidMinimum));
}

TEST(Pixel, IsLow) {
  EXPECT_FALSE(Pixel::IsLow(Isis::His));
  EXPECT_FALSE(Pixel::IsLow(Isis::Hrs));
  EXPECT_TRUE(Pixel::IsLow(Isis::Lis));
  EXPECT_TRUE(Pixel::IsLow(Isis::Lrs));
  EXPECT_FALSE(Pixel::IsLow(Isis::Null));
  EXPECT_FALSE(Pixel::IsLow(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsLow(Isis::ValidMinimum));
}

TEST(Pixel, IsHrs) {
  EXPECT_FALSE(Pixel::IsHrs(Isis::His));
  EXPECT_TRUE(Pixel::IsHrs(Isis::Hrs));
  EXPECT_FALSE(Pixel::IsHrs(Isis::Lis));
  EXPECT_FALSE(Pixel::IsHrs(Isis::Lrs));
  EXPECT_FALSE(Pixel::IsHrs(Isis::Null));
  EXPECT_FALSE(Pixel::IsHrs(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsHrs(Isis::ValidMinimum));
}

TEST(Pixel, IsHis) {
  EXPECT_TRUE(Pixel::IsHis(Isis::His));
  EXPECT_FALSE(Pixel::IsHis(Isis::Hrs));
  EXPECT_FALSE(Pixel::IsHis(Isis::Lis));
  EXPECT_FALSE(Pixel::IsHis(Isis::Lrs));
  EXPECT_FALSE(Pixel::IsHis(Isis::Null));
  EXPECT_FALSE(Pixel::IsHis(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsHis(Isis::ValidMinimum));
}

TEST(Pixel, IsLis) {
  EXPECT_FALSE(Pixel::IsLis(Isis::His));
  EXPECT_FALSE(Pixel::IsLis(Isis::Hrs));
  EXPECT_TRUE(Pixel::IsLis(Isis::Lis));
  EXPECT_FALSE(Pixel::IsLis(Isis::Lrs));
  EXPECT_FALSE(Pixel::IsLis(Isis::Null));
  EXPECT_FALSE(Pixel::IsLis(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsLis(Isis::ValidMinimum));
}

TEST(Pixel, IsLrs) {
  EXPECT_FALSE(Pixel::IsLrs(Isis::His));
  EXPECT_FALSE(Pixel::IsLrs(Isis::Hrs));
  EXPECT_FALSE(Pixel::IsLrs(Isis::Lis));
  EXPECT_TRUE(Pixel::IsLrs(Isis::Lrs));
  EXPECT_FALSE(Pixel::IsLrs(Isis::Null));
  EXPECT_FALSE(Pixel::IsLrs(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsLrs(Isis::ValidMinimum));
}

TEST(Pixel, ToString) {
  EXPECT_EQ(std::string("1"), Pixel::ToString(1.0));
  EXPECT_EQ(std::string("-1.2"), Pixel::ToString(-1.2));

  // Special pixels
  EXPECT_EQ(std::string("His"), Pixel::ToString(Isis::His));
  EXPECT_EQ(std::string("Hrs"), Pixel::ToString(Isis::Hrs));
  EXPECT_EQ(std::string("Lis"), Pixel::ToString(Isis::Lis));
  EXPECT_EQ(std::string("Lrs"), Pixel::ToString(Isis::Lrs));
  EXPECT_EQ(std::string("Null"), Pixel::ToString(Isis::Null));
  EXPECT_EQ(std::string("Invalid"), Pixel::ToString(-1.0e+1000));
}
