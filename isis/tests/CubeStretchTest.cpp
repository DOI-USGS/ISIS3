#include "Stretch.h"
#include "CubeStretch.h"
#include "IException.h"

#include <QString>
#include <QDebug> 

#include <gtest/gtest.h>

TEST(CubeStretch, DefaultConstructor) {
  Isis::CubeStretch cubeStretch;
  EXPECT_STREQ(cubeStretch.getName().toLatin1().data(), "DefaultStretch");
  EXPECT_STREQ(cubeStretch.getType().toLatin1().data(), "Default"); 
  EXPECT_EQ(cubeStretch.getBandNumber(), 1); 
}

TEST(CubeStretch, ConstructorWithName) {
  Isis::CubeStretch cubeStretch("name");
  EXPECT_STREQ(cubeStretch.getName().toLatin1().data(), "name");
  EXPECT_STREQ(cubeStretch.getType().toLatin1().data(), "Default");
  EXPECT_EQ(cubeStretch.getBandNumber(), 1);
}


TEST(CubeStretch, ConstructorAllArge) {
  Isis::CubeStretch cubeStretch("name", "type", 99);
  EXPECT_STREQ(cubeStretch.getName().toLatin1().data(), "name");
  EXPECT_STREQ(cubeStretch.getType().toLatin1().data(), "type");
  EXPECT_EQ(cubeStretch.getBandNumber(), 99);
}

TEST(CubeStretch, CopyConstructor) {
  Isis::CubeStretch cubeStretch("name", "type", 99);
  Isis::CubeStretch copyStretch(cubeStretch);
  EXPECT_STREQ(cubeStretch.getName().toLatin1().data(), copyStretch.getName().toLatin1().data());
  EXPECT_STREQ(cubeStretch.getType().toLatin1().data(), copyStretch.getType().toLatin1().data());
  EXPECT_EQ(cubeStretch.getBandNumber(), copyStretch.getBandNumber());
}

TEST(CubeStretch, Equality) {
  Isis::CubeStretch cubeStretch99("name", "type", 99);
  Isis::CubeStretch cubeStretch9("name", "type", 9);
  Isis::CubeStretch cubeStretchOtherName("othername", "type", 9);

  EXPECT_FALSE(cubeStretch99 == cubeStretch9);
  EXPECT_FALSE(cubeStretch9 == cubeStretchOtherName);
}

TEST(CubeStretch, GetSetType) {
  Isis::CubeStretch cubeStretch("name", "type", 99);
  cubeStretch.setType("NewType");
  EXPECT_STREQ(cubeStretch.getType().toLatin1().data(), "NewType");
}

TEST(CubeStretch, GetSetName) {
  Isis::CubeStretch cubeStretch("name", "type", 99);
  cubeStretch.setName("NewName");
  EXPECT_STREQ(cubeStretch.getName().toLatin1().data(), "NewName");
}

TEST(CubeStretch, GetSetBandNumber) {
  Isis::CubeStretch cubeStretch("name", "type", 99);
  cubeStretch.setBandNumber(50);
  EXPECT_EQ(cubeStretch.getBandNumber(), 50);
}

