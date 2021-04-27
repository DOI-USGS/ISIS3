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

// Tests a basic gap detection
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
  for(line.begin(); !line.end(); line.next()) {
    for(int i = 0; i < line.size(); i++) {
      if(line.Line() == startGap || line.Line() == endGap)
      {
        line[i] = NULL8;
      }
    }
    lineNum++;
    if(line.Line() == startGap || line.Line() == endGap)
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

  // test that nullified gaps were written to output cube as expected
  std::unique_ptr<Histogram> outHist (outCube.histogram());
  EXPECT_NEAR( outHist->Average(), 56.16, 0.01 );
  EXPECT_NEAR( outHist->Sum(), 3370, 1);
  EXPECT_EQ( outHist->ValidPixels(), 60 );

  // test that Gap group has been written to the logfile
  Pvl logFile = Pvl(logFileName);
  EXPECT_TRUE(logFile.hasGroup("Gap"));
}

// Tests gap detection for the end of a band
// also tests for differing BELOW parameter
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
  int startGap = 4;
  int endGap = 5;
  for(line.begin(); !line.end(); line++) {
    for(int i = 0; i < line.size(); i++) {
      if( line.Band() == 1 && (line.Line() == startGap || line.Line() == endGap) )
      {
        line[i] = NULL8;
      }
    }
    if( line.Band() == 1 && (line.Line() == startGap || line.Line() == endGap) )
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

  // test that nullified gaps were written to output cube as expected
  std::unique_ptr<Histogram> outHist (outCube.histogram());
  EXPECT_NEAR( outHist->Average(), 54.5, 0.01 );
  EXPECT_NEAR( outHist->Sum(), 2725, 1);
  EXPECT_EQ( outHist->ValidPixels(), 50 );

  // test that Gap group has been written to the logfile
  Pvl logFile = Pvl(logFileName);
  EXPECT_TRUE(logFile.hasGroup("Gap"));
}

// Tests the cortol parameter
// adds a few high DNs to lines 4 and 5 of input cube
// in order to have a correlation between 0.0 and 1.0
// also tests for differing ABOVE parameter
TEST_F( SmallCube, FindGapsCorTol )
{
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/findgaps_out.cub";
  QString logFileName = prefix.path() + "/findgaps_log.txt";
  QVector<QString> args = {"from=" + testCube->fileName(),
                          "to=" + cubeFileName,
                           "log=" + logFileName,
                           "above=2", "below=1", "cortol=0.9" };
  UserInterface options(APP_XML, args);

  // add some high value DNs from line 4 to line 5 on top band only
  LineManager line(*testCube);
  int pixelValue = 0;
  int startGap = 4;
  int endGap = 5;
  for(line.begin(); !line.end(); line++) {
    for(int i = 0; i < line.size(); i++) {
      if( line.Line() == startGap || line.Line() == endGap )
      {
        // startgap must be different from endgap
        // otherwise findgaps detects two one-line gaps
        if( line.Line() == startGap )
        {
          line[0] = 99;
        }

        if( i > 5 )
        {
          line[i] = 99;
        }
        else
        {
          line[i] = pixelValue;
        }
      }
      pixelValue++;
    }

    if( line.Line() == startGap || line.Line() == endGap )
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

  // test that nullified gaps were written to output cube as expected
  std::unique_ptr<Histogram> outHist (outCube.histogram());
  EXPECT_NEAR( outHist->Average(), 64.5, 0.01 );
  EXPECT_EQ( outHist->Sum(), 3225 );
  EXPECT_EQ( outHist->ValidPixels(), 50 );

  // test that Gap group has been written to the logfile
  Pvl logFile = Pvl(logFileName);
  ASSERT_TRUE(logFile.hasGroup("Gap"));

  // test that the gap correlation is >0 and < our correlation tolerence: 0.9
  PvlGroup gap = logFile.findGroup("Gap");
  EXPECT_GT( (double)gap.findKeyword("Correlation"), 0.0);
  EXPECT_LT( (double)gap.findKeyword("Correlation"), 0.9);
}
