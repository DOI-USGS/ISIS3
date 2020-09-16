#include <QTextStream>
#include <QStringList>
#include <QFile>
#include <QDomDocument>

#include "hyb2pds4gen.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "SpecialPixel.h"
#include "ProcessExportPds4.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/hyb2pds4gen.xml").expanded();

TEST_F(Hayabusa2OncW2Cube, FunctionalTestHyb2Pds4GenDefault) {
  setInstrument("-37100", "ONC-T", "HAYABUSA-2");

  PvlGroup newBandGroup; 
  std::istringstream  bbstr(R"(
    Group = BandBin
      Name         = W
      Center       = 700
      Width        = 70
      Unit         = Nanometers
      FilterNumber = 4
    End_Group
  )");  
  bbstr >> newBandGroup; 

  PvlGroup &bb = testCube->label()->findObject("IsisCube").findGroup("BandBin");
  bb = newBandGroup; 
   
  QString fileName = testCube->fileName();
  delete testCube;
  
  testCube = new Cube(fileName, "rw");

  QVector<QString> args = {"to=/tmp/output", "PDS4LOGICALIDENTIFIER=Whatever"};
  UserInterface options(APP_XML, args);
  
  hyb2pds4gen(testCube, options);
  
  QDomDocument pds4lab;

  QFile f("/tmp/output.xml");
  if (!f.open(QFile::ReadOnly|QFile::Text)) {
    FAIL() << "Error while reading output cube." << std::endl;
  }  
  
  pds4lab.setContent(&f);
  f.close();

  QDomElement testElem = pds4lab.firstChildElement("Product_Observational").firstChildElement("Observation_Area").firstChildElement("Observing_System");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, testElem.firstChildElement("name").firstChild().nodeValue(), "HAYABUSA-2 ONC-T");

  QDomNodeList elems = testElem.elementsByTagName("Observing_System_Component"); 
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, elems.at(0).toElement().firstChildElement("name").firstChild().nodeValue(), "HAYABUSA-2");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, elems.at(0).toElement().firstChildElement("type").firstChild().nodeValue(), "Spacecraft");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, elems.at(1).toElement().firstChildElement("name").firstChild().nodeValue(), "ONC-T");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, elems.at(1).toElement().firstChildElement("type").firstChild().nodeValue(), "Instrument");

  elems = pds4lab.elementsByTagName("Table_Binary");
  testElem = elems.at(0).toElement();
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, testElem.firstChildElement("local_identifier").firstChild().nodeValue(), "InstrumentPointing");
  QDomNodeList nodeList = testElem.elementsByTagName("name");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(0).firstChild().nodeValue(), "J2000Q0");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(1).firstChild().nodeValue(), "J2000Q1");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(2).firstChild().nodeValue(), "J2000Q2");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(3).firstChild().nodeValue(), "J2000Q3");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(4).firstChild().nodeValue(), "ET");

  testElem = elems.at(1).toElement(); 
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, testElem.firstChildElement("local_identifier").firstChild().nodeValue(), "InstrumentPosition");
  nodeList = testElem.elementsByTagName("name");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(0).firstChild().nodeValue(), "J2000X");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(1).firstChild().nodeValue(), "J2000Y");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(2).firstChild().nodeValue(), "J2000Z");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(3).firstChild().nodeValue(), "J2000XV");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(4).firstChild().nodeValue(), "J2000YV");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(5).firstChild().nodeValue(), "J2000ZV");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(6).firstChild().nodeValue(), "ET");

  testElem = elems.at(2).toElement();
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, testElem.firstChildElement("local_identifier").firstChild().nodeValue(), "BodyRotation");
  nodeList = testElem.elementsByTagName("name");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(0).firstChild().nodeValue(), "J2000Q0");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(1).firstChild().nodeValue(), "J2000Q1");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(2).firstChild().nodeValue(), "J2000Q2");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(3).firstChild().nodeValue(), "J2000Q3");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(4).firstChild().nodeValue(), "ET");

  testElem = elems.at(3).toElement(); 
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, testElem.firstChildElement("local_identifier").firstChild().nodeValue(), "SunPosition");
  nodeList = testElem.elementsByTagName("name");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(0).firstChild().nodeValue(), "J2000X");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(1).firstChild().nodeValue(), "J2000Y");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(2).firstChild().nodeValue(), "J2000Z");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(3).firstChild().nodeValue(), "J2000XV");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(4).firstChild().nodeValue(), "J2000YV");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(5).firstChild().nodeValue(), "J2000ZV");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(6).firstChild().nodeValue(), "ET");

  nodeList = testElem.elementsByTagName("field_location");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(0).firstChild().nodeValue(), "1");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(1).firstChild().nodeValue(), "9");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(2).firstChild().nodeValue(), "17");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(3).firstChild().nodeValue(), "25");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(4).firstChild().nodeValue(), "33");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(5).firstChild().nodeValue(), "41");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, nodeList.at(6).firstChild().nodeValue(), "49");



}
