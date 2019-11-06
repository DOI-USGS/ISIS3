#include "PixelType.h"
#include "TestUtilities.h"

#include <gtest/gtest.h>
#include <QPair>

using namespace Isis;


class PixelSize : public testing::TestWithParam<QPair<PixelType, int>> {
};

class PixelName : public testing::TestWithParam<QPair<PixelType, QString>> {
};

class PixelEnum : public testing::TestWithParam<QPair<QString, int>> {
};

TEST_P(PixelSize, TestSize) {
  EXPECT_EQ(SizeOf(GetParam().first), GetParam().second);
}

TEST_P(PixelName, TestName) {
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, PixelTypeName(GetParam().first), GetParam().second);
}

TEST_P(PixelEnum, TestEnum) {
  EXPECT_EQ(PixelTypeEnumeration(GetParam().first), GetParam().second);
}

INSTANTIATE_TEST_CASE_P(PixelType, PixelSize, ::testing::Values(
  qMakePair(PixelType::None, 0),
  qMakePair(PixelType::UnsignedByte, 1),
  qMakePair(PixelType::SignedByte, 1),
  qMakePair(PixelType::UnsignedWord, 2),
  qMakePair(PixelType::SignedWord, 2),
  qMakePair(PixelType::UnsignedInteger, 4),
  qMakePair(PixelType::SignedInteger, 4),
  qMakePair(PixelType::Real, 4),
  qMakePair(PixelType::Double, 8)));

INSTANTIATE_TEST_CASE_P(PixelType, PixelName, ::testing::Values(
  qMakePair(PixelType::None, QString("None")),
  qMakePair(PixelType::UnsignedByte, QString("UnsignedByte")),
  qMakePair(PixelType::SignedByte, QString("SignedByte")),
  qMakePair(PixelType::UnsignedWord, QString("UnsignedWord")),
  qMakePair(PixelType::SignedWord, QString("SignedWord")),
  qMakePair(PixelType::UnsignedInteger, QString("UnsignedInteger")),
  qMakePair(PixelType::SignedInteger, QString("SignedInteger")),
  qMakePair(PixelType::Real, QString("Real")),
  qMakePair(PixelType::Double, QString("Double"))));

INSTANTIATE_TEST_CASE_P(PixelType, PixelEnum, ::testing::Values(
  qMakePair(QString("None"), 0),
  qMakePair(QString("UnsignedByte"), 1),
  qMakePair(QString("SignedByte"), 2),
  qMakePair(QString("UnsignedWord"), 3),
  qMakePair(QString("SignedWord"), 4),
  qMakePair(QString("UnsignedInteger"), 5),
  qMakePair(QString("SignedInteger"), 6),
  qMakePair(QString("Real"), 7),
  qMakePair(QString("Double"), 8)));
