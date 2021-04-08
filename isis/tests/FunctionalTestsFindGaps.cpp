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
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/findgaps_out.cub";
  QString logFileName = prefix.path() + "/findgaps_log.txt";
  QVector<QString> args = {"from=" + testCube->fileName(),
                          "to=" + cubeFileName,
                           "log=" + logFileName,
                           "above=1", "below=1" };
  UserInterface options(APP_XML, args);

  // fill with nulls from line 4 to line 5 through all bands
  LineManager line(*testCube);
  int lineNum = 0;
  int startGap = 4;
  int endGap = 5;
  int height = 10;
  for(line.begin(); !line.end(); line++) {
    for(int i = 0; i < line.size(); i++) {
      if(lineNum == startGap || lineNum % height == startGap ||
         lineNum == endGap || lineNum % height == endGap)
      {
        line[i] = NULL8;
      }
    }
    lineNum++;

    if(lineNum == startGap || lineNum % height == startGap ||
        lineNum == endGap || lineNum % height == endGap)
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
  EXPECT_NEAR( outHist->Average(), 56.16, 0.01 );
  EXPECT_NEAR( outHist->Sum(), 3370, 1);
  EXPECT_EQ( outHist->ValidPixels(), 60 );

  Pvl logFile = Pvl(logFileName);
  EXPECT_TRUE(logFile.hasGroup("Gap"));
}

TEST_F( SmallCube, FindGapsEndOfBand )
{
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/findgaps_out.cub";
  QString logFileName = prefix.path() + "/findgaps_log.txt";
  QVector<QString> args = {"from=" + testCube->fileName(),
                          "to=" + cubeFileName,
                           "log=" + logFileName,
                           "above=1", "below=2" };

  UserInterface options(APP_XML, args);

  // fill with nulls from line 4 to line 5 on top band only
  LineManager line(*testCube);
  int lineNum = 0;
  int startGap = 4;
  int endGap = 5;
  for(line.begin(); !line.end(); line++) {
    for(int i = 0; i < line.size(); i++) {
      if( lineNum == startGap || lineNum == endGap )
      {
        line[i] = NULL8;
      }
    }
    lineNum++;

    if( lineNum == startGap || lineNum == endGap )
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
  EXPECT_NEAR( outHist->Average(), 54.5, 0.01 );
  EXPECT_NEAR( outHist->Sum(), 2725, 1);
  EXPECT_EQ( outHist->ValidPixels(), 50 );

  Pvl logFile = Pvl(logFileName);
  EXPECT_TRUE(logFile.hasGroup("Gap"));
}
