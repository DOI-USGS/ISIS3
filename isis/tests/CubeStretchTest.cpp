#include "CubeStretch.h"
#include "CubeFixtures.h"
#include "IException.h"
#include "Stretch.h"

#include <QString>
#include <QDebug>

#include <gtest/gtest.h>

using namespace Isis;

TEST(CubeStretch, DefaultConstructor) {
  CubeStretch cubeStretch;
  EXPECT_STREQ(cubeStretch.getName().toLatin1().data(), "DefaultStretch");
  EXPECT_STREQ(cubeStretch.getType().toLatin1().data(), "Default");
  EXPECT_EQ(cubeStretch.getBandNumber(), 1);
}

TEST(CubeStretch, ConstructorWithName) {
  CubeStretch cubeStretch("name");
  EXPECT_STREQ(cubeStretch.getName().toLatin1().data(), "name");
  EXPECT_STREQ(cubeStretch.getType().toLatin1().data(), "Default");
  EXPECT_EQ(cubeStretch.getBandNumber(), 1);
}


TEST(CubeStretch, ConstructorAllArge) {
  CubeStretch cubeStretch("name", "type", 99);
  EXPECT_STREQ(cubeStretch.getName().toLatin1().data(), "name");
  EXPECT_STREQ(cubeStretch.getType().toLatin1().data(), "type");
  EXPECT_EQ(cubeStretch.getBandNumber(), 99);
}

TEST(CubeStretch, CopyConstructor) {
  CubeStretch cubeStretch("name", "type", 99);
  CubeStretch copyStretch(cubeStretch);
  EXPECT_STREQ(cubeStretch.getName().toLatin1().data(), copyStretch.getName().toLatin1().data());
  EXPECT_STREQ(cubeStretch.getType().toLatin1().data(), copyStretch.getType().toLatin1().data());
  EXPECT_EQ(cubeStretch.getBandNumber(), copyStretch.getBandNumber());
}

TEST(CubeStretch, BlobConstructor) {
  // Set Stretch
  CubeStretch cubeStretch("TestStretch", "testType", 2);
 CubeStretch cubeStretchFromBlob(cubeStretch);


  EXPECT_STREQ(cubeStretchFromBlob.getName().toLatin1().data(), cubeStretch.getName().toLatin1().data());
  EXPECT_STREQ(cubeStretchFromBlob.getType().toLatin1().data(), cubeStretch.getType().toLatin1().data());

  EXPECT_STREQ(cubeStretchFromBlob.getType().toLatin1().data(), cubeStretch.getType().toLatin1().data());
  EXPECT_EQ(cubeStretchFromBlob.getBandNumber(), cubeStretch.getBandNumber());
};

TEST_F(SmallCube, CubeStretchWriteRead) {
  // Set up Stretch to write
  QString stretchName = "TestStretch";
  CubeStretch cubeStretch(stretchName, "testType", 2);

  // add pair(s)
  cubeStretch.AddPair(0.0, 1.0);
  cubeStretch.AddPair(0.25, 50.0);
  cubeStretch.AddPair(1.0, 100.0);

  // Write to Cube
  testCube->write(cubeStretch);
  testCube->reopen("rw");

  // Set up stretch and blob to restore to
  CubeStretch restoredStretch = testCube->readCubeStretch(stretchName);
  EXPECT_TRUE(restoredStretch == cubeStretch);
};


TEST(CubeStretch, Equality) {
  CubeStretch cubeStretch99("name", "type", 99);
  CubeStretch cubeStretch9("name", "type", 9);
  CubeStretch cubeStretchOtherName("othername", "type", 9);

  EXPECT_FALSE(cubeStretch99 == cubeStretch9);
  EXPECT_FALSE(cubeStretch9 == cubeStretchOtherName);
}

TEST(CubeStretch, GetSetType) {
  CubeStretch cubeStretch("name", "type", 99);
  cubeStretch.setType("NewType");
  EXPECT_STREQ(cubeStretch.getType().toLatin1().data(), "NewType");
}

TEST(CubeStretch, GetSetName) {
  CubeStretch cubeStretch("name", "type", 99);
  cubeStretch.setName("NewName");
  EXPECT_STREQ(cubeStretch.getName().toLatin1().data(), "NewName");
}

TEST(CubeStretch, GetSetBandNumber) {
  CubeStretch cubeStretch("name", "type", 99);
  cubeStretch.setBandNumber(50);
  EXPECT_EQ(cubeStretch.getBandNumber(), 50);
}
