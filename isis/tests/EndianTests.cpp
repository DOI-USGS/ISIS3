#include "Endian.h"

#include <gtest/gtest.h>

using namespace Isis;

TEST(ConversionTest, ByteOrder) {
	ASSERT_EQ(ByteOrderEnumeration("None"), Isis::ByteOrder::NoByteOrder);
	ASSERT_EQ(ByteOrderName(Isis::ByteOrder::NoByteOrder), "None");

	ASSERT_EQ(ByteOrderEnumeration("Lsb"), Isis::ByteOrder::Lsb);
	ASSERT_EQ(ByteOrderName(Isis::ByteOrder::Lsb), "Lsb");

	ASSERT_EQ(ByteOrderEnumeration("Msb"), Isis::ByteOrder::Msb);
	ASSERT_EQ(ByteOrderName(Isis::ByteOrder::Msb), "Msb");
}

