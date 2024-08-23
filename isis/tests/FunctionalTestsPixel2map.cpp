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
#include "Histogram.h"

#include "UserInterface.h"

#include "pixel2map.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString PIXEL2MAP_XML = FileName("$ISISROOT/bin/xml/pixel2map.xml").expanded();

// DefaultCube
// MroCtxCube
TEST_F(MroCtxCube, FunctionalTestPixel2mapVector) {
  
  // DefaultCube only
  //resizeCube(5,5,1);
  
  QString csvFileName = tempDir.path() + "/vect.csv";
  QString vrtFileName = tempDir.path() + "/vect.vrt";
  //QString csvFileName = "/Users/alf/gitwrk/ISIS3dev-b/build/DefaultCube.csv";
  //QString vrtFileName = "/Users/alf/gitwrk/ISIS3dev-b/build/DefaultCube.vrt";	
	
  QFile csvFile( csvFileName );
  QFile vrtFile( vrtFileName );
  //QString outCubeFileName = tempDir.path() + "/outTemp.cub";
  
  QVector<QString> args = {"TOVECT="+csvFile.fileName(), "FROM="+ testCube->fileName() };
  
  //QVector<QString> args = {"tovect="+csvFile.fileName(), "from=/Users/alf/gitwrk/ISIS3dev-b/isis/tests/data/mroCtxImage/ctxTestImage.cub"};

  UserInterface options(PIXEL2MAP_XML, args);

  try {
    pixel2map(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to open image: " << e.what() << std::endl;
  }

  // pre-TEST: Check that we have no null values in the cube
  std::unique_ptr<Histogram> hist (testCube->histogram());

  EXPECT_EQ(hist->ValidPixels(), testCube->sampleCount()*testCube->lineCount());
  EXPECT_EQ(hist->NullPixels(), 0);


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
  
  //std::cout << "Number of rows in the CSV: " << csvout.size() << '\n';
  //std::cout <<  testCube.sampleCount() << '\n';
  
  // The number of lines must be equal to the number of pixels
  EXPECT_EQ(testCube->sampleCount()*testCube->lineCount(), csvout.rows()-1);
  
  
}
