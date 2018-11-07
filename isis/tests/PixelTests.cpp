#include <gtest/gtest.h>

#include <iostream>
#include <limits>
#include <string>

#include "Pixel.h"
#include "SpecialPixel.h"

using namespace Isis;

// // Construction (fixture?)
// class TestPixel : public ::testing::Test {
// protected:

//   void SetUp() override {
//     pixel_f = new Pixel;
//   }

//   // void SetUp(int sample, int line, int band, double DN) {
//   //   pixel = new Pixel(sample, line, band, DN);
//   // }

//   void TearDown() override {
//     delete pixel;
//   }

//   Pixel *pixel_f;
// }

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
  // Negative test
  EXPECT_EQ(Isis::LOW_REPR_SATU2, Pixel::To16UBit(-1.0));
  // Positive test
  EXPECT_EQ(1.0, Pixel::To16UBit(1.0));
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

// TEST(Pixel, To16Bit) {
//   // EXPECT_EQ()
// }

// TEST(Pixel, To16UBit) {

// }



// TEST(PixelTest, DefaultConstruction) {
//   Pixel p;
  

//   EXPECT_EQ(0, p.to8Bit());
//   EXPECT_EQ(0, p.to16Bit());
//   EXPECT_EQ(0, p.to16UBit());
//   EXPECT_FLOAT_EQ(0, p.to32Bit());
//   EXPECT_FLOAT_EQ(0, p.toFloat());
//   EXPECT_DOUBLE_EQ(0, p.toDouble())
//   EXPECT_EQ(string("0.0"), p.toString());

//   EXPECT_FALSE(p.IsSpecial());
//   EXPECT_FALSE(p.IsValid());
//   EXPECT_FALSE(p.IsNull());
//   EXPECT_FALSE(p.IsHigh());
//   EXPECT_FALSE(p.IsLow());
//   EXPECT_FALSE(p.IsHrs());
//   EXPECT_FALSE(p.IsLrs());
//   EXPECT_FALSE(p.IsHis());
//   EXPECT_FALSE(p.IsLis());
// }

// TEST(PixelTest, Special) {
  
// }

// TEST(PixelTest, Valid) {

// }

// TEST(PixelTest, Null) {

// }

// TEST(PixelTest, High) {

// }

// TEST(PixelTest, Low) {

// }

// TEST(PixelTest, Hrs) {

// }

// TEST(PixelTest, Lrs) {

// }

// TEST(PixelTest, His) {

// }

// TEST(PixelTest, Lis) {

// }





// // Test accessors
// TEST(accessors, line) {
//   // Pixel p()
// }
// TEST(accessors, sample) {

// }
// TEST(accessors, band) {

// }
// TEST(accessors, DN) {

// }