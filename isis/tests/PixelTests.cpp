#include <gtest/gtest.h>

#include <functional>
#include <iostream>
#include <limits>
#include <string>
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

TEST(Pixel, static_To8Bit) {
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
}

TEST(Pixel, To8Bit) {
  // Zero test
  EXPECT_EQ(Isis::NULL1, Pixel(1, 2, 3, 0.0).To8Bit());
  // Negative test
  EXPECT_EQ(Isis::LOW_REPR_SAT1, Pixel(1, 2, 3, -1.0).To8Bit());
  // Trivial positive test
  EXPECT_EQ(1, Pixel(1, 2, 3, 1.0).To8Bit());
  // Minimum valid input // Isis::ValidMinimum becomes \0
  EXPECT_EQ(Isis::LOW_REPR_SAT1, Pixel(1, 2, 3, Isis::ValidMinimum).To8Bit());
  // Maximum valid input // Isis::ValidMaximum becomes \xFF (255)
  EXPECT_EQ(Isis::HIGH_REPR_SAT1, Pixel(1, 2, 3, Isis::ValidMaximum).To8Bit());
  // "Null" pixel
  EXPECT_EQ(Isis::NULL1, Pixel(1, 2, 3, Isis::Null).To8Bit());
  // HRS
  EXPECT_EQ(Isis::HIGH_REPR_SAT1, Pixel(1, 2, 3, Isis::Hrs).To8Bit());
  // HIS
  EXPECT_EQ(Isis::HIGH_INSTR_SAT1, Pixel(1, 2, 3, Isis::His).To8Bit());
  // LRS
  EXPECT_EQ(Isis::LOW_REPR_SAT1, Pixel(1, 2, 3, Isis::Lrs).To8Bit());
  // LIS
  EXPECT_EQ(Isis::LOW_INSTR_SAT1, Pixel(1, 2, 3, Isis::Lis).To8Bit());
}

TEST(Pixel, static_To16UBit) {
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

TEST(Pixel, To16UBit) {
  // Zero test
  EXPECT_EQ(Isis::NULLU2, Pixel(1, 2, 3, 0.0).To16Ubit());
  // Negative test // -1.0 becomes HIGH_REPR_SATU2, not LOW_REPR_SATU2
  EXPECT_EQ(Isis::HIGH_REPR_SATU2, Pixel(1, 2, 3, -1.0).To16Ubit());
  // Positive test
  EXPECT_EQ(1, Pixel(1, 2, 3, 1.0).To16Ubit());
  // Minimum valid input
  EXPECT_EQ(Isis::LOW_REPR_SATU2, Pixel(1, 2, 3, Isis::ValidMinimum).To16Ubit());
  // Maximum valid input
  EXPECT_EQ(Isis::HIGH_REPR_SATU2, Pixel(1, 2, 3, Isis::ValidMaximum).To16Ubit());
  // "Null" pixel
  EXPECT_EQ(Isis::NULLU2, Pixel(1, 2, 3, Isis::Null).To16Ubit());
  // HRS
  EXPECT_EQ(Isis::HIGH_REPR_SATU2, Pixel(1, 2, 3, Isis::Hrs).To16Ubit());
  // HIS
  EXPECT_EQ(Isis::HIGH_INSTR_SATU2, Pixel(1, 2, 3, Isis::His).To16Ubit());
  // LRS
  EXPECT_EQ(Isis::LOW_REPR_SATU2, Pixel(1, 2, 3, Isis::Lrs).To16Ubit());
  // LIS
  EXPECT_EQ(Isis::LOW_INSTR_SATU2, Pixel(1 ,2, 3, Isis::Lis).To16Ubit());
}

TEST(Pixel, static_To16Bit) {
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

TEST(Pixel, To16Bit) {
  // Zero test
  EXPECT_EQ(0, Pixel(1, 2, 3, 0.0).To16Bit());
  // Negative test
  EXPECT_EQ(-1, Pixel(1, 2, 3, -1.0).To16Bit());
  // Positive test
  EXPECT_EQ(1, Pixel(1, 2, 3, 1.0).To16Bit());
  // Minimum valid input
  EXPECT_EQ(Isis::LOW_REPR_SAT2, Pixel(1, 2, 3, Isis::ValidMinimum).To16Bit());
  // Maximum valid input
  EXPECT_EQ(Isis::HIGH_REPR_SAT2, Pixel(1, 2, 3, Isis::ValidMaximum).To16Bit());
  // "Null" pixel
  EXPECT_EQ(Isis::NULL2, Pixel(1, 2, 3, Isis::Null).To16Bit());
  // HRS
  EXPECT_EQ(Isis::HIGH_REPR_SAT2, Pixel(1, 2, 3, Isis::Hrs).To16Bit());
  // HIS
  EXPECT_EQ(Isis::HIGH_INSTR_SAT2, Pixel(1, 2, 3, Isis::His).To16Bit());
  // LRS
  EXPECT_EQ(Isis::LOW_REPR_SAT2, Pixel(1, 2, 3, Isis::Lrs).To16Bit());
  // LIS
  EXPECT_EQ(Isis::LOW_INSTR_SAT2, Pixel(1, 2, 3, Isis::Lis).To16Bit());
}

TEST(Pixel, static_To32Bit) {
  // Zero test
  EXPECT_EQ(0, Pixel::To32Bit(0.0));
  // Negative test
  EXPECT_EQ(-1, Pixel::To32Bit(-1.0));
  // Positive test
  EXPECT_EQ(1, Pixel::To32Bit(1.0));
  // Minimum valid input
  // EXPECT_FLOAT_EQ(Isis::LOW_REPR_SAT4, Pixel::To32Bit(Isis::ValidMinimum));
  // Maximum valid input // Isis::Maximum becomes inf (not IHIGH_REPR_SAT4)
  // EXPECT_FLOAT_EQ(Isis::HIGH_REPR_SAT4, Pixel::To32Bit(Isis::ValidMaximum));
  // "Null" pixel
  EXPECT_EQ(Isis::NULL4, Pixel::To32Bit(Isis::Null));
  // HRS
  EXPECT_EQ(Isis::HIGH_REPR_SAT4, Pixel::To32Bit(Isis::Hrs));
  // HIS
  EXPECT_EQ(Isis::HIGH_INSTR_SAT4, Pixel::To32Bit(Isis::His));
  // LRS
  EXPECT_EQ(Isis::LOW_REPR_SAT4, Pixel::To32Bit(Isis::Lrs));
  // LIS
  EXPECT_EQ(Isis::LOW_INSTR_SAT4, Pixel::To32Bit(Isis::Lis));
}

TEST(Pixel, To32Bit) {
  // Zero test
  EXPECT_EQ(0, Pixel(1, 2, 3, 0.0).To32Bit());
  // Negative test
  EXPECT_EQ(-1, Pixel(1, 2, 3, -1.0).To32Bit());
  // Positive test
  EXPECT_EQ(1, Pixel(1, 2, 3, 1.0).To32Bit());
  // Minimum valid input
  // EXPECT_EQ(Isis::LOW_REPR_SAT4, Pixel(1, 2, 3, Isis::ValidMinimum).To32Bit());
  // Maximum valid input // Isis::Maximum becomes inf (not HIGH_REPR_SAT4)
  // EXPECT_FLOAT_EQ(Isis::HIGH_REPR_SAT4, Pixel(1, 2, 3, Isis::ValidMaximum).To32Bit());
  // "Null" pixel
  EXPECT_EQ(Isis::NULL4, Pixel(1, 2, 3, Isis::Null).To32Bit());
  // HRS
  EXPECT_EQ(Isis::HIGH_REPR_SAT4, Pixel(1, 2, 3, Isis::Hrs).To32Bit());
  // HIS
  EXPECT_EQ(Isis::HIGH_INSTR_SAT4, Pixel(1, 2, 3, Isis::His).To32Bit());
  // LRS
  EXPECT_EQ(Isis::LOW_REPR_SAT4, Pixel(1, 2, 3, Isis::Lrs).To32Bit());
  // LIS
  EXPECT_EQ(Isis::LOW_INSTR_SAT4, Pixel(1, 2, 3, Isis::Lis).To32Bit());
}

TEST(Pixel, static_ToDouble) {
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

TEST(Pixel, ToDouble) {
  EXPECT_EQ(0.0, Pixel(1, 2, 3, 0.0).ToDouble());
}

TEST(Pixel, static_ToFloat) {
  // unsigned char
  unsigned char uc = 0;
  EXPECT_FLOAT_EQ(Isis::NULL4, Pixel::ToFloat(uc));

  // short
  short s = 0.0;
  EXPECT_FLOAT_EQ(0.0, Pixel::ToFloat(s));

  // unsigned short
  unsigned short us = 0.0;
  EXPECT_FLOAT_EQ(Isis::NULL4, Pixel::ToFloat(us));

  // float
  float f = 0.0;
  EXPECT_FLOAT_EQ(0.0, Pixel::ToFloat(f));
}

TEST(Pixel, ToFloat) {
  EXPECT_FLOAT_EQ(0.0, Pixel(1, 2, 3, 0.0).ToFloat());
}

TEST(Pixel, static_IsSpecial) {
  EXPECT_TRUE(Pixel::IsSpecial(Isis::His));
  EXPECT_TRUE(Pixel::IsSpecial(Isis::Hrs));
  EXPECT_TRUE(Pixel::IsSpecial(Isis::Lis));
  EXPECT_TRUE(Pixel::IsSpecial(Isis::Lrs));
  EXPECT_TRUE(Pixel::IsSpecial(Isis::Null));
  EXPECT_FALSE(Pixel::IsSpecial(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsSpecial(Isis::ValidMinimum));
}

TEST(Pixel, IsSpecial) {
  EXPECT_TRUE(Pixel(1,2,3,Isis::His).IsSpecial());
  EXPECT_TRUE(Pixel(1,2,3,Isis::Hrs).IsSpecial());
  EXPECT_TRUE(Pixel(1,2,3,Isis::Lis).IsSpecial());
  EXPECT_TRUE(Pixel(1,2,3,Isis::Lrs).IsSpecial());
  EXPECT_TRUE(Pixel(1,2,3,Isis::Null).IsSpecial());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMaximum).IsSpecial());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMinimum).IsSpecial());
}

TEST(Pixel, static_IsValid) {
  EXPECT_FALSE(Pixel::IsValid(Isis::His));
  EXPECT_FALSE(Pixel::IsValid(Isis::Hrs));
  EXPECT_FALSE(Pixel::IsValid(Isis::Lis));
  EXPECT_FALSE(Pixel::IsValid(Isis::Lrs));
  EXPECT_FALSE(Pixel::IsValid(Isis::Null));
  EXPECT_TRUE(Pixel::IsValid(Isis::ValidMaximum));
  EXPECT_TRUE(Pixel::IsValid(Isis::ValidMinimum));
}

TEST(Pixel, IsValid) {
  EXPECT_FALSE(Pixel(1,2,3,Isis::His).IsValid());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Hrs).IsValid());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Lis).IsValid());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Lrs).IsValid());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Null).IsValid());
  EXPECT_TRUE(Pixel(1,2,3,Isis::ValidMaximum).IsValid());
  EXPECT_TRUE(Pixel(1,2,3,Isis::ValidMinimum).IsValid());
}

TEST(Pixel, static_IsNull) {
  EXPECT_FALSE(Pixel::IsNull(Isis::His));
  EXPECT_FALSE(Pixel::IsNull(Isis::Hrs));
  EXPECT_FALSE(Pixel::IsNull(Isis::Lis));
  EXPECT_FALSE(Pixel::IsNull(Isis::Lrs));
  EXPECT_TRUE(Pixel::IsNull(Isis::Null));
  EXPECT_FALSE(Pixel::IsNull(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsNull(Isis::ValidMinimum));
}

TEST(Pixel, IsNull) {
  EXPECT_FALSE(Pixel(1,2,3,Isis::His).IsNull());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Hrs).IsNull());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Lis).IsNull());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Lrs).IsNull());
  EXPECT_TRUE(Pixel(1,2,3,Isis::Null).IsNull());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMaximum).IsNull());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMinimum).IsNull());
}

TEST(Pixel, static_IsHigh) {
  EXPECT_TRUE (Pixel::IsHigh(Isis::His));
  EXPECT_TRUE (Pixel::IsHigh(Isis::Hrs));
  EXPECT_FALSE(Pixel::IsHigh(Isis::Lis));
  EXPECT_FALSE(Pixel::IsHigh(Isis::Lrs));
  EXPECT_FALSE(Pixel::IsHigh(Isis::Null));
  EXPECT_FALSE(Pixel::IsHigh(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsHigh(Isis::ValidMinimum));
}

TEST(Pixel, IsHigh) {
  EXPECT_TRUE(Pixel(1,2,3,Isis::His).IsHigh());
  EXPECT_TRUE(Pixel(1,2,3,Isis::Hrs).IsHigh());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Lis).IsHigh());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Lrs).IsHigh());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Null).IsHigh());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMaximum).IsHigh());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMinimum).IsHigh());
}

TEST(Pixel, static_IsLow) {
  EXPECT_FALSE(Pixel::IsLow(Isis::His));
  EXPECT_FALSE(Pixel::IsLow(Isis::Hrs));
  EXPECT_TRUE(Pixel::IsLow(Isis::Lis));
  EXPECT_TRUE(Pixel::IsLow(Isis::Lrs));
  EXPECT_FALSE(Pixel::IsLow(Isis::Null));
  EXPECT_FALSE(Pixel::IsLow(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsLow(Isis::ValidMinimum));
}

TEST(Pixel, IsLow) {
  EXPECT_FALSE(Pixel(1,2,3,Isis::His).IsLow());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Hrs).IsLow());
  EXPECT_TRUE(Pixel(1,2,3,Isis::Lis).IsLow());
  EXPECT_TRUE(Pixel(1,2,3,Isis::Lrs).IsLow());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Null).IsLow());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMaximum).IsLow());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMinimum).IsLow());
}

TEST(Pixel, static_IsHrs) {
  EXPECT_FALSE(Pixel::IsHrs(Isis::His));
  EXPECT_TRUE(Pixel::IsHrs(Isis::Hrs));
  EXPECT_FALSE(Pixel::IsHrs(Isis::Lis));
  EXPECT_FALSE(Pixel::IsHrs(Isis::Lrs));
  EXPECT_FALSE(Pixel::IsHrs(Isis::Null));
  EXPECT_FALSE(Pixel::IsHrs(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsHrs(Isis::ValidMinimum));
}

TEST(Pixel, IsHrs) {
  EXPECT_FALSE(Pixel(1,2,3,Isis::His).IsHrs());
  EXPECT_TRUE(Pixel(1,2,3,Isis::Hrs).IsHrs());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Lis).IsHrs());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Lrs).IsHrs());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Null).IsHrs());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMaximum).IsHrs());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMinimum).IsHrs());
}

TEST(Pixel, static_IsHis) {
  EXPECT_TRUE(Pixel::IsHis(Isis::His));
  EXPECT_FALSE(Pixel::IsHis(Isis::Hrs));
  EXPECT_FALSE(Pixel::IsHis(Isis::Lis));
  EXPECT_FALSE(Pixel::IsHis(Isis::Lrs));
  EXPECT_FALSE(Pixel::IsHis(Isis::Null));
  EXPECT_FALSE(Pixel::IsHis(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsHis(Isis::ValidMinimum));
}

TEST(Pixel, IsHis) {
  EXPECT_TRUE(Pixel(1,2,3,Isis::His).IsHis());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Hrs).IsHis());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Lis).IsHis());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Lrs).IsHis());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Null).IsHis());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMaximum).IsHis());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMinimum).IsHis());
}

TEST(Pixel, static_IsLis) {
  EXPECT_FALSE(Pixel::IsLis(Isis::His));
  EXPECT_FALSE(Pixel::IsLis(Isis::Hrs));
  EXPECT_TRUE(Pixel::IsLis(Isis::Lis));
  EXPECT_FALSE(Pixel::IsLis(Isis::Lrs));
  EXPECT_FALSE(Pixel::IsLis(Isis::Null));
  EXPECT_FALSE(Pixel::IsLis(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsLis(Isis::ValidMinimum));
}

TEST(Pixel, IsLis) {
  EXPECT_FALSE(Pixel(1,2,3,Isis::His).IsLis());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Hrs).IsLis());
  EXPECT_TRUE(Pixel(1,2,3,Isis::Lis).IsLis());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Lrs).IsLis());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Null).IsLis());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMaximum).IsLis());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMinimum).IsLis());
}

TEST(Pixel, static_IsLrs) {
  EXPECT_FALSE(Pixel::IsLrs(Isis::His));
  EXPECT_FALSE(Pixel::IsLrs(Isis::Hrs));
  EXPECT_FALSE(Pixel::IsLrs(Isis::Lis));
  EXPECT_TRUE(Pixel::IsLrs(Isis::Lrs));
  EXPECT_FALSE(Pixel::IsLrs(Isis::Null));
  EXPECT_FALSE(Pixel::IsLrs(Isis::ValidMaximum));
  EXPECT_FALSE(Pixel::IsLrs(Isis::ValidMinimum));
}

TEST(Pixel, IsLrs) {
  EXPECT_FALSE(Pixel(1,2,3,Isis::His).IsLrs());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Hrs).IsLrs());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Lis).IsLrs());
  EXPECT_TRUE(Pixel(1,2,3,Isis::Lrs).IsLrs());
  EXPECT_FALSE(Pixel(1,2,3,Isis::Null).IsLrs());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMaximum).IsLrs());
  EXPECT_FALSE(Pixel(1,2,3,Isis::ValidMinimum).IsLrs());
}

TEST(Pixel, static_ToString) {
  EXPECT_EQ(std::string("1"), Pixel::ToString(1.0));
  EXPECT_EQ(std::string("-1.2"), Pixel::ToString(-1.2));
  // Special pixels
  EXPECT_EQ(std::string("His"), Pixel::ToString(Isis::His));
  EXPECT_EQ(std::string("Hrs"), Pixel::ToString(Isis::Hrs));
  EXPECT_EQ(std::string("Lis"), Pixel::ToString(Isis::Lis));
  EXPECT_EQ(std::string("Lrs"), Pixel::ToString(Isis::Lrs));
  EXPECT_EQ(std::string("Null"), Pixel::ToString(Isis::Null));
}

TEST(Pixel, ToString) {
  EXPECT_EQ(std::string("1"), Pixel(1,2,3,1.0).ToString());
  EXPECT_EQ(std::string("-1.2"), Pixel(1,2,3,-1.2).ToString());
  // Special pixels
  EXPECT_EQ(std::string("His"), Pixel(1,2,3,Isis::His).ToString());
  EXPECT_EQ(std::string("Hrs"), Pixel(1,2,3,Isis::Hrs).ToString());
  EXPECT_EQ(std::string("Lis"), Pixel(1,2,3,Isis::Lis).ToString());
  EXPECT_EQ(std::string("Lrs"), Pixel(1,2,3,Isis::Lrs).ToString());
  EXPECT_EQ(std::string("Null"), Pixel(1,2,3,Isis::Null).ToString());
}
