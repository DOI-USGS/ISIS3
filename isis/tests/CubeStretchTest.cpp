#include "Stretch.h"
#include "CubeStretch.h"
#include "IException.h"

#include <QString>
#include <QDebug> 

#include <gtest/gtest.h>

TEST(CubeStretch, DefaultConstructor) {
  Isis::CubeStretch cubeStretch;
  EXPECT_STREQ(cubeStretch.getName(), "DefaultStretch");
  EXPECT_STREQ(cubeStretch.getType(), "Default"); 
  EXPECT_STREQ(cubeStretch.getBandNumber(), 1); 
}


TEST(CubeStretch, ConstructorWithName) {
  Isis::CubeStretch cubeStretch("name");
  EXPECT_STREQ(cubeStretch.getName(), "name");
  EXPECT_STREQ(cubeStretch.getType(), "Default"); 
  EXPECT_STREQ(cubeStretch.getBandNumber(), 1); 
}


TEST(CubeStretch, ConstructorAllArge) {
  Isis::CubeStretch cubeStretch("name", "type", 99);
  EXPECT_STREQ(cubeStretch.getName(), "name");
  EXPECT_STREQ(cubeStretch.getType(), "type"); 
  EXPECT_STREQ(cubeStretch.getBandNumber(), 99); 
}

TEST(CubeStretch, Equality) {
  Isis::CubeStretch cubeStretch99("name", "type", 99);
  Isis::CubeStretch cubeStretch9("name", "type", 9);
  Isis::CubeStretch cubeStretchOtherName("othername", "type", 9)

  EXPECT_FALSE(cubeStretch99 == cubeStretch9);
  EXPECT_FALSE(cubeStretch9 == cubeStrechOtherName)
}

TEST(CubeStretch, GetSetType) {
  Isis::CubeStretch cubeStretch("name", "type", 99);
  cubeStretch.setType("NewType");
  EXPECT_STREQ(cubeStretch9.getType(), "NewType");
}

TEST(CubeStretch, GetSetName) {
  Isis::CubeStretch cubeStretch("name", "type", 99);
  cubeStretch.setName("NewName");
  EXPECT_STREQ(cubeStretch9.getName(), "NewName");
}

TEST(CubeStretch, GetSetBandNumber) {
  Isis::CubeStretch cubeStretch("name", "type", 99);
  cubeStretch.setBandNumber(50);
  EXPECT_EQ(cubeStretch9.getBandNumber(), 50);
}

