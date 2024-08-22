#include <QTextStream>
#include <QStringList>
#include <QTemporaryFile>

#include "Fixtures.h"
#include "Camera.h"
#include "CameraFixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "XmlToPvlTranslationManager.h"
#include "CSVReader.h"
#include "LineManager.h"

#include "UserInterface.h"

#include "pixel2map.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString PIXEL2MAP_XML = FileName("$ISISROOT/bin/xml/pixel2map.xml").expanded();


TEST_F(DefaultCube, FunctionalTestPixel2mapVector) {
 
  
  //QString csvFileName = tempDir.path() + "/vect.csv";
  //QString vrtFileName = tempDir.path() + "/vect.vrt";
  QString csvFileName = "P2Mvect.csv";
  QString vrtFileName = "P2Mvect.vrt";	
	
  QFile csvFile( csvFileName );
  QFile vrtFile( vrtFileName );
  //QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  QVector<QString> args = {"tovect="+csvFile.fileName(), "from="+ testCube->fileName()};

  UserInterface options(PIXEL2MAP_XML, args);

  try {
    pixel2map(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }

  // TEST 1a: Check we have both csv and vrt output files
  
  FileName csvFileOut( csvFileName );
  EXPECT_TRUE(csvFileOut.fileExists());
  FileName vrtFileOut( vrtFileName );
  EXPECT_TRUE(vrtFileOut.fileExists());
  
  // TEST 1b: Check the output csv file header
  
  CSVReader::CSVAxis csvLine;
  CSVReader csvout = CSVReader( csvFileName , false, 0, ',');
  
  csvLine = csvout.getRow(0);
  
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[0], "sampleno");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[1], "lineno");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[2], "pixelvalue");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, csvLine[3], "geom");
  
  std::cout << "Number of rows in the CSV: " << csvout.size() << '\n';
  //std::cout <<  testCube.sampleCount() << '\n';
  
  EXPECT_EQ(testCube->sampleCount()*testCube->lineCount(), csvout.rows()-1);
  
}
