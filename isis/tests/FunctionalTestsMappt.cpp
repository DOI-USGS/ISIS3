#include <QTextStream>
#include <QStringList>
#include <QFile>
#include <QDebug>

#include "mappt.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "SpecialPixel.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/mappt.xml").expanded();

TEST_F(DefaultCube, FunctionalTestMapptImageTest) {
  QVector<QString> args = {"append=false", "type=image", "sample=1", "line=1"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  mappt(projTestCube, options, &appLog);
  PvlGroup mapPoint = appLog.findGroup("Results");
  
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, mapPoint.findKeyword("FileName"), projTestCube->fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, mapPoint.findKeyword("FilterName"), "CLEAR");
  EXPECT_EQ( (double) mapPoint.findKeyword("Band"), 1);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PlanetographicLatitude"), 9.3870849567571);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PlanetocentricLatitude"), 9.2788326719634);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveWest360Longitude"), 359.14528612684);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveEast360Longitude"), 0.85471387315749);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveEast180Longitude"), 0.85471387315749);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveWest180Longitude"), -0.85471387315751);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("x"), 50000);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("y"), 550000);
}

TEST_F(DefaultCube, FunctionalTestMapptGroundTest) {
  QVector<QString> args = {"append=false", "type=ground", "latitude=9.2788326719634", "longitude=0.85471387315749"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  mappt(projTestCube, options, &appLog);
  
  PvlGroup mapPoint = appLog.findGroup("Results");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, mapPoint.findKeyword("FileName"), projTestCube->fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, mapPoint.findKeyword("FilterName"), "CLEAR");
  EXPECT_EQ( (double) mapPoint.findKeyword("Band"), 1);
  EXPECT_NEAR( (double) mapPoint.findKeyword("Sample"), 1, 1e-8);
  EXPECT_NEAR( (double) mapPoint.findKeyword("Line"), 1, 1e-8);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PlanetographicLatitude"), 9.3870849567571);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PlanetocentricLatitude"), 9.2788326719634);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveWest360Longitude"), 359.14528612684);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveEast360Longitude"), 0.85471387315749);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveEast180Longitude"), 0.85471387315749);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveWest180Longitude"), -0.85471387315751);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("x"), 50000);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("y"), 550000);
}

TEST_F(DefaultCube, FunctionalTestMapptProjectionTest) {
  QVector<QString> args = {"append=false", "type=projection", "x=50000.0", "y=550000.0"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  mappt(projTestCube, options, &appLog);
  
  PvlGroup mapPoint = appLog.findGroup("Results");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, mapPoint.findKeyword("FileName"), projTestCube->fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, mapPoint.findKeyword("FilterName"), "CLEAR");
  EXPECT_EQ( (double) mapPoint.findKeyword("Band"), 1);
  EXPECT_NEAR( (double) mapPoint.findKeyword("Sample"), 1, 1e-8);
  EXPECT_NEAR( (double) mapPoint.findKeyword("Line"), 1, 1e-8);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PlanetographicLatitude"), 9.3870849567571);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PlanetocentricLatitude"), 9.2788326719634);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveWest360Longitude"), 359.14528612684);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveEast360Longitude"), 0.85471387315749);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveEast180Longitude"), 0.85471387315749);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveWest180Longitude"), -0.85471387315751); 
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("x"), 50000);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("y"), 550000);
}

TEST_F(DefaultCube, FunctionalTestMapptCoordsysTest) {
  QVector<QString> args = {"append=false", 
                           "coordsys=userdefined", 
                           "type=ground", 
                           "lattype=planetographic"
                           "londir=positivewest",
                           "londom=180",
                           "latitude=9.3870849567571",
                           "longitude=0.85471387315749"};

  UserInterface options(APP_XML, args);
  Pvl appLog;
  mappt(projTestCube, options, &appLog);
  
  PvlGroup mapPoint = appLog.findGroup("Results");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, mapPoint.findKeyword("FileName"), projTestCube->fileName());
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, mapPoint.findKeyword("FilterName"), "CLEAR");
  EXPECT_EQ( (double) mapPoint.findKeyword("Band"), 1);
  EXPECT_NEAR( (double) mapPoint.findKeyword("Sample"), 1, 1e-8);
  EXPECT_NEAR( (double) mapPoint.findKeyword("Line"), 1, 1e-8);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PlanetographicLatitude"), 9.3870849567571);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PlanetocentricLatitude"), 9.2788326719634);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveWest360Longitude"), 359.14528612684);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveEast360Longitude"), 0.85471387315749);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveEast180Longitude"), 0.85471387315749);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("PositiveWest180Longitude"), -0.85471387315751); 
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("x"), 50000);
  EXPECT_DOUBLE_EQ( (double) mapPoint.findKeyword("y"), 550000); 

}

TEST_F(DefaultCube, FunctionalTestMapptFlatFileTest) {
  QFile flatFile(tempDir.path() + "/testOut.txt");
  QVector<QString> args = {"to="+flatFile.fileName(), "append=false", "type=projection", "x=50000.0", "y=550000.0", "format=flat"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  mappt(projTestCube, options, &appLog);
  PvlGroup mapPoint = appLog.findGroup("Results");
  
  int lineNumber = 0;
  QTextStream flatStream(&flatFile);
  
  if (flatFile.open(QIODevice::ReadOnly)) {
    while(!flatStream.atEnd()) {
      QString line = flatStream.readLine();
      QStringList fields = line.split(",");
      
      if(lineNumber == 0) {
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(0), "Filename");
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(1), "Sample");
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(2), "Line");
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(3), "Band");
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(4), "FilterName");
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(5), "PixelValue");
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(6), "X");
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(7), "Y");
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(8), "PlanetocentricLatitude");
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(9), "PlanetographicLatitude");
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(10), "PositiveEast360Longitude");
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(11), "PositiveEast180Longitude");
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(12), "PositiveWest360Longitude");
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(13), "PositiveWest180Longitude");
      }
      else if(lineNumber == 1) {
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(0), mapPoint.findKeyword("FileName"));
        EXPECT_DOUBLE_EQ(fields.value(1).toDouble(), mapPoint.findKeyword("Sample"));
        EXPECT_DOUBLE_EQ(fields.value(2).toDouble(), mapPoint.findKeyword("Line"));
        EXPECT_DOUBLE_EQ(fields.value(3).toDouble(), mapPoint.findKeyword("Band"));
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(4), mapPoint.findKeyword("FilterName"));
        EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(5), mapPoint.findKeyword("PixelValue"));
        EXPECT_DOUBLE_EQ(fields.value(6).toDouble(), mapPoint.findKeyword("X"));
        EXPECT_DOUBLE_EQ(fields.value(7).toDouble(), mapPoint.findKeyword("Y"));
        EXPECT_DOUBLE_EQ(fields.value(8).toDouble(), mapPoint.findKeyword("PlanetocentricLatitude"));
        EXPECT_DOUBLE_EQ(fields.value(9).toDouble(), mapPoint.findKeyword("PlanetographicLatitude") );
        EXPECT_DOUBLE_EQ(fields.value(10).toDouble(), mapPoint.findKeyword("PositiveEast360Longitude"));
        EXPECT_DOUBLE_EQ(fields.value(11).toDouble(), mapPoint.findKeyword("PositiveEast180Longitude"));
        EXPECT_DOUBLE_EQ(fields.value(12).toDouble(), mapPoint.findKeyword("PositiveWest360Longitude"));
        EXPECT_DOUBLE_EQ(fields.value(13).toDouble(), mapPoint.findKeyword("PositiveWest180Longitude"));
      }
      lineNumber++;
    }
  }
}

TEST_F(DefaultCube, FunctionalTestMapptAllowOutside) {
  QVector<QString> args = {"type=image", "sample=-1", "line=-1", "allowoutside=true"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  mappt(projTestCube, options, &appLog);
  PvlGroup groundPoint = appLog.findGroup("Results");
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Sample"), -1.0);
  EXPECT_DOUBLE_EQ( (double) groundPoint.findKeyword("Line"), -1.0);
  
  args = {"type=image", "sample=-1", "line=-1", "allowoutside=false"};
  mappt(projTestCube, options, &appLog);

  PvlGroup mapPoint = appLog.findGroup("Results");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, mapPoint.findKeyword("OutsideOfImage"), "Requested point falls outside of image boundaries");
}

TEST_F(DefaultCube, FunctionalTestMapptBandTest) {
  QVector<QString> args = { "from="+projTestCube->fileName()+"+2", "append=false", "type=image", "sample=1", "line=1"};
  UserInterface options(APP_XML, args);
  Pvl appLog;
  mappt(options, &appLog);
  PvlGroup mapPoint = appLog.findGroup("Results");
  
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, mapPoint.findKeyword("FilterName"), "NIR");
  EXPECT_EQ( (double) mapPoint.findKeyword("Band"), 2);
}
