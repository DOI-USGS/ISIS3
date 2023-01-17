/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <gtest/gtest.h>

#include "Longitude.h"
#include "SpecialPixel.h"

using namespace Isis;

// ----- Testing Constructors -----
// Default constructor
TEST(Longitude, DefaultConstructor) {
  Longitude lon;
  EXPECT_EQ(Isis::Null, lon.degrees());    
}

// Constructor given a value in degrees
TEST(Longitude, DegreesConstructor) {
  Longitude lon(180.0, Angle::Degrees);
  EXPECT_EQ(180, lon.degrees());    
}


// Constructor given a positive west value in degrees
TEST(Longitude, PositiveWest) {
  Longitude lon(180.0, Angle::Degrees, Longitude::PositiveWest);
  EXPECT_EQ(180, lon.degrees());    
}

// Constructor given a positive west, -90 value in degrees
TEST(Longitude, PWNegative90) {
  Longitude lon(-90.0, Angle::Degrees, Longitude::PositiveWest, 
        Longitude::Domain180);
  EXPECT_EQ(90, lon.degrees());    
}

// Constructor given -90 degrees PW & 360 domain
TEST(Longitude, PW360Domain) {
  Longitude lon(-90.0, Angle::Degrees, Longitude::PositiveWest,
        Longitude::Domain360);
  EXPECT_EQ(450, lon.degrees());
}

// Copy constructor
TEST(Longitude, CopyConstructor) {
  Longitude lon(90.0, Angle::Degrees);
  EXPECT_EQ(lon.degrees(), Longitude(lon).degrees());

  Longitude lonCopy(lon);
  lonCopy = lon;
  EXPECT_EQ(90, lonCopy.degrees());
}

// ----- Testing Set Methods -----
// Set to 90 degrees
TEST(Longitude, Set90Degrees) {
  Longitude lon(270.0, Angle::Degrees);
  lon.setPositiveEast(90, Angle::Degrees);
  EXPECT_EQ(90, lon.degrees());
}

// Set to 90 degrees PW
TEST(Longitude, Set90DegreesPW) {
  Longitude lon(270.0, Angle::Degrees);
  lon.setPositiveWest(90, Angle::Degrees);
  EXPECT_EQ(270, lon.degrees());
}

// ----- Testing Get Methods -----
TEST(Longitude, Get90Degrees) {
  Longitude lon(90.0, Angle::Degrees);

  // degrees universal
  EXPECT_EQ(90, lon.degrees());

  // degrees positive east
  EXPECT_EQ(90, lon.positiveEast(Angle::Degrees));

  // radians positive east
  EXPECT_EQ(0.5, lon.positiveEast(Angle::Radians) / PI);

  // degrees positive west
  EXPECT_EQ(270, lon.positiveWest(Angle::Degrees));

  // radians positive west
  EXPECT_EQ(1.5, lon.positiveWest(Angle::Radians) / PI);
}

TEST(Longitude, Get450Degrees) {
  Longitude lon(450.0, Angle::Degrees);

  // degrees universal
  EXPECT_EQ(450, lon.degrees());

  // degrees positive east
  EXPECT_EQ(450, lon.positiveEast(Angle::Degrees));

  // radians positive east
  EXPECT_EQ(2.5, lon.positiveEast(Angle::Radians) / PI);

  // degrees positive west
  EXPECT_EQ(-90, lon.positiveWest(Angle::Degrees));

  // radians positive west
  EXPECT_EQ(-0.5, lon.positiveWest(Angle::Radians) / PI);
}

TEST(Longitude, GetNegative450Degrees) {
  Longitude lon(-450.0, Angle::Degrees);

  // degrees universal
  EXPECT_EQ(-450, lon.degrees());

  // degrees positive east
  EXPECT_EQ(-450, lon.positiveEast(Angle::Degrees));

  // radians positive east
  EXPECT_EQ(-2.5, lon.positiveEast(Angle::Radians) / PI);

  // degrees positive west
  EXPECT_EQ(810, lon.positiveWest(Angle::Degrees));

  // radians positive west
  EXPECT_EQ(4.5, lon.positiveWest(Angle::Radians) / PI);
}

TEST(Longitude, GetNegative450DegreesPW) {
  Longitude lon(-450.0, Angle::Degrees, Longitude::PositiveWest);

  // degrees universal
  EXPECT_EQ(810, lon.degrees());

  // degrees positive east
  EXPECT_EQ(810, lon.positiveEast(Angle::Degrees));

  // radians positive east
  EXPECT_EQ(4.5, lon.positiveEast(Angle::Radians) / PI);

  // degrees positive west
  EXPECT_EQ(-450, lon.positiveWest(Angle::Degrees));

  // radians positive west
  EXPECT_EQ(-2.5, lon.positiveWest(Angle::Radians) / PI);
}

// ----- Testing Domain Methods -----
TEST(Longitude, ForceDomain) {
  Longitude lon(270.0, Angle::Degrees);

  // Test force180Domain
  EXPECT_EQ(-90, lon.force180Domain().degrees());

  // Test force360Domain
  EXPECT_EQ(270, lon.force360Domain().degrees());

  // Force 360 to the 360 domain
  lon.setPositiveEast(360.0, Angle::Degrees);
  EXPECT_EQ(360, lon.force360Domain().degrees());

}

// ----- Testing Range Methods -----

TEST(Longitude, InRange) {

  Longitude lon(45.0, Angle::Degrees);

  EXPECT_TRUE(lon.inRange(Longitude(0, Angle::Degrees), Longitude(360, Angle::Degrees)));
  EXPECT_FALSE(lon.inRange(Longitude(360, Angle::Degrees), Longitude(0, Angle::Degrees)));
  EXPECT_FALSE(lon.inRange(Longitude(350, Angle::Degrees), Longitude(355, Angle::Degrees)));
  EXPECT_TRUE(lon.inRange(Longitude(0, Angle::Degrees), Longitude(160, Angle::Degrees)));
  EXPECT_FALSE(lon.inRange(Longitude(0, Angle::Degrees), Longitude(44, Angle::Degrees)));
  EXPECT_FALSE(lon.inRange(Longitude(46, Angle::Degrees), Longitude(90, Angle::Degrees)));
  EXPECT_TRUE(lon.inRange(Longitude(0, Angle::Degrees), Longitude(45, Angle::Degrees)));
  EXPECT_TRUE(lon.inRange(Longitude(45, Angle::Degrees), Longitude(90, Angle::Degrees)));
}