#include <QTemporaryDir>

#include "findgaps.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"
#include "LineManager.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/findgaps.xml").expanded();

TEST_F( SmallCube, FindGapsDefault )
{

  LineManager line(*testCube);
  int lineNum = 0;
  for(line.begin(); !line.end(); line++) {
    for(int i = 0; i < line.size(); i++) {
      if(lineNum == 4 || lineNum % 10 == 4 ||
         lineNum == 5 || lineNum % 10 == 5)
      {
        line[i] = NULL8;
      }
    }
    lineNum++;

    if(lineNum == 4 || lineNum % 10 == 4 ||
       lineNum == 5 || lineNum % 10 == 5)
    {
      testCube->write(line);
    }
  }
  testCube->reopen("rw");



  QTemporaryDir prefix;
  // QString cubeFileName = prefix.path() + "/findgaps_out.cub";
  // QString logFileName = prefix.path() + "/findgaps_log.txt";

  QString cubeFileName = "/home/tgiroux/Desktop/findgaps_out_h.cub";
  QString logFileName = "/home/tgiroux/Desktop/findgaps_out_h.txt";

  QVector<QString> args = {"from=" + testCube->fileName(),
                          "to=" + cubeFileName,
                           "log=" + logFileName,
                           "above=1", "below=1" };
  UserInterface options(APP_XML, args);



  try {
    findgaps(options);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(cubeFileName);

  std::unique_ptr<Histogram> outHist (outCube.histogram());

  EXPECT_NEAR( outHist->Average(), 0, 1e-6 );
  EXPECT_NEAR( outHist->Sum(), 0, 1e-6);
  EXPECT_EQ( outHist->ValidPixels(), 999 );

}

TEST_F( SmallCube, FindGapsEndOfBand )
{
  QTemporaryDir prefix;
  // QString cubeFileName = prefix.path() + "/findgaps_out.cub";
  // QString logFileName = prefix.path() + "/findgaps_log.txt";

  QString cubeFileName = "/home/tgiroux/Desktop/findgaps_out_b.cub";
  QString logFileName = "/home/tgiroux/Desktop/findgaps_out_b.pvl";

  QVector<QString> args = {"from=" + testCube->fileName(),
                          "to=" + cubeFileName,
                           "log=" + logFileName,
                           "above=1", "below=2" };

  UserInterface options(APP_XML, args);

  LineManager line(*testCube);
  int lineNum = 0;
  for(line.begin(); !line.end(); line++) {
    for(int i = 0; i < line.size(); i++) {

      if( lineNum == 4 || lineNum == 5 )
      {
        line[i] = NULL8;
      }

    }
    lineNum++;

    if( lineNum == 4 || lineNum == 5 )
    {
      testCube->write(line);
    }
  }
  testCube->reopen("rw");

  try {
    findgaps(options);
  }
  catch (IException &e) {
    FAIL() << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(cubeFileName);

  std::unique_ptr<Histogram> outHist (outCube.histogram());
  EXPECT_NEAR( outHist->Average(), 0, 1e-6 );
  EXPECT_NEAR( outHist->Sum(), 0, 1e-6);
  EXPECT_EQ( outHist->ValidPixels(), 999 );

}