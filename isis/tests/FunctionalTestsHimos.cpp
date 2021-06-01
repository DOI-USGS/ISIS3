#include <QTemporaryDir>

#include "himos.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/himos.xml").expanded();

TEST_F(MroHiriseCube, FunctionalTestHimosDefault) {

  FileName mosFileList(tempDir.path() + "/himosFileList.lis");

  FileList * cubeList = new FileList();
  cubeList->append(dejitteredCube.fileName());
  cubeList->write(mosFileList);

  QVector<QString> args = {"from=" + mosFileList.toString(),
                           "to=" + tempDir.path() + "/outputMos.cub"};

  UserInterface options(APP_XML, args);
  try {
   himos(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to create mosaic image: " << e.toString().toStdString().c_str() << std::endl;
  }
  Cube outputMos(options.GetFileName("TO"));
  PvlObject inputCubeLabel = dejitteredCube.label()->findObject("IsisCube");
  PvlObject outputCubeLabel = outputMos.label()->findObject("IsisCube");
  PvlGroup dimensions = outputCubeLabel.findObject("Core").findGroup("Dimensions");
  PvlGroup pixels = outputCubeLabel.findObject("Core").findGroup("Pixels");

  EXPECT_EQ(int(dimensions["Samples"]), 21);
  EXPECT_EQ(int(dimensions["Lines"]), 91);
  EXPECT_EQ(int(dimensions["Bands"]), 3);

  EXPECT_EQ(pixels["Type"][0], "SignedWord");
  EXPECT_EQ(pixels["ByteOrder"][0], "Lsb");
  EXPECT_EQ(double(pixels["Base"]), 1.4996565881653);
  EXPECT_EQ(double(pixels["Multiplier"]), 4.57882446313283e-05);

  AssertPvlGroupEqual("InputMapping", "OutputMapping",
                      inputCubeLabel.findGroup("Mapping"), outputCubeLabel.findGroup("Mapping"));
  AssertPvlGroupEqual("InputMosaic", "OutputMosaic",
                      inputCubeLabel.findGroup("Mosaic"), outputCubeLabel.findGroup("Mosaic"));
}

TEST_F(MroHiriseCube, FunctionalTestHimosError) {

  FileName mosFileList(tempDir.path() + "/himosFileList.lis");

  FileList * cubeList = new FileList();
  cubeList->append(testCube->fileName());
  cubeList->write(mosFileList);

  QVector<QString> args = {"from=" + mosFileList.toString(),
                           "to=" + tempDir.path() + "/outputMos.cub"};

  UserInterface options(APP_XML, args);
  try {
   himos(options);
   FAIL() << "Should not have been able to create mosaic: " << options.GetFileName("TO").toStdString().c_str() << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), testing::HasSubstr("was NOT created"));
  }
}

TEST_F(MroHiriseCube, FunctionalTestHimosMismatchObs) {
  CubeAttributeOutput cubeAtts(FileName(dejitteredCube.fileName()));
  Cube *copyDejitteredCube = dejitteredCube.copy(tempDir.path() + "/copyDejitteredCube.cub", cubeAtts);
  copyDejitteredCube->label()->findObject("IsisCube").findGroup("Archive")["ObservationId"] = "Banana";
  copyDejitteredCube->reopen("rw");

  FileName mosFileList(tempDir.path() + "/himosFileList.lis");

  FileList * cubeList = new FileList();
  cubeList->append(dejitteredCube.fileName());
  cubeList->append(copyDejitteredCube->fileName());
  cubeList->write(mosFileList);

  QVector<QString> args = {"from=" + mosFileList.toString(),
                           "to=" + tempDir.path() + "/outputMos.cub"};

  UserInterface options(APP_XML, args);
  try {
   himos(options);
   FAIL() << "Should not have been able to create mosaic: " << options.GetFileName("TO").toStdString().c_str() << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), testing::HasSubstr("was NOT created"));
  }
}

TEST_F(MroHiriseCube, FunctionalTestHimosMismatchFilter) {
  CubeAttributeOutput cubeAtts(FileName(dejitteredCube.fileName()));
  Cube *copyDejitteredCube = dejitteredCube.copy(tempDir.path() + "/copyDejitteredCube.cub", cubeAtts);
  copyDejitteredCube->label()->findObject("IsisCube").findGroup("BandBin")["Name"] = "Red";
  copyDejitteredCube->reopen("rw");

  FileName mosFileList(tempDir.path() + "/himosFileList.lis");

  FileList * cubeList = new FileList();
  cubeList->append(dejitteredCube.fileName());
  cubeList->append(copyDejitteredCube->fileName());
  cubeList->write(mosFileList);

  QVector<QString> args = {"from=" + mosFileList.toString(),
                           "to=" + tempDir.path() + "/outputMos.cub"};

  UserInterface options(APP_XML, args);
  try {
   himos(options);
   FAIL() << "Should not have been able to create mosaic: " << options.GetFileName("TO").toStdString().c_str() << std::endl;
  }
  catch (IException &e) {
    EXPECT_THAT(e.what(), testing::HasSubstr("was NOT created"));
  }
}
