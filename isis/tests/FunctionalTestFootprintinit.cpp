#include <iostream>
#include <QTemporaryFile>
#include <QTemporaryDir>

#include "footprintinit.h"

#include "Cube.h"
#include "Pvl.h"
#include "TestUtilities.h"
#include "FileName.h"
#include "LineManager.h"

#include "Fixtures.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/footprintinit.xml").expanded();

TEST_F(DefaultCube, FunctionalTestFootprintinitDefault) {
  QVector<QString> footprintArgs = {};
  UserInterface footprintUi(APP_XML, footprintArgs);

  footprintinit(testCube, footprintUi);
  ASSERT_TRUE(testCube->label()->hasObject("Polygon"));

  ImagePolygon poly;
  testCube->read(poly);

  ASSERT_EQ(49, poly.numVertices());
}

TEST_F(DefaultCube, FunctionalTestFootprintinitLincSinc) {
  QVector<QString> footprintArgs = {"linc=50", "sinc=50"};
  UserInterface footprintUi(APP_XML, footprintArgs);

  footprintinit(testCube, footprintUi);
  ASSERT_TRUE(testCube->label()->hasObject("Polygon"));

  ImagePolygon poly;
  testCube->read(poly);

  ASSERT_EQ(95, poly.numVertices());
}

TEST_F(DefaultCube, FunctionalTestFootprintinitVertices) {
  QVector<QString> footprintArgs = {"incType=vertices", "numvertices=40"};
  UserInterface footprintUi(APP_XML, footprintArgs);

  footprintinit(testCube, footprintUi);
  ASSERT_TRUE(testCube->label()->hasObject("Polygon"));

  ImagePolygon poly;
  testCube->read(poly);

  ASSERT_EQ(43, poly.numVertices());
}

TEST_F(DefaultCube, FunctionalTestFootprintinitCamera) {
  QVector<QString> footprintArgs = {"maxemission=69", "maxincidence=70"};
  UserInterface footprintUi(APP_XML, footprintArgs);

  footprintinit(testCube, footprintUi);
  ASSERT_TRUE(testCube->label()->hasObject("Polygon"));

  ImagePolygon poly;
  testCube->read(poly);

  ASSERT_EQ(34, poly.numVertices());
}

TEST_F(DefaultCube, FunctionalTestFootprintinitTestXY) {
  QVector<QString> footprintArgs = {"testxy=yes"};
  UserInterface footprintUi(APP_XML, footprintArgs);

  footprintinit(testCube, footprintUi);
  ASSERT_TRUE(testCube->label()->hasObject("Polygon"));

  ImagePolygon poly;
  testCube->read(poly);

  ASSERT_EQ(49, poly.numVertices());
}

TEST_F(DefaultCube, FunctionalTestFootprintinitPrecision) {
  QVector<QString> footprintArgs = {"increaseprecision=yes"};
  UserInterface footprintUi(APP_XML, footprintArgs);

  Pvl log;

  footprintinit(testCube, footprintUi, &log);
  ASSERT_TRUE(testCube->label()->hasObject("Polygon"));

  ASSERT_TRUE(log.hasGroup("Results"));
  ASSERT_EQ(100, log.findGroup("Results").findKeyword("LINC")[0].toInt());
  ASSERT_EQ(100, log.findGroup("Results").findKeyword("SINC")[0].toInt());

  ImagePolygon poly;
  testCube->read(poly);

  ASSERT_EQ(49, poly.numVertices());
}
