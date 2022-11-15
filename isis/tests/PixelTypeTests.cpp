#include "PixelType.h"
#include "TestUtilities.h"

#include <gtest/gtest.h>

using namespace Isis;

TEST(PixelSize, TestSize) {
  EXPECT_EQ(SizeOf(PixelType::None), 0);
  EXPECT_EQ(SizeOf(PixelType::UnsignedByte), 1);
  EXPECT_EQ(SizeOf(PixelType::SignedByte), 1);
  EXPECT_EQ(SizeOf(PixelType::UnsignedWord), 2);
  EXPECT_EQ(SizeOf(PixelType::SignedWord), 2);
  EXPECT_EQ(SizeOf(PixelType::UnsignedInteger), 4);
  EXPECT_EQ(SizeOf(PixelType::SignedInteger), 4);
  EXPECT_EQ(SizeOf(PixelType::Real), 4);
  EXPECT_EQ(SizeOf(PixelType::Double), 8);
}

TEST(PixelName, TestName) {
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, PixelTypeName(PixelType::None), QString("None"));
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, PixelTypeName(PixelType::UnsignedByte), QString("UnsignedByte"));
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, PixelTypeName(PixelType::SignedByte), QString("SignedByte"));
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, PixelTypeName(PixelType::UnsignedWord), QString("UnsignedWord"));
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, PixelTypeName(PixelType::SignedWord), QString("SignedWord"));
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, PixelTypeName(PixelType::UnsignedInteger), QString("UnsignedInteger"));
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, PixelTypeName(PixelType::SignedInteger), QString("SignedInteger"));
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, PixelTypeName(PixelType::Real), QString("Real"));
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, PixelTypeName(PixelType::Double), QString("Double"));
}

TEST(PixelEnum, TestEnum) {
  EXPECT_EQ(PixelTypeEnumeration(QString("None")), 0);
  EXPECT_EQ(PixelTypeEnumeration(QString("UnsignedByte")), 1);
  EXPECT_EQ(PixelTypeEnumeration(QString("SignedByte")), 2);
  EXPECT_EQ(PixelTypeEnumeration(QString("UnsignedWord")), 3);
  EXPECT_EQ(PixelTypeEnumeration(QString("SignedWord")), 4);
  EXPECT_EQ(PixelTypeEnumeration(QString("UnsignedInteger")), 5);
  EXPECT_EQ(PixelTypeEnumeration(QString("Real")), 7);
  EXPECT_EQ(PixelTypeEnumeration(QString("Double")), 8);
}

