#include "sumspice.h"

#include "Fixtures.h"
#include "TestUtilities.h"
#include "UserInterface.h"
#include "Histogram.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/sumspice.xml").expanded();

TEST_F(DefaultCube, FunctionalTestSumspiceDefault) {
  QString singleCubeListPath = tempDir.path() + "/newCubeList.lis";
  QString toLogPath = tempDir.path() + "/sumspiceUpdateClocksToLog.txt";
  QString sumFileListPath = tempDir.path() + "/sumfilelist.txt";
  FileList singleCubeList;
  singleCubeList.append(testCube->fileName());
  singleCubeList.write(singleCubeListPath);
  QString outPath = tempDir.path() + "/mosaic.cub";

  QVector<QString> args = {"fromlist=" + singleCubeListPath,
                           "sumfilelist= data/sumspice/sumfilelist.txt", //+ sumFileListPath,
                           "sumtime=start",
                           "update=times",
                           "timediff=1.0",
                           "tolog=" + toLogPath
                          };
  UserInterface options(APP_XML, args);
  sumspice(options);



}
